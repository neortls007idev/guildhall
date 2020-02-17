#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EventSystem.hpp"
#include <tuple>

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

	void PrintString( const Rgba8& textColor = WHITE , const std::string& devConsolePrintString = "INVALID STRING" );

	void ProcessInput();
	void OnKeyPress( char character );
	bool AddCharacterToInput( char character );
	int	 GetCurrentInputLength() const																				{ return ( int ) m_currentText.length(); }

	void ProcessCommand();
	void ExecuteCommand();
	void CreateCommand( DevConsoleCommand newCommand );
	void CreateCommand( std::string newCommand , std::string commandDescription );
	void CreateCommand( std::string newCommand , std::string commandDescription , EventArgs commandArgs );

	bool ExecuteHelp( EventArgs& commandArgs );
	bool ExecuteQuit( EventArgs& commandArgs );
	bool Close( EventArgs& commandArgs );

	void SetIsOpen( bool isOpen );
	void ToggleVisibility();
	void Close();
	bool IsOpen() const;

	void ChangeOverlayColor( Rgba8 newOverlayColor );
	void ResetConsole();
	void HandleArrowKeys();

private:
	void ExecuteHelp();
	void ExecuteQuit();

protected:

	bool							m_isConsoleOpen			= false;
	Rgba8							m_OverlayColor			= Rgba8( 100 , 100 , 100 , 100 );
	Rgba8							m_carrotColor			= Rgba8( 255 , 255 , 255 , 255 );
	float							m_carrotPosX			= 0.f;
	size_t							m_carrotOffset			= 0;
	std::vector<ColoredLine>		m_consoleText;
	std::vector<DevConsoleCommand>	m_consoleCommands;
	//std::tuple<std::string, std::string , EventArgs> m_consoleCommandsTuple;
	bool							m_isCommandFound		= false;
	size_t							m_commandSearchIndex	= 0;

private:
	std::string						m_currentText			= "";
};