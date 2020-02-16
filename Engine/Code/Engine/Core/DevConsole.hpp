#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

struct  DevConsoleCommand
{
	std::string command;
	std::string Description;
	//	TODO :- ("add EventSubscrition data here");
};

//--------------------------------------------------------------------------------------------------------------------------------------------

struct ColoredLine
{
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

	void PrintString( const Rgba8& textColor = WHITE, const std::string& devConsolePrintString = "INVALID STRING");
	void Render( RenderContext& renderer , const Camera& camera , float lineHeight ) const;
	void OnKeyPress( char character );

	void ProcessCommand();
	void ExecuteCommand();
	void CreateCommand( DevConsoleCommand newCommand );
	void CreateCommand( std::string newCommand , std::string commandDescription );

	void SetIsOpen( bool isOpen );
	void ToggleVisibility();
	void Close();
	bool IsOpen() const;

	void ChangeOverlayColor( Rgba8 newOverlayColor );
	void Update();
	void ProcessInput();
	bool AddCharacterToInput( char character );
	void ResetConsole();

	int GetCurrentInputLength() const																				{ return ( int ) m_currentText.length(); }

protected:

	bool							m_isConsoleOpen			= false;
	Rgba8							m_OverlayColor			= Rgba8( 100 , 100 , 100 , 100 );
	std::vector<ColoredLine>		m_consoleText;
	std::vector<DevConsoleCommand>	m_consoleCommands;
	bool							m_isCommandFound		= false;
	size_t							m_commandSearchIndex	= 0;

private:
	std::string						m_currentText			= "";
	void ExecuteQuit();
	void ExecuteHelp();
};