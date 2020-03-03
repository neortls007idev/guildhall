#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/TextureView.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

Texture::Texture( const char* imageFilePath , unsigned int textureID , IntVec2 dimensions ) : m_imageFilePath( imageFilePath ) , m_textureID( textureID ) , m_dimensions( dimensions )
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Texture::Texture():m_imageFilePath(nullptr),m_textureID(0)
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Texture::Texture( RenderContext* renderContext , ID3D11Texture2D* handle ) :
																				m_owner( renderContext ) , 
																				m_handle( handle )
{
	D3D11_TEXTURE2D_DESC desc;
	handle->GetDesc( &desc );
	m_dimensions = IntVec2( desc.Width , desc.Height );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Texture::Texture( const char* filePath , RenderContext* renderContext , ID3D11Texture2D* handle ) : Texture( renderContext , handle )
{
	m_imageFilePath = filePath;
// 	m_owner = renderContext;
// 	m_handle = handle;
// 
// 	D3D11_TEXTURE2D_DESC desc;
// 	m_handle->GetDesc( &desc );
// 	m_dimensions.x = desc.Width;
// 	m_dimensions.y = desc.Height;
}
// 
// Texture::Texture( Rgba8 color , RenderContext* renderContext , ID3D11Texture2D* handle ) : Texture( renderContext , handle )
// {
// 	m_color = color;
// }

//--------------------------------------------------------------------------------------------------------------------------------------------

Texture::~Texture()
{
	delete m_depthStencilView;
	m_depthStencilView = nullptr;
	
	delete m_shaderResourceView;
	m_shaderResourceView = nullptr;

	delete m_renderTargetView;
	m_renderTargetView = nullptr;

	DX_SAFE_RELEASE( m_handle );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TextureView* Texture::GetOrCreateRenderTargetView()
{
	if ( m_renderTargetView)
	{
		return m_renderTargetView;
	}

	ID3D11Device* device = m_owner->m_device;
	ID3D11RenderTargetView* rtv = nullptr;
	device->CreateRenderTargetView( m_handle , nullptr , &rtv );

	if ( nullptr != rtv )
	{
		m_renderTargetView = new TextureView();
		m_renderTargetView->m_rtv = rtv;
	}

	return m_renderTargetView;
}

TextureView* Texture::GetOrCreateShaderResourceView()
{
	if ( m_shaderResourceView )
	{
		return m_shaderResourceView;
	}

	ID3D11Device* device = m_owner->m_device;
	ID3D11ShaderResourceView* srv = nullptr;

	device->CreateShaderResourceView( m_handle , nullptr , &srv );
	
	if ( srv )
	{
		m_shaderResourceView = new TextureView();
		m_shaderResourceView->m_srv = srv;
	}

	return m_shaderResourceView;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TextureView* Texture::GetOrCreateDepthStencilView()
{
	if ( m_depthStencilView )
	{
		return m_depthStencilView;
	}

	ID3D11Device* device			= m_owner->m_device;
	ID3D11DepthStencilView* dsv		= nullptr;
	
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	//descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	// Create the depth stencil view	
	device->CreateDepthStencilView( m_handle ,				// Depth stencil texture
									&descDSV ,				// Depth stencil desc
									&dsv );				// [out] Depth stencil view

	if ( dsv )
	{
		m_depthStencilView = new TextureView();
		m_depthStencilView->m_dsv = dsv;
	}

	return m_depthStencilView;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

unsigned int Texture::GetTextureID() const
{
	return m_textureID;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float Texture::GetAspect() const
{
	return static_cast< float >( m_dimensions.x / m_dimensions.y );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const IntVec2 Texture::GetDimensions() const
{
	return m_dimensions;
}

//--------------------------------------------------------------------------------------------------------------------------------------------