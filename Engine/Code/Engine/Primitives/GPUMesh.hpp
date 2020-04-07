#pragma once

#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class GPUMesh // A04
{
public:
	GPUMesh( RenderContext* owner );
	~GPUMesh();
	void UpdateVertices( uint vcount , void const* vertexData , uint vertexStride , buffer_attribute_t const* layout );
	void UpdateVertices( std::vector<Vertex_PCU> const& vertices );
	void UpdateIndices( uint icount , uint const* indices );

	int GetIndexCount() const;
	int GetVertexCount() const;
	VertexBuffer* GetVertexBuffer() const																		{ return m_vertices; }
	IndexBuffer*  GetIndexBuffer() const																		{ return m_indices; }

	void AddVertices( uint vcount , void const* verts );
	void AddIndices( uint icount , uint const* indices );

	void UpdateIndices( std::vector<uint> const& indices );
		
	// helper template
	template <typename VERTEX_TYPE>
	void UpdateVertices( uint vcount , VERTEX_TYPE const* vertices )
	{
		m_buffer_attribute = &VERTEX_TYPE::LAYOUT;
		UpdateVertices( vcount , vertices , sizeof( VERTEX_TYPE ) , VERTEX_TYPE::LAYOUT );
	}

public:
	VertexBuffer*		m_vertices		= nullptr;
	IndexBuffer*		m_indices		= nullptr;
	uint				m_vertexCount	= 0;
	uint				m_indexCount	= 0;
	buffer_attribute_t const*	m_buffer_attribute;
};

//--------------------------------------------------------------------------------------------------------------------------------------------
