#include "Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Vertex_PCU::Vertex_PCU()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vertex_PCU::Vertex_PCU( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords )
{
	m_position.x = position.x;
	m_position.y = position.y;
	m_position.z = position.z;

	m_color.r = tint.r;
	m_color.g = tint.g;
	m_color.b = tint.b;
	m_color.a = tint.a;

	m_uvTexCoords.x = uvTexCoords.x;
	m_uvTexCoords.y = uvTexCoords.y;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vertex_PCU::~Vertex_PCU()
{

}

