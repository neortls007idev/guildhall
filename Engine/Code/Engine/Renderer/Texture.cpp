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

Texture::~Texture()
{
	delete m_renderTargetView;
	m_renderTargetView = nullptr;

	DX_SAFE_RELEASE( m_handle );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

TextureView* Texture::GetRenderTargetView()
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