#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

struct buffer_attribute_t;

//--------------------------------------------------------------------------------------------------------------------------------------------


// ALSO CALLED AS VERTEX_PCU_TBN
struct VertexLit
{

public:
	VertexLit();
	~VertexLit();
	explicit VertexLit( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords , const Vec3& tangent , const Vec3& bitangent , const Vec3& normal );
	explicit VertexLit( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords , const Vec3& normal );

public:
	Vec3	m_position		= Vec3::ZERO;
	Rgba8	m_color			= WHITE;
	Vec2	m_uvTexCoords	= Vec2::ZERO;
	Vec3	m_tangent		= Vec3::UNIT_VECTOR_ALONG_I_BASIS;
	Vec3	m_biTangent		= Vec3::UNIT_VECTOR_ALONG_J_BASIS;
	Vec3	m_normal		= Vec3::UNIT_VECTOR_ALONG_K_BASIS;
	static buffer_attribute_t const LAYOUT[];

};

//--------------------------------------------------------------------------------------------------------------------------------------------