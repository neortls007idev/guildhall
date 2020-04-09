#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Primitives/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Time/Time.hpp"

#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Core/VertexMaster.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*	g_theRenderer;
extern TheApp*			g_theApp;
extern DevConsole*		g_theDevConsole;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	
	m_litShader		 = g_theRenderer->GetOrCreateShader( "Data/Shaders/litDefault2.hlsl" );
	//m_litShader		 = g_theRenderer->GetOrCreateShader( "Data/Shaders/normalLit.hlsl" );
	m_worldMapSphere = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/2kEarthDaymap.png" );

	InitializeCameras();
	intializeGameObjects();
		
	m_cubeMeshTransform.SetPosition( 1.f , 0.0f , -20.0f );
	m_sphereMeshTransform.SetPosition( 7.f , 0.0f , -20.0f );

	m_lights.ambientLight = Vec4( 1.f , 0.f , 0.f , 1.f );
	m_lights.lights[ 0 ].color = Vec3( 1.f , 1.f , 1.f );
	m_lights.lights[ 0 ].intensity = 1.f;
	m_lights.lights[ 0 ].world_position = Vec3( 0.f , 0.f , 0.f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::intializeGameObjects()
{
	m_cubeMesh = new GPUMesh( g_theRenderer );
	std::vector<VertexMaster>	cubeMeshVerts;
	std::vector<VertexLit>		cubeMeshLitVerts;
	std::vector<uint>			cubeMeshIndices;

	AABB3 box( Vec3::ZERO , Vec3( 1 , 1 , 5 ) );
	CreateCuboid( cubeMeshVerts , cubeMeshIndices , box , WHITE );
	VertexMaster::ConvertVertexMasterToVertexLit( cubeMeshLitVerts , cubeMeshVerts );

	m_cubeMesh->UpdateVertices( ( uint ) cubeMeshVerts.size() , cubeMeshVerts.data() );
	m_cubeMesh->UpdateIndices( cubeMeshIndices );

	std::vector<VertexMaster>	sphereMeshVerts;
	std::vector<VertexLit>		sphereMeshLitVerts;
	std::vector<uint>			sphereIndices;

	CreateUVSphere( m_hCuts , m_vCuts , sphereMeshVerts , sphereIndices , 1.f );

	// this is incorrect it seems
	VertexMaster::ConvertVertexMasterToVertexLit( sphereMeshLitVerts , sphereMeshVerts );

	m_meshSphere = new GPUMesh( g_theRenderer );
	//m_meshSphere->UpdateVertices( ( uint ) sphereMeshLitVerts.size() , sphereMeshLitVerts.data() );
	m_meshSphere->UpdateVertices( ( uint ) sphereMeshVerts.size() , sphereMeshVerts.data() );
	m_meshSphere->UpdateIndices( sphereIndices );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::~Game()
{
	delete m_cubeMesh;
	m_cubeMesh			= nullptr;

	delete m_meshSphere;
	m_meshSphere		= nullptr;

	m_meshTex_D			= nullptr;
	m_meshTex_N			= nullptr;
	m_worldMapSphere	= nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::InitializeCameras()
{
		m_uiCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
		m_gameCamera.SetProjectionPerspective( 60.f , CLIENT_ASPECT , -.1f , -100.f );
		m_gameCamera.SetPosition( Vec3( 0.f , 0.f , 0.f ) );
		m_gameCamera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 37 , 127 , 139 , 255 ) , 1.f , 0 );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	static float y = 0;
	y += deltaSeconds;
	m_cubeMeshTransform.SetRotation( /*10.f * ( float ) GetCurrentTimeSeconds()*/ 0.f ,  20.f * ( float ) GetCurrentTimeSeconds() , 0.f );
	m_sphereMeshTransform.SetRotation( /*20.f * ( float ) GetCurrentTimeSeconds()*/ 0.f,  50.f * ( float ) GetCurrentTimeSeconds() , 0.f );
	UpdateFromKeyBoard( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	g_theRenderer->BeginCamera( m_gameCamera );
	m_gameCamera.CreateMatchingDepthStencilTarget(); 
	g_theRenderer->BindDepthStencil( m_gameCamera.GetDepthStencilTarget() );

	g_theRenderer->BindShader( nullptr );
	g_theRenderer->SetRasterState( FILL_SOLID );
	
	g_theRenderer->SetCullMode( CULL_BACK );
	g_theRenderer->SetDepthTest( COMPARE_LEQUAL , true );
	//lightDataT lightData;
	g_theRenderer->SetAmbientLight( WHITE , 1.f );
	g_theRenderer->EnableLight( 0 , m_lights.lights[ 0 ] );

	//g_theRenderer->SetBlendMode( SOLID );
	
	g_theRenderer->BindShader( m_litShader );
	g_theRenderer->BindTexture( m_worldMapSphere );
		
	g_theRenderer->SetModelMatrix( m_cubeMeshTransform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_cubeMesh );

	g_theRenderer->BindTexture( m_worldMapSphere );

 	g_theRenderer->SetModelMatrix( m_sphereMeshTransform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_meshSphere );

	g_theRenderer->SetRasterState( FILL_SOLID );

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindShader( nullptr );
	g_theRenderer->SetDepthTest( COMPARE_ALWAYS , true );
	g_theRenderer->SetCullMode( CULL_NONE );
	g_theRenderer->DisableLight( 0 );
	g_theRenderer->EndCamera( m_gameCamera );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderUI() const
{

}


//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCamera()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::AddScreenShakeIntensity( float deltaShakeIntensity )
{
	m_screenShakeIntensity += deltaShakeIntensity;
	//clamp it!
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::GarbageCollection()
{
	GarbageDeletion();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::GarbageDeletion()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Die()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateFromKeyBoard( float deltaSeconds )
{
	if ( g_theDevConsole->IsOpen() )
	{
		return;
	}
	//DebugLineStripDrawModeTest();
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

	//CreateDebugObjectsFromUserInput();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::CreateDebugObjectsFromUserInput()
{
	if ( g_theInput->WasKeyJustPressed( 'U' ) )
	{
		m_debugRenderMode = DEBUG_RENDER_ALWAYS;
	}

	if ( g_theInput->WasKeyJustPressed( 'J' ) )
	{
		m_debugRenderMode = DEBUG_RENDER_USE_DEPTH;
	}

	if ( g_theInput->WasKeyJustPressed( 'N' ) )
	{
		m_debugRenderMode = DEBUG_RENDER_XRAY;
	}

	if ( m_lineStripMode )
	{
		return;
	}

	if ( g_theInput->WasKeyJustPressed( '1' ) )
	{
		DebugAddWorldPoint( m_gameCamera.GetPosition() , 1.f , Rgba8( 0 , 255 , 0 , 127 ) , Rgba8( 0 , 255 , 0 , 127 ) ,
			5.f , m_debugRenderMode );
	}

	if ( g_theInput->WasKeyJustPressed( '2' ) )
	{
		DebugAddWorldLine( m_gameCamera.GetPosition() , GREEN , PINK , m_gameCamera.GetPosition() + Vec3::ONE ,
			PURPLE , ORANGE , 5.f , m_debugRenderMode , 0.5f );
		//DebugAddWorldLine( m_gameCamera.GetPosition() , m_gameCamera.GetPosition() + Vec3::ONE , PURPLE ,15.f , DEBUG_RENDER_ALWAYS , 0.5f );
	}

	if ( g_theInput->WasKeyJustPressed( '3' ) )
	{
		//DebugAddWorldArrow( m_gameCamera.GetPosition() , GREEN , RED , m_gameCamera.GetPosition() - Vec3::ONE ,
		//					PURPLE , ORANGE , 5.f , m_debugRenderMode , 0.5f );

		DebugAddWorldArrow( m_gameCamera.GetPosition() ,
			m_gameCamera.GetPosition() - m_gameCamera.GetCameraTransform().GetAsMatrix().GetKBasis3D() ,
			GREEN , BLUE , PURPLE , ORANGE ,
			CYAN , PINK , YELLOW , MAGENTA , 5.f ,
			m_debugRenderMode , 0.5f , 0.55f );

		//DebugAddWorldLine( m_gameCamera.GetPosition() , m_gameCamera.GetPosition() + Vec3::ONE , PURPLE ,5.f , DEBUG_RENDER_ALWAYS , 0.5f );
	}

	if ( g_theInput->WasKeyJustPressed( '4' ) )
	{
		DebugAddWorldWireBounds(
			AABB3( m_gameCamera.GetPosition() ,
				m_gameCamera.GetPosition() + m_gameCamera.GetCameraTransform().GetAsMatrix().GetJBasis3D() ) ,
			WHITE , 5.0f , m_debugRenderMode );
	}

	if ( g_theInput->WasKeyJustPressed( '5' ) )
	{
		DebugAddWorldWireSphere( m_gameCamera.GetPosition() , 0.5f , YELLOW , 5.0f , m_debugRenderMode );
	}

	if ( g_theInput->WasKeyJustPressed( '6' ) )
	{
		DebugAddWorldBasis( m_gameCamera.GetCameraTransform().GetAsMatrix() , 5.f , m_debugRenderMode );
		//DebugAddWorldBasis( m_gameCamera.GetCameraTransform().GetAsMatrix() , CYAN , PINK , 10.f );
	}

	if ( g_theInput->WasKeyJustPressed( '7' ) )
	{
		//DebugAddWorldBasis( m_gameCamera.GetCameraTransform().GetAsMatrix() , 10.f );
		DebugAddWorldTextf( m_gameCamera.GetCameraTransform().GetAsMatrix() , Vec2::ZERO , ORANGE ,
			10.f , m_debugRenderMode , " Hello %d %f" , 5 , 22.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DebugLineStripDrawModeTest()
{
	if ( g_theInput->IsKeyHeldDown( KEY_SHIFT ) && g_theInput->WasLeftMouseButtonJustPressed() )
	{
		m_lineStripMode = true;
	}

	if ( g_theInput->IsKeyHeldDown( KEY_SHIFT ) && g_theInput->WasRightMouseButtonJustPressed() )
	{
		if ( m_lineStripPoints.size() > 0 )
		{
			DebugAddWorldLineStrip( ( uint ) m_lineStripPoints.size() , &m_lineStripPoints[ 0 ] , GREEN , WHITE , PINK ,
				BLUE , 20.f , m_debugRenderMode );
		}
		m_lineStripPoints.clear();
		m_lineStripMode = false;
	}

	if ( m_lineStripMode && g_theInput->WasLeftMouseButtonJustPressed() )
	{
		DebugAddWorldPoint( m_gameCamera.GetPosition() , 0.25 , ORANGE ,
			YELLOW , 10.f , m_debugRenderMode );
		m_lineStripPoints.push_back( m_gameCamera.GetPosition() );
	}

	if ( m_lineStripMode && g_theInput->WasKeyJustPressed( KEY_ESC ) )
	{
		m_lineStripMode = false;
		m_lineStripPoints.clear();
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::CameraPositionUpdateOnInput( float deltaSeconds )
{
	Vec3 movement = Vec3::ZERO;
	Vec3 rotation = Vec3::ZERO;

	Mat44 cameraTransform = m_gameCamera.GetCameraTransform().GetAsMatrix();
	Vec3 forwardVector = cameraTransform.GetKBasis3D();
	Vec3 rightVector = cameraTransform.GetIBasis3D();

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
		m_gameCamera.SetPosition( m_gameCamera.GetPosition() - forwardVector * speed * deltaSeconds );
	}
	if ( g_theInput->IsKeyHeldDown( 'S' ) )
	{
		m_gameCamera.SetPosition( m_gameCamera.GetPosition() + forwardVector * speed * deltaSeconds );
	}
	if ( g_theInput->IsKeyHeldDown( 'Q' ) )
	{
		movement.y -= 1.f;
	}
	if ( g_theInput->IsKeyHeldDown( 'E' ) )
	{
		movement.y += 1.f;
	}

	if ( g_theInput->WasKeyJustPressed( 'O' ) )
	{
		m_cameraPosition = Vec3::ZERO;
		m_cameraRotation = Vec3::ZERO;
	}

	Vec2 mousePos		= g_theInput->GetRelativeMovement();

	m_cameraRotation.x -= mousePos.y * speed * deltaSeconds;
	m_cameraRotation.y -= mousePos.x * speed * deltaSeconds;

	float finalPitch	= Clamp( m_cameraRotation.x , -180.f , 180.f );
	float finalYaw		= Clamp( m_cameraRotation.z , -175.f , 175.f );
	float finalRoll		= Clamp( m_cameraRotation.y , -85.f , 85.f );

	m_gameCamera.SetPitchYawRollRotation( finalPitch , finalRoll , finalYaw );
}

//--------------------------------------------------------------------------------------------------------------------------------------------