#pragma once
#include "Engine/Math/Vec2.hpp"

class	RenderContext;
struct	ID3D11Resource;
struct	ID3D11DepthStencilView;

//--------------------------------------------------------------------------------------------------------------------------------------------

class DepthStencilTargetView                           
{
public: // external use
	DepthStencilTargetView( RenderContext* renderContext );                          
	~DepthStencilTargetView();                         

	void CreateDepthStencilResource();
	void CreateDepthStencilState();
	void CreateDepthStencilView();
	
	float GetWidth() const;                            
	float GetHeight() const;                           

public: // a renderable view of a texture // private
	RenderContext*			m_owner		= nullptr;
	ID3D11Resource*			m_source	= nullptr;        
	ID3D11DepthStencilView* m_rtv		= nullptr;
	Vec2					m_size		= Vec2::ZERO;     

};


//--------------------------------------------------------------------------------------------------------------------------------------------