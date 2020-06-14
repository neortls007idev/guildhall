#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/ParticleSystem/ParticleSystem2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TheApp.hpp"
#include "Game/TileDefinition.hpp"
//#include <utility>

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*		g_theRenderer;
extern BitmapFont*			g_bitmapFont;
extern AudioSystem*			g_theAudioSystem;
extern TheApp*				g_theApp;
extern InputSystem*			g_theInput;
extern ParticleSystem2D*	g_theParticleSystem2D;

//--------------------------------------------------------------------------------------------------------------------------------------------

SpriteSheet* g_tileSpriteSheet		= nullptr;
SpriteSheet* g_characterSpriteSheet = nullptr;
extern	BallTexEnumRGBA8Map	g_theBallTexTable[ NUM_GAME_TEX ];

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	g_theInput->PushCursorSettings( CursorSettings( RELATIVE_MODE , MOUSE_IS_WINDOWLOCKED , false ) );
	float cameraHalfHeight	= g_gameConfigBlackboard.GetValue( "cameraHalfHeight" , 540.f );
	float cameraAspectRatio = g_gameConfigBlackboard.GetValue( "windowAspect" , 1.77f );
	
	m_worldCamera.SetOrthoView( cameraHalfHeight , cameraAspectRatio );
	LoadAssets();	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAssets()
{
	LoadAllTextures();
	LoadAllSounds();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllSounds()
{
	//m_sounds[ SFX_BACKGROUND_1 ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/WindsweptRealms.mp3" );
	m_sounds[ SFX_BACKGROUND_2 ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/LSCrystallize.mp3" );
	m_sounds[ SFX_BACKGROUND_3 ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/LSCelticCarol.mp3" );
	m_sounds[ SFX_BACKGROUND_4 ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/LSSenbonzakura.mp3" );
	
	m_sounds[ SFX_LEAVES_RUSTLE ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/Rustle/rustle1.mp3" );
	
	m_sounds[ SFX_GLASS_BREAK_1 ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/Shatter/glassBreak1.mp3" );
	m_sounds[ SFX_GLASS_BREAK_2 ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/Shatter/LightBulbBreaking.wav" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllTextures()
{
	m_gameTex[ TEX_BACKGROUND_FOREST ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Background/ParallaxForestBlue.png" );
	m_gameTex[ TEX_PADDLE ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Paddle/Paddle.png" );
	m_gameTex[ TEX_LEFT_WALL ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Bounds/LeftWall.png" );
	m_gameTex[ TEX_RIGHT_WALL ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Bounds/RightWall.png" );
	m_gameTex[ TEX_TOP_WALL_SECTION ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Bounds/TopWallSection.png" );

	m_gameTex[ TEX_BALL_RED ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Balls/02.png" );
	m_gameTex[ TEX_BALL_GREEN ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Balls/06.png" );
	m_gameTex[ TEX_BALL_BLUE ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Balls/35.png" );
	m_gameTex[ TEX_BALL_YELLOW ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Balls/07.png" );
	m_gameTex[ TEX_BALL_MAGENTA ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Balls/04.png" );
	m_gameTex[ TEX_BALL_CYAN ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Balls/25.png" );
	m_gameTex[ TEX_BALL_PINK ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Balls/20.png" );
	m_gameTex[ TEX_BALL_PURPLE ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Balls/09.png" );
	m_gameTex[ TEX_BALL_ORANGE ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Balls/17.png" );
	m_gameTex[ TEX_BALL_GREY ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Balls/34.png" );

	m_gameTex[ TEX_FLARE_RED ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/VFX/GlareFlare/01.png" );
	m_gameTex[ TEX_FLARE_GREEN ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/VFX/GlareFlare/04_02.png" );
	m_gameTex[ TEX_FLARE_BLUE ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/VFX/GlareFlare/08_01.png" );
	m_gameTex[ TEX_FLARE_YELLOW ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/VFX/GlareFlare/02.png" );
	m_gameTex[ TEX_FLARE_MAGENTA ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/VFX/GlareFlare/12.png" );
	m_gameTex[ TEX_FLARE_CYAN ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/VFX/GlareFlare/03_02.png" );
	m_gameTex[ TEX_FLARE_PINK ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/VFX/GlareFlare/07.png" );
	m_gameTex[ TEX_FLARE_PURPLE ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/VFX/GlareFlare/05.png" );
	m_gameTex[ TEX_FLARE_ORANGE ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/VFX/GlareFlare/06.png" );
	m_gameTex[ TEX_FLARE_GREY ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/VFX/GlareFlare/10_01.png" );
	 
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::PostGameConstructDataOnce()
{
	TileDefinition::CreateTileDefinitions( "Data/GamePlay/TileDefs.xml" );
	MapDefinition::CreateMapDefinitions( "Data/GamePlay/MapDefs.xml" );
	
	g_theBallTexTable[ TEX_BALL_RED		].texture		= TEX_BALL_RED;
	g_theBallTexTable[ TEX_BALL_GREEN	].texture		= TEX_BALL_GREEN;
	g_theBallTexTable[ TEX_BALL_BLUE	].texture		= TEX_BALL_BLUE;
	g_theBallTexTable[ TEX_BALL_YELLOW	].texture		= TEX_BALL_YELLOW;
	g_theBallTexTable[ TEX_BALL_MAGENTA ].texture		= TEX_BALL_MAGENTA;
	g_theBallTexTable[ TEX_BALL_CYAN	].texture		= TEX_BALL_CYAN;
	g_theBallTexTable[ TEX_BALL_PINK	].texture		= TEX_BALL_PINK;
	g_theBallTexTable[ TEX_BALL_PURPLE	].texture		= TEX_BALL_PURPLE;
	g_theBallTexTable[ TEX_BALL_ORANGE	].texture		= TEX_BALL_ORANGE;
	g_theBallTexTable[ TEX_BALL_GREY	].texture		= TEX_BALL_GREY;

	g_theBallTexTable[ TEX_BALL_RED		].color			= RED;
	g_theBallTexTable[ TEX_BALL_GREEN	].color			= GREEN;
	g_theBallTexTable[ TEX_BALL_BLUE	].color			= BLUE;
	g_theBallTexTable[ TEX_BALL_YELLOW	].color			= YELLOW;
	g_theBallTexTable[ TEX_BALL_MAGENTA ].color			= MAGENTA;
	g_theBallTexTable[ TEX_BALL_CYAN	].color			= CYAN;
	g_theBallTexTable[ TEX_BALL_PINK	].color			= PINK;
	g_theBallTexTable[ TEX_BALL_PURPLE	].color			= PURPLE;
	g_theBallTexTable[ TEX_BALL_ORANGE	].color			= ORANGE;
	g_theBallTexTable[ TEX_BALL_GREY	].color			= GRAY;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::PostGameConstruct()
{
	for( auto totalMaps = MapDefinition::s_definitions.cbegin(); totalMaps != MapDefinition::s_definitions.cend(); totalMaps++ )
	{
		Map* temp = new Map( this , totalMaps->second , totalMaps->second->m_name );
		m_levels.push_back( temp );
	}

	m_currentLevel = m_levels[ 0 ];

	m_currentLevel->SpawnNewEntity( PADDLE , Vec2::ZERO );
	m_currentLevel->SpawnNewEntity( BALL , Vec2::ZERO_ONE * 300.f );

	//Vec2 testTilePos = Vec2( 100.f , 100.f );
	//
	//m_currentLevel->SpawnNewEntity( TILE , 0 * testTilePos , TileDefinition::s_definitions.at( "NormalYellow" ) );
	//m_currentLevel->SpawnNewEntity( TILE , 1 * testTilePos , TileDefinition::s_definitions.at( "NormalPurple" ) );
	//m_currentLevel->SpawnNewEntity( TILE , 2 * testTilePos , TileDefinition::s_definitions.at( "NormalOrange" ) );
	//m_currentLevel->SpawnNewEntity( TILE , 3 * testTilePos , TileDefinition::s_definitions.at( "SteelGrey" ) );

	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			DebugTile
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//m_currentLevel->SpawnNewEntity( TILE , Vec2::ZERO , TileDefinition::s_definitions.at( "NormalOrange" ) );

	g_theAudioSystem->PlaySound( m_sounds[ SFX_BACKGROUND_4 ] , true , 0.11f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
//	UpdateCamera();
	m_currentLevel->Update( deltaSeconds );
	UpdateFromKeyBoard();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	m_currentLevel->Render();

	g_theParticleSystem2D->Render();
	
	if ( m_isDebugDraw )
	{
		Vec2 cameraMins = m_worldCamera.GetOrthoMin().GetXYComponents();
		Vec2 cameraMaxs = m_worldCamera.GetOrthoMax().GetXYComponents();
		g_theRenderer->DrawLine( Vec2( cameraMins.x , 0.f ) , Vec2( cameraMaxs.x , 0.f ) , MAGENTA , 5.f );
		g_theRenderer->DrawLine( Vec2( 0.f , cameraMins.y ) , Vec2( 0.f , cameraMaxs.y ) , MAGENTA , 5.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCamera()
{
//--------------------------------------------------------------------------------------------------------------------------------------------
//					CLAMPING THE CAMERA 
//--------------------------------------------------------------------------------------------------------------------------------------------

	Vec2 cameraCoords;

	m_screenShakeIntensity -= SCREEN_SHAKE_ABLATION_PER_SECOND;
	m_screenShakeIntensity = ClampZeroToOne( m_screenShakeIntensity );

	float maxShakeDist = m_screenShakeIntensity * MAX_CAMERA_SHAKE;
	float cameraShakeX = static_cast< float > ( g_RNG->RollRandomIntInRange( ( int ) -maxShakeDist , ( int ) maxShakeDist ) );
	float cameraShakeY = static_cast< float > ( g_RNG->RollRandomIntInRange( ( int ) -maxShakeDist , ( int ) maxShakeDist ) );
	
	m_worldCamera.Translate2D( Vec2( cameraShakeX , cameraShakeY ) );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::AddScreenShakeIntensity( float deltaShakeIntensity )
{
	m_screenShakeIntensity += deltaShakeIntensity;
	//clamp it!
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Die()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

SoundPlaybackID Game::GetSFX( eGameAudioFX SFXid ) const
{
	return m_sounds[ SFXid ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateFromKeyBoard()
{
	if( g_theInput->WasKeyJustPressed( KEY_F1 ) )
	{
		m_isDebugDraw = !m_isDebugDraw;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------
