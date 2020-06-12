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
class TileMap;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  eGameTextures
{
	TEST_TEXTURE,
	HUD_BASE,
	TERRAIN_SPRITE_SHEET,
	PLAYER_SPRITE_SHEET,

	NUM_TOTAL_GAME_TEX
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  eGameSounds
{
	TEST_SOUND ,
	TELEPORTER ,
	
	NUM_GAME_SOUNDS,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{

public:
			Game();

			//--------------------------------------------------------------------------------------------------------------------------------------------

			void LoadShaders();
			void LoadTextures();
			void LoadAudio();
			void InitializeCameras();
			void IntializeGameObjects();
			void InitializeHUDElements();
			void InitializeObjectTransforms();
			
			~Game();

			void Update( float deltaSeconds );
			void DebugDrawUI( float deltaSeconds );
				
			void Render() const;
			void RenderHUD() const;
private:
//--------------------------------------------------------------------------------------------------------------------------------------------
//				METHODS TO HANDLE USER INPUT
//--------------------------------------------------------------------------------------------------------------------------------------------
			void UpdateFromKeyBoard( float deltaSeconds );
			void UpdateDebugDrawFromKeyBoard();
			void CameraPositionUpdateOnInput( float deltaSeconds );
			void UpdateFromTestCodeKeyBoard( float deltaSeconds );

private:

	int							m_controllerID										= -1;
	float						m_screenShakeIntensity								= 0.f;
	float						m_fps												= 0.f;
	
public:

	GPUMesh*					m_cubeMesh;
	GPUMesh*					m_basisMesh;
	
	Transform					m_cubeMesh1Transform;
	Transform					m_cubeMesh2Transform;
	Transform					m_cubeMesh3Transform;
	Transform					m_basisMeshTransform;
	Transform					m_compassMeshTransform;
	
	mutable Camera				m_gameCamera;
			Camera				m_uiCamera;

	AABB2						m_HUD;
	AABB2						m_playerGun;
	
	Vec3						m_cameraPosition									= Vec3::ZERO;
	float						m_pitch												= 0.f;
	float						m_yaw												= 0.f;
	
	bool						m_debugDraw											= false;

	Shader*						m_imageEffectShader									= nullptr;

	Sampler*					m_pointSampler										= nullptr;
	Texture*					m_textures[ NUM_TOTAL_GAME_TEX ];
	SoundPlaybackID				m_sounds[ NUM_GAME_SOUNDS ];
	RandomNumberGenerator		m_rng;
	
	eDebugRenderMode			m_debugRenderMode									= DEBUG_RENDER_ALWAYS;


	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			TEST CODE
	//--------------------------------------------------------------------------------------------------------------------------------------------
	
	TileMap*					m_testMap											= nullptr;
	bool						m_isSamplerEnabled									= false;
};