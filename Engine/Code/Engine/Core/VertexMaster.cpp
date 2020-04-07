#include "Engine/Core/VertexMaster.hpp"

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC buffer_attribute_t const	VertexMaster::LAYOUT[] = {
   buffer_attribute_t( "POSITION",  BUFFER_FORMAT_VEC3,      		offsetof( VertexMaster, m_position ) ),
   buffer_attribute_t( "COLOR",     BUFFER_FORMAT_R8G8B8A8_UNORM, 	offsetof( VertexMaster, m_color ) ),
   buffer_attribute_t( "TEXCOORD",  BUFFER_FORMAT_VEC2,      		offsetof( VertexMaster, m_uvTexCoords ) ),
   buffer_attribute_t( "NORMAL",	  BUFFER_FORMAT_VEC3,      		offsetof( VertexMaster, m_normal ) ),
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