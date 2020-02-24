#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/Game.hpp"

#include "Engine/Math/MatrixUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Math/Vec4.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;
extern TheApp* g_theApp;

Game::Game()
{
	//m_gameCamera.SetOrthoView( Vec2( -WORLD_CAMERA_SIZE_X , -WORLD_CAMERA_SIZE_Y ) , Vec2( WORLD_CAMERA_SIZE_X , WORLD_CAMERA_SIZE_Y ) );
	m_uiCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
	m_color = BLACK;
	m_invertColorShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/InvertColor.hlsl" );
	m_imageTex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/PlayerTankBase.png" );
	m_gameCamera.SetProjectionPerspective( 60.f , CLIENT_ASPECT , 1.0f , -1.f );
	Vec3 orthoMin = m_gameCamera.GetOrthoMin();
	Vec3 orthoMax = m_gameCamera.GetOrthoMax();
	m_normalImage = AABB2( -WORLD_CAMERA_SIZE_X , -WORLD_CAMERA_SIZE_Y , WORLD_CAMERA_SIZE_X , WORLD_CAMERA_SIZE_Y );

	AABB2 boxCopy = m_normalImage;
	m_invertedColorImage = boxCopy.CarveBoxOffRight( 0.5f , 0.f );
	m_normalImage = m_normalImage.GetBoxAtLeft( 0.5f , 0.f );

	m_normalImage.SetDimensions( m_normalImage.GetDimensions() * 0.75f );
	m_normalImage.AlignWithinAABB2( boxCopy , ALIGN_CENTERED );

	boxCopy = m_invertedColorImage;
	m_invertedColorImage.SetDimensions( m_invertedColorImage.GetDimensions() * 0.75f );
	m_invertedColorImage.AlignWithinAABB2( boxCopy , ALIGN_CENTERED );

// 	float testInv[] = { 4,0,0,0, 0,0,2,0, 0,1,2,0, 1,0,0,1 };
// 	Mat44 testMat( testInv );
// 	MatrixInvert( testMat );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::~Game()
{
//	delete m_invertColorShader;
	m_invertColorShader = nullptr;

	delete m_imageTex;
	m_imageTex = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	UpdateFromKeyBoard( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	g_theRenderer->BeginCamera( m_gameCamera );

	g_theRenderer->BindShader( nullptr );

	//g_theRenderer->SetBlendMode( BlendMode::ALPHA );
	const Vertex_PCU AABB2Verts[ 6 ] = {
							Vertex_PCU( Vec3( -50.f,-50.f,-10.f ) , WHITE, Vec2( 0.f, 0.f ) ),
							Vertex_PCU( Vec3(  50.f,-50.f,-10.f ) , WHITE, Vec2( 1.f, 0.f ) ),
							Vertex_PCU( Vec3(	50.f,50.f,-10.f ) , WHITE, Vec2( 0.f, 1.f ) ),

							Vertex_PCU( Vec3( 50.f,50.f,-10.f ) , WHITE, Vec2( 1.f, 0.f ) ),
							Vertex_PCU( Vec3( -50.f,50.f,-10.f ) , WHITE, Vec2( 1.f, 1.f ) ),
							Vertex_PCU( Vec3( -50.f,-50.f,-10.f ) , WHITE, Vec2( 0.f, 1.f ) ) };

	g_theRenderer->DrawVertexArray( 6 , AABB2Verts );


	g_theRenderer->BindTexture( m_imageTex );
	g_theRenderer->DrawAABB2( m_normalImage , WHITE );
	g_theRenderer->BindShader( m_invertColorShader );
	g_theRenderer->DrawAABB2( m_invertedColorImage , WHITE );

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
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::CameraPositionUpdateOnInput( float deltaSeconds )
{
	Vec3 movement = Vec3::ZERO;

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

	float speed = 4.0f;
	m_cameraPosition += movement * speed * deltaSeconds;
	m_gameCamera.SetPostion( m_cameraPosition );
}

//--------------------------------------------------------------------------------------------------------------------------------------------