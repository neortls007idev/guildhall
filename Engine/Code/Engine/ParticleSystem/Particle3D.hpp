#pragma once
#include <vector>

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Primitives/AABB2.hpp"

class GPUMesh;

//--------------------------------------------------------------------------------------------------------------------------------------------

struct Particle3D
{
public:
	//Particle3D();
	Particle3D( Vec3 position , Vec3 velocity , float age , float maxAge , Rgba8 color );

	Particle3D( AABB2 cosmeticBounds , Vec3 position , Vec3 velocity , float age , float maxAge , Rgba8 color );

	Particle3D ( AABB2 cosmeticBounds , Vec3 position , float scale ,
	             Vec3 velocity , float age , float maxAge , Rgba8 color );

	Particle3D ( AABB2 cosmeticBounds , Vec2 minUVs , Vec2 maxUVs , Vec3 position , float scale ,
	             Vec3 velocity , float age , float maxAge , Rgba8 color );

	Particle3D ( AABB2 cosmeticBounds , Vec2 minUVs , Vec2 maxUVs , Vec3 position , Vec3 velocity ,
 	             float age , float maxAge , Rgba8 color );
	~Particle3D();
	
	void Update( float deltaSeconds );
	void Move( float deltaSeconds );

	void UpdateTransformation();
	
public:
	Vec3		m_position;
	Vec3		m_velocity;
	float		m_age;
	float		m_maxAge;
	Rgba8		m_color;
 	AABB2		m_cosmeticBounds;
 	Vec2		m_minsUVs			= Vec2::ZERO;
 	Vec2		m_maxsUVs			= Vec2::ONE;
};

//--------------------------------------------------------------------------------------------------------------------------------------------