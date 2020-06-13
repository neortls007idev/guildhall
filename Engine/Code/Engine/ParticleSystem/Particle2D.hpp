#pragma once
#include <vector>

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Vec2.hpp"

class GPUMesh;

//--------------------------------------------------------------------------------------------------------------------------------------------

struct Particle2D
{
public:
	Particle2D();
	Particle2D( Vec2 position , float orientation , Vec2 velocity , float age , float maxAge , Rgba8 color );
	~Particle2D();
	
	void Update( float deltaSeconds );
	void testRender();
	
public:
	Vec2		m_position;
	float		m_orientation;
	Vec2		m_scale;
	Vec2		m_velocity;
	float		m_age;
	float		m_maxAge;
	Rgba8		m_color;
};

//--------------------------------------------------------------------------------------------------------------------------------------------