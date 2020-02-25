#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

IndexBuffer::IndexBuffer( RenderContext* owner , eRenderBufferUsage usage , eRenderMemoryHint memHint ) : RenderBuffer( owner , usage , memHint )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

IndexBuffer::~IndexBuffer()
{
	DX_SAFE_RELEASE( m_handle );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void IndexBuffer::Update( uint icount , uint const* indices )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void IndexBuffer::Update( std::vector<uint> const& indices )
{
	Update( ( uint ) indices.size() , &indices[ 0 ] );
}

//--------------------------------------------------------------------------------------------------------------------------------------------