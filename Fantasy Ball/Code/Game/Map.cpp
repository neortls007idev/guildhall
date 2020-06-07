﻿#include "Ball.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/StdExtensions.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game//Game.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Paddle.hpp"

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
/*
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

	RandomNumberGenerator rng;

	for( int mapGenSteps = 0; mapGenSteps < m_mapDefinition->m_generationSteps.size(); mapGenSteps++ )
	{
		FloatRange* chanceToRun = &m_mapDefinition->m_generationSteps[ mapGenSteps ]->m_chanceToRun;
		float chanceToRunStep = chanceToRun->GetRandomInRange( rng );

		if( rng.RollPercentChance( chanceToRunStep ) )
		{
			m_mapDefinition->m_generationSteps[ mapGenSteps ]->RunStep( *this );
		}
	}

	//SpawnWorms( 30 , TileDefinition::s_definitions[ "Stone" ] );
	LevelBounds();
	//InitializeTileVertices();

	//m_player = new Actor( m_theGame , Vec2::ONE , 0.f , ActorDefinition::s_definitions[ "Player" ] );
}
*/
//--------------------------------------------------------------------------------------------------------------------------------------------

void Map::LevelBounds()
{
	Vec2 cameraMins = m_owner->GetWorldCamera()->GetOrthoMin().GetXYComponents();
	Vec2 cameraMaxs = m_owner->GetWorldCamera()->GetOrthoMax().GetXYComponents();

	m_leftWall	= AABB2( cameraMins.x , cameraMins.y , cameraMins.x + 5.f , cameraMaxs.y );
	m_rightWall = AABB2( cameraMaxs.x - 5.f , cameraMins.y , cameraMaxs.x , cameraMaxs.y );
	m_topWall	= AABB2( cameraMins.x , cameraMaxs.y , cameraMaxs.x , cameraMaxs.y + 50.f );
	m_pit		= AABB2( cameraMins.x , cameraMins.y , cameraMaxs.x , cameraMins.y - 50.f );
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
	g_theRenderer->DrawAABB2( m_leftWall	, WHITE );
	g_theRenderer->DrawAABB2( m_rightWall	, WHITE );
	g_theRenderer->DrawAABB2( m_topWall		, WHITE );
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

void Map::SpawnNewEntity( eEntityType type , const Vec2& position )
{
	Entity* newEntity = nullptr;

	switch ( type )
	{
	case INVALID_ENTITY:
							break;
	case PADDLE:
		newEntity = new Paddle( m_owner , m_owner->GetPaddleHealth() ,
				AABB2( -100.f , -25.f , 100.f , 25.f ) ,
		                    Vec2( 0.f , m_pit.m_mins.y + 100.f ) );
							break;
	case BALL:
				newEntity = new Ball( m_owner , 1.f , 25.f , 25.f , Vec2::ZERO , Vec2(5.5,-3.f) );
							break;
	case TILE:
				//newEntity = Paddle( m_owner , m_owner->GetPaddleHealth() , AABB2() )
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

				ball->m_velocity.Reflect( edgeNormal );
				//ball->m_velocity.Reflect( Vec2::ZERO_ONE );
				PushDiscOutOfAABB( ball->m_pos , ball->m_cosmeticRadius , paddle->GetCollider() );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------