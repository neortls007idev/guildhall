#include "Engine/Renderer/DepthStencilTargetView.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SwapChain.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

DepthStencilTargetView::DepthStencilTargetView( RenderContext* renderContext )
{
	m_owner = renderContext;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

DepthStencilTargetView::~DepthStencilTargetView()
{
	DX_SAFE_RELEASE( m_source );
	DX_SAFE_RELEASE( m_rtv );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DepthStencilTargetView::CreateDepthStencilResource()
{
	ID3D11Texture2D* pDepthStencil = NULL;
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = m_owner->m_swapChain->GetBackBuffer()->GetDimensions().x;
	descDepth.Height = m_owner->m_swapChain->GetBackBuffer()->GetDimensions().y;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_IMMUTABLE;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	HRESULT result = m_owner->m_device->CreateTexture2D( &descDepth , NULL , &pDepthStencil );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DepthStencilTargetView::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	ID3D11DepthStencilState* pDSState;
	m_owner->m_device->CreateDepthStencilState( &dsDesc , &pDSState );

	m_owner->m_context->OMSetDepthStencilState( pDSState , 1 );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DepthStencilTargetView::CreateDepthStencilView()
{
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	// Create the depth stencil view
	HRESULT result = m_owner->m_device->CreateDepthStencilView( m_source , // Depth stencil texture
																&descDSV , // Depth stencil desc
																&m_rtv );  // [out] Depth stencil view
}
