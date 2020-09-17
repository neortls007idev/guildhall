#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/Transform.hpp"
#include "Game/GameCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Shader;
class Texture;
class GPUMesh;

typedef std::vector< Transform* > OBJInstances;
//typedef std::vector< Transform* > OBJInstancesInsideViewFrustum;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{

public:
			Game();

			void InitializeLightData();
			void LoadShaders();
			void LoadTextures();
			void LoadModels();
			void LoadScene();
			void InitializeCameras();
			void IntializeGameObjects();
			void InitializeShaderMaterialData();

			~Game();

			void Update( float deltaSeconds );
			void UpdateLightData( int lightIndex );
			void UpdateToneMapData();
			
			void UpdateLightPosition( float deltaSeconds );
			void UpdateViewFrustumCulling();
			void Render() const;
			void BindShaderSpecificMaterialData() const;
			void RenderFresnelShader2ndPass() const;

			void SetLightPositionToOrigin( int lightIndex );
			void SetLightPositionToCameraPos( int lightIndex );
			void SetLightDirectionToCameraForwardVector( int lightIndex );

private:
			void DebugUI();
			bool AddGameOBJInstance( eGameObjModels modelType );
			bool DestroyGameOBJInstance( eGameObjModels modelType , int instanceCount );
			void RenderAllInstancesOfType( eGameObjModels modelType ) const;
			void SaveScene();

//--------------------------------------------------------------------------------------------------------------------------------------------
//				METHODS TO HANDLE USER INPUT
//--------------------------------------------------------------------------------------------------------------------------------------------
			void UpdateFromKeyBoard( float deltaSeconds );
			void UpdateLightsFromKeyBoard( float deltaSeconds );
			void UpdateLightPositionOnUserInput();
			void CameraPositionUpdateOnInput( float deltaSeconds );
	
public:

	GPUMesh*					m_cubeMesh;
	GPUMesh*					m_meshSphere;
	GPUMesh*					m_quadMesh;
	GPUMesh*					m_objSciFiShipMesh;
	Transform					m_cubeMeshTransform;
	Transform					m_sphereMeshTransform;
	Transform					m_quadTransform;
	Transform					m_objSciFiShipTransform;
	Texture*					m_meshTex_D											= nullptr;
	Texture*					m_meshTex_N											= nullptr;
	Texture*					m_tileDiffuse										= nullptr;
	Texture*					m_tileNormal										= nullptr;
	Texture*					m_objSciFiShipMeshTex_D								= nullptr;
	Texture*					m_objSciFiShipMeshTex_N								= nullptr;

	uint						m_hCuts												= 32;		// slices
	uint						m_vCuts												= 16;		// stacks

	mutable Camera				m_gameCamera;
	
	Vec3						m_cameraPosition									= Vec3::ZERO;
	float						m_pitch												= 0.f;
	float						m_yaw												= 0.f;
	
	Shader*						m_lightShaders[ LitShaderTypes::TOTAL_LITSHADERS ];
	Shader* 					m_currentShader;
	Shader* 					m_blurShader;
	Shader* 					m_toneMapShader;

	int							m_currentShaderIndex;
	bool						m_isFresnelShaderActive								= false;
	bool						m_isblurShaderActive								= false;
	bool						m_isToneMapShaderActive								= false;
	//bool						m_isFogShaderActive									= false;

	Texture*					m_dissolveShaderPatternTexture						= nullptr;
	Texture*					m_triplanarShaderTextures[ 6 ];
	int							m_currentToneMap									= ToneMap::NO_TONE;
	Mat44						m_toneMapTransform;
	float						m_tonePower											= 1.f;
	
	shaderLightDataT			m_lights;
	Rgba8						m_ambientLightColor;
	int							m_lightType[ TOTAL_LIGHTS ];
	bool						m_isLightFollowingTheCamera							= false;
	bool						m_isLightAnimated									= false;
	int							m_currentLightIndex									= 0;
																					
	//--------------------------------------------------------------------------------------------------------------------------------------------
	GPUMesh*					m_unitCubeMesh	= nullptr;
	Texture*					m_cubeMapex		= nullptr;
	Shader*						m_cubeMapTest	= nullptr;
	Sampler*					m_cubeSampler	= nullptr;
	Sampler*					m_linear		= nullptr;
	//--------------------------------------------------------------------------------------------------------------------------------------------

	GPUMesh*					m_gameModels[ NUM_GAME_MODELS ];
	Texture*					m_gameModelsDiffuse[ NUM_GAME_MODELS ];
	Texture*					m_gameModelsNormals[ NUM_GAME_MODELS ];
	OBJInstances				m_ModelInstances[ NUM_GAME_MODELS ];
	OBJInstances				m_ModelDrawableInstances[ NUM_GAME_MODELS ];

private:
	fresnelData_t				m_fresnelShaderData;
	dissolveData_t				m_dissolveShaderData;
	fogDataT					m_fogData;
	float						m_frameRate			  = 0.f;
	bool						m_isMouseUnlocked	  = false;
	bool						m_debugSwitchs[ NUM_GAME_DEBUG_SWITCHS ];
	bool						m_dirtyUBOs[ NUM_DIRTY_UBOS ];
	uint						m_totalDrawableMeshes;
	uint						m_currentlyDrawingMeshes;
};