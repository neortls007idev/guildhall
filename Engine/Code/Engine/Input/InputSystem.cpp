#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Platform/Window.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include <windows.h>					// #include this (massive, platform-specific) header in very few places

//--------------------------------------------------------------------------------------------------------------------------------------------

extern	DevConsole* g_theDevConsole;
extern	Window*		g_theWindow;

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
	POINT cursorPos;
	GetCursorPos( &cursorPos );
	ScreenToClient( ( HWND ) g_theWindow->m_hwnd , &cursorPos );
	m_trackMouseVelocityOverFrames[ 0 ] = Vec2( cursorPos.x , cursorPos.y );
	m_currentlyTrackingFrameIndex ++;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::BeginFrame()
{
	for ( int controllerID = 0; controllerID < MAX_XBOX_CONTROLLERS; controllerID++ )
	{
			m_controllers[controllerID].Update();
	}
	UpdateMouse();
	m_currentlyTrackingFrameIndex++;
	m_currentlyTrackingFrameIndex %= TOTAL_MOUSE_DRAG_TRACK_FRAMES;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	POINT cursorPos;
	GetCursorPos( &cursorPos );
	ScreenToClient( ( HWND ) g_theWindow->m_hwnd , &cursorPos );

	m_trackMouseVelocityOverFrames[ m_currentlyTrackingFrameIndex % TOTAL_MOUSE_DRAG_TRACK_FRAMES ] =
		( Vec2( cursorPos.x , cursorPos.y ) - m_trackMouseVelocityOverFrames[ m_currentlyTrackingFrameIndex ] ) * ( 1.f / 60.f );

	Vec2 tempVelocity = Vec2::ZERO;
	for ( int index = 0 ; index < TOTAL_MOUSE_DRAG_TRACK_FRAMES ; index++ )
	{
		tempVelocity += m_trackMouseVelocityOverFrames[ index ];
	}

	m_mouseDragVelocity = tempVelocity / TOTAL_MOUSE_DRAG_TRACK_FRAMES ;
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
	m_relativeMovement = Vec2::ZERO;
	//m_characters.clear();
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
	ScreenToClient( ( HWND ) g_theWindow->m_hwnd , &mousePosition );
	Vec2 clientMousePosition( ( float ) mousePosition.x , ( float ) mousePosition.y );

	RECT clientRect;
	GetClientRect( ( HWND ) g_theWindow->m_hwnd , &clientRect );
	AABB2 clientBounds = AABB2( clientRect.left , clientRect.top , clientRect.right , clientRect.bottom );
	m_mouseNormalizedPosition = clientBounds.GetUVForPoint( clientMousePosition );
	m_mouseNormalizedPosition.y = 1.f - m_mouseNormalizedPosition.y;

	if ( m_mouseMode == RELATIVE_MODE )
	{
		UpdateRelativeMode();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::HideSystemCursor()
{
	while ( ShowCursor( FALSE ) >= 0 );
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
void InputSystem::ShowSystemCursor()
{
	while ( ShowCursor( TRUE ) <= 0 );

	g_theWindow->SetCursorIconToLoadedIcon();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::ClipSystemCursor( eMouseClipping mouseLockMode )
{
	static RECT mouseNewClipArea;;           // new area for ClipCursor

	GetWindowRect( ( HWND ) g_theWindow->m_hwnd , &mouseNewClipArea );

	switch (mouseLockMode)
	{
		case MOUSE_IS_WINDOWLOCKED :
										ClipCursor( &mouseNewClipArea );
										break;
		case  MOUSE_IS_UNLOCKED		:
										ClipCursor( nullptr );
										break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::SetCursorMode( eMouseMode mode )
{
	m_mouseMode = mode;
	
	switch ( m_mouseMode )
	{
		case RELATIVE_MODE: //ClipSystemCursor( MOUSE_IS_WINDOWLOCKED );
							//HideSystemCursor();
							UpdateRelativeMode();
							break;

		case ABSOLUTE_MODE: //ClipSystemCursor( MOUSE_IS_UNLOCKED );
							//ShowSystemCursor();
							break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::PushCursorSettings( CursorSettings newCursorSettings )
{
	m_cursorSettings = newCursorSettings;
	SetCursorMode( newCursorSettings.m_cursorMode );
	ClipSystemCursor( newCursorSettings.m_cursorClipMode );

	if ( newCursorSettings.m_isCursorVisible )
	{
		ShowSystemCursor();
	}
	else
	{
		HideSystemCursor();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::PopCursorSettings()
{
	//m_cursorSettings.m_cursorClipMode	= MOUSE_IS_UNLOCKED;
	//m_cursorSettings.m_cursorMode		= ABSOLUTE_MODE;
	//m_cursorSettings.m_isCursorVisible	= true;
	
	ClipSystemCursor( MOUSE_IS_UNLOCKED );
	SetCursorMode( ABSOLUTE_MODE );
	ShowSystemCursor();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::UpdateRelativeMode()
{
		POINT cursorPos;
		GetCursorPos( &cursorPos );
	
		Vec2 positionThisFrame( cursorPos.x , cursorPos.y );
		m_relativeMovement = positionThisFrame - m_positionLastFrame;
		// remap relative movement ()

		// move back to center
		RECT clientWindow;
		GetWindowRect( ( HWND ) g_theWindow->m_hwnd , &clientWindow );

		// GetClientRect() to get rectangle, find center of that
		Vec2 windowCenter = Vec2( clientWindow.right - clientWindow.left , clientWindow.bottom - clientWindow.top ) * 0.5f;

		SetCursorPos( ( int ) windowCenter.x , ( int ) windowCenter.y );

		// one little trick... without - will cause drift (maybe)
		POINT point;
		GetCursorPos( &point  );
		windowCenter = Vec2( point.x , point.y );

		// recenter
		m_positionLastFrame = windowCenter;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Vec2 InputSystem::GetRelativeMovement() const
{
	return m_relativeMovement;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void InputSystem::PushCharacter( char character )
{
	if ( character == 8 && g_theDevConsole->GetCurrentInputLength() <= 0 )
	{
		return;
	}
	m_characters.push( character );
}

bool InputSystem::PopCharacter( char* outCharacter )
{
	if ( !m_characters.empty() )
	{
		*outCharacter = m_characters.front();
		m_characters.pop();
		return true;
	}
	else
	{
		return false;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------
