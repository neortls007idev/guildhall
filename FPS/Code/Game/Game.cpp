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

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;
extern TheApp* g_theApp;

Game::Game()
{
	m_uiCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
	m_color = BLACK;
	m_invertColorShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/InvertColor.hlsl" );
	m_imageTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/PlayerTankBase.png" );
	m_worldMapSphere = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/2kEarthDaymap.png" );
	m_gameCamera.SetProjectionPerspective( 60.f , CLIENT_ASPECT , -.1f , -100.f );
	m_gameCamera.SetPosition( Vec3( 0.f , 0.f , 0.f ) );

	m_gameCamera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT, BLACK , 1.f , 0 );
	
	m_meshCube = new GPUMesh( g_theRenderer );
	m_cubeTransform.SetPosition( 1.f , 0.5f , -12.0f );

	std::vector<Vertex_PCU> meshVerts;
	AddCubeVerts( meshVerts , nullptr );

	m_meshCube->UpdateVertices( meshVerts );
	m_meshCube->UpdateIndices( 36 , GetCubeIndices() );
	
	std::vector<Vertex_PCU> sphereMeshVerts;
	std::vector<uint>		sphereIndices;
	
	CreateUVSphere( m_hCuts , m_vCuts , sphereMeshVerts , sphereIndices , 1.f );

	m_meshSphere = new GPUMesh( g_theRenderer );
	m_meshSphere->UpdateVertices( sphereMeshVerts );
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
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::~Game()
{
	delete m_meshCube;
	m_meshCube			= nullptr;

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
	m_cubeTransform.SetRotation( 0.f ,  5.f * ( float ) GetCurrentTimeSeconds() , 0.f );
	UpdateFromKeyBoard( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	g_theRenderer->BeginCamera( m_gameCamera );
	m_gameCamera.CreateMatchingDepthStencilTarget(); 
	g_theRenderer->BindDepthStencil( m_gameCamera.GetDepthStencilTarget() );
	g_theRenderer->SetModelMatrix( m_cubeTransform.GetAsMatrix() );

	g_theRenderer->BindShader( nullptr );

	g_theRenderer->DrawMesh( m_meshCube );

	g_theRenderer->SetBlendMode( SOLID );
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
		g_theRenderer->DrawMesh( m_meshSphere );
	}

	g_theRenderer->BindTexture( nullptr );

	Transform quadTransform;
	quadTransform.SetPosition( 0.f , 0.f , -10.f );
	g_theRenderer->BindShader( nullptr );
	g_theRenderer->SetModelMatrix( quadTransform.GetAsMatrix() );
	g_theRenderer->DrawAABB2( AABB2( Vec2( 0.f , 0.f ) , Vec2( 2.f , 2.f ) ) , PINK );
	
	g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindShader( nullptr );
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

void Game::CameraPositionUpdateOnInput( float deltaSeconds )
{
	Vec3 movement = Vec3::ZERO;
	Vec3 rotation = Vec3::ZERO;

	if ( g_theInput->IsKeyHeldDown( 'A' ) )
	{
		movement.x -= 1.f;
	}
	if ( g_theInput->IsKeyHeldDown( 'D' ) )
	{
		movement.x += 1.f;
	}
	if ( g_theInput->IsKeyHeldDown( 'W' ) )
	{
		movement.y += 1.f;
	}
	if ( g_theInput->IsKeyHeldDown( 'S' ) )
	{
		movement.y -= 1.f;
	}
	if ( g_theInput->IsKeyHeldDown( 'Q' ) )
	{
		movement.z += 1.f;
	}
	if ( g_theInput->IsKeyHeldDown( 'E' ) )
	{
		movement.z -= 1.f;
	}
	if ( g_theInput->IsKeyHeldDown( KEY_UPARROW ) )
	{
		rotation.x += 1.f;
	}
	if ( g_theInput->IsKeyHeldDown( KEY_DOWNARROW ) )
	{
		rotation.x -= 1.f;
	}

	if ( g_theInput->IsKeyHeldDown( KEY_RIGHTARROW ) )
	{
		rotation.z -= 1.f;
	}
	if ( g_theInput->IsKeyHeldDown( KEY_LEFTARROW ) )
	{
		rotation.z += 1.f;
	}
	if ( g_theInput->WasKeyJustPressed('O'))
	{
		m_cameraPosition = Vec3::ZERO;
		m_cameraRotation = Vec3::ZERO;
	}

	float speed = 4.0f;

	if ( g_theInput->IsKeyHeldDown( KEY_SHIFT ) )
	{
		speed = 20.f;
	}

	Vec2 mousePos = g_theInput->GetRelativeMovement();
	
// 	float rotationPitch = RangeMapFloat( 0.f , 1.f , -90.f , 90.f , mousePos.y );
// 	float rotationYaw = RangeMapFloat( 0.f , 1.f , -180.f , 180.f , mousePos.x );

	m_cameraPosition	+= movement * speed * deltaSeconds;
	m_cameraRotation.x	-= mousePos.y * speed * deltaSeconds;
	m_cameraRotation.y	-= mousePos.x * speed * deltaSeconds;

	float finalPitch = Clamp( m_cameraRotation.x , -85.f , 85.f );
	float finalYaw	 = Clamp( m_cameraRotation.z, -175.f , 175.f );
	float finalRoll  = Clamp( m_cameraRotation.y, -175.f , 175.f );

	m_gameCamera.SetPosition( m_cameraPosition );
	m_gameCamera.SetPitchYawRollRotation( finalPitch , finalRoll, finalYaw  );
}

//--------------------------------------------------------------------------------------------------------------------------------------------