#pragma once
#include <vector>

//--------------------------------------------------------------------------------------------------------------------------------------------

class Entity
{
public:
	float	m_mass						= 1.f;
	bool	m_canBePushedByWalls		= false;
	bool	m_canBePushedByEntities		= false;
	bool	m_canPushEntities			= false;
};

//--------------------------------------------------------------------------------------------------------------------------------------------