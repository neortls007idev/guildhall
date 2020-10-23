#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Core/SimpleTriangleFont.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"

extern DevConsole* g_theDevConsole;

//--------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern AudioSystem*		g_theAudioSystem;
extern TheApp*			g_theApp;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	m_font = g_theRenderer->GetOrCreateBitmapFontFromFile( "Data/Fonts/SquirrelFixedFont" );
	m_worldCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( INTITAL_MAP_SIZE_X , INTITAL_MAP_SIZE_Y ) );
	m_world = new World( this );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	UpdateCamera();

	if( !g_theDevConsole->IsOpen() )
	{
		UpdateFromKeyBoard();
		UpdateGameplayFromXbox();
	}
		
	m_world->Update( deltaSeconds );
}
	
//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateGameplayFromXbox()
{
	m_controllerID = g_theInput->GetXboxController( m_controllerID ).GetControllerID();
	if ( m_controllerID < 0 )
		return;
	const XboxController& controller = g_theInput->GetXboxController( m_controllerID );
	if ( !controller.IsConnected() )
		return;
	const KeyButtonState& startButton = controller.GetButtonState( XBOX_BUTTON_ID_START );
	
	if ( startButton.WasJustPressed() )
	{
		g_theApp->m_isPaused = !g_theApp->m_isPaused;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	RenderGameplay();
}


//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderGameplay() const
{
	if ( m_world->m_currentMapNumber >= 0 && m_world->m_currentMapNumber <= TOTAL_MAPS_IN_THE_GAME )
	{
		g_theRenderer->BindTexture( nullptr );
		m_world->Render();

		if ( !m_world->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ] )
		{
			g_theRenderer->BindTexture( m_textures[ TEXTURE_TILE_TERRAIN_8x8 ] );
			Vec2 textPosition = m_worldCamera.GetOrthoMin().GetXYComponents() +
								( ( m_worldCamera.GetOrthoMax().GetXYComponents() - m_worldCamera.GetOrthoMin().GetXYComponents() ) * 0.5f );
			DrawTextTriangles2D( *g_theRenderer , "GAME OVER" , Vec2( textPosition.x - 4 , textPosition.y ) , 1.5f , RED , 0.5f );
			g_theRenderer->BindTexture( nullptr );
		}
		g_theRenderer->BindTexture( nullptr );
		if ( m_world->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ] )
		{
			//int healthIcons = m_world1->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ][ 0 ]->m_health;
			//for ( healthIcons ; healthIcons > 0; healthIcons-- )
			//{
				
			//}
		}
		if ( g_theApp->m_isPaused )
		{
			m_worldCamera.SetClearMode( CLEAR_NONE , BLACK );
			g_theRenderer->BeginCamera( m_worldCamera );
			g_theRenderer->SetBlendMode( ALPHA );
			g_theRenderer->BindTexture( nullptr );
			AABB2 pausePanel( Vec2::ZERO , Vec2( m_world->m_currentMap->m_size.x , m_world->m_currentMap->m_size.y ) );
			std::vector<Vertex_PCU> pausePanelVerts;
			AppendVertsForAABB2( pausePanelVerts , pausePanel , Rgba8( 0 , 0 , 0 , 150 ) );
			g_theRenderer->DrawVertexArray( pausePanelVerts );
			Vec2 textPosition = m_worldCamera.GetOrthoMin().GetXYComponents() +
							( ( m_worldCamera.GetOrthoMax().GetXYComponents() - m_worldCamera.GetOrthoMin().GetXYComponents() ) * 0.5f );
			DrawTextTriangles2D( *g_theRenderer , "GAME PAUSED" , Vec2( textPosition.x - 4.f , textPosition.y ) , 1.f , YELLOW , 0.5f );
			DrawTextTriangles2D( *g_theRenderer , "PRESS P or START BUTTON TO RESUME" , Vec2( textPosition.x - 3.15f, textPosition.y - 0.5f ) , 0.5f , YELLOW , 0.25f );
			DrawTextTriangles2D( *g_theRenderer , "OR PRESS ESC BUTTON TO RETURN TO THE ATTRACT SCREEN" , Vec2( textPosition.x - 4.5f , textPosition.y - 1.f ) , 0.5f , YELLOW , 0.25f );
			g_theRenderer->EndCamera( m_worldCamera );
			m_worldCamera.SetClearMode( CLEAR_COLOR_BIT , BLACK );
		}

	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCamera()
{
//--------------------------------------------------------------------------------------------------------------------------------------------
//					CLAMPING THE CAMERA 
//--------------------------------------------------------------------------------------------------------------------------------------------

	Entitylist& playerEntityList = m_world->m_currentMap->m_entityListsByType[ PLAYERTANK_ENTITY ];
	static Vec2 tankPosition;
	if ( playerEntityList[0] )
	{
		tankPosition = playerEntityList[ 0 ]->m_position;
	}

	Vec2 cameraCoords;
	
	cameraCoords.x = Clamp( tankPosition.x , ( m_tilesInViewHorizontally / 2 ) , m_world->m_currentMap->m_size.x - ( m_tilesInViewHorizontally / 2 ) );
	cameraCoords.y = Clamp( tankPosition.y , ( m_tilesInViewVertically / 2 )   , m_world->m_currentMap->m_size.y - ( m_tilesInViewVertically   / 2 ) );

	m_worldCamera.SetOrthoView( cameraCoords - m_CameraCenter , cameraCoords + m_CameraCenter );

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

void Game::UpdateFromKeyBoard()
{
	if ( g_theInput->GetButtonState( KEY_F1 ).WasJustPressed() )
	{ m_debugDraw = !m_debugDraw; }
	
	if ( g_theInput->GetButtonState( KEY_F3 ).WasJustPressed() ) 
	{ m_isClipping = !m_isClipping; }
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAssets()
{
	TileDefinition::CreateTileDefinitions();
	LoadAllEntityTextures();
	//LoadAllSounds();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllEntityTextures()
{
	m_textures[ TEXTURE_PLAYER_TANK ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/PlayerTankBase.png" );
	m_textures[ TEXTURE_PLAYER_TURRET ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/PlayerTankTop.png" );
	m_textures[ TEXTURE_ENEMY_TANK1 ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/EnemyTank1.png" );
	m_textures[ TEXTURE_ENEMY_TANK2 ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/EnemyTank2.png" );
	m_textures[ TEXTURE_ENEMY_TANK3 ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/EnemyTank3.png" );
	m_textures[ TEXTURE_ENEMY_TANK4 ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/EnemyTank4.png" );
	m_textures[ TEXTURE_FRIENDLY_TANK1 ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FriendlyTank1.png" );
	m_textures[ TEXTURE_FRIENDLY_TANK2 ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FriendlyTank2.png" );
	m_textures[ TEXTURE_FRIENDLY_TANK3 ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FriendlyTank3.png" );
	m_textures[ TEXTURE_FRIENDLY_TANK4 ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FriendlyTank4.png" );
	m_textures[ TEXTURE_ENEMY_TURRET ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/EnemyTurretTop.png" );
	m_textures[ TEXTURE_ENEMY_TURRET_BASE ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/EnemyTurretBase.png" );
	m_textures[ TEXTURE_FRIENDLY_TURRET_BASE ]		= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FriendlyTurretBase.png" );
	m_textures[ TEXTURE_FRIENDLY_TURRET ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FriendlyTurretTop.png" );
	m_textures[ TEXTURE_NEMESIS_TANK ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/NemesisTankBase.png" );
	m_textures[ TEXTURE_NEMESIS_TURRET ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/NemesisTankTop.png" );
	m_textures[ TEXTURE_BULLET ]					= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Bullet.png" );
	m_textures[ TEXTURE_TILE_TERRAIN_8x8 ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Terrain_8x8.png" );
	m_textures[ TEXTURE_TILE_BOULDER ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Extras_4x4.png" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAllSounds()
{
	m_sounds[ SOUND_GAME_START ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/GoGoGO.mp3" );
	m_sounds[ SOUND_GAME_OVER ]		= g_theAudioSystem->CreateOrGetSound( "Data/Audio/terroristswin.mp3" );
	m_sounds[ SOUND_GAME_WON ]		= g_theAudioSystem->CreateOrGetSound( "Data/Audio/counterterroristswin.mp3" );
	m_sounds[ SOUND_MAP_CHANGE ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/MapExited.mp3" );
	m_sounds[ SOUND_PLAYER_SHOOT ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/PlayerShoot.wav" );
	m_sounds[ SOUND_ENEMY_SHOOT ]	= g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyShoot.wav" );
	m_sounds[ SOUND_ENEMY_SPOTTED ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemySpotted.mp3" );
	m_sounds[ SOUND_BOULDER_MOVED ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/Rock_DebrisFall2.wav" );
	m_sounds[SOUND_PLAYER_DAMAGED]  = g_theAudioSystem->CreateOrGetSound( "Data/Audio/PlayerHit.wav" );
	m_sounds[ SOUND_ENEMY_DAMAGED ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyHit.wav" );
	m_sounds[ SOUND_PLAYER_DIED ]   = g_theAudioSystem->CreateOrGetSound( "Data/Audio/PlayerDied.wav" );
	m_sounds[ SOUND_ENEMY_DIED ]    = g_theAudioSystem->CreateOrGetSound( "Data/Audio/EnemyDied.wav" );

	m_sounds[ SOUND_ATTRACT_MUSIC ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/AttractMusic.mp3" );
	m_sounds[ SOUND_GAME_PAUSED ]   = g_theAudioSystem->CreateOrGetSound( "Data/Audio/Pause.mp3" );
	m_sounds[ SOUND_GAME_UNPAUSED ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/Unpause.mp3" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
