#include "Engine/Core/VertexMaster.hpp"

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC buffer_attribute_t const	VertexMaster::LAYOUT[] = {
   buffer_attribute_t( "POSITION",		BUFFER_FORMAT_VEC3,      			offsetof( VertexMaster, m_position ) ),
   buffer_attribute_t( "COLOR",			BUFFER_FORMAT_VEC4, 				offsetof( VertexMaster, m_normalizedColor ) ),
   buffer_attribute_t( "TEXCOORD",		BUFFER_FORMAT_VEC2,      			offsetof( VertexMaster, m_uvTexCoords ) ),
   //buffer_attribute_t( "WORLD_POSITION",  BUFFER_FORMAT_VEC3,      			offsetof( VertexMaster, m_position ) ),
   buffer_attribute_t( "NORMAL",			BUFFER_FORMAT_VEC3,      			offsetof( VertexMaster, m_normal ) ),
   buffer_attribute_t() // end - terminator element; 
};

//--------------------------------------------------------------------------------------------------------------------------------------------

VertexMaster::VertexMaster()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

VertexMaster::VertexMaster( const Vec3& position , const Rgba8& tint , const Vec2& uvTexCoords , const Vec3& normal ) :
																														m_position( position ) ,
																														m_color( tint ) ,
																														m_uvTexCoords( uvTexCoords ),
																														m_normal( normal )

{
	m_normalizedColor = m_color.GetAsNormalizedFloat4();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

VertexMaster::VertexMaster( const Vertex_PCU& vert ) :
														m_position( vert.m_position ) ,
														m_color( vert.m_color ) ,
														m_uvTexCoords( vert.m_uvTexCoords )
{
	m_normal = Vec3::UNIT_VECTOR_ALONG_K_BASIS;
	m_normalizedColor = m_color.GetAsNormalizedFloat4();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

VertexMaster::VertexMaster( const Vertex_PCU& vert , const Vec3& vertNormal ) : m_position( vert.m_position ) ,
																				m_color( vert.m_color ) ,
																				m_uvTexCoords( vert.m_uvTexCoords ),
																				m_normal( vertNormal )

{
	m_normalizedColor = m_color.GetAsNormalizedFloat4();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC void VertexMaster::ConvertVertexMasterToVertexPCU( std::vector<Vertex_PCU>& output , std::vector<VertexMaster> const& input )
{
	for ( VertexMaster const& vert : input )
	{
		output.push_back( Vertex_PCU( vert ) );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

VertexMaster::~VertexMaster()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------