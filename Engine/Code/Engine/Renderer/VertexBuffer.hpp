#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class  RenderContext;
struct ID3D11Buffer;

//--------------------------------------------------------------------------------------------------------------------------------------------

class VertexBuffer : public RenderBuffer
{
public:
	VertexBuffer( RenderContext* ctx , eRenderMemoryHint hint);
	~VertexBuffer() {};

	uint GetVBOStride() const { return m_stride; }
	void SetVBOStide( const uint stride );
private:
	uint m_stride = 0;
};
