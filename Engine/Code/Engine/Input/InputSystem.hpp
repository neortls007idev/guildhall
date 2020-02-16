#pragma once
#include "Engine/Input/XboxController.hpp"
#include <queue>

//--------------------------------------------------------------------------------------------------------------------------------------------

constexpr int MAX_XBOX_CONTROLLERS = 4;
constexpr int MAX_KEYS = 256;

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

	void PushCharacter( char character );
	bool PopCharacter( char* outCharacter );

public:
	std::queue<char> m_characters;

private:

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
