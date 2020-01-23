#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include <windows.h>					// #include this (massive, platform-specific) header in very few places

//--------------------------------------------------------------------------------------------------------------------------------------------

InputSystem::InputSystem()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

InputSystem::~InputSystem()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::Startup()
{
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::BeginFrame()
{
	for ( int controllerID = 0; controllerID < MAX_XBOX_CONTROLLERS; controllerID++ )
	{
			m_controllers[controllerID].Update();
	}
	UpdateMouse();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::EndFrame()
{
	for ( int keycode = 0 ; keycode < MAX_KEYS ; keycode++)
	{
		m_keyStates[ keycode ].m_wasPressedLastFrame = m_keyStates[ keycode ].m_isPressed;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::Shutdown()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			KEYBOARD FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::IsKeyHeldDown(unsigned char keyCode) const
{
	if ( m_keyStates[keyCode].m_isPressed && m_keyStates[keyCode].m_wasPressedLastFrame )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::WasKeyJustReleased(unsigned char keyCode) const
{
	if ( ( !m_keyStates[ keyCode ].m_isPressed ) && m_keyStates[ keyCode ].m_wasPressedLastFrame )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::WasKeyJustPressed( unsigned char keyCode ) const
{
	if ( m_keyStates[keyCode].m_isPressed && ( !m_keyStates[keyCode].m_wasPressedLastFrame ) )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::HandleKeyDown( unsigned char keycode )
{
	m_keyStates[ keycode ].m_isPressed = true;
	return m_keyStates[ keycode ].m_isPressed;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::HandleKeyUp( unsigned char keycode )
{
	m_keyStates[ keycode ].m_isPressed = false;
	return m_keyStates[ keycode ].m_isPressed;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

const KeyButtonState& InputSystem::GetButtonState(unsigned char keyCode) const
{
	return m_keyStates[keyCode];
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//				XBOX INPUT FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------------------

const XboxController& InputSystem::GetXboxController(int controllerID)
{
	return m_controllers[controllerID];	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

IntVec2 InputSystem::GetMouseRawDesktopPosition() const
{
	POINT rawMouseDesktopPosition;
	GetCursorPos( &rawMouseDesktopPosition );
	return IntVec2( rawMouseDesktopPosition.x , rawMouseDesktopPosition.y );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 InputSystem::GetMouseNormalizedClientPosition() const
{
	return m_mouseNormalizedPosition;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::UpdateMouse()
{
	//ShowCursor( FALSE );

// 	POINT mousePosition;
// 	GetCursorPos( &mousePosition );
// 	ScreenToClient( g_hWnd , &mousePosition );
// 	Vec2 clientMousePosition( ( float ) mousePosition.x , ( float ) mousePosition.y );
// 
// 	RECT clientRect;
// 	GetClientRect( g_hWnd , &clientRect );
// 	AABB2 clientBounds = AABB2( clientRect.left , clientRect.top , clientRect.right , clientRect.bottom );
// 	m_mouseNormalizedPosition = clientBounds.GetUVForPoint( clientMousePosition );
// 	m_mouseNormalizedPosition.y = 1.f - m_mouseNormalizedPosition.y;
	GUARANTEE_OR_DIE( false , "Starting Stuff replace with D3D11" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
