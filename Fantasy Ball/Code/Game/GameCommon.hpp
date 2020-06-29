#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Input/InputSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;
class Window;

//--------------------------------------------------------------------------------------------------------------------------------------------

extern InputSystem* g_theInput;
extern Game*		g_theGame;
extern Window*		g_theWindow;

//--------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------
//  GENERAL GAME CONSTANTS
//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr float CLIENT_ASPECT											=  16.f / 9.f; // We are requesting a 2:1 aspect (square) window area
constexpr float UI_SIZE_X												=  1600.f;
constexpr float UI_SIZE_Y												=  900.f;
constexpr float MIN_CAMERA_SHAKE										= -2.5f;
constexpr float MAX_CAMERA_SHAKE										=  5.5f;
constexpr float SCREEN_SHAKE_ABLATION_PER_SECOND						=  0.05f;

//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr float LEVEL_SIDEWALL_PERCENTAGE = 0.119791667f;

//constexpr float TILE_LENGTH = 112.6667f;
//constexpr float TILE_HEIGHT = 32.f;
//constexpr float TILE_LENGTH = 110.f;
constexpr float TILE_LENGTH					= 112.6667f;
constexpr float TILE_HEIGHT					= 32.f;
const Vec2 halfTileDimensions				= Vec2( TILE_LENGTH * 0.5f , TILE_HEIGHT * 0.5f );

constexpr float BALL_INITIAL_VELOCITY_MAGNITUDE = 4.5f;
constexpr float BALL_INITIAL_LAUNCH_ORIENTATION_DEG = 107.f;
const Vec2 BALL_INITIAL_VELOCITY			= Vec2::MakeFromPolarDegrees( BALL_INITIAL_LAUNCH_ORIENTATION_DEG , BALL_INITIAL_VELOCITY_MAGNITUDE );

constexpr float VFX_PARTICLE_HEIGHT			= 40.f;
const Vec2		VFX_PARTICLE_DIMENSIONS		= Vec2( 1.7777777f , 1.f ) * VFX_PARTICLE_HEIGHT;
constexpr float VFX_PARTICLE_VELOCITY		= 25.f;
constexpr float VFX_PARTICLE_MIN_AGE		= 40.f;
constexpr float VFX_PARTICLE_MAX_AGE		= 70.f;

constexpr float LEAVES_PARTICLE_HEIGHT		= 20.f;
const Vec2 LEAVES_PARTICLE_DIMENSIONS		= Vec2( 1.f , 1.f ) * LEAVES_PARTICLE_HEIGHT;

constexpr float LEAVES_PARTICLE_VELOCITY	= 0.5f * VFX_PARTICLE_VELOCITY;
constexpr float LEAVES_PARTICLE_MIN_AGE		= 35.f;
constexpr float LEAVES_PARTICLE_MAX_AGE		= 50.f;

constexpr float FLOWER_PARTICLE_HEIGHT		= 20.f;
const Vec2		FLOWER_PARTICLE_DIMENSIONS	= Vec2( 1.f , 1.f ) * FLOWER_PARTICLE_HEIGHT;
constexpr float FLOWER_PARTICLE_VELOCITY	= 0.35f * VFX_PARTICLE_VELOCITY;
constexpr float FLOWER_PARTICLE_MIN_AGE		= 22.5f;
constexpr float FLOWER_PARTICLE_MAX_AGE		= 30.f;

constexpr float PADDLE_COLLISION_DEVIATION	= 10.f;

constexpr float TILE_DEVIATION				= .6667f * 0.1f;

//--------------------------------------------------------------------------------------------------------------------------------------------
//			UI SYSTEM DATA
//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr int   MAX_HIGHSCORE_ENTRIES		= 10;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eGameTextures
{
	TEX_NONE = -1,
	TEX_BACKGROUND_FOREST_1,
	TEX_BACKGROUND_FOREST_2,
	TEX_BACKGROUND_AURORA_1,

	TEX_LEFT_WALL,
	TEX_RIGHT_WALL,
	TEX_TOP_WALL_SECTION,
	TEX_PADDLE,

	TEX_BALL_RED,
	TEX_BALL_GREEN,
	TEX_BALL_BLUE,
	TEX_BALL_YELLOW,
	TEX_BALL_MAGENTA,
	TEX_BALL_CYAN,
	TEX_BALL_PINK,
	TEX_BALL_PURPLE,
	TEX_BALL_ORANGE,
	TEX_BALL_GREY,
	
	NUM_GAME_TEX
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eGameSpriteSheets
{
	SS_NONE = -1,
	SS_BRICKS,
	SS_BALL,	
	SS_VFX_FLARE,
	SS_VFX_FLOWERS,
	SS_VFX_LEAVES,

	NUM_GAME_SS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eGameShaders
{
	GSHADER_NONE = 0 ,
	GSHADER_VMOVE,
	GSHADER_HMOVE,
	GSHADER_COMBINEIMG,

	NUM_GAME_SHADERS,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eGameAudioFX
{
	SFX_BACKGROUND_1,
	SFX_BACKGROUND_2,
	SFX_BACKGROUND_3,
	SFX_BACKGROUND_4,
	
	SFX_LEAVES_RUSTLE,

	SFX_GLASS_BREAK_1,
	SFX_GLASS_BREAK_2,

	NUM_GAME_SFX
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct BallTexEnumRGBA8Map
{
	Rgba8			color	= WHITE;
	eGameTextures	texture = TEX_NONE;
};