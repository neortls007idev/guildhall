#pragma once
#include <string>
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class InputSystem;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eIcon : uint
{
	WARNING,
	ERROR_ICON,
 	INFORMATION,
	TOTAL_ICONS
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eDisplaySettings : uint
{
	BORDERLESS,
	REGULAR,
	FULLSCREEN
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Window
{
public:
	Window();
	~Window();

	//void SetInputSystem( InputSystem* inputSystem );

	bool Open( std::string const& title , float aspect , float maxClientFractionOfDesktop = 0.9f );
	void Close();

	void SetInputSystem( InputSystem* InputSystem );
	InputSystem* GetInputSytem() const { return m_inputSystem; }

	void BeginFrame(); // process window messages

	int GetClientWidth();
	int GetClientHeight();

	bool IsQuitting() const { return m_isQuitting; }
	bool HandleQuitRequested();

	void SetTitle( std::string const& title );
	void SetNewIcon( void* const& icon );
	void SetNewIcon( eIcon newIcon );
	void CreateIcons();
	void DisplaySettings( eDisplaySettings settings );


public:
	void* m_hwnd = nullptr;

private:
	int					m_clientHeight		= 0;
	int					m_ClientWidth		= 0;
	InputSystem*		m_inputSystem		= nullptr;
	bool				m_isQuitting		= false;
	eDisplaySettings	m_windowStyleFlags	= REGULAR;
	void*				m_icons[ TOTAL_ICONS ];
};
