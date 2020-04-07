#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include <string>
//#pragma warning ( 2816:error);

//--------------------------------------------------------------------------------------------------------------------------------------------

class RandomNumberGenerator;
class DevConsole;

//--------------------------------------------------------------------------------------------------------------------------------------------

typedef unsigned int	uint;
typedef unsigned char	uchar;

//--------------------------------------------------------------------------------------------------------------------------------------------

#define UNUSED(x) (void)(x);
#define STATIC // DO NOTHING JUST A PLACEHOLDER STATIC KEYWORD TO IDENTIFY STATIC TYPES IN CPPS
#define BIT_FLAG( b ) ( 1 << ( b ) )
#define BREAKPOINT( x ) int myBreakpoint ## x; \ UNUSED( myBreakPoint ## x );
#define SAFE_RELEASE_POINTER( ptr ) if ( nullptr != ptr ) { delete ptr; ptr = nullptr; }

//--------------------------------------------------------------------------------------------------------------------------------------------

// 
// #define COMMAND( name ) \
// static void name##_impl( NamedStrings& args ); \				// static void some_method_impl( NamedStrings& args );
// static EventRegistrar name##_register( #name , name##impl ); \  // static EventRegistrar some_method_register( "name" , name_impl )
// static  void name##_impl( NamedStrings & args )			//


//--------------------------------------------------------------------------------------------------------------------------------------------

enum eBufferFormatType
{
	// add types as you need them - for now, we can get by with just this
	BUFFER_FORMAT_INVALID = -1,
	BUFFER_FORMAT_VEC2 ,
	BUFFER_FORMAT_VEC3 ,
	BUFFER_FORMAT_R8G8B8A8_UNORM ,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct buffer_attribute_t
{
	std::string name; 			// used to link to a D3D11 shader
	// uint location; 			// used to link to a GL/Vulkan shader 
	eBufferFormatType type;		// what data are we describing
	uint offset; 				// where is it relative to the start of a vertex
	//uint count;
	buffer_attribute_t( char const* n , eBufferFormatType t , uint offset_o );
	buffer_attribute_t();
	uint GetAttributeCount;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eCompareOp            // A04
{
	COMPARE_NEVER ,       // false,      "never"
	COMPARE_ALWAYS ,      // true,       "always"
	COMPARE_EQUAL ,       // ==,         "equal"
	COMPARE_NOTEQUAL ,    // !=,         "not"
	COMPARE_LESS ,        // <           "less"
	COMPARE_LEQUAL ,      // <=          "lequal"
	COMPARE_GREATER ,     // >           "greater"
	COMPARE_GEQUAL ,      // >=          "gequal"
};

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RandomNumberGenerator* g_RNG;
extern NamedStrings		      g_gameConfigBlackboard; // declared in EngineCommon.hpp, defined in EngineCommon.cpp
extern DevConsole*			  g_theDevConsole;
extern EventSystem*			  g_theEventSystem;


//--------------------------------------------------------------------------------------------------------------------------------------------

const Vec2 ALIGN_BOTTOM_LEFT( 0.0f , 0.0f );
const Vec2 ALIGN_BOTTOM_CENTERED( 0.5f , 0.0f );
const Vec2 ALIGN_BOTTOM_RIGHT( 1.0f , 0.0f );
const Vec2 ALIGN_CENTERED( 0.5f , 0.5f );
const Vec2 ALIGN_CENTERED_LEFT( 0.0f , 0.5f );
const Vec2 ALIGN_CENTERED_RIGHT( 1.0f , 0.5f );
const Vec2 ALIGN_TOP_LEFT( 0.0f , 1.0f );
const Vec2 ALIGN_TOP_CENTERED( 0.5f , 1.0f );
const Vec2 ALIGN_TOP_RIGHT( 1.0f , 1.0f );


//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr float DEVCONSOLE_CAMERA_SIZE_Y = 800.f;
constexpr float DEVCONSOLE_CAMERA_SIZE_X = DEVCONSOLE_CAMERA_SIZE_Y * ( 16.f / 9.f );
constexpr uint	DEVCONSOLE_MAX_COMMAND_HISTORY = 10;

//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr int	TOTAL_MOUSE_DRAG_TRACK_FRAMES = 120;

//--------------------------------------------------------------------------------------------------------------------------------------------