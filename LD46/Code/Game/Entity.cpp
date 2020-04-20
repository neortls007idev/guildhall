#include "Game/Entity.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity::Entity( Game* pointerToGameInstance , Vec2 pos , Vec2 velocity , float orientation )
{
	m_game = pointerToGameInstance;
	m_position = pos;
	m_velocity = velocity;
	m_orientationDegrees = orientation;
}


//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::Update( float deltaSeconds )
{
	Movement( deltaSeconds );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::Movement( float deltaSeconds )
{
	m_position += m_velocity * deltaSeconds;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::WrapAroundScreen( Collider2D* collider )
{

	float ScreenMinX = m_game->m_gameCamera.GetOrthoMin().x;
	float ScreenMaxX = m_game->m_gameCamera.GetOrthoMax().x;
	float ScreenMinY = m_game->m_gameCamera.GetOrthoMin().y;
	float ScreenMaxY = m_game->m_gameCamera.GetOrthoMax().y;
	
	if ( GetType() == PLAYER_ENTITY )
	{
		PolygonCollider2D* polyCollider = ( PolygonCollider2D* ) collider;
		Vec2 leftMostPoint		= *GetLeftMostPointFromPointCloud( &polyCollider->m_polygon.m_points[ 0 ] , ( int ) polyCollider->m_polygon.m_points.size() );
		Vec2 rightMostPoint		= *GetRightMostPointFromPointCloud( &polyCollider->m_polygon.m_points[ 0 ] , ( int ) polyCollider->m_polygon.m_points.size() );
		Vec2 topMostPoint		= *GetTopMostPointFromPointCloud( &polyCollider->m_polygon.m_points[ 0 ] , ( int ) polyCollider->m_polygon.m_points.size() );
		Vec2 bottomMostPoint	= *GetBottomMostPointFromPointCloud( &polyCollider->m_polygon.m_points[ 0 ] , ( int ) polyCollider->m_polygon.m_points.size() );

		if ( rightMostPoint.x < ScreenMinX )
		{
			Vec2 offset =  /*rigidBody->m_worldPosition*/ polyCollider->m_worldPosition - leftMostPoint;
			//rigidBody->SetPosition( Vec2( ScreenMaxX + ( offset.x /*widthOfPolygon * 0.5f */ ) , rbCurrentPos.y ) );
		}

		if ( leftMostPoint.x > ScreenMaxX )
		{
			Vec2 offset = rightMostPoint - polyCollider->m_worldPosition;
			//rigidBody->SetPosition( Vec2( ScreenMinX - ( offset.x /*widthOfPolygon * 0.5f*/ ) , rbCurrentPos.y ) );
		}

		if ( topMostPoint.y > ScreenMaxY )
		{
			Vec2 offset = topMostPoint - polyCollider->m_worldPosition;
			//rigidBody->SetPosition( Vec2( ScreenMinX - ( offset.x /*widthOfPolygon * 0.5f*/ ) , rbCurrentPos.y ) );
		}

		if ( bottomMostPoint.y < ScreenMinY )
		{
			Vec2 offset = bottomMostPoint - polyCollider->m_worldPosition;
			//rigidBody->SetPosition( Vec2( ScreenMinX - ( offset.x /*widthOfPolygon * 0.5f*/ ) , rbCurrentPos.y ) );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::Render() const
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::IsDead()
{
	if ( m_health <= 0 )
	{
		m_isDead = true;
		m_game->AddScreenShakeIntensity( 1.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Entity::IsAlive()
{
	if ( this != nullptr )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------


