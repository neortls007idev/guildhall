﻿#include "Engine/Physics/PhysicsMaterial.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

void PhysicsMaterial::SetBounciness( float const newBounciness )
{
	m_bounciness = newBounciness;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void PhysicsMaterial::SetFriciton( float const newfriciton )
{
	m_friction = newfriciton;
}

//--------------------------------------------------------------------------------------------------------------------------------------------