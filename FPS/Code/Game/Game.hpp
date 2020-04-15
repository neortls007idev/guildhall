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
	UNLIT,
	UV,
	NORMAL,
	TANGENT,
	BITANGENT,
	SURFACE_NORMAL,
	FRESNEL,
	TRIPLANAR_UNLIT,
	TRIPLANAR_LIT,
	TOTAL,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{

public:

			Game();

			void LoadShaders();
			void LoadTextures();
			void InitializeCameras();
			void intializeGameObjects();

			~Game();

			void Update( float deltaSeconds );
			void DebugDrawUI( float deltaSeconds );
			void UpdateLightPosition( float deltaSeconds );
NOT_IN_USE	void UpdateCamera();
	
			void Render() const;
			void RenderFresnelShader2ndPass() const;
NOT_IN_USE	void RenderUI() const;
			
NOT_IN_USE	void AddScreenShakeIntensity( float deltaShakeIntensity );

NOT_IN_USE	void GarbageCollection();
NOT_IN_USE	void GarbageDeletion();
NOT_IN_USE	void Die();

private:

//--------------------------------------------------------------------------------------------------------------------------------------------
//				DEVCONSOLE COMMANDS
//--------------------------------------------------------------------------------------------------------------------------------------------
			void AddLightDevConsoleCommands( DevConsole* devConsole );
	static	bool ChangeLightColorViaConsole( EventArgs& args );
	static	bool ChangeLightAttenuationViaConsole( EventArgs& args );
	static	bool ChangeAmbientLightColorViaConsole( EventArgs& args );
	static	bool ChangeAmbientLightIntensityViaConsole( EventArgs& args );

//--------------------------------------------------------------------------------------------------------------------------------------------
//				METHODS TO HANDLE USER INPUT
//--------------------------------------------------------------------------------------------------------------------------------------------
			void UpdateFromKeyBoard( float deltaSeconds );

			void UpdateLightsFromKeyBoard( float deltaSeconds );
			void UpdateCurrentShaderFromUserInput();
			void UpdateAmbientLightFromUserInput( float deltaSeconds );
			void UpdateSpecularLightFromUserInput( float deltaSeconds );
			void UpdateLightAttenuationFromUserInput();
			void UpdateLightPositionOnUserInput();

NOT_IN_USE	void CreateDebugObjectsFromUserInput();
NOT_IN_USE	void DebugLineStripDrawModeTest();
			void CameraPositionUpdateOnInput( float deltaSeconds );

private:

	int							m_controllerID			= -1;
	float						m_screenShakeIntensity	= 0.f;

public:

	GPUMesh*					m_cubeMesh;
	GPUMesh*					m_meshSphere;
	GPUMesh*					m_quadMesh;
	Transform					m_cubeMeshTransform;
	Transform					m_sphereMeshTransform;
	Transform					m_quadTransform;
	Texture*					m_meshTex_D											= nullptr;
	Texture*					m_meshTex_N											= nullptr;
	Texture*					m_tileDiffuse										= nullptr;
	Texture*					m_tileNormal										= nullptr;

	uint						m_hCuts												= 32;		// slices
	uint						m_vCuts												= 16;		// stacks

	mutable Camera				m_gameCamera;
	Camera						m_uiCamera;

	Vec3						m_cameraPosition									= Vec3::ZERO;
	Vec3						m_cameraRotation									= Vec3::ZERO;
	
	Shader*						m_lightShaders[ LitShaderTypes::TOTAL ];
	Shader* 					m_currentShader;
	int							m_currentShaderIndex;
	bool						m_isFresnelShaderActive								= false;
	Texture*					m_triplanarShaderTextures[ 6 ];
	
	static shaderLightDataT		m_lights;
	static Rgba8				m_ambientLightColor;
	bool						m_isLightFollowingTheCamera							= false;
	bool						m_isLightAnimated									= false;
	uint						m_currentLightIndex									= 0;
																					
	bool						m_lineStripMode										= false;
	std::vector<Vec3>			m_lineStripPoints;									
	eDebugRenderMode			m_debugRenderMode									= DEBUG_RENDER_ALWAYS;
	
};