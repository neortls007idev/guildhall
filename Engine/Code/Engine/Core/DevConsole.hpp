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

	void Update( float deltaSeconds );

	void PrintString( const Rgba8& textColor = WHITE, const std::string& devConsolePrintString = "INVALID STRING");
	void Render( RenderContext& renderer , const Camera& camera , float lineHeight ) const;
	void RenderPhoenixAnimation( RenderContext& renderer , const Camera& camera , const AABB2& animArea ) const;
	void RenderCatAnimation( RenderContext& renderer , const Camera& camera , const AABB2& animArea ) const;

	void SetIsOpen( bool isOpen );
	void ToggleVisibility();
	void Close();
	bool IsOpen() const;

	void ChangeOverlayColor( Rgba8 newOverlayColor );

protected:

	bool							m_isConsoleOpen				= false;
	Rgba8							m_OverlayColor				= Rgba8( 100 , 100 , 100 , 127 );
	std::vector<ColoredLine>		m_consoleText;
	
	float							m_phoenixAnimationDuration	= 3.8f;
	float							m_currentPhoenixAnimFrame	= 0.f;
	Rgba8							m_devConsoleAnimationColor	= Rgba8( 255 , 255 , 255 , 127 );

	float							m_catAnimationDuration		= 4.4f;
	float							m_currentCatAnimFrame	= 0.f;

	Vec2 m_animUVMins;
	Vec2 m_animUVMaxs;

	Vertex_PCU m_devConsoleAnimVerts[ 6 ] = {

				Vertex_PCU( Vec3( -0.4f , -0.225f , 0.f ) , Rgba8( 255 , 255 , 255 ) , m_animUVMins ),
				Vertex_PCU( Vec3( 0.4f  , -0.225f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( m_animUVMaxs.x , m_animUVMins.y ) ),
				Vertex_PCU( Vec3( -0.4f , 0.225f  , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( m_animUVMins.x , m_animUVMaxs.y ) ),

				Vertex_PCU( Vec3( 0.4f  , -0.225f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( m_animUVMaxs.x , m_animUVMins.y ) ),
				Vertex_PCU( Vec3( 0.4f  ,  0.225f , 0.f ) , Rgba8( 255 , 255 , 255 ) , m_animUVMaxs ),
				Vertex_PCU( Vec3( -0.4f ,  0.225f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( m_animUVMins.x , m_animUVMaxs.y ) ) };
};