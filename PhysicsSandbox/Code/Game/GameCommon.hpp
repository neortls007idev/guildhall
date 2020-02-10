#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Input/InputSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class Game;
extern InputSystem* g_theInput;
extern Game* g_theGame;

//--------------------------------------------------------------------------------------------------------------------------------------------
//  GENERAL GAME CONSTANTS
//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr float CLIENT_ASPECT											=  16.f / 8.f; // We are requesting a 2:1 aspect (square) window area
constexpr float MAX_CAMERA_MOVE_VELOCITY_X								=  70.f;
constexpr float MAX_CAMERA_MOVE_VELOCITY_Y								=  70.f;
constexpr float MAX_CAMERA_ZOOM_VELOCITY_X								=  15000.f;
constexpr float MAX_CAMERA_ZOOM_VELOCITY_Y								=  15000.f;
constexpr float MIN_CAMERA_SHAKE										= -10.0f;
constexpr float MAX_CAMERA_SHAKE										=  10.0f;

constexpr float	MIN_COLLIDER_RADIUS										=  50.f;
constexpr float	MAX_COLLIDER_RADIUS										=  200.f;

constexpr int	MIN_INITIAL_GAME_OBJECTS								=  5;
constexpr int	MAX_INITIAL_GAME_OBJECTS								=  10;

constexpr float	DELTA_GRAVITY_CHANGE_ALONGY								= 10.f;

//--------------------------------------------------------------------------------------------------------------------------------------------
