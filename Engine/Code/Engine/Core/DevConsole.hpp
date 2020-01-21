#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Rgba8.hpp"

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
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void PrintString( const Rgba8& textColor = WHITE, const std::string& devConsolePrintString = "INVALID STRING");
	void Render( RenderContext& renderer , const Camera& camera , float lineHeight ) const;
	
	void SetIsOpen( bool isOpen );
	void ToggleVisibility();
	void Close();
	bool IsOpen() const;

	void ChangeOverlayColor( Rgba8 newOverlayColor );

protected:

	bool						  m_isConsoleOpen = false;
	Rgba8						  m_OverlayColor = Rgba8( 100 , 100 , 100 , 100 );
	std::vector<ColoredLine>	  m_consoleText;
};