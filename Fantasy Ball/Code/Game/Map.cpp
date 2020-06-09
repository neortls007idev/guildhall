#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/StdExtensions.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game//Game.hpp"
#include "Game/Ball.hpp"
#include "Game/Map.hpp"

#include "GameCommon.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Paddle.hpp"
#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern AudioSystem*		g_theAudioSystem;

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

	for( int verticalIndex = 0; verticalIndex < m_dimensions.y; verticalIndex++ )
	{
		for( int horizontalIndex = 0; horizontalIndex < m_dimensions.x; horizontalIndex++ )
		{
			//m_tiles.push_back( Tile( IntVec2( horizontalIndex , verticalIndex ) , m_mapDefinition->m_fillTile ) );
		}
	}

	//RandomNumberGenerator rng;

	for( int mapGenSteps = 0; mapGenSteps < m_mapDefinition->m_generationSteps.size(); mapGenSteps++ )
	{
// 		FloatRange* chanceToRun = &m_mapDefinition->m_generationSteps[ mapGenSteps ]->m_chanceToRun;
// 		float chanceToRunStep = chanceToRun->GetRandomInRange( rng );
// 
// 		if( rng.RollPercentChance( chanceToRunStep ) )
// 		{
// 		}
			m_mapDefinition->m_generationSteps[ mapGenSteps ]->RunStep( *this );
	}

	//SpawnWorms( 30 , TileDefinition::s_definitions[ "Stone" ] );
	LevelBounds();
	//InitializeTileVertices();

	//m_player = new Actor( m_theGame , Vec2::ONE , 0.f , ActorDefinition::s_definitions[ "Player" ] );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::LevelBounds()
{
	Camera* gameCamera		= m_owner->GetWorldCamera();
	Vec2 cameraMins			= gameCamera->GetOrthoMin().GetXYComponents();
	Vec2 cameraMaxs			= gameCamera->GetOrthoMax().GetXYComponents();

	//float aspectRatio		= gameCamera->GetAspectRatio();
	AABB2 cameraArea		= AABB2( cameraMins , cameraMaxs );
	Vec2 cameraDimensions	= cameraArea.GetDimensions(); 

	float sideWallOffset	= LEVEL_SIDEWALL_PERCENTAGE * 0.5f * cameraDimensions.x;
	float topWallOffset		= 0.125f * 0.75f * cameraDimensions.y;
	float pitOffset			= 0.1f * cameraDimensions.y * 0.5f;
	
	m_backGround			= cameraArea;
	m_leftWall				= AABB2( cameraMins.x , cameraMins.y , cameraMins.x + sideWallOffset , cameraMaxs.y );
	m_rightWall				= AABB2( cameraMaxs.x - sideWallOffset , cameraMins.y , cameraMaxs.x , cameraMaxs.y );
	m_topWall				= AABB2( cameraMins.x , cameraMaxs.y - topWallOffset , cameraMaxs.x , cameraMaxs.y + 50.f );
	m_pit					= AABB2( cameraMins.x , cameraMins.y , cameraMaxs.x , cameraMins.y - pitOffset );
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
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::Render()
{
	g_theRenderer->BindTexture( m_owner->m_gameTex[ TEX_BACKGROUND_FOREST ] );
	g_theRenderer->DrawAABB2( m_backGround	, WHITE );
	
	g_theRenderer->BindTexture( m_owner->m_gameTex[ TEX_LEFT_WALL ] );
	g_theRenderer->DrawAABB2( m_leftWall	, WHITE );
	g_theRenderer->BindTexture( m_owner->m_gameTex[ TEX_RIGHT_WALL ] );
	g_theRenderer->DrawAABB2( m_rightWall	, WHITE );
	g_theRenderer->BindTexture( m_owner->m_gameTex[ TEX_TOP_WALL_SECTION ] );
	g_theRenderer->DrawAABB2( m_topWall		, WHITE );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawAABB2( m_pit			, WHITE );

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
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::SpawnNewEntity( eEntityType type , const Vec2& position , TileDefinition* tileDef )
{
	Entity* newEntity = nullptr;

	Paddle* x = nullptr;
	float y;
	float z;
	switch ( type )
	{
	case INVALID_ENTITY:
							break;
	case PADDLE:
		newEntity = new Paddle( m_owner , m_owner->GetPaddleHealth() ,
				AABB2( -100.f , -25.f , 100.f , 25.f ) ,
		                    Vec2( 0.f , m_pit.m_mins.y + 83.f ) );
// 							x = (Paddle*)newEntity;
// 							y = x->GetCollider().GetCenter().y;
// 							z = x->GetCollider().GetDimensions().y;
							break;
	case BALL:
				newEntity = new Ball( m_owner , 1 , 25.f , 25.f , Vec2::ZERO , Vec2::MakeFromPolarDegrees(15.f,6.f) );
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

void Map::ResolveCollisions()
{
	ResolveBallvBoundsCollisions();
	ResolveBallvPaddleCollisions();
	ResolveBallvTileCollisions();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::ResolveBallvBoundsCollisions()
{
	Entitylist& currentList = m_entityListsByType[ BALL ];
	for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ BALL ].size(); entityIndex++ )
	{
		if ( currentList[ entityIndex ] )
		{
			Ball* ball = ( Ball* ) currentList[ entityIndex ];

			if ( DoDiscAndAABBOverlap( ball->m_pos , ball->m_cosmeticRadius , m_leftWall ) )
			{
				Vec2 refPoint = m_leftWall.GetNearestPoint( ball->m_pos );
				Vec2 outVert1;
				Vec2 outVert2;
								
				m_leftWall.GetClosestEdgeFromRefrerencePoint( refPoint , outVert1 , outVert2 );
				Vec2 edgeNormal = ( outVert2 - outVert1 )/*.GetRotated90Degrees()*/.GetNormalized();
				//ball->m_velocity.Reflect( Vec2::ONE_ZERO );
				ball->m_velocity.Reflect( edgeNormal );
				
				PushDiscOutOfAABB( ball->m_pos , ball->m_cosmeticRadius , m_leftWall );
			}

			if ( DoDiscAndAABBOverlap( ball->m_pos , ball->m_cosmeticRadius , m_rightWall ) )
			{
				//ball->m_velocity.Reflect( -Vec2::ONE_ZERO );
				Vec2 refPoint = m_rightWall.GetNearestPoint( ball->m_pos );
				Vec2 outVert1;
				Vec2 outVert2;

				m_rightWall.GetClosestEdgeFromRefrerencePoint( refPoint , outVert1 , outVert2 );
				Vec2 edgeNormal = ( outVert2 - outVert1 )/*.GetRotated90Degrees()*/.GetNormalized();
				
				ball->m_velocity.Reflect( edgeNormal );
				PushDiscOutOfAABB( ball->m_pos , ball->m_cosmeticRadius , m_rightWall );
			}

			if ( DoDiscAndAABBOverlap( ball->m_pos , ball->m_cosmeticRadius , m_topWall ) )
			{
				ball->m_velocity.Reflect( -Vec2::ZERO_ONE );
				PushDiscOutOfAABB( ball->m_pos , ball->m_cosmeticRadius , m_topWall );
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
			
			if ( DoDiscAndAABBOverlap( ball->m_pos , ball->m_cosmeticRadius , paddle->GetCollider() ) )
			{
				Vec2 refPoint = paddle->GetCollider().GetNearestPoint( ball->m_pos );
				Vec2 outVert1;
				Vec2 outVert2;

				paddle->GetCollider().GetClosestEdgeFromRefrerencePoint( ball->m_pos , outVert1 , outVert2 );
				Vec2 edgeNormal = ( outVert2 - outVert1 ).GetRotated90Degrees().GetNormalized();

				if ( edgeNormal.x <= -1.f && edgeNormal.x < 0.001f && edgeNormal.y <= 0.01f )
				{
					edgeNormal = -edgeNormal;
					ball->m_velocity.Reflect( edgeNormal );
				}
				else if( edgeNormal.x <= 1.f && edgeNormal.x > 0.001f && edgeNormal.y <= 0.01f )
				{
					edgeNormal = edgeNormal;
					ball->m_velocity.Reflect( edgeNormal );
				}
				else if ( /*edgeNormal.x <= 0.1f && edgeNormal.x >= 0.0f &&*/ edgeNormal.y >= 0.9f )
				{
					ball->m_velocity.Reflect( edgeNormal );
				}
				//ball->m_velocity.Reflect( Vec2::ZERO_ONE );
				PushDiscOutOfAABB( ball->m_pos , ball->m_cosmeticRadius , paddle->GetCollider() );
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
					currentTile->TileCollisionResponse( ball );
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------