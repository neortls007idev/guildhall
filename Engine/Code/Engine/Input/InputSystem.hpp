#pragma once
#include "Engine/Input/XboxController.hpp"
#include <queue>

//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr int MAX_XBOX_CONTROLLERS = 4;
constexpr int MAX_KEYS = 256;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eMouseClipping
{
	MOUSE_IS_WINDOWLOCKED,
	MOUSE_IS_UNLOCKED,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eMouseMode
{
	ABSOLUTE_MODE,
	RELATIVE_MODE,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	bool  IsKeyHeldDown( unsigned char keyCode ) const;
	bool  WasKeyJustReleased( unsigned char keyCode ) const;
	bool  WasKeyJustPressed( unsigned char keyCode ) const;
	bool  HandleKeyDown( unsigned char keyCode );
	bool  HandleKeyUp( unsigned char keyCode );

	const KeyButtonState& GetButtonState( unsigned char keyCode ) const;
	const XboxController& GetXboxController( int controllerID );

	IntVec2 GetMouseRawDesktopPosition() const;
	Vec2    GetMouseNormalizedClientPosition() const;

	void UpdateMouse();

	void HideSystemCursor();
	void ShowSystemCursor();
	void ClipSystemCursor( eMouseClipping mouseLockMode );
	void SetCursorMode( eMouseMode mode );
	void UpdateRelativeMode();

	eMouseMode GetCursorMode() const														{ return m_mouseMode;  }

	void PushCharacter( char character );
	bool PopCharacter( char* outCharacter );

public:
	std::queue<char> m_characters;

private:
	eMouseMode m_mouseMode		= ABSOLUTE_MODE;
	Vec2 m_relativeMovement		= Vec2::ZERO;
	Vec2 m_positionLastFrame	= Vec2::ZERO;

	KeyButtonState m_keyStates[ MAX_KEYS ];
	XboxController m_controllers[ MAX_XBOX_CONTROLLERS ] =
	{
		XboxController( 0 ),
		XboxController( 1 ),
		XboxController( 2 ),
		XboxController( 3 ),
	};

	Vec2 m_mouseNormalizedPosition = Vec2::ZERO;

};

//--------------------------------------------------------------------------------------------------------------------------------------------
