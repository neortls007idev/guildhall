#include "Engine/Primitives/GPUMesh.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

void GPUMesh::UpdateVertices( uint vcount , void const* vertexData , uint vertexStride , buffer_attribute_t const* layout )
{
	m_vertices->Update( vertexData , sizeof( layout ) * vcount , sizeof( layout ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void GPUMesh::UpdateIndices( uint icount , uint const* indices )
{
	m_indices->Update( icount , indices );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

GPUMesh::GPUMesh( RenderContext* ctx )
{
	m_vertices	= new VertexBuffer( ctx, MEMORY_HINT_DYNAMIC );
	m_indices	= new IndexBuffer( ctx , MEMORY_HINT_DYNAMIC );
}

//--------------------------------------------------------------------------------------------------------------------------------------------