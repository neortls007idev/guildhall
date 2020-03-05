#include "Engine/Math/Vec3.hpp"
#include "Engine/Primitives/AABB2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Transform.hpp"
#include "Game/GameCommon.hpp"
#pragma once

//--------------------------------------------------------------------------------------------------------------------------------------------

class Shader;
class Texture;
class GPUMesh;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{

public:

	Game();
	~Game();
	void Update( float deltaSeconds );


	void Render() const;
	void UpdateCamera();
	void RenderUI() const;
	
	void AddScreenShakeIntensity( float deltaShakeIntensity );

	void GarbageCollection();
	void GarbageDeletion();
	void Die();

private:

	void UpdateFromKeyBoard( float deltaSeconds );
	void CameraPositionUpdateOnInput( float deltaSeconds );

private:

	int			m_controllerID			= -1;
	float		m_screenShakeIntensity	= 0.f;

public:

	AABB2		m_normalImage;
	AABB2		m_invertedColorImage;
	GPUMesh*	m_meshCube;
	GPUMesh*	m_meshSphere;
	Transform	m_cubeTransform;
	Texture*	m_imageTex				= nullptr;
	Texture*	m_worldMapSphere		= nullptr;

	uint		m_hCuts					= 64;		// slices
	uint		m_vCuts					= 32;		// stacks

	Camera		m_gameCamera;
	Camera		m_uiCamera;

	Rgba8		m_color;
	Vec3		m_cameraPosition = Vec3::ZERO;
	Vec3		m_cameraRotation = Vec3::ZERO;
	Shader* 	m_invertColorShader;
};