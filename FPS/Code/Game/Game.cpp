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
	m_uiCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
	m_color = BLACK;
	m_invertColorShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/InvertColor.hlsl" );
	m_litShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/litDefault2.hlsl" );
	m_imageTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/PlayerTankBase.png" );
	m_worldMapSphere = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/2kEarthDaymap.png" );
	m_gameCamera.SetProjectionPerspective( 60.f , CLIENT_ASPECT , -.1f , -100.f );
	m_gameCamera.SetPosition( Vec3( 0.f , 0.f , 0.f ) );

	m_gameCamera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT, BLACK , 1.f , 0 );
	
	m_meshTest = new GPUMesh( g_theRenderer );
	m_testMeshTransform.SetPosition( 1.f , 0.5f , -12.0f );

	std::vector<Vertex_PCU> meshVerts;
	std::vector<uint>		meshIndices;


	//AABB3 box( Vec3::ZERO , Vec3(1,1,5) );
	//CreateCuboid( meshVerts , meshIndices , box , PURPLE );
	CreateArrow3D( meshVerts , meshIndices , .5f , 0.55f , Vec3( 0.5 , 0.5 , -1 ) , Vec3( 1 , 1 , -2 ) , RED , BLUE ,
	               GREEN , PINK );
	//AddCubeVerts( meshVerts , nullptr );

	m_meshTest->UpdateVertices( meshVerts );
	m_meshTest->UpdateIndices( meshIndices );
	
	//std::vector<Vertex_PCU> sphereMeshVerts;
	std::vector<VertexMaster> sphereMeshVerts;
	std::vector<uint>		sphereIndices;
	
	CreateUVSphere( m_hCuts , m_vCuts , sphereMeshVerts , sphereIndices , 1.f );

	m_meshSphere = new GPUMesh( g_theRenderer );
	m_meshSphere->UpdateVertices( sphereMeshVerts.size() , &sphereMeshVerts[ 0 ] );
	m_meshSphere->UpdateIndices( sphereIndices );
	
	m_normalImage = AABB2( -WORLD_CAMERA_SIZE_X , -WORLD_CAMERA_SIZE_Y , WORLD_CAMERA_SIZE_X , WORLD_CAMERA_SIZE_Y );
	AABB2 boxCopy = m_normalImage;
	m_invertedColorImage = boxCopy.CarveBoxOffRight( 0.5f , 0.f );
	m_normalImage = m_normalImage.GetBoxAtLeft( 0.5f , 0.f );

	m_normalImage.SetDimensions( m_normalImage.GetDimensions() * 0.75f );
	m_normalImage.AlignWithinAABB2( boxCopy , ALIGN_CENTERED );

	boxCopy = m_invertedColorImage;
	m_invertedColorImage.SetDimensions( m_invertedColorImage.GetDimensions() * 0.75f );
	m_invertedColorImage.AlignWithinAABB2( boxCopy , ALIGN_CENTERED );


	std::vector<Vec3> lineStripTest;
	 
	lineStripTest.push_back( Vec3( 2 , 2 , -4 ) );
	lineStripTest.push_back( Vec3( 4 , 4 , -8 ) );
	lineStripTest.push_back( Vec3( 4 , 10 , -8 ) );
	lineStripTest.push_back( Vec3( 4 , 10 , -13 ) );

	DebugAddWorldLine( Vec3(0,0,0 ) ,  Vec3(0,-1,0) , PURPLE , 15.f , DEBUG_RENDER_ALWAYS , 0.5f );
	
	DebugAddWorldLineStrip( ( uint ) lineStripTest.size() , &lineStripTest[0] , GREEN , WHITE , PINK , BLUE , 20.f );

// 	DebugAddWorldQuad( lineStripTest[ 0 ] , lineStripTest[ 1 ] , lineStripTest[ 2 ] , lineStripTest[ 3 ] ,
// 	                   AABB2::ZERO_TO_ONE , WHITE , RED , 10.f , DEBUG_RENDER_USE_DEPTH , m_imageTex );
// 	
	DebugAddScreenPoint( Vec2( 100 , 100 ) , 100.f , RED , BLUE , 5.f );

	DebugAddScreenLine( Vec2( 600 , 400 ) , GREEN , BLUE ,
						Vec2( 700 , 900 ) , CYAN , YELLOW , 10.f );
	
	DebugAddScreenArrow( Vec2( 500 , 300 ) , Vec2( 100 , 100 ) ,
				GREEN , BLUE , PURPLE , ORANGE ,
								CYAN , PINK , MAGENTA , YELLOW ,
	                     5.f , 10.f );
// 
// 	DebugAddWorldBasis( m_gameCamera.GetCameraTransform().GetAsMatrix() , 10.f );
 	DebugAddScreenTextf( Vec4( 00.f , -000.f , 1.f , 1.f ) , Vec2(1.f,0.5f) , 20.f , ORANGE , GREEN ,
 					10.f , "Hello %d %f" , 5 , 22.f );

//	DebugAddWorldPoint( m_cubeTransform.GetPostion() , 0.25 , Rgba8( 0 , 255 , 0 , 127 ) , Rgba8( 0 , 255 , 0 , 127 ) , 50.f , DEBUG_RENDER_USE_DEPTH );
	
	DebugAddWorldTextf( m_testMeshTransform.GetAsMatrix() , Vec2(0.5f,0.5f) , ORANGE ,
		10000.f , DEBUG_RENDER_USE_DEPTH , " Hello %d %f" , 5 , 22.f );

	//m_cubeTransform.SetPosition( 1.f , 0.5f , -12.0f );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::~Game()
{
	delete m_meshTest;
	m_meshTest			= nullptr;

	delete m_meshSphere;
	m_meshSphere		= nullptr;

	m_imageTex			= nullptr;
	m_invertColorShader = nullptr;
	m_worldMapSphere	= nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	static float y = 0;
	y += deltaSeconds;
	m_testMeshTransform.SetRotation( 0.f ,  20.f * ( float ) GetCurrentTimeSeconds() , 0.f );
	UpdateFromKeyBoard( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	g_theRenderer->BeginCamera( m_gameCamera );
	m_gameCamera.CreateMatchingDepthStencilTarget(); 
	g_theRenderer->BindDepthStencil( m_gameCamera.GetDepthStencilTarget() );
	g_theRenderer->SetModelMatrix( m_testMeshTransform.GetAsMatrix() );

	g_theRenderer->BindShader( nullptr );
	g_theRenderer->SetRasterState( FILL_SOLID );

	g_theRenderer->DrawMesh( m_meshTest );
	g_theRenderer->DrawVertexArray( m_meshTest->GetVertexCount() , m_meshTest->m_vertices );

	g_theRenderer->SetCullMode( CULL_BACK );
	g_theRenderer->SetDepthTest( COMPARE_GEQUAL , true );
	lightDataT lightData;
	g_theRenderer->EnableLight( 0 , lightData );

	g_theRenderer->SetBlendMode( SOLID );
	g_theRenderer->SetRasterState( FILL_SOLID );
	
	g_theRenderer->BindShader( m_litShader );
	g_theRenderer->BindTexture( m_worldMapSphere );
		
	Transform sphereRing;
	float deltaDegrees = 360.f / 30.f;

	for ( float sphereIndex = 0 ; sphereIndex <= 360 ; sphereIndex += deltaDegrees )
		{
			Vec3 position = Vec3::MakeFromSpericalCoordinates( 0.f , 20.f * ( float ) GetCurrentTimeSeconds() + sphereIndex , 15.f );
			position.z -= 30.f;
			sphereRing.SetPosition( position );
			sphereRing.SetRotation( 0.f , 20.f * ( float ) GetCurrentTimeSeconds() + sphereIndex , 0.f );
			g_theRenderer->SetModelMatrix( sphereRing.GetAsMatrix() );
			g_theRenderer->SetRasterState( FILL_SOLID );
			g_theRenderer->DrawMesh( m_meshSphere );
		}

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindShader( nullptr );
	g_theRenderer->EndCamera( m_gameCamera );

// 	DebugRenderWorldToCamera( &m_gameCamera );
// 	DebugRenderScreenTo( nullptr );
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
	DebugLineStripDrawModeTest();
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

	if( m_lineStripMode )
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
			10.f, m_debugRenderMode , " Hello %d %f" , 5 , 22.f );
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

	//Vec2 newMovement = Vec2::MakeFromPolarDegrees( m_cameraRotation.y );
	//Vec3 newMovement = Vec3::MakeFromSpericalCoordinates( m_cameraRotation.y , m_cameraRotation.x , 1 );

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
// 	if ( g_theInput->IsKeyHeldDown( KEY_UPARROW ) )
// 	{
// 		rotation.x += 1.f;
// 	}
// 	if ( g_theInput->IsKeyHeldDown( KEY_DOWNARROW ) )
// 	{
// 		rotation.x -= 1.f;
// 	}
// 
// 	if ( g_theInput->IsKeyHeldDown( KEY_RIGHTARROW ) )
// 	{
// 		rotation.z -= 1.f;
// 	}
// 	if ( g_theInput->IsKeyHeldDown( KEY_LEFTARROW ) )
// 	{
// 		rotation.z += 1.f;
// 	}
	if ( g_theInput->WasKeyJustPressed( 'O' ) )
	{
		m_cameraPosition = Vec3::ZERO;
		m_cameraRotation = Vec3::ZERO;
	}

	Vec2 mousePos = g_theInput->GetRelativeMovement();

	m_cameraRotation.x -= mousePos.y * speed * deltaSeconds;
	m_cameraRotation.y -= mousePos.x * speed * deltaSeconds;

	float finalPitch = Clamp( m_cameraRotation.x , -180.f , 180.f );
	float finalYaw = Clamp( m_cameraRotation.z , -175.f , 175.f );
	float finalRoll = Clamp( m_cameraRotation.y , -85.f , 85.f );

	m_gameCamera.SetPitchYawRollRotation( finalPitch , finalRoll , finalYaw );
}

//--------------------------------------------------------------------------------------------------------------------------------------------