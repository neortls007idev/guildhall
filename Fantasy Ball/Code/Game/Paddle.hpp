﻿#pragma once
#include "Engine/Primitives/AABB2.hpp"
#include "Entity.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Paddle : public Entity
{
public:
	Paddle( Game* owner , int health , AABB2 paddleCollider , Vec2 startPos );
	~Paddle(){};

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;

	void UpdateFromUserInput( float deltaSeconds );

	AABB2 GetCollider() const																	{ return m_paddleCollider; }
	
private:
	AABB2		m_paddleCollider;
	Vec2		m_position			= Vec2::ZERO; 
};

//--------------------------------------------------------------------------------------------------------------------------------------------