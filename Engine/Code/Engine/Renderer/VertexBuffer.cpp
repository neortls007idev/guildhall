#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

VertexBuffer::VertexBuffer( RenderContext* ctx , eRenderMemoryHint hint ) : RenderBuffer( ctx , VERTEX_BUFFER_BIT , hint )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void VertexBuffer::SetVBOStide( const uint stride )
{
	m_stride = stride;
}

//--------------------------------------------------------------------------------------------------------------------------------------------