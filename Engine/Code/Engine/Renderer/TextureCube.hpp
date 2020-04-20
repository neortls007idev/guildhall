#pragma once

//--------------------------------------------------------------------------------------------------------------------------------------------

class RenderContext;
class TextureView;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eTextureCubeSide : uint
{
	TEXCUBE_RIGHT ,
	TEXCUBE_LEFT ,
	TEXCUBE_TOP ,
	TEXCUBE_BOTTOM ,
	TEXCUBE_FRONT , // -z
	TEXCUBE_BACK ,  // +z

	TEXCUBE_SIDE_COUNT ,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class TextureCube
{
public:

private:
	RenderContext*			m_owner					= nullptr;
	TextureView*			m_renderTargetView		= nullptr;
	TextureView*			m_shaderResourceView	= nullptr;		
	TextureView*			m_depthStencilView		= nullptr;

	const char*				m_imageFilePath			= nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------------------------