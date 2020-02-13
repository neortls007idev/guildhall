#pragma once

//--------------------------------------------------------------------------------------------------------------------------------------------

struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Resource;

class  Texture;

//--------------------------------------------------------------------------------------------------------------------------------------------

class TextureView
{
public:
	TextureView();
	~TextureView();
	ID3D11RenderTargetView*		 GetRTVHandle() const { return m_rtv;  }
	ID3D11ShaderResourceView*	 GetSRVHandle() const { return m_srv; }

public:
	Texture* m_owner; 

      union {
         ID3D11Resource*			m_handle;			// A01
         ID3D11ShaderResourceView*	m_srv;				// A03 - what is bound to a shader stage for reading
         ID3D11RenderTargetView*	m_rtv;				// A01 - alias as an rtv
      }; 

private:

};