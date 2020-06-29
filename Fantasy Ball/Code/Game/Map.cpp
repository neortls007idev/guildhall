﻿#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/StdExtensions.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/ParticleSystem/ParticleEmitter2D.hpp"
#include "Engine/ParticleSystem/ParticleSystem2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Game//Game.hpp"
#include "Game/Ball.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"

#include "Engine/Time/Time.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Paddle.hpp"
#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*				g_theRenderer;
extern AudioSystem*					g_theAudioSystem;
extern ParticleSystem2D*			g_theParticleSystem2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::Map( Game* owner ) :
								m_owner( owner ) 
{
	LevelBounds();

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::Map( Game* owner , MapDefinition* mapDefinition , std::string mapName ) :
																				m_owner( owner ) ,
																				m_name( mapName ) ,
																				m_mapDefinition( mapDefinition )
{
	m_dimensions.x = m_mapDefinition->m_width;
	m_dimensions.y = m_mapDefinition->m_height;

	LevelBounds();

	for( size_t mapGenSteps = 0; mapGenSteps < m_mapDefinition->m_generationSteps.size(); mapGenSteps++ )
	{
		m_mapDefinition->m_generationSteps[ mapGenSteps ]->RunStep( *this );
	}

	m_tileEmitter = g_theParticleSystem2D->CreateNewParticleEmitter( g_theRenderer , m_owner->m_gameSS[ SS_VFX_FLARE ] ,
	                                                                 10000 , nullptr , ADDITIVE );
	m_boundsEmitter = g_theParticleSystem2D->CreateNewParticleEmitter( g_theRenderer , m_owner->m_gameSS[ SS_VFX_LEAVES ] ,
																	   2500 , nullptr , ADDITIVE );
	m_paddleEmitter = g_theParticleSystem2D->CreateNewParticleEmitter( g_theRenderer , m_owner->m_gameSS[ SS_VFX_FLOWERS ] ,
																	   2000 , nullptr , ADDITIVE );

	SpawnNewEntity( PADDLE , Vec2::ZERO );

	Paddle* thePaddle = ( Paddle* ) m_entityListsByType[ PADDLE ][ 0 ];
	AABB2	paddleBounds = thePaddle->GetCollider();
	
	SpawnNewEntity( BALL , paddleBounds.GetCenter() + Vec2::ZERO_ONE * 37.5f );
	m_numAliveBalls++;
	
	ResolveBallvPaddleCollisions();
	
	m_backgroundIndex = g_RNG->RollRandomIntInRange( TEX_BACKGROUND_FOREST_1 , TEX_BACKGROUND_AURORA_1 );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::LevelBounds()
{
	Camera* gameCamera					= m_owner->GetWorldCamera();
	Vec2 cameraMins						= gameCamera->GetOrthoMin().GetXYComponents();
	Vec2 cameraMaxs						= gameCamera->GetOrthoMax().GetXYComponents();

	AABB2 cameraArea					= AABB2( cameraMins , cameraMaxs );
	Vec2 cameraDimensions				= cameraArea.GetDimensions(); 

	float sideWallOffset				= LEVEL_SIDEWALL_PERCENTAGE * 0.5f * cameraDimensions.x;
	float topWallOffset					= 0.125f * 0.75f * cameraDimensions.y;
	float pitOffset						= 0.1f * cameraDimensions.y * 0.5f;
	float additionalHeight				= cameraDimensions.y * 0.05f;
	float additionalWidth				= cameraDimensions.x * 0.05f;
	
	m_backGround						= cameraArea;
	
	m_leftWallCosmeticBounds			= AABB2( cameraMins.x , cameraMins.y - additionalHeight , cameraMins.x + sideWallOffset ,
												 cameraMaxs.y + additionalHeight );
	m_rightWallCosmeticBounds			= AABB2( cameraMaxs.x - sideWallOffset , cameraMins.y - additionalHeight , cameraMaxs.x ,
												 cameraMaxs.y + additionalHeight );
	m_topWallCosmeticBounds				= AABB2( cameraMins.x - additionalWidth , cameraMaxs.y - topWallOffset ,
												 cameraMaxs.x + additionalWidth , cameraMaxs.y + 50.f );
	m_pitCosmeticBounds					= AABB2( cameraMins.x * 10.f , cameraMins.y ,
												 cameraMaxs.x * 10.f, cameraMins.y + pitOffset );

	m_leftWallPhysicalBounds			= m_leftWallCosmeticBounds.GetBoxAtLeft( 0.85f , 0.f );
	m_rightWallPhysicalBounds			= m_rightWallCosmeticBounds.GetBoxAtRight( 0.85f , 0.f );
	m_topWallPhysicalBounds				= m_topWallCosmeticBounds.GetBoxAtTop( 0.15f , 0.f );
	m_pitPhysicalBounds					= m_pitCosmeticBounds.GetBoxAtBottom( 0.9f , 0.f );
	m_leftWallPhysicalBounds.m_mins.x	*= 50.f;
	m_rightWallPhysicalBounds.m_maxs.x	*= 50.f;
	m_topWallPhysicalBounds.m_maxs.y	*= 50.f;
	m_pitPhysicalBounds.m_mins.y		*= 1000.f;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Map::~Map()
{
	g_theParticleSystem2D->DestroyParticleEmitter( m_tileEmitter );
	m_tileEmitter = nullptr;

	g_theParticleSystem2D->DestroyParticleEmitter( m_boundsEmitter );
	m_boundsEmitter = nullptr;

	g_theParticleSystem2D->DestroyParticleEmitter( m_paddleEmitter );
	m_paddleEmitter = nullptr;
	
	m_mapDefinition = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::Update( float deltaSeconds )
{
	for ( int Entitytype = 0; Entitytype < NUM_ENTITY_TYPES; Entitytype++ )
	{
		Entitylist& currentList = m_entityListsByType[ Entitytype ];
		for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ Entitytype ].size(); entityIndex++ )
		{
			if ( currentList[ entityIndex ] )
			{
				currentList[ entityIndex ]->Update( deltaSeconds );
			}

		}
	}
	ResolveCollisions();

	g_theRenderer->UpdateFrameTime( deltaSeconds );
	UpdateSideCosmeticBounds();
	UpdateTopCosmeticBounds();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::Render()
{
	g_theRenderer->BindTexture( m_owner->m_gameTex[ m_backgroundIndex ] );
	g_theRenderer->DrawAABB2( m_backGround , WHITE );

	for ( int Entitytype = 0; Entitytype < NUM_ENTITY_TYPES; Entitytype++ )
	{
		Entitylist& currentList = m_entityListsByType[ Entitytype ];
		for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ Entitytype ].size(); entityIndex++ )
		{
			if ( currentList[ entityIndex ] )
			{
				currentList[ entityIndex ]->Render();
			}
		}
	}
	
	RenderLevelBounds();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::UpdateSideCosmeticBounds()
{
	float positionY = RangeMapFloat( -1.f , 1.f , -50.f , 50.f ,
								  SinDegrees( 100.f * ( float ) GetCurrentTimeSeconds() ) );

	Vec2 leftWallCenter = m_leftWallCosmeticBounds.GetCenter();
	m_leftWallCosmeticBounds.SetCenter( Vec2( leftWallCenter.x , positionY ) );

	Vec2 rightWallCenter = m_rightWallCosmeticBounds.GetCenter();
	m_rightWallCosmeticBounds.SetCenter( Vec2( rightWallCenter.x , positionY ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::UpdateTopCosmeticBounds()
{
	float positionX = RangeMapFloat( -1.f , 1.f , -50.f , 50.f ,
								  SinDegrees( 100.f * ( float ) GetCurrentTimeSeconds() ) );
	Vec2 movement = Vec2( CosDegrees( 300.f * ( float ) GetCurrentTimeSeconds() ) , 0.0f );
	Vec2 topWallCenter = m_topWallCosmeticBounds.GetCenter();
	m_topWallCosmeticBounds.SetCenter( Vec2( positionX , topWallCenter.y ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::RenderLevelBounds()
{
	RenderLevelSideBounds();
	g_theRenderer->BindTexture( m_owner->m_gameTex[ TEX_TOP_WALL_SECTION ] );
	g_theRenderer->DrawAABB2( m_topWallCosmeticBounds , WHITE );
	
	if( m_owner->m_isDebugDraw )
	{
		g_theRenderer->BindTexture( nullptr );
		g_theRenderer->DrawUnfilledAABB2( m_leftWallPhysicalBounds	, CYAN , 5.f );
		g_theRenderer->DrawUnfilledAABB2( m_rightWallPhysicalBounds , CYAN , 5.f );
		g_theRenderer->DrawUnfilledAABB2( m_topWallPhysicalBounds	, CYAN , 5.f );
		g_theRenderer->DrawUnfilledAABB2( m_pitPhysicalBounds		, CYAN , 5.f );

		g_theRenderer->DrawUnfilledAABB2( m_leftWallCosmeticBounds	, MAGENTA );
		g_theRenderer->DrawUnfilledAABB2( m_rightWallCosmeticBounds , MAGENTA );
		g_theRenderer->DrawUnfilledAABB2( m_topWallCosmeticBounds	, MAGENTA );
		g_theRenderer->DrawUnfilledAABB2( m_pitCosmeticBounds		, MAGENTA );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::RenderLevelSideBounds() const
{
	g_theRenderer->BindTexture( m_owner->m_gameTex[ TEX_LEFT_WALL ] );
	g_theRenderer->DrawAABB2( m_leftWallCosmeticBounds , WHITE );
	g_theRenderer->BindTexture( m_owner->m_gameTex[ TEX_RIGHT_WALL ] );
	g_theRenderer->DrawAABB2( m_rightWallCosmeticBounds , WHITE );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::SpawnNewEntity( eEntityType type , const Vec2& position , TileDefinition* tileDef )
{
	Entity* newEntity = nullptr;

	switch ( type )
	{
		case INVALID_ENTITY:
			break;
		case PADDLE:
			newEntity = new Paddle( m_owner , m_owner->GetPaddleHealth() ,
			AABB2( -100.f , -25.f , 100.f , 25.f ) ,
			Vec2( 0.f , m_pitPhysicalBounds.m_maxs.y + 83.f ) );
			break;
		case BALL:
			newEntity = new Ball( m_owner , 1 , 25.f * 1.5f , 25.f , position , BALL_INITIAL_VELOCITY );
			break;
		case TILE:
			newEntity = new Tile( this , IntVec2( position ) , tileDef );
			break;
		default:
			break;
	}

	AddEntityToMap( newEntity );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::AddEntityToMap( Entity* entity )
{
	Entitylist& currentList = m_entityListsByType[ entity->GetEntityType() ];
	AddEntityToList( currentList , entity );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::AddEntityToList( Entitylist& entityList , Entity* entity )
{
	PushBackAtEmptySpace( entityList , entity );	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::UpdateBallPosWhenGrabbed( float newPosX )
{
	Entitylist& ballList = m_entityListsByType[ BALL ];
	for( size_t index = 0 ; index < ballList.size() ; index++ )
	{
		if( ballList[ index ] != nullptr && m_owner->m_isBallLaunchable )
		{
			Ball* ball = ( Ball* ) ballList[ index ];
			ball->m_pos.x = newPosX;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveCollisions()
{
	ResolveBallvBoundsCollisions();
	ResolveBallvPaddleCollisions();
	ResolveBallvTileCollisions();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveBallvBoundsCollisions()
{
	std::vector<Vertex_PCU> particleVerts;
	std::vector<uint> particleIndices;
	Rgba8 color;
	static RandomNumberGenerator rng;
	color.RollRandomColor( rng );
		
	Entitylist& currentList = m_entityListsByType[ BALL ];
	for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ BALL ].size(); entityIndex++ )
	{
		if ( currentList[ entityIndex ] )
		{
			Ball* ball = ( Ball* ) currentList[ entityIndex ];

			if ( DoDiscAndAABBOverlap( ball->m_pos , ball->m_physicsRadius , m_leftWallPhysicalBounds ) )
			{
				Vec2 refPoint = m_leftWallPhysicalBounds.GetNearestPoint( ball->m_pos );
				Vec2 outVert1;
				Vec2 outVert2;
								
				m_leftWallPhysicalBounds.GetClosestEdgeFromRefrerencePoint( refPoint , outVert1 , outVert2 );
				Vec2 edgeNormal = ( outVert2 - outVert1 )/*.GetRotated90Degrees()*/.GetNormalized();
				//ball->m_velocity.Reflect( Vec2::ONE_ZERO );
				ball->m_velocity.Reflect( edgeNormal );
				
				PushDiscOutOfAABB( ball->m_pos , ball->m_physicsRadius , m_leftWallPhysicalBounds );

				uint numParticles = ( uint ) g_RNG->RollRandomIntInRange( 5 , 10 );
				SpawnLeafParticlesOnBallCollision( ball , ball->m_pos , numParticles );
				g_theAudioSystem->PlaySound( m_owner->GetSFX( SFX_LEAVES_RUSTLE ) , false , 0.1f , 0.f , 1.f );
				m_owner->AddScreenShakeIntensity();
			}

			if ( DoDiscAndAABBOverlap( ball->m_pos , ball->m_physicsRadius , m_rightWallPhysicalBounds ) )
			{
				//ball->m_velocity.Reflect( -Vec2::ONE_ZERO );
				Vec2 refPoint = m_rightWallPhysicalBounds.GetNearestPoint( ball->m_pos );
				Vec2 outVert1;
				Vec2 outVert2;

				m_rightWallPhysicalBounds.GetClosestEdgeFromRefrerencePoint( refPoint , outVert1 , outVert2 );
				Vec2 edgeNormal = ( outVert2 - outVert1 )/*.GetRotated90Degrees()*/.GetNormalized();
				
				ball->m_velocity.Reflect( edgeNormal );
				PushDiscOutOfAABB( ball->m_pos , ball->m_physicsRadius , m_rightWallPhysicalBounds );

				uint numParticles = ( uint ) g_RNG->RollRandomIntInRange( 5 , 10 );
				SpawnLeafParticlesOnBallCollision( ball , ball->m_pos , numParticles );
				g_theAudioSystem->PlaySound( m_owner->GetSFX( SFX_LEAVES_RUSTLE ) , false , 0.33f , 0.f , 1.f );
				m_owner->AddScreenShakeIntensity();
			}

			if ( DoDiscAndAABBOverlap( ball->m_pos , ball->m_physicsRadius , m_topWallPhysicalBounds ) )
			{
				ball->m_velocity.Reflect( -Vec2::ZERO_ONE );
				PushDiscOutOfAABB( ball->m_pos , ball->m_physicsRadius , m_topWallPhysicalBounds );

				uint numParticles = ( uint ) g_RNG->RollRandomIntInRange( 5 , 10 );
				SpawnLeafParticlesOnBallCollision( ball , ball->m_pos , numParticles );
				g_theAudioSystem->PlaySound( m_owner->GetSFX( SFX_LEAVES_RUSTLE ) , false , 0.33f , 0.f , 1.f );
				m_owner->AddScreenShakeIntensity();
			}

			if( DoDiscAndAABBOverlap( ball->m_pos , ball->m_physicsRadius , m_pitPhysicalBounds ) )
			{
				Entity* ballEntity = ball;
				delete ballEntity;
				ballEntity = nullptr;
				ball = nullptr;
				currentList[ entityIndex ] = nullptr;

				--m_numAliveBalls;

				if ( m_numAliveBalls <= 0 && m_owner->GetPaddleHealth() > 0 )
				{
					m_owner->m_playerHealth = --m_owner->m_playerHealth;
					m_owner->m_isBallLaunchable = true;
					Paddle* thePaddle = ( Paddle* ) m_entityListsByType[ PADDLE ][ 0 ];
					AABB2	paddleBounds = thePaddle->GetCollider();

					if ( m_owner->GetPaddleHealth() > 0 )
					{
						SpawnNewEntity( BALL , paddleBounds.GetCenter() + Vec2::ZERO_ONE * 37.5f );
						m_numAliveBalls++;
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveBallvPaddleCollisions()
{
	Entitylist& currentList = m_entityListsByType[ BALL ];
	for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ BALL ].size(); entityIndex++ )
	{
		if ( currentList[ entityIndex ] )
		{
			Ball* ball = ( Ball* ) currentList[ entityIndex ];
			
			Paddle* paddle = nullptr;
			if ( m_entityListsByType[ PADDLE ].data() )
			{
				paddle = ( Paddle* ) m_entityListsByType[ PADDLE ][ 0 ];
			}
			
			if ( DoDiscAndAABBOverlap( ball->m_pos , ball->m_physicsRadius , paddle->GetCollider() ) && !ball->m_isInCooldown )
			{
				if ( !m_owner->m_isBallLaunchable )
				{
					Vec2 refPoint = paddle->GetCollider().GetNearestPoint( ball->m_pos );
					
					Vec2 edgeNormal = ( ball->m_pos - refPoint ).GetNormalized();

					float deviationFactor = RangeMapFloat( paddle->GetCollider().m_mins.x , paddle->GetCollider().m_maxs.x ,
					                                  -PADDLE_COLLISION_DEVIATION , PADDLE_COLLISION_DEVIATION , refPoint.x );

					uint numParticles = ( uint ) g_RNG->RollRandomIntInRange( 5 , 10 );

					SpawnFlowerParticlesOnBallCollision( ball , refPoint , numParticles );
					
					ball->m_velocity.Reflect( edgeNormal );
					
					float magnitude = ball->m_velocity.GetLength();
						  magnitude *= 1.01f;
						  magnitude = Clamp( magnitude , BALL_INITIAL_VELOCITY_MAGNITUDE , MAX_BALL_VELOCITY_MAGNITUDE );
					float angleDegrees = ball->m_velocity.GetAngleDegrees() - deviationFactor;
					
					ball->m_velocity = Vec2::MakeFromPolarDegrees( angleDegrees , magnitude );

					ball->m_isInCooldown = true;
				}
				
				PushDiscOutOfAABB( ball->m_pos , ball->m_physicsRadius , paddle->GetCollider() );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveBallvTileCollisions()
{
	Entitylist& currentList = m_entityListsByType[ BALL ];
	for( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ BALL ].size(); entityIndex++ )
	{
		if( nullptr != currentList[ entityIndex ] )
		{
			Ball* ball = ( Ball* ) currentList[ entityIndex ];
			Tile* currentTile = nullptr;

			for( size_t tileIndex = 0 ; tileIndex < m_entityListsByType[ TILE ].size() ; tileIndex++ )
			{
				currentTile = ( Tile* ) m_entityListsByType[ TILE ][ tileIndex ];
				if ( nullptr != currentTile )
				{
					if ( currentTile->TileCollisionResponse( ball ) )
					{
						g_theAudioSystem->PlaySound( m_owner->GetSFX( SFX_GLASS_BREAK_2 ) , false , 0.1f );
						m_owner->AddScreenShakeIntensity();
					}

					if( currentTile->GetHealth() <= 0 )
					{
						delete currentTile;
						m_entityListsByType[ TILE ][ tileIndex ] = nullptr;
						currentTile = nullptr;
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolvePaddlevPowerUpCollisions()
{
	Entitylist& currentList = m_entityListsByType[ POWERUP ];
	for( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ POWERUP ].size(); entityIndex++ )
	{
		if( currentList[ entityIndex ] )
		{
			Paddle* paddle = nullptr;
			if( m_entityListsByType[ PADDLE ].data() )
			{
				paddle = ( Paddle* ) m_entityListsByType[ PADDLE ][ 0 ];
			}

			__debugbreak();
			// TODO:- FIX THIS SHIT BRUH... YOU NEED IT NOW!!
			if ( DoAABBsOverlap(paddle->GetCollider() , AABB2::ZERO_TO_ONE) )
			{
				
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::SpawnLeafParticlesOnBallCollision( Ball* ball , Vec2 refPoint , uint num4XParticles )
{
	for( uint index = 0 ; index < num4XParticles ; index++ )
	{
		int spriteCoordX = g_RNG->RollRandomIntInRange( 0 , 2 );
		int spriteCoordY = g_RNG->RollRandomIntInRange( 0 , 2 );
		
		IntVec2 spriteCoords( spriteCoordX , spriteCoordY );

		float ballVelocityMagnitude = ball->m_velocity.GetLength();

		Vec2 deviation				= g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		float orientationDegrees	= g_RNG->RollRandomFloatInRange( -360.f , 360.f );
		float angularVelocity		= g_RNG->RollRandomFloatInRange( -45.f , 45.f );
		float scale					= g_RNG->RollRandomFloatInRange( 0.f , 1.5f );
		float maxAge				= g_RNG->RollRandomFloatInRange( LEAVES_PARTICLE_MIN_AGE , LEAVES_PARTICLE_MAX_AGE );

		m_boundsEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , LEAVES_PARTICLE_DIMENSIONS ) , refPoint ,
											orientationDegrees , scale , angularVelocity ,
											( ball->m_velocity + deviation ) * LEAVES_PARTICLE_VELOCITY , 0.0f ,
										   maxAge , WHITE , spriteCoords );

		deviation					= g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		orientationDegrees			= g_RNG->RollRandomFloatInRange( -360.f , 360.f );
		angularVelocity				= g_RNG->RollRandomFloatInRange( -45.f , 45.f );
		scale						= g_RNG->RollRandomFloatInRange( 0.f , 1.5f );
		maxAge						= g_RNG->RollRandomFloatInRange( LEAVES_PARTICLE_MIN_AGE , LEAVES_PARTICLE_MAX_AGE );
		
		spriteCoordX				= g_RNG->RollRandomIntInRange( 0 , 2 );
		spriteCoordY				= g_RNG->RollRandomIntInRange( 0 , 2 );

		spriteCoords				= IntVec2( spriteCoordX , spriteCoordY );
		
		m_boundsEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , LEAVES_PARTICLE_DIMENSIONS ) , refPoint ,
										   orientationDegrees , scale , angularVelocity ,
										   ( ball->m_velocity + deviation ) * LEAVES_PARTICLE_VELOCITY , 0.0f ,
										   maxAge , WHITE , spriteCoords );

		deviation					= g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		orientationDegrees			= g_RNG->RollRandomFloatInRange( -360.f , 360.f );
		angularVelocity				= g_RNG->RollRandomFloatInRange( -45.f , 45.f );
		scale						= g_RNG->RollRandomFloatInRange( 0.f , 1.5f );
		maxAge						= g_RNG->RollRandomFloatInRange( LEAVES_PARTICLE_MIN_AGE , LEAVES_PARTICLE_MAX_AGE );

		spriteCoordX				= g_RNG->RollRandomIntInRange( 0 , 2 );
		spriteCoordY				= g_RNG->RollRandomIntInRange( 0 , 2 );

		spriteCoords				= IntVec2( spriteCoordX , spriteCoordY );
		
		m_boundsEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , LEAVES_PARTICLE_DIMENSIONS ) , refPoint ,
										   orientationDegrees , scale , angularVelocity ,
										   ( ball->m_velocity + deviation ) * LEAVES_PARTICLE_VELOCITY , 0.0f ,
										   maxAge , WHITE , spriteCoords );

		deviation					= g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		orientationDegrees			= g_RNG->RollRandomFloatInRange( -360.f , 360.f );
		angularVelocity				= g_RNG->RollRandomFloatInRange( -45.f , 45.f );
		scale						= g_RNG->RollRandomFloatInRange( 0.f , 1.5f );
		maxAge						= g_RNG->RollRandomFloatInRange( LEAVES_PARTICLE_MIN_AGE , LEAVES_PARTICLE_MAX_AGE );
		
		spriteCoordX				= g_RNG->RollRandomIntInRange( 0 , 2 );
		spriteCoordY				= g_RNG->RollRandomIntInRange( 0 , 2 );

		spriteCoords				= IntVec2( spriteCoordX , spriteCoordY );
		
		m_boundsEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , LEAVES_PARTICLE_DIMENSIONS ) , refPoint ,
										   orientationDegrees , scale , angularVelocity ,
										   ( ball->m_velocity + deviation ) * LEAVES_PARTICLE_VELOCITY , 0.0f ,
										   maxAge , WHITE , spriteCoords );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::SpawnFlowerParticlesOnBallCollision( Ball* ball , Vec2 refPoint , uint num4XParticles )
{
	for( uint index = 0 ; index < num4XParticles ; index++ )
	{
		int spriteCoordX			= g_RNG->RollRandomIntInRange( 0 , 1 );
		int spriteCoordY			= g_RNG->RollRandomIntInRange( 0 , 1 );

		IntVec2 spriteCoords( spriteCoordX , spriteCoordY );

		float ballVelocityMagnitude = ball->m_velocity.GetLength();

		Vec2 deviation				= g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		float orientationDegrees	= g_RNG->RollRandomFloatInRange( -360.f , 360.f );
		float angularVelocity		= g_RNG->RollRandomFloatInRange( -45.f , 45.f );
		float scale					= g_RNG->RollRandomFloatInRange( 0.f , 1.5f );
		float maxAge				= g_RNG->RollRandomFloatInRange( FLOWER_PARTICLE_MIN_AGE , FLOWER_PARTICLE_MAX_AGE );
		
		m_paddleEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , FLOWER_PARTICLE_DIMENSIONS ) , refPoint ,
										   orientationDegrees , scale , angularVelocity ,
										   ( ball->m_velocity + deviation ) * FLOWER_PARTICLE_VELOCITY , 0.0f ,
										   maxAge , WHITE , spriteCoords );

		deviation					= g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		orientationDegrees			= g_RNG->RollRandomFloatInRange( -360.f , 360.f );
		angularVelocity				= g_RNG->RollRandomFloatInRange( -45.f , 45.f );
		scale						= g_RNG->RollRandomFloatInRange( 0.f , 1.5f );
		maxAge						= g_RNG->RollRandomFloatInRange( FLOWER_PARTICLE_MIN_AGE , FLOWER_PARTICLE_MAX_AGE );
		
		spriteCoordX				= g_RNG->RollRandomIntInRange( 0 , 1 );
		spriteCoordY				= g_RNG->RollRandomIntInRange( 0 , 1 );

		spriteCoords				= IntVec2( spriteCoordX , spriteCoordY );

		m_paddleEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , FLOWER_PARTICLE_DIMENSIONS ) , refPoint ,
										   orientationDegrees , scale , angularVelocity ,
										   ( ball->m_velocity + deviation ) * FLOWER_PARTICLE_VELOCITY , 0.0f ,
										   maxAge , WHITE , spriteCoords );

		deviation					= g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		orientationDegrees			= g_RNG->RollRandomFloatInRange( -360.f , 360.f );
		angularVelocity				= g_RNG->RollRandomFloatInRange( -45.f , 45.f );
		scale						= g_RNG->RollRandomFloatInRange( 0.f , 1.5f );
		maxAge						= g_RNG->RollRandomFloatInRange( FLOWER_PARTICLE_MIN_AGE , FLOWER_PARTICLE_MAX_AGE );
		
		spriteCoordX				= g_RNG->RollRandomIntInRange( 0 , 1 );
		spriteCoordY				= g_RNG->RollRandomIntInRange( 0 , 1 );

		spriteCoords				= IntVec2( spriteCoordX , spriteCoordY );

		m_paddleEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , FLOWER_PARTICLE_DIMENSIONS ) , refPoint ,
										   orientationDegrees , scale , angularVelocity ,
										   ( ball->m_velocity + deviation ) * FLOWER_PARTICLE_VELOCITY , 0.0f ,
										   maxAge , WHITE , spriteCoords );

		deviation					= g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		orientationDegrees			= g_RNG->RollRandomFloatInRange( -360.f , 360.f );
		angularVelocity				= g_RNG->RollRandomFloatInRange( -45.f , 45.f );
		scale						= g_RNG->RollRandomFloatInRange( 0.f , 1.5f );
		maxAge						= g_RNG->RollRandomFloatInRange( FLOWER_PARTICLE_MIN_AGE , FLOWER_PARTICLE_MAX_AGE );
		
		spriteCoordX				= g_RNG->RollRandomIntInRange( 0 , 1 );
		spriteCoordY				= g_RNG->RollRandomIntInRange( 0 , 1 );

		spriteCoords				= IntVec2( spriteCoordX , spriteCoordY );

		m_paddleEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , FLOWER_PARTICLE_DIMENSIONS ) , refPoint ,
										   orientationDegrees , scale , angularVelocity ,
										   ( ball->m_velocity + deviation ) * FLOWER_PARTICLE_VELOCITY , 0.0f ,
										   maxAge , WHITE , spriteCoords );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------