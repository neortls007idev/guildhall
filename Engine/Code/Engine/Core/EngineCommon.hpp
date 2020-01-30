#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

#include <vector>
#include <string>
#include <map>

//--------------------------------------------------------------------------------------------------------------------------------------------

//#pragma warning ( 2816:error);

class RandomNumberGenerator;

#define UNUSED(x) (void)(x);
#define STATIC // DO NOTHING JUST A PLACEHOLDER STATIC KEYWORD TO IDENTIFY STATIC TYPES IN CPPS

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RandomNumberGenerator* g_RNG;
extern NamedStrings		      g_gameConfigBlackboard; // declared in EngineCommon.hpp, defined in EngineCommon.cpp
extern DevConsole*			  g_theDevConsole;
extern EventSystem*			  g_theEventSystem;

//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2 ALIGN_BOTTOM_LEFT	 ( 0.0f , 0.0f );
const Vec2 ALIGN_BOTTOM_CENTERED ( 0.5f , 0.0f );
const Vec2 ALIGN_BOTTOM_RIGHT	 ( 1.0f , 0.0f );
const Vec2 ALIGN_CENTERED		 ( 0.5f , 0.5f );
const Vec2 ALIGN_CENTERED_LEFT	 ( 0.0f , 0.5f );
const Vec2 ALIGN_CENTERED_RIGHT  ( 1.0f , 0.5f );
const Vec2 ALIGN_TOP_LEFT		 ( 0.0f , 1.0f );
const Vec2 ALIGN_TOP_CENTERED	 ( 0.5f , 1.0f );
const Vec2 ALIGN_TOP_RIGHT		 ( 1.0f , 1.0f );
