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

constexpr float GAME_CAM_NEAR_Z = 0.1f;
constexpr float GAME_CAM_FAR_Z	= 100.0f;
constexpr float GAME_CAM_FOV	= 60.0f;

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

enum eGameObjModels
{
	SPACESHIP ,
	LUMINARIS_SHIP ,
	CITY_BUILDING_1 ,
	
	NUM_GAME_MODELS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum GAME_DEBUG_SWITCHS
{
	GAME_CAMERA_VIEW_FRUSTUM_CULLING ,

	NUM_GAME_DEBUG_SWITCHS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum DIRTY_UBOS
{
	GAME_CAMERA ,

	NUM_DIRTY_UBOS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

