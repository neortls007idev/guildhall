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

private:

};
