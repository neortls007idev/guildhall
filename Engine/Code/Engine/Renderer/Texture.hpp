#pragma once
#include "Engine/Math/IntVec2.hpp"

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
	~Texture();

	TextureView*	GetRenderTargetView();

	unsigned int	 GetTextureID() const;
		     float	 GetAspect() const;
	const    IntVec2 GetDimensions() const;

private:
	RenderContext*		m_owner = nullptr;
	ID3D11Texture2D*	m_handle = nullptr;
	TextureView*		m_renderTargetView = nullptr;
	
	const char*			m_imageFilePath = nullptr;
	unsigned int		m_textureID;
	IntVec2				m_dimensions;

};
