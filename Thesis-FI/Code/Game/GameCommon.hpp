#pragma once
#include "Engine/Input/InputSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;
class Window;
class ParticleEmitter3D;

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

struct GameParticleEmitter
{
	ParticleEmitter3D*	m_emitter;
//	SpriteSheet*		m_spriteSheetInUse;
	Vec3				m_center						= Vec3::ZERO;
	float				m_movementRadius				= 5.f;
	Rgba8				m_particleStartColor;
	Rgba8				m_particleEndColor;
	float				m_particleMinLifeTime			= 0.25f;
	float				m_particleMaxLifeTime			= 2.f;
	float				m_particleVelocity				= 15.f;
	Vec2				m_particleSize					= Vec2::ONE;
	uint				m_numParticlesToSpawnPerFrame	= 1000;

	uint				m_numAliveParticles				= 0;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum GameStarEmitters
{
	RED_ORANGE_STAR,
	BLUE_CYAN_STAR,

	NUM_STARS_EMITTERS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum GameEmitterSpriteSheets
{
	STAR_SS ,
	
	NUM_GAME_SS
};
