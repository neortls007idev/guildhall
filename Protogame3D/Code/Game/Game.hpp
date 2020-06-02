#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DebugRenderObject.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
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

			void LoadShaders();
			void LoadTextures();
			void LoadAudio();
			void InitializeCameras();
			void IntializeGameObjects();
			
			~Game();

			void Update( float deltaSeconds );
			void DebugDrawUI( float deltaSeconds );
				
			void Render() const;
private:
//--------------------------------------------------------------------------------------------------------------------------------------------
//				METHODS TO HANDLE USER INPUT
//--------------------------------------------------------------------------------------------------------------------------------------------
			void UpdateFromKeyBoard( float deltaSeconds );
			void UpdateAudioFromKeyBoard();
			void CameraPositionUpdateOnInput( float deltaSeconds );

private:

	int							m_controllerID										= -1;
	float						m_screenShakeIntensity								= 0.f;

public:

	GPUMesh*					m_cubeMesh;
	GPUMesh*					m_basisMesh;
	
	Transform					m_cubeMesh1Transform;
	Transform					m_cubeMesh2Transform;
	Transform					m_cubeMesh3Transform;
	Transform					m_basisMeshTransform;
	
	uint						m_hCuts												= 32;		// slices
	uint						m_vCuts												= 16;		// stacks

	mutable Camera				m_gameCamera;
	Camera						m_uiCamera;

	Vec3						m_cameraPosition									= Vec3::ZERO;
	Vec3						m_cameraRotation									= Vec3::ZERO;
	
	bool						m_isHUDEnabled										= true;

	Shader*						m_imageEffectShader									= nullptr;
	
	Texture*					m_testTexture										= nullptr;
	SoundPlaybackID				m_testSound;
	RandomNumberGenerator		m_rng;
	
	eDebugRenderMode			m_debugRenderMode									= DEBUG_RENDER_ALWAYS;
};