#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/MathUtils.hpp"


//--------------------------------------------------------------------------------------------------------------------------------------------

extern BitmapFont* g_bitmapFont;

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
	Vec2 cameraBottomLeft	= camera.GetPosition() - ( camera.GetOutputSize() / 2.f );
	Vec2 cameraTopRight		= camera.GetPosition() + ( camera.GetOutputSize() / 2.f );
	AABB2 consoleArea		= AABB2( cameraBottomLeft , cameraTopRight );
	float offsetBetweenLines = 1.f;

	float dimensionOfConsole = cameraTopRight.y - cameraBottomLeft.y;
	int numberOfLinesToDisplay = RoundDownToInt( dimensionOfConsole / ( lineHeight + offsetBetweenLines) );
	Vec2 startMins = cameraBottomLeft;
	int myStringIndex = ( int ) m_consoleText.size() - 1;
	Vec2 alignment = ALIGN_BOTTOM_LEFT;
	float alignmentDeltaChange = 0.f;
	
	//renderer.BeginCamera( camera );
	
	renderer.DrawAABB2( consoleArea , m_OverlayColor );
	std::vector<Vertex_PCU> consoleTextVerts;

	for ( int index = 0; index < numberOfLinesToDisplay; index++ )
	{
		if ( myStringIndex < 0 )
		{
			break;
		}

		g_bitmapFont->AddVertsForTextInBox2D( consoleTextVerts , consoleArea , lineHeight , m_consoleText[ myStringIndex ].text , m_consoleText[ myStringIndex ].lineColor , 1.f , alignment );
		myStringIndex--;
				
		//startMins.y += lineHeight;
		alignmentDeltaChange += ( offsetBetweenLines /*+ 20.f */);

		alignment.y = RangeMapFloat( 0.f , ( float ) numberOfLinesToDisplay , 0.f , 1.f , alignmentDeltaChange );
	}

	renderer.BindTexture( g_bitmapFont->GetTexture() );
	if ( consoleTextVerts.size() > 0)
	{
		renderer.DrawVertexArray( consoleTextVerts );
	}
	renderer.BindTexture( nullptr );

	//renderer.EndCamera( camera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::SetIsOpen( bool isOpen )
{
	m_isConsoleOpen = isOpen;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::ToggleVisibility()
{
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