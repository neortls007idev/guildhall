#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Primitives/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Core/VertexMaster.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Time/Time.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern AudioSystem*		g_theAudioSystem;
extern RenderContext*	g_theRenderer;
extern TheApp*			g_theApp;
extern DevConsole*		g_theDevConsole;

bool   s_wasDataLoaded = false;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	if ( !s_wasDataLoaded )
	{
		LoadShaders();
		LoadTextures();
		LoadAudio();
	}
	
	InitializeCameras();
	IntializeGameObjects();
		
	m_cubeMesh1Transform.SetPosition( 2.5f , 0.5f , 0.5f );
	m_cubeMesh1Transform.SetScale( 1.f , 1.f , 1.f );
	m_cubeMesh2Transform.SetPosition( 0.5f , 2.5f , 0.5f );
	m_cubeMesh1Transform.SetScale( 1.f , 1.f , 1.f );
	m_cubeMesh3Transform.SetPosition( 2.5f , 2.5f , 0.5f );
	m_cubeMesh1Transform.SetScale( 1.f , 1.f , 1.f );
	//m_cubeMesh1Transform.SetRotation( -90.f , 0.0f , 0.0f );

	std::string gameConfigData = g_gameConfigBlackboard.GetValue( "testkey" , "Invalid Value" );
	g_theDevConsole->PrintString( gameConfigData , eDevConsoleMessageType::DEVCONSOLE_SYTEMLOG );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadShaders()
{
	m_imageEffectShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/selectiveImageEffect.hlsl" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadTextures()
{
	m_testTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadAudio()
{
	m_testSound = g_theAudioSystem->CreateOrGetSound( "Data/Audio/TestSound.mp3" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::IntializeGameObjects()
{
	m_cubeMesh = new GPUMesh( g_theRenderer ); 
	std::vector<VertexMaster>	cubeMeshVertsMaster;
	std::vector<Vertex_PCU>		cubeMeshVerts;
	std::vector<uint>			cubeMeshIndices;

	AABB3 box( Vec3( -0.5f, -0.5f , -0.5f ) , Vec3( 0.5f , 0.5f , 0.5f ) );
	//CreateCuboid( cubeMeshVerts , cubeMeshIndices , box , WHITE );
	CreateCuboidXInYLeftZUp( cubeMeshVertsMaster , cubeMeshIndices , box , WHITE );
	VertexMaster::ConvertVertexMasterToVertexPCU( cubeMeshVerts , cubeMeshVertsMaster );
	
	m_cubeMesh->UpdateVertices( ( uint ) cubeMeshVerts.size() , cubeMeshVerts.data() );
	m_cubeMesh->UpdateIndices( cubeMeshIndices );

	m_sphereMesh = new GPUMesh( g_theRenderer );
	std::vector<VertexMaster>	SphereVertsMaster;
	std::vector<Vertex_PCU>		SphereVerts;
	std::vector<uint>			SphereIndices;

	CreateUVSphere( 32 , 16 , SphereVertsMaster , SphereIndices , 5.f );
	VertexMaster::ConvertVertexMasterToVertexPCU( SphereVerts , SphereVertsMaster );

	m_sphereMesh->UpdateVertices( ( uint ) SphereVerts.size() , SphereVerts.data() );
	m_sphereMesh->UpdateIndices( SphereIndices );
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::~Game()
{
	delete m_cubeMesh;
	m_cubeMesh			= nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::InitializeCameras()
{
		m_uiCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
		m_gameCamera.SetWorldCoordinateSystem( X_IN_Y_LEFT_Z_UP );
		m_gameCamera.SetProjectionPerspective( 70.f , CLIENT_ASPECT , -.1f , -100.f );
		m_gameCamera.SetPosition( Vec3( 1.f , 1.f , 1.f ) );
		m_gameCamera.SetPosition( Vec3( -10.f , 1.f , 1.f ) );
		//m_gameCamera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 37 , 70 , 87 , 127 ) , 1.f , 0 );
		m_gameCamera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , BLACK , 1.f , 0 );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{	
	if ( m_isHUDEnabled )
	{
		DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.625f , 1.00f ) , Vec2::ONE , 20.f , RED , deltaSeconds ,
			"[ H ] : HIDE HELP HUD" );
		DebugDrawUI( deltaSeconds );
	}
	else
	{
		DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.625f , 1.00f ) , Vec2::ONE , 20.f , RED , deltaSeconds ,
			"[ H ] : SHOW HELP HUD" );
	}

	//m_cubeMesh1Transform.SetRotation(  90.f ,  180.f , 90.f );

	UpdateFromKeyBoard( deltaSeconds );
	UpdateAudioFromKeyBoard();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DebugDrawUI( float deltaSeconds )
{

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
	//g_theRenderer->SetWindingOrder( CLOCKWISE );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	
	g_theRenderer->BindShader( nullptr );
	g_theRenderer->BindTexture( m_testTexture );
			
	g_theRenderer->SetModelMatrix( m_cubeMesh1Transform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_cubeMesh );

	g_theRenderer->SetModelMatrix( m_cubeMesh2Transform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_cubeMesh );

	g_theRenderer->SetModelMatrix( m_cubeMesh3Transform.GetAsMatrix( X_IN_Y_LEFT_Z_UP ) );
	g_theRenderer->DrawMesh( m_cubeMesh );

	g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	g_theRenderer->DrawMesh( m_sphereMesh );
	
	g_theRenderer->BindShader( nullptr );
	g_theRenderer->BindTexture( nullptr );
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
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateFromKeyBoard( float deltaSeconds )
{
	if ( g_theDevConsole->IsOpen() )
	{
		return;
	}
	//DebugLineStripDrawModeTest();
	
	if ( g_theInput->WasKeyJustPressed( 'H' ) )
	{
		m_isHUDEnabled = !m_isHUDEnabled;
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

void Game::UpdateAudioFromKeyBoard()
{
	if ( g_theInput->WasKeyJustPressed( KEY_F1 ) )
	{
		float volume	= m_rng.RollRandomFloatInRange( 0.5f , 1.f );
		float panning	= m_rng.RollRandomFloatInRange( -1.f , 1.f );
		float speed		= m_rng.RollRandomFloatInRange( 0.5f , 2.f );
		g_theAudioSystem->PlaySound( m_testSound , false , volume , panning , speed );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::CameraPositionUpdateOnInput( float deltaSeconds )
{
	Vec3 movement = Vec3::ZERO;
	Vec3 rotation = Vec3::ZERO;

	Mat44 cameraTransform	= m_gameCamera.GetCameraTransform().GetAsMatrix();
	Vec3 forwardVector		= cameraTransform.GetIBasis3D();
	Vec3 rightVector		= -cameraTransform.GetJBasis3D();
	Vec3 upVector			= cameraTransform.GetKBasis3D();

	float speed = 4.0f;

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
		m_gameCamera.SetPosition( m_gameCamera.GetPosition() - upVector * speed * deltaSeconds );
	}
	if ( g_theInput->IsKeyHeldDown( 'E' ) )
	{
		m_gameCamera.SetPosition( m_gameCamera.GetPosition() + upVector * speed * deltaSeconds );
	}

	if ( g_theInput->WasKeyJustPressed( 'O' ) )
	{
		m_cameraPosition = Vec3::ZERO;
		m_cameraRotation = Vec3::ZERO;
	}

	Vec2 mousePos		= g_theInput->GetRelativeMovement();

	//m_cameraRotation.x -= mousePos.y * speed * deltaSeconds;
	//m_cameraRotation.y -= mousePos.x * speed * deltaSeconds;

	//float finalPitch	= Clamp( m_cameraRotation.x , -180.f , 180.f );
	//float finalYaw		= m_cameraRotation.z;//Clamp( m_cameraRotation.z , -175.f , 175.f );
	//float finalRoll		= m_cameraRotation.y;//Clamp( m_cameraRotation.y , -85.f , 85.f );

	//m_gameCamera.SetPitchYawRollRotation( finalPitch , finalRoll , finalYaw );
	//m_gameCamera.SetPitchYawRollRotation( m_cameraRotation.x , m_cameraRotation.z , m_cameraRotation.y );
}


//--------------------------------------------------------------------------------------------------------------------------------------------