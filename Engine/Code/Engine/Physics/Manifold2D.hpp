﻿#pragma once
#include "Engine/Math/Vec2.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

struct Manifold2D
{
public:
	Vec2	m_contactPoint	= Vec2::ZERO;
	Vec2	m_normal		= Vec2::ZERO;
	float	m_overlap		= 0.f;
};

//--------------------------------------------------------------------------------------------------------------------------------------------