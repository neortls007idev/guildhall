#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
class  RenderContext;
struct ID3D11Texture2D;

class TextureView;
//--------------------------------------------------------------------------------------------------------------------------------------------

class Texture
{

public:
	 Texture(); // DEFAULT CONSTRUCTOR ENSURING THE FILE PATH IS NULL AND NO TEXTURE IS MAPPED TO THE OBJECT
	 Texture( const char* imageFilePath, unsigned int textureID, IntVec2 dimensions );
	 Texture( RenderContext* renderContext , ID3D11Texture2D* handle );
	 Texture( const char* filePath, RenderContext* renderContext , ID3D11Texture2D* handle );
	 //Texture( Rgba8 color , RenderContext* renderContext , ID3D11Texture2D* handle );
	~Texture();

	TextureView*	GetOrCreateRenderTargetView();
	TextureView*	GetOrCreateShaderResourceView();
	TextureView*	GetOrCreateDepthStencilView();

	unsigned int	 GetTextureID() const;
		     float	 GetAspect() const;
	const    IntVec2 GetDimensions() const;
	const   ID3D11Texture2D* GetHandle() const { return m_handle; }
private:
	RenderContext*		m_owner = nullptr;
	ID3D11Texture2D*	m_handle = nullptr;
	//ID3D11Texture1D*
	//ID3D11Texture3D*
	
	TextureView*		m_renderTargetView = nullptr;
	TextureView*		m_shaderResourceView = nullptr;

	const char*			m_imageFilePath = nullptr;
	unsigned int		m_textureID;
	IntVec2				m_dimensions;
	//Rgba8				m_color;
};
