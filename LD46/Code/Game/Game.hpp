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
class Player;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum ShaderTypes
{
	UNLIT,
	DISSOLVE,
	FRESNEL,
	
	TOTAL,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  LightType
{
	POINT_LIGHT,
	DIRECTIONAL_LIGHT,
	SPOT_LIGHT,
	TOTAL_LIGHT_TYPES
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct fresnelData_t
{
	Vec3	fresnelColor	= Vec3::UNIT_VECTOR_ALONG_J_BASIS;
	float	fresnelPower	= 1.f; 

	Vec3	padding00		= Vec3::ZERO; 
	float	fresnelfactor	= 1.f; 
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct dissolveData_t
{
	Vec3	startColor;
	float	burnEdgeWidth;

	Vec3	endColor;
	float	burnValue;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game
{

public:
			Game();

			void InitializeLightData();
			void LoadShaders();
			void LoadTextures();
			void InitializeCameras();
			void intializeGameObjects();
			void InitializeShaderMaterialData();

			~Game();

			void Update( float deltaSeconds );
			void DebugDrawUI( float deltaSeconds );
			void UpdateLightPosition( float deltaSeconds );
			void AddScreenShakeIntensity( float deltaShakeIntensity );

	
			void Render() const;
			void BindShaderSpecificMaterialData() const;
			void RenderFresnelShader2ndPass() const;
private:

//--------------------------------------------------------------------------------------------------------------------------------------------
//				METHODS TO HANDLE USER INPUT
//--------------------------------------------------------------------------------------------------------------------------------------------
			void UpdateFromKeyBoard( float deltaSeconds );
			void CameraPositionUpdateOnInput( float deltaSeconds );

private:

	int							m_controllerID										= -1;
	float						m_screenShakeIntensity								= 0.f;
	Shader*						m_shaders[ ShaderTypes::TOTAL ];

public:
	float						m_timeElapsed										= 0.f;
	Camera						m_gameCamera;
	Player*						m_player;

//--------------------------------------------------------------------------------------------------------------------------------------------
	
};