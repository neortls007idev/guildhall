#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include <windows.h>					// #include this (massive, platform-specific) header in very few places
#include "../Core/ErrorWarningAssert.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern HWND g_hWnd;

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

	m_leftMouseButton.m_wasPressedLastFrame   = m_leftMouseButton.m_isPressed;
	m_rightMouseButton.m_wasPressedLastFrame  = m_rightMouseButton.m_isPressed;
	m_middleMouseButton.m_wasPressedLastFrame = m_middleMouseButton.m_isPressed;

	m_mouseWheel = ( int ) 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::Shutdown()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			KEYBOARD INPUT FUNCTIONS
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
//					MOUSE INPUT FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::WasLeftMouseButtonJustPressed() const
{
	return m_leftMouseButton.WasJustPressed();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::WasLeftMouseButtonJustReleased() const
{
	return m_leftMouseButton.WasJustReleased();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::IsLeftMouseButtonHeldDown() const
{
	return m_leftMouseButton.IsPressed();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::HandleLeftMouseButtonPressed()
{
	m_leftMouseButton.UpdateStatus( true );
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::HandleLeftMouseButtonReleased()
{
	m_leftMouseButton.UpdateStatus( false );
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::WasRightMouseButtonJustPressed() const
{
	return m_rightMouseButton.WasJustPressed();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::WasRightMouseButtonJustReleased() const
{
	return m_rightMouseButton.WasJustReleased();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::IsRightMouseButtonHeldDown() const
{
	return m_rightMouseButton.IsPressed();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::HandleRightMouseButtonPressed()
{
	m_rightMouseButton.UpdateStatus( true );
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::HandleRightMouseButtonReleased()
{
	m_rightMouseButton.UpdateStatus( false );
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::WasMiddleMouseButtonJustPressed() const
{
	return m_middleMouseButton.WasJustPressed();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::WasMiddleMouseButtonJustReleased() const
{
	return m_middleMouseButton.WasJustReleased();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::IsMiddleMouseButtonHeldDown() const
{
	return m_middleMouseButton.IsPressed();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::HandleMiddleMouseButtonPressed()
{
	m_middleMouseButton.UpdateStatus( true );
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool InputSystem::HandleMiddleMouseButtonReleased()
{
	m_middleMouseButton.UpdateStatus( false );
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::UpdateMouse()
{
	//ShowCursor( FALSE );

	POINT mousePosition;
	GetCursorPos( &mousePosition );
	ScreenToClient( g_hWnd , &mousePosition );
	Vec2 clientMousePosition( ( float ) mousePosition.x , ( float ) mousePosition.y );

	RECT clientRect;
	GetClientRect( g_hWnd , &clientRect );
	AABB2 clientBounds = AABB2( clientRect.left , clientRect.top , clientRect.right , clientRect.bottom );
	m_mouseNormalizedPosition = clientBounds.GetUVForPoint( clientMousePosition );
	m_mouseNormalizedPosition.y = 1.f - m_mouseNormalizedPosition.y;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::UpdateMouseWheel( int deltaWheel )
{
	m_mouseWheel += deltaWheel;
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