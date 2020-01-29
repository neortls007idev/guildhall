#pragma once
#include "Engine/Input/XboxController.hpp"

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

	bool WasLeftMouseButtonJustPressed() const;
	bool WasLeftMouseButtonJustReleased() const;
	bool  IsLeftMouseButtonHeldDown() const;

	bool HandleLeftMouseButtonPressed();
	bool HandleLeftMouseButtonReleased();

	bool WasRightMouseButtonJustPressed() const;
	bool WasRightMouseButtonJustReleased() const;
	bool  IsRightMouseButtonHeldDown() const;

	bool HandleRightMouseButtonPressed();
	bool HandleRightMouseButtonReleased();

	bool WasMiddleMouseButtonJustPressed() const;
	bool WasMiddleMouseButtonJustReleased() const;
	bool  IsMiddleMouseButtonHeldDown() const;

	bool HandleMiddleMouseButtonPressed();
	bool HandleMiddleMouseButtonReleased();

	IntVec2 GetMouseRawDesktopPosition() const;
	Vec2    GetMouseNormalizedClientPosition() const;

	void UpdateMouse();
	void UpdateMouseWheel();

	const XboxController& GetXboxController( int controllerID );
	   
private:

	KeyButtonState m_keyStates[ MAX_KEYS ];

	KeyButtonState m_leftMouseButton;
	KeyButtonState m_rightMouseButton;
	KeyButtonState m_middleMouseButton;

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
