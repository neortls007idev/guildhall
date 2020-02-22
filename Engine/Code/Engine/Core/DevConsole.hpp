#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EventSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

enum eDevConsoleMessageType
{
	DEVCONSOLE_COMMAND,
	DEVCONSOLE_ERROR,
	DEVCONSOLE_USERINPUT,
	DEVCONSOLE_WARNING,
	DEVCONSOLE_SYTEMLOG,
	DEVCONSOLE_USERLOG ,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct  DevConsoleCommand
{
	std::string command;
	std::string Description;
	EventArgs commandArgs;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct ColoredLine
{
	eDevConsoleMessageType messageType;
	Rgba8 lineColor;
	std::string text;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class DevConsole
{

public:

	DevConsole();
	void Startup();

	void InitializeCommands();

	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void Update();

	void Render( RenderContext& renderer , const Camera& camera , float lineHeight ) const;

	static void PrintString( const Rgba8& textColor = WHITE , const std::string& devConsolePrintString = "INVALID STRING" ,
								eDevConsoleMessageType messageType= DEVCONSOLE_USERINPUT );

	static void PrintString( const std::string& devConsolePrintString = "INVALID STRING" , eDevConsoleMessageType messageType= DEVCONSOLE_USERINPUT );

	void ProcessInput();
	void OnKeyPress( char character );
	bool AddCharacterToInput( char character );
	int	 GetCurrentInputLength() const																				{ return ( int ) m_currentText.length(); }
	void ResetCurrentInput();

	void ProcessCommand();
	void CreateCommand( DevConsoleCommand newCommand );
	void CreateCommand( std::string newCommand , std::string commandDescription );
	void CreateCommand( std::string newCommand , std::string commandDescription , EventArgs commandArgs );

	static bool ResetConsole( EventArgs& commandArgs );
	static bool ClearConsoleMessagesOfType( EventArgs& commandArgs , eDevConsoleMessageType messageType = DEVCONSOLE_USERINPUT );
	static bool ExecuteHelp( EventArgs& commandArgs );
	static bool ExecuteQuit( EventArgs& commandArgs );
	static bool Close( EventArgs& commandArgs );

	void SetIsOpen( bool isOpen );
	void ToggleVisibility();
	void Close();
	bool IsOpen() const;

	void ChangeOverlayColor( Rgba8 newOverlayColor );
	void HandleInput( unsigned char keycode );

protected:

	static bool								m_isConsoleOpen;
	static Rgba8							m_OverlayColor;
	static Rgba8							m_carrotColor;
	static float							m_carrotPosX;
	static size_t							m_carrotOffset;
	static std::vector<ColoredLine>			m_consoleText;
	static std::vector<DevConsoleCommand>	m_consoleCommands;

private:
	static std::string						m_currentText;
};