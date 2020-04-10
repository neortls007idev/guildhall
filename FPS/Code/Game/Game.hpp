#include "Engine/Core/DebugRenderObject.hpp"
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

enum LitShaderTypes
{
	LIT,
	UV,
	NORMAL,
	//TANGENT,
	//BITANGENT,
	TOTAL,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{

public:

	Game();
	~Game();

	void InitializeCameras();
	void intializeGameObjects();
	
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
	void UpdateLightsFromKeyBoard( float deltaSeconds );
	void CreateDebugObjectsFromUserInput();
	void DebugLineStripDrawModeTest();
	void CameraPositionUpdateOnInput( float deltaSeconds );

private:

	int					m_controllerID			= -1;
	float				m_screenShakeIntensity	= 0.f;

public:

	AABB2				m_normalImage;
	AABB2				m_invertedColorImage;
	GPUMesh*			m_cubeMesh;
	GPUMesh*			m_meshSphere;
	GPUMesh*			m_quadMesh;
	Transform			m_cubeMeshTransform;
	Transform			m_sphereMeshTransform;
	Transform			m_quadTransform;
	Texture*			m_meshTex_D					= nullptr;
	Texture*			m_meshTex_N					= nullptr;
	Texture*			m_worldMapSphere			= nullptr;

	uint				m_hCuts						= 32;		// slices
	uint				m_vCuts						= 16;		// stacks

	mutable Camera		m_gameCamera;
	Camera				m_uiCamera;

	Vec3				m_cameraPosition			= Vec3::ZERO;
	Vec3				m_cameraRotation			= Vec3::ZERO;
	
	Shader*				m_lightShaders[ LitShaderTypes::TOTAL ];
	Shader* 			m_currentShader;
	int					m_currentShaderIndex;
	
	shaderLightDataT	m_lights;
	Rgba8				m_ambientLightColor;
	bool				m_isLightFollowingTheCamera = false;
	uint				m_currentLightIndex			= 0;

	bool				m_lineStripMode				= false;
	std::vector<Vec3>	m_lineStripPoints;
	eDebugRenderMode	m_debugRenderMode		= DEBUG_RENDER_ALWAYS;
	
};