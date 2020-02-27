#include "Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC buffer_attribute_t const	Vertex_PCU::LAYOUT[] = {
   buffer_attribute_t( "POSITION",  BUFFER_FORMAT_VEC3,      		offsetof( Vertex_PCU, m_position ) ),
   buffer_attribute_t( "COLOR",     BUFFER_FORMAT_R8G8B8A8_UNORM, 	offsetof( Vertex_PCU, m_color ) ),
   buffer_attribute_t( "TEXCOORD",  BUFFER_FORMAT_VEC2,      		offsetof( Vertex_PCU, m_uvTexCoords ) ),
   buffer_attribute_t() // end - terminator element; 
};

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

