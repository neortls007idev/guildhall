#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "VertexUtils.hpp"

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

void DevConsole::Update( float deltaSeconds )
{
	m_currentPhoenixAnimFrame += deltaSeconds;
	if ( m_currentPhoenixAnimFrame >= m_phoenixAnimationDuration )
	{
		m_currentPhoenixAnimFrame = 0.f;
	}

	m_currentCatAnimFrame += deltaSeconds;
	if ( m_currentCatAnimFrame >= m_catAnimationDuration )
	{
		m_currentCatAnimFrame = 0.f;
	}
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
	
	AABB2 devConsolePhoenixAnimArea = consoleArea.GetBoxAtTop( 0.5f , 0.f ).GetBoxAtRight( 0.5f , 0.f );
	RenderPhoenixAnimation( renderer , camera , devConsolePhoenixAnimArea );

	AABB2 devConsoleCatAnimArea = consoleArea.GetBoxAtBottom( 0.5f , 0.f ).GetBoxAtRight( 0.5f , 0.f );
	RenderCatAnimation( renderer , camera , devConsoleCatAnimArea );

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

void DevConsole::RenderPhoenixAnimation( RenderContext& renderer , const Camera& camera, const AABB2& animArea ) const
{
	UNUSED( camera );
	Texture* texture = renderer.GetOrCreateTextureFromFile( "Data/DevConsole/devconsolePhoenixSpriteSheet4.png" );
	SpriteSheet* spriteSheet = new SpriteSheet( *texture , IntVec2( 5 , 8 ) );
	SpriteAnimDefinition anim = SpriteAnimDefinition( *spriteSheet , 0 , 39 , m_phoenixAnimationDuration , SpriteAnimPlaybackType::LOOP );
	const SpriteDefinition& devConsoleAnim = anim.GetSpriteDefAtTime( m_currentPhoenixAnimFrame );
	Vec2 uvMins;
	Vec2 uvMaxs;
	devConsoleAnim.GetUVs( uvMins , uvMaxs );
	
	Vertex_PCU tempDevConsoleAnim[ 6 ];
	for ( int index = 0; index < 6; index++ )
	{

		tempDevConsoleAnim[ index ]					= m_devConsoleAnimVerts[ index ];
		tempDevConsoleAnim[ index ].m_color			= m_devConsoleAnimationColor;
	}
	tempDevConsoleAnim[ 0 ].m_uvTexCoords = uvMins;
	tempDevConsoleAnim[ 1 ].m_uvTexCoords = Vec2( uvMaxs.x , uvMins.y );
	tempDevConsoleAnim[ 2 ].m_uvTexCoords = Vec2( uvMins.x , uvMaxs.y );

	tempDevConsoleAnim[ 3 ].m_uvTexCoords = Vec2( uvMaxs.x , uvMins.y );
	tempDevConsoleAnim[ 4 ].m_uvTexCoords = uvMaxs;
	tempDevConsoleAnim[ 5 ].m_uvTexCoords = Vec2( uvMins.x , uvMaxs.y );
	
	Vec2 translateBy = animArea.GetCenter() + ( animArea.m_maxs - animArea.GetCenter() ) / 2.f;

	TransformVertexArray2D( 6 , tempDevConsoleAnim , animArea.GetDimensions().y , 0.f , animArea.GetCenter() + ( animArea.m_maxs - animArea.GetCenter() ) / 2.f );
	renderer.BindTexture( texture );
	renderer.SetBlendMode( BlendMode::ADDITIVE );
	renderer.DrawVertexArray( 6 , tempDevConsoleAnim );
	renderer.SetBlendMode( BlendMode::ALPHA );
	renderer.BindTexture( nullptr );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void DevConsole::RenderCatAnimation( RenderContext& renderer , const Camera& camera , const AABB2& animArea ) const
{
	UNUSED( camera );
	Texture* texture = renderer.GetOrCreateTextureFromFile( "Data/DevConsole/GamerCatHFlippedSpriteSheet1.png" );
	SpriteSheet* spriteSheet = new SpriteSheet( *texture , IntVec2( 3 , 11 ) );
	SpriteAnimDefinition anim = SpriteAnimDefinition( *spriteSheet , 0 , 32 , m_catAnimationDuration , SpriteAnimPlaybackType::LOOP );
	const SpriteDefinition& devConsoleAnim = anim.GetSpriteDefAtTime( m_currentCatAnimFrame );
	Vec2 uvMins;
	Vec2 uvMaxs;
	devConsoleAnim.GetUVs( uvMins , uvMaxs );

	Vertex_PCU tempDevConsoleAnim[ 6 ];
	for ( int index = 0; index < 6; index++ )
	{

		tempDevConsoleAnim[ index ] = m_devConsoleAnimVerts[ index ];
		tempDevConsoleAnim[ index ].m_color = m_devConsoleAnimationColor;
	}
	tempDevConsoleAnim[ 0 ].m_uvTexCoords = uvMins;
	tempDevConsoleAnim[ 1 ].m_uvTexCoords = Vec2( uvMaxs.x , uvMins.y );
	tempDevConsoleAnim[ 2 ].m_uvTexCoords = Vec2( uvMins.x , uvMaxs.y );

	tempDevConsoleAnim[ 3 ].m_uvTexCoords = Vec2( uvMaxs.x , uvMins.y );
	tempDevConsoleAnim[ 4 ].m_uvTexCoords = uvMaxs;
	tempDevConsoleAnim[ 5 ].m_uvTexCoords = Vec2( uvMins.x , uvMaxs.y );

	Vec2 translateBy = animArea.m_mins + ( animArea.m_maxs - animArea.GetCenter() ) / 2.f ;
	translateBy.x -= 200.f;

	TransformVertexArray2D( 6 , tempDevConsoleAnim , animArea.GetDimensions().y , 0.f , translateBy );
	renderer.BindTexture( texture );
	renderer.SetBlendMode( BlendMode::ALPHA );
	renderer.DrawVertexArray( 6 , tempDevConsoleAnim );
	renderer.SetBlendMode( BlendMode::ALPHA );
	renderer.BindTexture( nullptr );
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