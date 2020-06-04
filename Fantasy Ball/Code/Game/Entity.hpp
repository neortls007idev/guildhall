﻿#pragma once

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eEntityType
{
	PADDLE,
	BALL,
	TILE,

	NUM_ENTITY_TYPES,
	INVALID_ENTITY,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Entity
{
public:
	Entity( Game* owner, int health , eEntityType eEntityType );
	~Entity(){};

	//----------------------------------------------------------------------------------------------------------

	virtual void Update( float deltaSeconds );
	virtual void Render() const;
	
	//----------------------------------------------------------------------------------------------------------
	//						GETTERS
	//----------------------------------------------------------------------------------------------------------

	int								GetHealth() const												{ return m_health; }
	eEntityType						GetEntityType() const											{ return m_entityType; }

	//----------------------------------------------------------------------------------------------------------
	//						SETTERS
	//----------------------------------------------------------------------------------------------------------

	void							SetHealth( const int health )									{ m_health = health; }
	void							SetEntityType( const eEntityType entityType )					{ m_entityType = entityType; }
	
public:
	Game*							m_owner;
	int								m_health				=	0;
	eEntityType						m_entityType			=	INVALID_ENTITY;
};

//--------------------------------------------------------------------------------------------------------------------------------------------