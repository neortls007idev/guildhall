#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern BitmapFont* g_bitmapFont;
extern InputSystem* g_theInput;

//--------------------------------------------------------------------------------------------------------------------------------------------

DevConsole::DevConsole()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::Startup()
{

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
	
	//renderer.BeginCamera( camera );
	
	renderer.DrawAABB2( consoleArea , m_OverlayColor );
	renderer.DrawAABB2( typingArea , Rgba8( 0 , 0 , 255 , 100 ) );

	float translateCaratX = m_currentText.length() * lineHeight;
	carat.Translate( Vec2( translateCaratX , 0.f ) );
	renderer.DrawAABB2( carat , Rgba8( 255 , 0 , 255 , 100 ) );
	
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

	while ( g_theInput->PopCharacter( &character ) )
	{
		if ( character == 13 )
		{
			m_currentText = "";
			break;
		}

		if ( character == 8 && m_currentText.size() > 0 )
		{
			m_currentText.pop_back();
			break;
		}
		AddCharacterToInput( character );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool DevConsole::AddCharacterToInput( char character )
{
	m_currentText += character;

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
}

//--------------------------------------------------------------------------------------------------------------------------------------------