#pragma once
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/LightStructs.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
	
class DirectionalLight : public Light
{
public:
	DirectionalLight( float orthoHeight , float aspectRatio , float nearZ , float farZ , dirLightDataT lightData , float shadowMapEdgeDimension );
	~DirectionalLight();

	void UpdateLightData();

public:
	Texture*			m_shadowMap = nullptr;
	dirLightDataT		m_lightData;

private:

};

//--------------------------------------------------------------------------------------------------------------------------------------------
	