#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

SwapChain::SwapChain( RenderContext* owner , IDXGISwapChain* handle ) : 
																		m_owner( owner ) ,
																		m_handle( handle )
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------

SwapChain::~SwapChain()
{
	DX_SAFE_RELEASE( m_handle );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void SwapChain::Present( int vsync )
{
	m_handle->Present( vsync , 0 );
}

//--------------------------------------------------------------------------------------------------------------------------------------------