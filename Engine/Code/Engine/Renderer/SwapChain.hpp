#pragma once

class  RenderContext;
struct IDXGISwapChain;

//--------------------------------------------------------------------------------------------------------------------------------------------

class SwapChain
{
public:
	SwapChain( RenderContext* owner, IDXGISwapChain* handle );
	~SwapChain();

	void Present( int vsync = 0 );

private:
	RenderContext*  m_owner  = nullptr; // creator render context
	IDXGISwapChain* m_handle = nullptr; // D3D11 created pointer, what we use when calling DXD311
};

//--------------------------------------------------------------------------------------------------------------------------------------------