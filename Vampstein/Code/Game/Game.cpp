﻿#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/VertexMaster.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Primitives/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Time/Time.hpp"
#include "Game/Game.hpp"

#include "MapMaterial.hpp"
#include "MapRegion.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "Game/TileMap.hpp"
#include "World.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern AudioSystem*		g_theAudioSystem;
extern RenderContext*	g_theRenderer;
extern TheApp*			g_theApp;
extern DevConsole*		g_theDevConsole;

bool   s_wasDataLoaded = false;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	g_theInput->PushCursorSettings( CursorSettings( RELATIVE_MODE , MOUSE_IS_WINDOWLOCKED , false ) );
	
	if ( !s_wasDataLoaded )
	{
		LoadShaders();
		LoadTextures();
		LoadAudio();
	}
	
	InitializeCameras();
	IntializeGameObjects();
	InitializeObjectTransforms();
	InitializeHUDElements();
	
	std::string gameConfigData = g_gameConfigBlackboard.GetValue( "testkey" , "Invalid Value" );
	if( gameConfigData == "Invalid Value" )
	{
		g_theDevConsole->PrintString( gameConfigData , eDevConsoleMessageType::DEVCONSOLE_ERROR );
	}
	else
	{
		g_theDevConsole->PrintString( gameConfigData , eDevConsoleMessageType::DEVCONSOLE_SYTEMLOG );
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			WORLD/MAP CONSTRUCTION
	//--------------------------------------------------------------------------------------------------------------------------------------------
	MapMaterial::LoadDefinitions( "Data/Definitions/MapMaterialTypes.xml" );
	MapRegion::LoadDefinitions( "Data/Definitions/MapRegionTypes.xml" );
			
	//m_testMap = new TileMap( "Test" , IntVec2( 5 , 5 ) );
	m_testMap =	TileMap::CreateTileMapFromXml( "TestXMLMap" , "Data/Maps/TestRoom.xml" );
	m_pointSampler = g_theRenderer->GetOrCreateSampler( SAMPLER_POINT );

	m_world = new World( this , "TheWorld" , "Data/Maps" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::InitializeObjectTransforms()
{
	m_cubeMesh1Transform.SetPosition( 2.5f , 0.5f , 0.5f );
	m_cubeMesh1Transform.SetScale( 1.f , 1.f , 1.f );
	m_cubeMesh2Transform.SetPosition( 0.5f , 2.5f , 0.5f );
	m_cubeMesh1Transform.SetScale( 1.f , 1.f , 1.f );
	m_cubeMesh3Transform.SetPosition( 2.5f , 2.5f , 0.5f );
	m_cubeMesh1Transform.SetScale( 1.f , 1.f , 1.f );
	m_basisMeshTransform.SetPosition( 0.f , 0.f , 0.f );

	Mat44 cameraTransform = m_gameCamera.GetCameraTransform().GetAsMatrix();
	Vec3 forwardVector = cameraTransform.GetIBasis3D();
	m_compassMeshTransform.SetPosition( m_gameCamera.GetPosition() + 0.1f * forwardVector );
	m_compassMeshTransform.SetScale( 0.01f , 0.01f , 0.01f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadShaders()
{
	m_imageEffectShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/selectiveImageEffect.hlsl" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadTextures()
{
	m_textures[ TEST_TEXTURE ]			= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	m_textures[ HUD_BASE ]				= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Hud_Base.png" );
	m_textures[ TERRAIN_SPRITE_SHEET ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Terrain_8x8.png" );
	m_textures[ PLAYER_SPRITE_SHEET ]	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/ViewModelsSpriteSheet_8x8.png" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAudio()
{
	m_sounds[ TEST_SOUND ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/TestSound.mp3" );
	m_sounds[ TELEPORTER ] = g_theAudioSystem->CreateOrGetSound( "Data/Audio/Teleporter.wav" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::IntializeGameObjects()
{
	m_cubeMesh = new GPUMesh( g_theRenderer ); 
	std::vector<VertexMaster>	cubeMeshVertsMaster;
	std::vector<Vertex_PCU>		cubeMeshVerts;
	std::vector<uint>			cubeMeshIndices;

	AABB3 box( Vec3( -0.5f, -0.5f , -0.5f ) , Vec3( 0.5f , 0.5f , 0.5f ) );
	CreateCuboidXInYLeftZUp( cubeMeshVertsMaster , cubeMeshIndices , box , WHITE );
	VertexMaster::ConvertVertexMasterToVertexPCU( cubeMeshVerts , cubeMeshVertsMaster );
	
	m_cubeMesh->UpdateVertices( ( uint ) cubeMeshVerts.size() , cubeMeshVerts.data() );
	m_cubeMesh->UpdateIndices( cubeMeshIndices );

// 	//----------------------------------------------------------------------------------------------------------

	Mat44 cameraTransform	= m_gameCamera.GetCameraTransform().GetAsMatrix();
	Vec3 forwardVector		= cameraTransform.GetIBasis3D();
	Vec3 rightVector		= cameraTransform.GetJBasis3D();
	Vec3 upVector			= cameraTransform.GetKBasis3D();

	constexpr float basisShaftRadius	= 0.01f;
	constexpr float basisTipRadius		= 0.03f;

	m_basisMesh = new GPUMesh( g_theRenderer );
	std::vector<Vertex_PCU> arrowIBasis3DMeshVerts;
	std::vector<uint>		arrowIBasis3DIndices;
	CreateArrow3D( arrowIBasis3DMeshVerts , arrowIBasis3DIndices , basisShaftRadius , basisTipRadius ,
		Vec3::ZERO , forwardVector ,
		RED , RED , RED , RED );

	std::vector<Vertex_PCU> arrowJBasis3DMeshVerts;
	std::vector<uint>		arrowJBasis3DIndices;
	CreateArrow3D( arrowJBasis3DMeshVerts , arrowJBasis3DIndices , basisShaftRadius , basisTipRadius ,
		Vec3::ZERO , rightVector ,
		GREEN , GREEN , GREEN , GREEN );

	std::vector<Vertex_PCU> arrowKBasis3DMeshVerts;
	std::vector<uint>		arrowKBasis3DIndices;
	CreateArrow3D( arrowKBasis3DMeshVerts , arrowKBasis3DIndices , basisShaftRadius , basisTipRadius ,
		Vec3::ZERO , upVector ,
		BLUE , BLUE , BLUE , BLUE );

	std::vector<Vertex_PCU> worldBasis3DMeshVerts;
	std::vector<uint>		worldBasis3DIndices;

	AppendIndexedVerts( arrowIBasis3DMeshVerts , arrowIBasis3DIndices , worldBasis3DMeshVerts , worldBasis3DIndices );
	AppendIndexedVerts( arrowJBasis3DMeshVerts , arrowJBasis3DIndices , worldBasis3DMeshVerts , worldBasis3DIndices );
	AppendIndexedVerts( arrowKBasis3DMeshVerts , arrowKBasis3DIndices , worldBasis3DMeshVerts , worldBasis3DIndices );
	m_basisMesh->UpdateVertices( worldBasis3DMeshVerts );
	m_basisMesh->UpdateIndices( worldBasis3DIndices );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::InitializeHUDElements()
{
	m_HUD			= AABB2( Vec2::ZERO , Vec2( UI_SIZE_X , 117.f ) );
	m_playerGun		= AABB2( Vec2( 330.f , 117.f ) , Vec2( 1270.f , 1057.f ) );
			
	m_pointSampler	= g_theRenderer->GetOrCreateSampler( SAMPLER_POINT );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::~Game()
{
	m_pointSampler		= nullptr;
	m_imageEffectShader = nullptr;

	delete m_testMap;
	m_testMap			= nullptr;

	for ( size_t index = 0; index < NUM_TOTAL_GAME_TEX ; index++ )
	{
		m_textures[ index ] = nullptr;
	}
	
	delete m_cubeMesh;
	m_cubeMesh			= nullptr;

	delete m_basisMesh;
	m_basisMesh			= nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::InitializeCameras()
{
	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			GAME CAMERA
	//--------------------------------------------------------------------------------------------------------------------------------------------
	
		m_gameCamera.SetWorldCoordinateSystem( X_IN_Y_LEFT_Z_UP );
		m_gameCamera.SetProjectionPerspective( 40.f , CLIENT_ASPECT , -.09f , -100.f );
		m_gameCamera.SetPosition( Vec3( 1.f , 1.f , 1.f ) );
		m_gameCamera.SetPosition( Vec3( -10.f , 1.f , 1.f ) );
		m_gameCamera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , BLACK , 1.f , 0 );

	//--------------------------------------------------------------------------------------------------------------------------------------------
	//			HUD CAMERA
	//--------------------------------------------------------------------------------------------------------------------------------------------
	
		m_uiCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
		m_uiCamera.SetClearMode( CLEAR_NONE , BLACK , 1.f , 0 );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{	
	
	if ( m_debugDraw )
	{
		DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.625f , 1.00f ) , Vec2::ONE , 20.f , RED , deltaSeconds ,
			"[ F1 ] : SHOW DEBUG HUD" );
		DebugDrawUI( deltaSeconds );
	}
	else
	{
		DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.625f , 1.00f ) , Vec2::ONE , 20.f , RED , deltaSeconds ,
			"[ F1 ] : SHOW DEBUG HUD" );
	}

	m_fps = 1.f / deltaSeconds;
	
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1.00f ) , Vec2::ONE , 20.f , PURPLE , deltaSeconds ,
						 "( ms/frame ) FPS = %.0f" , m_fps );

	m_testMap->UpdateMeshes();
	
	UpdateFromKeyBoard( deltaSeconds );
	
	Mat44 cameraTransform = m_gameCamera.GetCameraTransform().GetAsMatrix( X_IN_Y_LEFT_Z_UP );
	Vec3 forwardVector = cameraTransform.GetIBasis3D();
	m_compassMeshTransform.SetPosition( m_gameCamera.GetPosition() + 0.1f * forwardVector );
	
	UpdateDebugDrawFromKeyBoard();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DebugDrawUI( float deltaSeconds )
{
	Mat44 cameraTransform	= m_gameCamera.GetCameraTransform().GetAsMatrix();
	Vec3 forwardVector		= cameraTransform.GetIBasis3D();
	Vec3 rightVector		= cameraTransform.GetJBasis3D();
	Vec3 upVector			= cameraTransform.GetKBasis3D();
	Vec3 position			= m_gameCamera.GetPosition();

	float leftVerticalAlignment = ( 1080.f * 0.25f ) / 10.f;
	float normalizedOffset		= RangeMapFloat( 0.f , 1080.f , 0.f , 1.f , leftVerticalAlignment );
	float fontSize				= 16.5f;
		
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.0f , 1 - ( 2 * normalizedOffset ) ) , Vec2::ZERO_ONE , fontSize , YELLOW , deltaSeconds ,
		"Camera Yaw = %.1f	Pitch = %.1f	Roll = %.1f	(XYZ) = (%.2f,%.2f,%.2f)" ,
		 m_gameCamera.GetCameraTransform().GetYaw(),m_gameCamera.GetCameraTransform().GetPitch(),m_gameCamera.GetCameraTransform().GetRoll(),
		 position.x,position.y,position.z);
	
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.0f , 1 - ( 3 * normalizedOffset ) ) , Vec2::ZERO_ONE , fontSize , RED , deltaSeconds ,
		"iBasis (forward, +world-east when identity) = (%.2f , %.2f , %.2f)" , forwardVector.x , forwardVector.y , forwardVector.z );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.0f , 1 - ( 4 * normalizedOffset ) ) , Vec2::ZERO_ONE , fontSize , GREEN , deltaSeconds ,
		"jBasis (left, +world-north when identity) = (%.2f , %.2f , %.2f)" , rightVector.x , rightVector.y , rightVector.z );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.0f , 1 - ( 5 * normalizedOffset ) ) , Vec2::ZERO_ONE , fontSize , BLUE , deltaSeconds ,
		"kBasis (up, +world-up when identity) = (%.2f , %.2f , %.2f)" , upVector.x , upVector.y , upVector.z );

	Mat44 basis = m_gameCamera.GetCameraTransform().GetAsMatrix();
	basis.SetTranslation3D( m_gameCamera.GetPosition() + 3.f * forwardVector );
	
	DebugAddWorldBasis( basis , deltaSeconds , m_debugRenderMode );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	Texture* backBuffer		= g_theRenderer->m_swapChain->GetBackBuffer();
	Texture* colorTarget	= g_theRenderer->GetOrCreatematchingRenderTarget( backBuffer );
	Texture* finalImage		= g_theRenderer->GetOrCreatematchingRenderTarget( colorTarget );

	m_gameCamera.SetColorTarget( 0 , colorTarget );
	m_gameCamera.SetColorTarget( 1 , finalImage );

	g_theRenderer->BeginCamera( m_gameCamera );
	m_gameCamera.CreateMatchingDepthStencilTarget(); 
	g_theRenderer->BindDepthStencil( m_gameCamera.GetDepthStencilTarget() );

	g_theRenderer->SetCullMode( CULL_BACK );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	
	g_theRenderer->BindShader( nullptr );
	g_theRenderer->BindTexture( m_textures[ TEST_TEXTURE ] );
	g_theRenderer->BindSampler( m_pointSampler );
	
	m_testMap->Render();
			
	if ( m_debugDraw )
	{
		g_theRenderer->BindShader( nullptr );
		g_theRenderer->BindTexture( nullptr );

		g_theRenderer->SetCullMode( CULL_NONE );
		g_theRenderer->SetDepthTest( COMPARE_ALWAYS , false );
		g_theRenderer->SetBlendMode( eBlendMode::SOLID );
		g_theRenderer->SetModelMatrix( m_basisMeshTransform.GetAsMatrix() );
		g_theRenderer->DrawMesh( m_basisMesh );

		g_theRenderer->SetDepthTest( COMPARE_ALWAYS , false );
		g_theRenderer->SetModelMatrix( m_compassMeshTransform.GetAsMatrix() );
		g_theRenderer->DrawMesh( m_basisMesh );
	}
	
	g_theRenderer->SetCullMode( CULL_BACK );
	g_theRenderer->SetDepthTest( COMPARE_LESS , false );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	
	g_theRenderer->EndCamera( m_gameCamera );

	g_theRenderer->StartEffect( finalImage , colorTarget , m_imageEffectShader );
	g_theRenderer->EndEffect();

	g_theRenderer->CopyTexture( backBuffer , finalImage );
	g_theRenderer->ReleaseRenderTarget( finalImage );
	g_theRenderer->ReleaseRenderTarget( colorTarget );
	
	m_gameCamera.SetColorTarget( backBuffer );
	
	GUARANTEE_OR_DIE( g_theRenderer->GetTotalRenderTargetPoolSize() < 8 , "LEAKING RENDER TARGETS" );

	DebugRenderWorldToCamera( &m_gameCamera );
	DebugRenderScreenTo( nullptr );
	
	RenderHUD();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderHUD() const
{
	Texture* playerSpriteSheetTex = m_textures[ PLAYER_SPRITE_SHEET];
	SpriteSheet playerSpriteSheet( *playerSpriteSheetTex , IntVec2( 8 , 8 ) );;
	const SpriteDefinition& playerHand = playerSpriteSheet.GetSpriteDefinition( 0 );
	Vec2 uvMins;
	Vec2 uvMaxs;
	playerHand.GetUVs( uvMins , uvMaxs );

	g_theRenderer->BeginCamera( m_uiCamera );
	g_theRenderer->BindShader( nullptr );
	g_theRenderer->BindTexture( m_textures[ PLAYER_SPRITE_SHEET ] );
	g_theRenderer->BindSampler( m_pointSampler );
	
	g_theRenderer->DrawAABB2( m_playerGun , WHITE , uvMins , uvMaxs );

	g_theRenderer->BindShader( nullptr );
	g_theRenderer->BindTexture( m_textures[ HUD_BASE ] );
	g_theRenderer->DrawAABB2( m_HUD , WHITE );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->EndCamera( m_uiCamera );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateFromKeyBoard( float deltaSeconds )
{
	if ( g_theDevConsole->IsOpen() )
	{
		return;
	}
	
	CameraPositionUpdateOnInput( deltaSeconds );

	if ( g_theInput->WasKeyJustPressed( 'I' ) )
	{
		g_theInput->ShowSystemCursor();
	}

	if ( g_theInput->WasKeyJustPressed( 'K' ) )
	{
		g_theInput->HideSystemCursor();
	}

	if ( g_theInput->WasKeyJustPressed( 'P' ) )
	{
		g_theInput->ClipSystemCursor( MOUSE_IS_WINDOWLOCKED );
	}

	if ( g_theInput->WasKeyJustPressed( 'L' ) )
	{
		g_theInput->ClipSystemCursor( MOUSE_IS_UNLOCKED );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateDebugDrawFromKeyBoard()
{
	if ( g_theInput->WasKeyJustPressed( KEY_F1 ) )
	{
		m_debugDraw = !m_debugDraw;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::CameraPositionUpdateOnInput( float deltaSeconds )
{
	Vec3 movement = Vec3::ZERO;
	Vec3 rotation = Vec3::ZERO;

	//--------------------------------------------------------------------------------
	//			NOTES
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//	NOTE - WHEN DEALING WITH BASIS YOU WANT TO MOVE FROM WORLD TO ENGINE.
	//	
	//	So doing m_gameCamera.GetCameraTransform().GetAsMatrix( worldCoordinateSystem );
	//	is doing A Game * Engine * Engine system multiplication which results in Leaving us to old Engine System.
	//--------------------------------------------------------------------------------
	
	Mat44 cameraTransform	= m_gameCamera.GetCameraTransform().GetAsMatrix();
	Vec3 forwardVector		= cameraTransform.GetIBasis3D();
	//forwardVector.z			= 0.f;
	Vec3 rightVector		= -cameraTransform.GetJBasis3D();
	rightVector.z			= 0.f;
	Vec3 upMovement			= Vec3::UNIT_VECTOR_ALONG_K_BASIS;
	
	float speed = 4.f;

	if ( g_theInput->IsKeyHeldDown( KEY_SHIFT ) )
	{
		speed = 20.f;
	}

	if ( g_theInput->IsKeyHeldDown( 'A' ) )
	{
		m_gameCamera.SetPosition( m_gameCamera.GetPosition() - rightVector * speed * deltaSeconds );
	}
	if ( g_theInput->IsKeyHeldDown( 'D' ) )
	{
		m_gameCamera.SetPosition( m_gameCamera.GetPosition() + rightVector * speed * deltaSeconds );
	}
	if ( g_theInput->IsKeyHeldDown( 'W' ) )
	{
		Vec3 newPos = m_gameCamera.GetPosition() + forwardVector * speed * deltaSeconds;
		m_gameCamera.SetPosition( newPos );
	}
	if ( g_theInput->IsKeyHeldDown( 'S' ) )
	{
		Vec3 newPos = m_gameCamera.GetPosition() - forwardVector * speed * deltaSeconds;
		m_gameCamera.SetPosition( newPos );
	}
	if ( g_theInput->IsKeyHeldDown( 'Q' ) )
	{
		m_gameCamera.SetPosition( m_gameCamera.GetPosition() - upMovement * speed * deltaSeconds );
	}
	if ( g_theInput->IsKeyHeldDown( 'E' ) )
	{
		m_gameCamera.SetPosition( m_gameCamera.GetPosition() + upMovement * speed * deltaSeconds );
	}

	if ( g_theInput->WasKeyJustPressed( 'O' ) )
	{
		m_cameraPosition = Vec3::ZERO;
		m_gameCamera.SetPosition( m_cameraPosition );
		m_pitch = 0.f;
		m_yaw = 0.f;
	}

	Vec2 mousePos		= g_theInput->GetRelativeMovement();
	
	m_yaw	-= mousePos.x * speed * deltaSeconds;
	m_pitch += mousePos.y * speed * deltaSeconds;

	m_pitch				= Clamp( m_pitch , -89.9f , 89.9f );
	float finalRoll		= 0.f;

	m_gameCamera.SetPitchYawRollRotation( m_pitch , m_yaw , finalRoll );	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateFromTestCodeKeyBoard( float deltaSeconds )
{
	if( g_theInput->WasKeyJustPressed( KEY_F2 ) )
	{
		m_isSamplerEnabled = !m_isSamplerEnabled;
		
		if( m_isSamplerEnabled )
		{
			m_pointSampler = g_theRenderer->GetOrCreateSampler( SAMPLER_POINT );
		}
		else
		{
			m_pointSampler = g_theRenderer->GetOrCreateSampler( SAMPLER_BILINEAR );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------