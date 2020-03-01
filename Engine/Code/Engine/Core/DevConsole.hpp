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
	~DevConsole();
	void Startup();

	void InitializeCommands();

	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void Update( float deltaSeconds );

	void Render( RenderContext& renderer , const Camera& camera , float lineHeight ) const;
	void RenderPhoenixAnimation( RenderContext& renderer , const Camera& camera , const AABB2& animArea ) const;
	void RenderCatAnimation( RenderContext& renderer , const Camera& camera , const AABB2& animArea ) const;


	//void PrintString( const Rgba8& textColor = WHITE, const std::string& devConsolePrintString = "INVALID STRING");
	static void PrintString( const Rgba8& textColor = WHITE , const std::string& devConsolePrintString = "INVALID STRING" ,
								eDevConsoleMessageType messageType= DEVCONSOLE_USERINPUT );

	static void PrintString( const std::string& devConsolePrintString = "INVALID STRING" , eDevConsoleMessageType messageType= DEVCONSOLE_USERINPUT );

	void ProcessInput();
	void OnKeyPress( char character );
	bool AddCharacterToInput( char character );
	int	 GetCurrentInputLength() const																{ return ( int ) m_currentText.length(); }
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

	Camera* GetDevConsoleCamera() const																{ return m_devConsoleCamera; }

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
	Vec2 m_animUVMaxs;
	Vec2 m_animUVMins;
	float							m_catAnimationDuration		= 4.4f;
	float							m_currentCatAnimFrame		= 0.f;
	float							m_phoenixAnimationDuration	= 3.8f;
	float							m_currentPhoenixAnimFrame	= 0.f;
	Rgba8							m_devConsoleAnimationColor	= Rgba8( 255 , 255 , 255 , 127 );
	Camera*							m_devConsoleCamera			= nullptr;
	Vertex_PCU						m_devConsoleAnimVerts[ 6 ]	= {

				Vertex_PCU( Vec3( -0.4f , -0.225f , 0.f ) , Rgba8( 255 , 255 , 255 ) , m_animUVMins ),
				Vertex_PCU( Vec3( 0.4f  , -0.225f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( m_animUVMaxs.x , m_animUVMins.y ) ),
				Vertex_PCU( Vec3( -0.4f , 0.225f  , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( m_animUVMins.x , m_animUVMaxs.y ) ),

				Vertex_PCU( Vec3( 0.4f  , -0.225f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( m_animUVMaxs.x , m_animUVMins.y ) ),
				Vertex_PCU( Vec3( 0.4f  ,  0.225f , 0.f ) , Rgba8( 255 , 255 , 255 ) , m_animUVMaxs ),
				Vertex_PCU( Vec3( -0.4f ,  0.225f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( m_animUVMins.x , m_animUVMaxs.y ) )
	};

};