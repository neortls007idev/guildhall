﻿#include "Game/Entity.hpp"
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity::Entity( Game* owner , int health , eEntityType eEntityType ) :
																		m_owner( owner ),
																		m_health( health ) ,
																		m_entityType( eEntityType )
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Entity::~Entity()
{
	m_owner = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Entity::Render() const
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------
