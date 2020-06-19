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
#include "Ball.hpp"
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

Game::~Game()
{
	for ( int index = 0 ; index < NUM_GAME_SS ; index++ )
	{
		delete m_gameSS[ index ];
		m_gameSS[ index ] = nullptr;
	}

	for( int index = 0 ; index < NUM_GAME_TEX ; index++ )
	{
		m_gameTex[ index ] = nullptr;
	}

	for( int index = 0 ; index < m_levels.size() ; index++ )
	{
		delete m_levels[ index ];
		m_levels[ index ] = nullptr;
	}
	
	m_currentLevel = nullptr;
	g_theAudioSystem->StopSound( m_currentBackgroundsound );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAssets()
{
	LoadAllTextures();
	LoadAllSounds();
	LoadAllShaders();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllSounds()
{
	m_sounds[ SFX_BACKGROUND_1 ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/BackgroundMusic/LSCarolOfTheBells.mp3" );
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
	m_gameTex[ TEX_BACKGROUND_FOREST_1 ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Background/ParallaxForestBlue.png" );
	m_gameTex[ TEX_BACKGROUND_FOREST_2 ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Background/enchantedForest.png" );
	m_gameTex[ TEX_BACKGROUND_AURORA_1 ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Background/aurora1.png" );
	
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

	Texture* FlowersTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FlowerSpriteSheet.png" );
	m_gameSS[ SS_VFX_FLOWERS ] = new SpriteSheet( *FlowersTex , IntVec2( 2 , 2 ) );

	Texture* LeavesTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/LeavesSpriteSheet.png" );
	m_gameSS[ SS_VFX_LEAVES ] = new SpriteSheet( *LeavesTex , IntVec2( 3 , 3 ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllShaders()
{
	m_gameShader[ GSHADER_VMOVE ]		= g_theRenderer->GetOrCreateShader( "Data/Shaders/vMovementEffect.hlsl" );
	m_gameShader[ GSHADER_HMOVE ]		= g_theRenderer->GetOrCreateShader( "Data/Shaders/hMovementEffect.hlsl" );
	m_gameShader[ GSHADER_COMBINEIMG ]	= g_theRenderer->GetOrCreateShader( "Data/Shaders/combineImage.hlsl" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::PostGameConstructDataOnce()
{
	TileDefinition::s_definitions.clear();
	MapDefinition::s_definitions.clear();
	
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
	
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			DebugTile
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//m_currentLevel->SpawnNewEntity( TILE , Vec2::ZERO , TileDefinition::s_definitions.at( "NormalOrange" ) );

	int backgroundSoundIndex = g_RNG->RollRandomIntInRange( SFX_BACKGROUND_1 , SFX_BACKGROUND_4 );
	
	m_currentBackgroundsound = g_theAudioSystem->PlaySound( m_sounds[ backgroundSoundIndex ] , true , 0.11f );
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
	g_theRenderer->BeginCamera( m_worldCamera );
	g_theRenderer->SetBlendMode( ALPHA );

	m_currentLevel->Render();

	g_theParticleSystem2D->Render();
	
	if ( m_isDebugDraw )
	{
		Vec2 cameraMins = m_worldCamera.GetOrthoMin().GetXYComponents();
		Vec2 cameraMaxs = m_worldCamera.GetOrthoMax().GetXYComponents();
		g_theRenderer->DrawLine( Vec2( cameraMins.x , 0.f ) , Vec2( cameraMaxs.x , 0.f ) , MAGENTA , 5.f );
		g_theRenderer->DrawLine( Vec2( 0.f , cameraMins.y ) , Vec2( 0.f , cameraMaxs.y ) , MAGENTA , 5.f );
	}

	g_theRenderer->EndCamera( m_worldCamera );
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
	if ( g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_isBallLaunchable = false;
	}
	
	if( g_theInput->WasKeyJustPressed( KEY_F2 ) )
	{
		Entitylist& ballList = m_currentLevel->m_entityListsByType[ BALL ];
		Ball* theBall = ( Ball* ) ballList[ 0 ];
		theBall->m_pos = Vec2::ZERO_ONE * 300.f;
	}
	
	if( g_theInput->WasKeyJustPressed( KEY_F1 ) )
	{
		m_isDebugDraw = !m_isDebugDraw;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------
