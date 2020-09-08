#pragma once
#include "Engine/Input/InputSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;
class Window;

//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem* g_theInput;
extern Game*		g_theGame;
extern Window*		g_theWindow;

//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr float CLIENT_ASPECT											= 16.f/9.f;

constexpr float WORLD_SIZE_X											= 20.f;
constexpr float WORLD_SIZE_Y											= 12.f;
constexpr float WORLD_CAMERA_SIZE_X										= /*16.f;*/ WORLD_SIZE_Y*CLIENT_ASPECT;
constexpr float WORLD_CAMERA_SIZE_Y										= /*9.f;*/WORLD_SIZE_Y;
constexpr float UI_SIZE_X												= 1600.f;
constexpr float UI_SIZE_Y												= 800.f;
constexpr float WORLD_CENTER_X											= WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y											= WORLD_SIZE_Y / 2.f;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum LitShaderTypes
{
	LIT_SHADOW ,
	LIT ,
	UNLIT ,
	UV ,
	NORMAL ,
	TANGENT ,
	BITANGENT ,
	SURFACE_NORMAL ,
	DISSOLVE ,
	FRESNEL ,
	TRIPLANAR_UNLIT ,
	TRIPLANAR_LIT ,

	TOTAL_LITSHADERS ,
};

enum ToneMap
{
	NO_TONE ,
	GRAYSCALE ,
	SEPHIA ,

	TOTAL_TONEMAPS ,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  LightType
{
	POINT_LIGHT ,
	DIRECTIONAL_LIGHT ,
	SPOT_LIGHT ,

	TOTAL_LIGHT_TYPES
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct fresnelData_t
{
	Vec3	fresnelColor = Vec3::UNIT_VECTOR_ALONG_J_BASIS;
	float	fresnelPower = 1.f;

	Vec3	padding00 = Vec3::ZERO;
	float	fresnelfactor = 1.f;
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