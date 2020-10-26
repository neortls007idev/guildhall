#include "Engine/Renderer/DirectionalLight.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*		g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
DirectionalLight::DirectionalLight( float orthoHeight , float aspectRatio , float nearZ , float farZ , dirLightDataT lightData , float shadowMapEdgeDimension )
{
	m_lightCam = new Camera();
	m_lightCam->SetOrthoView3D( orthoHeight , aspectRatio , nearZ , farZ );
	m_lightCam->SetPitchYawRollRotation( lightData.direction.z , lightData.direction.y , lightData.direction.x );

	m_shadowMap = g_theRenderer->CreateUnPooledRenderTargetOfSize( IntVec2( shadowMapEdgeDimension , shadowMapEdgeDimension ) , "lightDepthTarget" , D3D_DXGI_FORMAT_R32_FLOAT );

	m_lightData = lightData;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

DirectionalLight::~DirectionalLight()
{
	SAFE_RELEASE_POINTER( m_shadowMap );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
