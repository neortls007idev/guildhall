#include "Engine/Core/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Entity.hpp"
#include "Game/EntityDef.hpp"
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity::Entity( EntityDef const& entityDef , Map* map )
{
	m_owner						= map;
	m_canBePushedByWalls		= entityDef.m_pushedByWalls;
	m_canBePushedByEntities		= entityDef.m_pushedByEntities;
	m_canPushEntities			= entityDef.m_pushesEntities;
								
	m_physicsRadius				= entityDef.m_physicsRadius;
	m_height					= entityDef.m_height;
	m_eyeHeight					= entityDef.m_eyeHeight;
	m_spriteSize				= entityDef.m_spriteSize;
	m_mass						= entityDef.m_mass;
	m_damage					= entityDef.m_damage;
	m_speed						= entityDef.m_speed;
	m_billboardType				= entityDef.m_billboard;
	m_entityDef					= const_cast< EntityDef* >( &entityDef );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity::Entity()
{
	m_canBePushedByWalls				= true;
	m_canBePushedByEntities				= true;
	m_canPushEntities					= true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity::~Entity()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::ConstructEntityBillboard()
{
	Transform transform;
	transform.SetPosition( m_pos );
	transform.SetRotation( 0.f , m_startYawDegrees , 0.f );
	Vec2 halfDimensions = m_spriteSize * 0.5f;
	Texture* tex = const_cast< Texture* >( &( m_entityDef->m_spriteSheet->GetTexture() ) );

	m_billBoard = new BillBoard( AABB2( -halfDimensions.x , 0.f , halfDimensions.x ,m_spriteSize.y ) , transform.GetAsMatrix() , m_pos ,
	                             Vec2::ZERO , Vec2::ONE , tex , m_billboardType );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::Update( float deltaSeconds )
{
	if( m_billBoard != nullptr )
	{
		m_billBoard->m_pos = m_pos;
		m_billBoard->Update();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::Render() const
{
	if ( m_billBoard != nullptr )
	{
		m_billBoard->Render();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Entity::IsPlayer()
{
	if ( m_entityType == PLAYER )
	{
		return true;
	}
	
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Entity::IsProjectile()
{
	if( m_entityType == PROJECTILE )
	{
		return true;
	}
	
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Entity::IsNPC()
{
	if( m_entityType == NPC )
	{
		return true;
	}
	
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------