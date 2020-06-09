#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/Ball.hpp"
#include "Game/Map.hpp"
#include "Game/Tile.hpp"

#include "GameCommon.hpp"
#include "Game/TileDefinition.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*				g_theRenderer;
extern AudioSystem*					g_theAudioSystem;

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile::Tile( Map* owner , IntVec2 tileCoordinates , TileDefinition* tileType ) :
																								Entity( owner->m_owner , 0 , eEntityType::TILE ) ,
																								m_tileDef( tileType ) ,
																								m_tileCoords( tileCoordinates )
{
	SetHealth( m_tileDef->m_health );
	m_tileColor = m_tileDef->m_tileColor;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tile::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Tile::Render() const
{
	AABB2 tile = GetCollider();
	g_theRenderer->DrawUnfilledAABB2( tile , WHITE , 5.f );
	g_theRenderer->DrawAABB2( tile , m_tileColor );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int Tile::GetTileIndex()
{
	IntVec2 mapDimensions = m_owner->m_dimensions;

	return m_tileCoords.y * mapDimensions.x + m_tileCoords.x; 
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Tile::TileCollisionResponse( Ball* ball )
{
	if( DoDiscAndAABBOverlap( ball->m_pos , ball->m_cosmeticRadius , GetCollider() ) )
	{
		Vec2 refPoint = GetCollider().GetNearestPoint( ball->m_pos );
		Vec2 outVert1;
		Vec2 outVert2;

		GetCollider().GetClosestEdgeFromRefrerencePoint( ball->m_pos , outVert1 , outVert2 );
		Vec2 edgeNormal = ( outVert2 - outVert1 ).GetRotated90Degrees().GetNormalized();

		if( edgeNormal.x <= -1.f && edgeNormal.x < 0.001f && edgeNormal.y <= 0.01f )
		{
			edgeNormal = -edgeNormal;
			ball->m_velocity.Reflect( edgeNormal );
		}
		else if( edgeNormal.x <= 1.f && edgeNormal.x > 0.001f && edgeNormal.y <= 0.01f )
		{
			edgeNormal = edgeNormal;
			ball->m_velocity.Reflect( edgeNormal );
		}
		else if( /*edgeNormal.x <= 0.1f && edgeNormal.x >= 0.0f &&*/ edgeNormal.y >= 0.9f )
		{
			ball->m_velocity.Reflect( edgeNormal );
		}
		else
		{
			ball->m_velocity.Reflect( -edgeNormal );
		}
		
		PushDiscOutOfAABB( ball->m_pos , ball->m_cosmeticRadius , GetCollider() );

		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

AABB2 Tile::GetCollider() const
{
	//Vec2 offset(halfTileDimensions.x*2.f,halfTileDimensions.y);
	//AABB2 collider = AABB2( Vec2::ZERO , Vec2( TILE_LENGTH , TILE_HEIGHT ) );
	AABB2 collider = AABB2( -halfTileDimensions , halfTileDimensions );
	collider.SetCenter( Vec2( m_tileCoords ) );
	return collider;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
