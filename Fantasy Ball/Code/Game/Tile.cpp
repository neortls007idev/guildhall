#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/Ball.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/Tile.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/ParticleSystem/ParticleEmitter2D.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*				g_theRenderer;
extern AudioSystem*					g_theAudioSystem;
extern Game*						g_theGame;

//--------------------------------------------------------------------------------------------------------------------------------------------

Tile::Tile( Map* owner , IntVec2 tileCoordinates , TileDefinition* tileType ) :
																								Entity( owner->m_owner , 0 , eEntityType::TILE ) ,
																								m_tileDef( tileType ) ,
																								m_tileCoords( tileCoordinates ) ,
																								m_ownerMap( owner )
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
	const Texture* tex = &g_theGame->m_gameSS[ SS_BRICKS ]->GetTexture();
	g_theRenderer->BindTexture( tex );
	g_theRenderer->DrawAABB2( tile , WHITE , m_tileDef->m_spriteUVs.m_mins , m_tileDef->m_spriteUVs.m_maxs );
	g_theRenderer->BindTexture( nullptr );

	if ( m_owner->m_isDebugDraw  )
	{
		g_theRenderer->DrawUnfilledAABB2( tile , WHITE , 5.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int Tile::GetTileIndex()
{
	IntVec2 mapDimensions = m_ownerMap->m_dimensions;

	return m_tileCoords.y * mapDimensions.x + m_tileCoords.x; 
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Tile::TileCollisionResponse( Ball* ball )
{
	if( DoDiscAndAABBOverlap( ball->m_pos , ball->m_cosmeticRadius , GetCollider() ) )
	{
		Vec2 refPoint = GetCollider().GetNearestPoint( ball->m_pos );
		
		Vec2 edgeNormal = ( ball->m_pos - refPoint ).GetNormalized();

		float ballVelocityMagnitude = ball->m_velocity.GetLength();

		Vec2 deviation = g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		
		m_ownerMap->m_testEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , particleDimensions ) , refPoint , 0.f ,
													 ( ball->m_velocity + deviation ) * PARTICLE_VELOCITY , 0.0f ,
		                                             2.0f , WHITE , m_tileDef->m_VFXSpriteCoords );

		deviation = g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;

		m_ownerMap->m_testEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , particleDimensions ) , refPoint , 0.f ,
													 ( ball->m_velocity.GetRotated90Degrees() + deviation ) *
		                                             PARTICLE_VELOCITY , 0.0f , 2.0f , WHITE ,
		                                             m_tileDef->m_VFXSpriteCoords );

		deviation = g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;

		m_ownerMap->m_testEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , particleDimensions ) , refPoint , 0.f ,
													 ( ball->m_velocity.GetRotatedMinus90Degrees() + deviation ) *
		                                             PARTICLE_VELOCITY , 0.0f , 2.0f , WHITE ,
		                                             m_tileDef->m_VFXSpriteCoords );

		deviation = g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		
		m_ownerMap->m_testEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , particleDimensions ) , refPoint , 0.f ,
													 ( ball->m_velocity + deviation ) * -PARTICLE_VELOCITY , 0.0f ,
		                                             2.0f , WHITE , m_tileDef->m_VFXSpriteCoords );

		deviation = g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		
		ball->m_velocity.Reflect( edgeNormal );


		deviation = g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		
		m_ownerMap->m_testEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , particleDimensions ) , refPoint , 0.f ,
													 ( ball->m_velocity + deviation ) * PARTICLE_VELOCITY , 0.0f ,
		                                             2.0f , WHITE , m_tileDef->m_VFXSpriteCoords );

		deviation = g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		
		m_ownerMap->m_testEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , particleDimensions ) , refPoint , 0.f ,
													 ( ball->m_velocity.GetRotated90Degrees() + deviation ) *
		                                             PARTICLE_VELOCITY , 0.0f , 2.0f , WHITE ,
		                                             m_tileDef->m_VFXSpriteCoords );

		deviation = g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		
		m_ownerMap->m_testEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , particleDimensions ) , refPoint , 0.f ,
													 ( ball->m_velocity.GetRotatedMinus90Degrees() + deviation ) *
		                                             PARTICLE_VELOCITY , 0.0f , 2.0f , WHITE ,
		                                             m_tileDef->m_VFXSpriteCoords );

		deviation = g_RNG->RollRandomDirection2D() * ballVelocityMagnitude;
		
		m_ownerMap->m_testEmitter->SpawnNewParticle( AABB2( Vec2::ZERO , particleDimensions ) , refPoint , 0.f ,
													 ( ball->m_velocity + deviation ) * -PARTICLE_VELOCITY , 0.0f ,
		                                             2.0f , WHITE , m_tileDef->m_VFXSpriteCoords );
		
		PushDiscOutOfAABB( ball->m_pos , ball->m_cosmeticRadius , GetCollider() );

		--m_health;
		ball->UpdateCurrentTexture( m_tileColor );
		
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
