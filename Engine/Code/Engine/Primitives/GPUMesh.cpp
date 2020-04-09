#include "Engine/Primitives/GPUMesh.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------


void GPUMesh::UpdateVertices( uint vcount , void const* vertexData , uint vertexStride , buffer_attribute_t const* layout )
{
	m_vertices->SetVertexBufferLayout( layout );
	m_vertices->SetVBOStide( vertexStride );
	m_vertices->Update( vertexData , vcount * vertexStride , vertexStride );
	m_vertexCount = vcount;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GPUMesh::UpdateVertices( std::vector<Vertex_PCU> const& vertices )
{
	UpdateVertices( ( uint ) vertices.size() ,
		&vertices[ 0 ] ,
		sizeof( Vertex_PCU ) ,
		Vertex_PCU::LAYOUT );

	m_vertexCount = ( uint ) vertices.size();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GPUMesh::UpdateIndices( uint icount , uint const* indices )
{
	m_indices->Update( icount , indices );

	m_indexCount = icount;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GPUMesh::UpdateIndices( std::vector<uint> const& indices )
{
	UpdateIndices( ( uint ) indices.size() , &indices[ 0 ] );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GPUMesh::GPUMesh( RenderContext* owner )
{
	m_vertices	= new VertexBuffer( owner, MEMORY_HINT_DYNAMIC );
	m_indices	= new IndexBuffer( owner , MEMORY_HINT_DYNAMIC );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int GPUMesh::GetIndexCount() const
{
	return m_indexCount;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int GPUMesh::GetVertexCount() const
{
	return m_vertexCount;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GPUMesh::~GPUMesh()
{
	delete m_vertices;
	m_vertices = nullptr;

	delete m_indices;
	m_indices = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------