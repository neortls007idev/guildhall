#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/Level.hpp"

#include "Ball.hpp"
#include "Engine/Core/StdExtensions.hpp"
#include "Game/Paddle.hpp"
#include "Game//Game.hpp"
#include "Engine/Math/MathUtils.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern AudioSystem*		g_theAudioSystem;

//--------------------------------------------------------------------------------------------------------------------------------------------

Level::Level( Game* owner ) :
								m_owner( owner ) 
{
	Vec2 cameraMins = owner->GetWorldCamera()->GetOrthoMin().GetXYComponents();
	Vec2 cameraMaxs = owner->GetWorldCamera()->GetOrthoMax().GetXYComponents();

	m_leftWall		= AABB2( cameraMins.x        , cameraMins.y , cameraMins.x + 5.f , cameraMaxs.y );
	m_rightWall		= AABB2( cameraMaxs.x - 5.f , cameraMins.y , cameraMaxs.x       , cameraMaxs.y );
	m_topWall		= AABB2( cameraMins.x		 , cameraMaxs.y , cameraMaxs.x		  , cameraMaxs.y + 50.f );
	m_pit			= AABB2( cameraMins.x		 , cameraMins.y , cameraMaxs.x		  , cameraMins.y - 50.f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Level::Update( float deltaSeconds )
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

void Level::Render()
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

void Level::SpawnNewEntity( eEntityType type , const Vec2& position )
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

void Level::AddEntityToMap( Entity* entity )
{
	Entitylist& currentList = m_entityListsByType[ entity->GetEntityType() ];
	AddEntityToList( currentList , entity );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Level::AddEntityToList( Entitylist& entityList , Entity* entity )
{
	PushBackAtEmptySpace( entityList , entity );	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Level::ResolveCollisions()
{
	ResolveBallvBoundsCollisions();
	ResolveBallvPaddleCollisions();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Level::ResolveBallvBoundsCollisions()
{
	Entitylist& currentList = m_entityListsByType[ BALL ];
	for ( int entityIndex = 0; entityIndex < ( int ) m_entityListsByType[ BALL ].size(); entityIndex++ )
	{
		if ( currentList[ entityIndex ] )
		{
			Ball* ball = ( Ball* ) currentList[ entityIndex ];

			if ( DoDiscAndAABBOverlap( ball->m_pos , ball->m_cosmeticRadius , m_leftWall ) )
			{
				ball->m_velocity.Reflect( Vec2::ONE_ZERO );
				PushDiscOutOfAABB( ball->m_pos , ball->m_cosmeticRadius , m_leftWall );
			}

			if ( DoDiscAndAABBOverlap( ball->m_pos , ball->m_cosmeticRadius , m_rightWall ) )
			{
				ball->m_velocity.Reflect( -Vec2::ONE_ZERO );
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

void Level::ResolveBallvPaddleCollisions()
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
				ball->m_velocity.Reflect( Vec2::ZERO_ONE );
				PushDiscOutOfAABB( ball->m_pos , ball->m_cosmeticRadius , paddle->GetCollider() );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------