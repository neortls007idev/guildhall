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
constexpr float MAX_CAMERA_SHAKE										=  2.5f;
constexpr float SCREEN_SHAKE_ABLATION_PER_SECOND						=  0.05f;

constexpr int   MIN_WORM_LENGTH											= 3;
constexpr int   MAX_WORM_LENGTH											= 13;

//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr float LEVEL_SIDEWALL_PERCENTAGE = 0.119791667f;

constexpr float TILE_LENGTH = 112.6667f;
constexpr float TILE_HEIGHT = 32.f;
const Vec2 halfTileDimensions = Vec2( TILE_LENGTH * 0.5f , TILE_HEIGHT * 0.5f );
//--------------------------------------------------------------------------------------------------------------------------------------------

enum eGameTextures
{
	TEX_NONE = -1,
	TEX_BACKGROUND_FOREST,

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

	TEX_FLARE_RED,
	TEX_FLARE_GREEN,
	TEX_FLARE_BLUE,
	TEX_FLARE_YELLOW,
	TEX_FLARE_MAGENTA,
	TEX_FLARE_CYAN,
	TEX_FLARE_PINK,
	TEX_FLARE_PURPLE,
	TEX_FLARE_ORANGE,
	TEX_FLARE_GREY,
	
	NUM_GAME_TEX
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eGameSpriteSheets
{
	SS_NONE = -1 ,
	SS_BALL ,	
	SS_VFX_FLARE ,

	NUM_GAME_SS
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