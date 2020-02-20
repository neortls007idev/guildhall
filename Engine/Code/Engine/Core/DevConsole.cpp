#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Time/Time.hpp"
#include "../Input/VirtualKeyboard.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern BitmapFont*	g_bitmapFont;
extern InputSystem* g_theInput;
extern Window* g_theWindow;

//--------------------------------------------------------------------------------------------------------------------------------------------

DevConsole::DevConsole()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::Startup()
{
	g_theDevConsole->PrintString( CYAN , "DEVCONSOLE STARTED" );

	InitializeCommands();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::InitializeCommands()
{
	CreateCommand( "help" , "List All Commands" );
	//g_theEventSystem->SubscribeToEvent( "help" , ExecuteHelp );
	CreateCommand( "quit" , "Quits the Application" );
	CreateCommand( "close" , "Closes the Devconsole" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::BeginFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::EndFrame()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::Shutdown()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::PrintString( const Rgba8& textColor , const std::string& devConsolePrintString )
{
	ColoredLine newLineText;
	newLineText.lineColor = textColor;
	newLineText.text = devConsolePrintString;
	m_consoleText.push_back( newLineText );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::Render( RenderContext& renderer , const Camera& camera , float lineHeight ) const
{
	AABB2 consoleArea = AABB2( camera.GetOrthoBottomLeft() , camera.GetOrthoTopRight() );
	AABB2 typingArea = consoleArea.CarveBoxOffBottom( 0.075f , 0.f );
	consoleArea = consoleArea.CarveBoxOffTop( 0.925f , 0.f );

	Vec2 caratDimensions = typingArea.GetDimensions();
	

	AABB2 carat = typingArea.GetBoxAtLeft( 0.995f , 0.f );
	carat.SetDimensions( carat.GetDimensions() * 0.7f );
	carat.AlignWithinAABB2( typingArea , ALIGN_CENTERED_LEFT );

	float offsetBetweenLines = 0.75f;

	float dimensionOfConsole = camera.GetOrthoTopRight().y - camera.GetOrthoBottomLeft().y;
	int numberOfLinesToDisplay = RoundDownToInt( dimensionOfConsole / ( lineHeight + offsetBetweenLines) );
	Vec2 startMins = camera.GetOrthoBottomLeft();
	int myStringIndex = ( int ) m_consoleText.size() - 1;
	Vec2 alignment = ALIGN_BOTTOM_LEFT;
	float alignmentDeltaChange = 0.f;
	
	renderer.DrawAABB2( consoleArea , m_OverlayColor );
	renderer.DrawAABB2( typingArea , Rgba8( 0 , 0 , 255 , 100 ) );

	/*MoveCarrot( lineHeight );*/

	float translateCaratX = ( m_currentText.length() - m_carrotOffset ) * lineHeight;
	carat.Translate( Vec2( translateCaratX , 0.f ) );
	renderer.DrawAABB2( carat , m_carrotColor );
	
	std::vector<Vertex_PCU> consoleTextVerts;

	for ( int index = 0; index < numberOfLinesToDisplay; index++ )
	{
		if ( myStringIndex < 0 )
		{
			break;
		}

		g_bitmapFont->AddVertsForTextInBox2D( consoleTextVerts , consoleArea , lineHeight , m_consoleText[ myStringIndex ].text , m_consoleText[ myStringIndex ].lineColor , 1.f , alignment );
		myStringIndex--;

		alignmentDeltaChange += ( offsetBetweenLines );

		alignment.y = RangeMapFloat( 0.f , ( float ) numberOfLinesToDisplay , 0.f , 1.f , alignmentDeltaChange );
	}
	std::vector<Vertex_PCU> curretnTextVerts;
	g_bitmapFont->AddVertsForTextInBox2D( curretnTextVerts , typingArea , lineHeight , m_currentText , WHITE , 1.f , ALIGN_CENTERED_LEFT );

	renderer.BindTexture( g_bitmapFont->GetTexture() );
	if ( consoleTextVerts.size() > 0)
	{
		renderer.DrawVertexArray( consoleTextVerts );
	}

	if ( curretnTextVerts.size() > 0 )
	{
		renderer.DrawVertexArray( curretnTextVerts );
	}

	renderer.BindTexture( nullptr );
	curretnTextVerts.clear();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::OnKeyPress( char character )
{
	ColoredLine newLineText;
	newLineText.lineColor = WHITE;
	newLineText.text = m_currentText;

	if ( character == 13 )
	{
		m_consoleText.push_back( newLineText );
	}
}

void DevConsole::ResetCurrentInput()
{
	m_currentText.clear(); 
	m_carrotOffset = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::ProcessCommand()
{
	m_isCommandFound = false;
	
	for ( m_commandSearchIndex = 0; m_commandSearchIndex < m_consoleCommands.size(); m_commandSearchIndex++ )
	{
		if ( StringCompare( m_currentText , m_consoleCommands[ m_commandSearchIndex ].command ) )
		{
			m_isCommandFound = true;
			break;
		}
		else
		{
			m_isCommandFound = false;
		}
	}

	if ( !m_isCommandFound )
	{
		PrintString( RED , "Invalid Console Command :- Use \"help\" command  " );
	}
	else
	{
		ExecuteCommand();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::ExecuteCommand()
{
	if ( m_isCommandFound )
	{
		if ( m_commandSearchIndex == 0 )
		{
			ExecuteHelp();
		}

		if ( m_commandSearchIndex == 1 )
		{
			ExecuteQuit();
		}

		if ( m_commandSearchIndex == 2 )
		{
			Close();
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::CreateCommand( DevConsoleCommand newCommand )
{
	m_consoleCommands.push_back( newCommand );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::CreateCommand( std::string newCommand , std::string commandDescription )
{
	DevConsoleCommand newConsoleCommand;
	newConsoleCommand.command = newCommand;
	newConsoleCommand.Description = commandDescription;
	m_consoleCommands.push_back( newConsoleCommand );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::CreateCommand( std::string newCommand , std::string commandDescription , EventArgs commandArgs )
{
	DevConsoleCommand newConsoleCommand;
	newConsoleCommand.command = newCommand;
	newConsoleCommand.Description = commandDescription;
	newConsoleCommand.commandArgs = commandArgs;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::SetIsOpen( bool isOpen )
{
	m_isConsoleOpen = isOpen;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::ToggleVisibility()
{
	ResetConsole();
	m_isConsoleOpen = !m_isConsoleOpen;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::Close()
{
	m_isConsoleOpen = false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DevConsole::Close( EventArgs& commandArgs )
{
	UNUSED( commandArgs );
	Close();
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DevConsole::IsOpen() const
{
	return m_isConsoleOpen;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::ChangeOverlayColor( Rgba8 newOverlayColor )
{
	m_OverlayColor = newOverlayColor;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::Update()
{
	ProcessInput();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::ProcessInput()
{
	char character;
	m_carrotColor.a = ( uchar ) RangeMapFloat( -1.f , 1.f , 0 , 255 , SinDegrees( 127.f * ( float ) GetCurrentTimeSeconds() ) );
	//HandleArrowKeys();
		
	size_t curStringLength = m_currentText.length();

	if ( g_theInput->WasKeyJustPressed( KEY_DELETE ) && ( curStringLength - m_carrotOffset ) < curStringLength )
	{
		m_currentText.erase( ( curStringLength - m_carrotOffset ) , 1 );
		m_carrotColor.a = 255;
		m_carrotOffset--;
		return;
	}

	while ( g_theInput->PopCharacter( &character ) )
	{
		if ( character == EASCII_ESCAPE )
		{
			return;
		}

		if ( character == EASCII_RETURNCARRIAGE || character == EASCII_NEWLINE )
		{
			ProcessCommand();
			m_currentText = "";
			m_carrotColor.a = 255;
			m_carrotOffset = 0;
			break;
		}

		curStringLength = m_currentText.length();

		if ( character == EASCII_BACKSPACE && ( curStringLength - m_carrotOffset ) > 0 )
		{
			m_currentText.erase( curStringLength - m_carrotOffset - 1 , 1 );
			m_carrotColor.a = 255;
			break;
		}
		AddCharacterToInput( character );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DevConsole::AddCharacterToInput( char character )
{
	size_t curStringLength = m_currentText.length();

	if ( character != 8 && character != 127 )
	{
		m_currentText.insert( curStringLength - m_carrotOffset , 1 , character );/* = character;*/
		//m_currentText.insert()
		m_carrotColor.a = 255;
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::ResetConsole()
{
	while ( g_theInput->m_characters.size() > 0 )
	{
		g_theInput->m_characters.pop();
	}
	m_consoleText.clear();
	m_currentText = "";
	m_carrotOffset = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::HandleArrowKeys()
{
	size_t curTextLength = m_currentText.length();

	if ( curTextLength - m_carrotOffset > 0 )
	{
		if ( g_theInput->WasKeyJustPressed( KEY_LEFTARROW ) || g_theInput->IsKeyHeldDown( KEY_LEFTARROW ) )
		{
			m_carrotOffset += 1;
		}
	}
	if ( curTextLength - m_carrotOffset < curTextLength )
	{
		if ( g_theInput->WasKeyJustPressed( KEY_RIGHTARROW ) || g_theInput->IsKeyHeldDown( KEY_RIGHTARROW ) )
		{
			m_carrotOffset -= 1;
			//translateCaratX += lineHeight;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::HandleInput( unsigned char keycode )
{
	size_t curTextLength = m_currentText.length();

	if ( curTextLength - m_carrotOffset > 0 )
	{
		if ( keycode == KEY_LEFTARROW  )
		{
			m_carrotOffset += 1;
		}
	}
	if ( curTextLength - m_carrotOffset < curTextLength )
	{
		if ( keycode == KEY_RIGHTARROW )
		{
			m_carrotOffset -= 1;
			//translateCaratX += lineHeight;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::ExecuteQuit()
{
	g_theWindow->HandleQuitRequested();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DevConsole::ExecuteQuit( EventArgs& commandArgs )
{
	UNUSED( commandArgs );
	ExecuteQuit();
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::ExecuteHelp()
{
	for ( m_commandSearchIndex = 0; m_commandSearchIndex < m_consoleCommands.size(); m_commandSearchIndex++ )
	{
		PrintString( GREEN , m_consoleCommands[ m_commandSearchIndex ].command + " : " + m_consoleCommands[ m_commandSearchIndex ].Description );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DevConsole::ExecuteHelp( EventArgs& commandArgs )
{
	UNUSED( commandArgs );
	ExecuteHelp();
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------