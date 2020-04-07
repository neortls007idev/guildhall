#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/VertexLit.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC buffer_attribute_t const	VertexLit::LAYOUT[] = {
   buffer_attribute_t( "POSITION",  BUFFER_FORMAT_VEC3,      		offsetof( VertexLit, m_position ) ),
   buffer_attribute_t( "COLOR",     BUFFER_FORMAT_R8G8B8A8_UNORM, 	offsetof( VertexLit, m_color ) ),
   buffer_attribute_t( "TEXCOORD",  BUFFER_FORMAT_VEC2,      		offsetof( VertexLit, m_uvTexCoords ) ),
   buffer_attribute_t( "NORMAL",	  BUFFER_FORMAT_VEC3,      		offsetof( VertexLit, m_normal ) ),
   buffer_attribute_t() // end - terminator element; 
};

//--------------------------------------------------------------------------------------------------------------------------------------------

VertexLit::VertexLit()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

VertexLit::VertexLit( const Vec3& position , const Rgba8& tint , const Vec2& uvTexCoords , const Vec3& tangent , const Vec3& bitangent , const Vec3& normal ) :
	m_position( position ) ,
	m_color( tint ) ,
	m_uvTexCoords( uvTexCoords ) ,
	//m_tangent( tangent ),
	//m_biTangent( bitangent ),
	m_normal( normal )
{
	UNUSED( tangent );
	UNUSED( bitangent );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

VertexLit::VertexLit( const Vec3& position , const Rgba8& tint , const Vec2& uvTexCoords , const Vec3& normal ) :
																													m_position( position ) ,
																													m_color( tint ) ,
																													m_uvTexCoords( uvTexCoords ) ,
																													m_normal( normal )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

VertexLit::~VertexLit()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------