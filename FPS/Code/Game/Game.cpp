#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Primitives/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Time/Time.hpp"
#include "Game/Game.hpp"

#include "Engine/Math/MathConstants.hpp"
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
	m_gameCamera.SetProjectionPerspective( 60.f , CLIENT_ASPECT , -.1f , -100.f );
	m_gameCamera.SetPostion( Vec3( 0.f , 0.f , 0.f ) );

	m_meshCube = new GPUMesh( g_theRenderer );
	m_cubeTransform.SetPosition( 1.f , 0.5f , -15.0f );

	std::vector<Vertex_PCU> meshVerts;
	AddCubeVerts( meshVerts , nullptr );

	m_meshCube->UpdateVertices( meshVerts );
	m_meshCube->UpdateIndices( 36 , GetCubeIndices() );

	int stacks = 100;
	int slices = 100;
	std::vector<Vertex_PCU> sphereMeshVerts;
	for ( int t = 0; t < stacks; t++ ) // stacks are ELEVATION so they count theta
	{
		float theta1 = ( ( float ) ( t ) / stacks ) * PI;
		float theta2 = ( ( float ) ( t + 1 ) / stacks ) * PI;

		for ( int p = 0; p < slices; p++ ) // slices are ORANGE SLICES so the count azimuth
		{
			float phi1 = ( ( float ) ( p ) / slices ) * 2 * PI; // azimuth goes around 0 .. 2*PI
			float phi2 = ( ( float ) ( p + 1 ) / slices ) * 2 * PI;

			//phi2   phi1
			// |      |
			// 2------1 -- theta1
			// |\ _   |
			// |    \ |
			// 3------4 -- theta2
			//

			//vertex1 = vertex on a sphere of radius r at spherical coords theta1, phi1
			//vertex2 = vertex on a sphere of radius r at spherical coords theta1, phi2
			//vertex3 = vertex on a sphere of radius r at spherical coords theta2, phi2
			//vertex4 = vertex on a sphere of radius r at spherical coords theta2, phi1

			// facing out
//			if ( t == 0 ) // top cap
				
// 				mesh->addTri( vertex1 , vertex3 , vertex4 ); //t1p1, t2p2, t2p1
// 			else if ( t + 1 == stacks ) //end cap
// 				mesh->addTri( vertex3 , vertex1 , vertex2 ); //t2p2, t1p1, t1p2
// 			else
// 			{
// 				// body, facing OUT:
// 				mesh->addTri( vertex1 , vertex2 , vertex4 );
// 				mesh->addTri( vertex2 , vertex3 , vertex4 );
//			}
		}
	}

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
	m_meshCube = nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	static float y = 0;
	y += deltaSeconds;
	m_cubeTransform.SetRotation( 180.f * SinDegrees( y )/*( float ) GetCurrentTimeSeconds()*/ , 0.f , 0.f );
	UpdateFromKeyBoard( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	g_theRenderer->BeginCamera( m_gameCamera );
	g_theRenderer->SetModelMatrix( m_cubeTransform.GetAsMatrix() );

	g_theRenderer->BindShader( nullptr );

	//g_theRenderer->SetBlendMode( BlendMode::ALPHA );
// 	const Vertex_PCU AABB2Verts[ 6 ] = {
// 							Vertex_PCU( Vec3( -5.f,-5.f,-10.f ) , WHITE, Vec2( 0.f, 0.f ) ),
// 							Vertex_PCU( Vec3( 5.f,-5.f,-10.f ) , WHITE, Vec2( 1.f, 0.f ) ),
// 							Vertex_PCU( Vec3( 5.f,5.f,-10.f ) , WHITE, Vec2( 0.f, 1.f ) ),
// 
// 							Vertex_PCU( Vec3( 5.f,5.f,-10.f ) , WHITE, Vec2( 1.f, 0.f ) ),
// 							Vertex_PCU( Vec3( -5.f,5.f,-10.f ) , WHITE, Vec2( 1.f, 1.f ) ),
// 							Vertex_PCU( Vec3( -5.f,-5.f,-10.f ) , WHITE, Vec2( 0.f, 1.f ) ) };
// 
// 	g_theRenderer->DrawVertexArray( 6 , AABB2Verts );

	//g_theRenderer->DrawVertexArray( m_meshCube->GetVertexCount() , m_meshCube->GetVertexBuffer() );
	g_theRenderer->DrawMesh( m_meshCube );
	g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	g_theRenderer->BindTexture( m_imageTex );
	g_theRenderer->DrawAABB2( m_normalImage , WHITE );
	g_theRenderer->BindShader( m_invertColorShader );
	g_theRenderer->DrawAABB2( m_invertedColorImage , WHITE );
	g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindShader( nullptr );

	Vec2 mouseRelPos = g_theInput->GetRelativeMovement();
	g_theRenderer->DrawDisc( mouseRelPos , 3.f , RED );
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
	m_cameraRotation.x	+= mousePos.y * 0.1f;
	m_cameraRotation.y	+= mousePos.x * 0.1f;

	float finalPitch = Clamp( m_cameraRotation.x , -180.f , 180.f );
	float finalYaw	 = Clamp( m_cameraRotation.y, -180.f , 180.f );
	float finalRoll  = Clamp( m_cameraRotation.z, -90.f , 90.f );

	m_gameCamera.SetPostion( m_cameraPosition );
	m_gameCamera.SetPitchYawRollRotation( finalPitch , finalRoll, finalYaw  );
}

//--------------------------------------------------------------------------------------------------------------------------------------------