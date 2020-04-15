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

static  bool			s_areDevconsoleCommandsLoaded = false;

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC shaderLightDataT Game::m_lights;
STATIC Rgba8 Game::m_ambientLightColor;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	if ( !s_areDevconsoleCommandsLoaded )
	{
		AddLightDevConsoleCommands( g_theDevConsole );
		s_areDevconsoleCommandsLoaded = true;
	}
	
	LoadShaders();
	LoadTextures();
	
	m_tileDiffuse	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/tile_diffuse.png" );
	m_tileNormal	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/tile_normal.png" );
	   	
	InitializeCameras();
	intializeGameObjects();
		
	m_cubeMeshTransform.SetPosition( 5.f , 0.0f , -10.0f );
	m_sphereMeshTransform.SetPosition( -5.f , 0.0f , -10.0f );
	m_quadTransform.SetPosition( 0.f , 0.0f , -10.0f );

	m_lights.ambientLight = Vec4( 1.f , 1.f , 1.f , 0.f );
	m_ambientLightColor.SetColorFromNormalizedFloat( m_lights.ambientLight );
	m_lights.lights[ 0 ].color = Vec3( 1.f , 1.f , 1.f );
	//m_lights.lights[ 0 ].intensity = 0.0001f;
	m_lights.lights[ 0 ].intensity = 1.0f;
	m_lights.lights[ 0 ].world_position = Vec3( 0.f , 0.f , -5.f );
	m_lights.lights[ 0 ].attenuation = Vec3::UNIT_VECTOR_ALONG_J_BASIS;
	m_lights.lights[ 0 ].spec_attenuation = Vec3::UNIT_VECTOR_ALONG_K_BASIS;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadShaders()
{
	m_lightShaders[ LitShaderTypes::LIT ]						= g_theRenderer->GetOrCreateShader( "Data/Shaders/litDefault2.hlsl" );
	m_lightShaders[ LitShaderTypes::UNLIT ]						= g_theRenderer->GetOrCreateShader( "Data/Shaders/default.hlsl" );
	m_lightShaders[ LitShaderTypes::UV ]						= g_theRenderer->GetOrCreateShader( "Data/Shaders/uvDebugger.hlsl" );
	m_lightShaders[ LitShaderTypes::NORMAL ]					= g_theRenderer->GetOrCreateShader( "Data/Shaders/normalLit.hlsl" );
	m_lightShaders[ LitShaderTypes::TANGENT ]					= g_theRenderer->GetOrCreateShader( "Data/Shaders/tangentLit.hlsl" );
	m_lightShaders[ LitShaderTypes::BITANGENT ]					= g_theRenderer->GetOrCreateShader( "Data/Shaders/bitangentLit.hlsl" );
	m_lightShaders[ LitShaderTypes::SURFACE_NORMAL ]			= g_theRenderer->GetOrCreateShader( "Data/Shaders/surfaceNormalLit.hlsl" );
	m_lightShaders[ LitShaderTypes::FRESNEL ]					= g_theRenderer->GetOrCreateShader( "Data/Shaders/fresnel.hlsl" );

	m_currentShader = m_lightShaders[ LitShaderTypes::LIT ];
	m_currentShaderIndex = LitShaderTypes::LIT;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadTextures()
{
	m_triplanarShaderTextures[ 0 ] = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/rockT6/rock_06_diff_1k.png" );
	m_triplanarShaderTextures[ 1 ] = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/rockT6/rock_06_nor_1k.png" );
	m_triplanarShaderTextures[ 2 ] = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/grass/aerial_grass_rock_diff_1k.png" );
	m_triplanarShaderTextures[ 3 ] = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/grass/aerial_grass_rock_nor_1k.png" );
	m_triplanarShaderTextures[ 4 ] = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/dirt/dirt_aerial_03_diff_1k.png" );
	m_triplanarShaderTextures[ 5 ] = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/dirt/dirt_aerial_03_nor_1k.png" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::intializeGameObjects()
{
	m_cubeMesh = new GPUMesh( g_theRenderer );
	std::vector<VertexMaster>	cubeMeshVerts;
	std::vector<VertexLit>		cubeMeshLitVerts;
	std::vector<uint>			cubeMeshIndices;

	AABB3 box( Vec3( -1 , -1 , -1 ) , Vec3( 1 , 1 , 1 ) );
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

	std::vector<VertexMaster>	quadMeshVerts;
	std::vector<VertexLit>		quadMeshLitVerts;
	std::vector<uint>			quadIndices;

	CreateQuad( quadMeshVerts , quadIndices , AABB2::ZERO_TO_ONE );
	m_quadMesh = new GPUMesh( g_theRenderer );
//	m_quadMesh->UpdateVertices( ( uint ) sphereMeshLitVerts.size() , sphereMeshLitVerts.data() );
	m_quadMesh->UpdateVertices( ( uint ) quadMeshVerts.size() , quadMeshVerts.data() );
	m_quadMesh->UpdateIndices( quadIndices );
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::~Game()
{
	delete m_cubeMesh;
	m_cubeMesh			= nullptr;

	delete m_meshSphere;
	m_meshSphere		= nullptr;

	delete m_quadMesh;
	m_quadMesh			= nullptr;

	m_meshTex_D			= nullptr;
	m_meshTex_N			= nullptr;
	m_tileDiffuse		= nullptr;
	m_tileNormal		= nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::InitializeCameras()
{
		m_uiCamera.SetOrthoView( Vec2( 0.f , 0.f ) , Vec2( UI_SIZE_X , UI_SIZE_Y ) );
		m_gameCamera.SetProjectionPerspective( 60.f , CLIENT_ASPECT , -.1f , -100.f );
		m_gameCamera.SetPosition( Vec3( 0.f , 0.f , 0.f ) );
		m_gameCamera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 37 , 70 , 87 , 127 ) , 1.f , 0 );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	m_ambientLightColor.SetColorFromNormalizedFloat( m_lights.ambientLight );
	DebugDrawUI( deltaSeconds );
	UpdateLightPosition( deltaSeconds );

	static float y = 0;
	y += deltaSeconds;
	m_cubeMeshTransform.SetRotation( -15.f * ( float ) GetCurrentTimeSeconds()/* 0.f*/ ,  -20.f * ( float ) GetCurrentTimeSeconds() , 0.f );
	m_sphereMeshTransform.SetRotation( 20.f * ( float ) GetCurrentTimeSeconds() /*0.f*/,  50.f * ( float ) GetCurrentTimeSeconds() , 0.f );
	UpdateFromKeyBoard( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateLightPosition( float deltaSeconds )
{
	if ( m_isLightFollowingTheCamera )
	{
		m_lights.lights[ m_currentLightIndex ].world_position = m_gameCamera.GetPosition();
	}

	Rgba8 lightColor;
	lightColor.SetColorFromNormalizedFloat( Vec4( m_lights.lights[ 0 ].color , m_lights.lights[ 0 ].intensity ) );

	if ( !m_isLightFollowingTheCamera )
	{
		DebugAddWorldPoint( m_lights.lights[ 0 ].world_position , 0.125f , lightColor , deltaSeconds * 0.5f , DEBUG_RENDER_USE_DEPTH );
	}

	if ( m_isLightAnimated )
	{
		m_lights.lights[ 0 ].world_position = Vec3( 0 , 0 , -5 ) + Vec3::MakeFromSpericalCoordinates(
			45.f * ( float ) GetCurrentTimeSeconds() , 30.f * SinDegrees( ( float ) GetCurrentTimeSeconds() ) , 7.f );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::DebugDrawUI( float deltaSeconds )
{
	std::string cureentShaderName;

	switch ( m_currentShaderIndex )
	{
		case LitShaderTypes::LIT:
			cureentShaderName = "LIT SHADER";
			break;

		case LitShaderTypes::UNLIT:
		cureentShaderName = "UNLIT SHADER";
		break;

		case LitShaderTypes::UV:
			cureentShaderName = "UV SHADER";
			break;

		case LitShaderTypes::NORMAL:
			cureentShaderName = "VERTEX NORMAL SHADER";
			break;

		case LitShaderTypes::TANGENT:
			cureentShaderName = "VERTEX TANGENT SHADER";
			break;

		case LitShaderTypes::BITANGENT:
			cureentShaderName = "VERTEX BITANGENT SHADER";
			break;

		case LitShaderTypes::SURFACE_NORMAL:
			cureentShaderName = "SURFACE NORMAL SHADER";
			break;
		
		case LitShaderTypes::FRESNEL:
			cureentShaderName = "FRESNEL SHADER";
	}

	float leftVerticalAlignment = ( 1080.f * 0.25f ) / 11.f;
	float normalizedOffset = RangeMapFloat( 0.f , 1080.f , 0.f , 1.f , leftVerticalAlignment );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1.00f ) , Vec2::ZERO_ONE , 16.f , PINK , deltaSeconds ,
		"[ LT-ARRW / RT-ARRW ] : SWITCH SHADERS" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 1 * normalizedOffset ) ) , Vec2::ZERO_ONE , 16.f , PINK , deltaSeconds ,
		"CURRENT SHADER = %s" , cureentShaderName.c_str() );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 2 * normalizedOffset ) ) , Vec2::ZERO_ONE , 16.f , PINK , deltaSeconds ,
		"[ F5 ] = Position Light At Origin" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 3 * normalizedOffset ) ) , Vec2::ZERO_ONE , 16.f , PINK , deltaSeconds ,
		"[ F6 ] = Position Light At Current Camera Position" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 4 * normalizedOffset ) ) , Vec2::ZERO_ONE , 16.f , PINK , deltaSeconds ,
		"[ F7 ] = Light Follows Camera" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 5 * normalizedOffset ) ) , Vec2::ZERO_ONE , 16.f , PINK , deltaSeconds ,
		"[ F9 ] = Light Moves in an fixed Path" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 6 * normalizedOffset ) ) , Vec2::ZERO_ONE , 16.f , PINK , deltaSeconds ,
		"[ 9 / 0 ] = Change Ambient Light Intensity (Clamped 0-1)" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 7 * normalizedOffset ) ) , Vec2::ZERO_ONE , 16.f , PINK , deltaSeconds ,
		"[ - / + ] = Change Light Intensity (UnClamped)" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 8 * normalizedOffset ) ) , Vec2::ZERO_ONE , 16.f , PINK , deltaSeconds ,
		"[ [ / ] ] = Change Specular Factor (Clamped 0-1)" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 9 * normalizedOffset ) ) , Vec2::ZERO_ONE , 16.f , PINK , deltaSeconds ,
		"[ U / J ] = Change Specular Power (Clamped 1-INF)" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 10 * normalizedOffset ) ) , Vec2::ZERO_ONE , 16.f , PINK , deltaSeconds ,
		"[ T / R / G / Y ] = Change Light Attenuation to ZERO / Constant / Linear / Quadratic" );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1.f ) , Vec2::ONE , 16.f , ORANGE , deltaSeconds ,
		"Specular Power = %.2f" , m_lights.SPECULAR_POWER );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 1 * normalizedOffset ) ) , Vec2::ONE , 16.f , ORANGE , deltaSeconds ,
		"Specular Factor = %.2f" , m_lights.SPECULAR_FACTOR );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 2 * normalizedOffset ) ) , Vec2::ONE , 16.f , ORANGE , deltaSeconds ,
		"Attenuation = ( %.0f , %.0f , %.0f ) " , m_lights.lights[ 0 ].attenuation.x ,
		m_lights.lights[ 0 ].attenuation.y , m_lights.lights[ 0 ].attenuation.z );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 3 * normalizedOffset ) ) , Vec2::ONE , 16.f , ORANGE , deltaSeconds , "Ambient Light Intensity = %.2f" , m_lights.ambientLight.w );

	Rgba8 ambientLightColor;
	ambientLightColor.SetColorFromNormalizedFloat( m_lights.ambientLight );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 4 * normalizedOffset ) ) , Vec2::ONE , 16.f , ORANGE , deltaSeconds , "Ambient Light Color = R(%u) G(%u) B(%u) " , ambientLightColor.r , ambientLightColor.g , ambientLightColor.b );

	Rgba8 debuglightColor;
	debuglightColor.SetColorFromNormalizedFloat( Vec4( m_lights.lights[ 0 ].color , 1.f ) );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 5 * normalizedOffset ) ) , Vec2::ONE , 16.f , ORANGE ,
		deltaSeconds , "Light Intensity = %.2f" , m_lights.lights[ 0 ].intensity );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 6 * normalizedOffset ) ) , Vec2::ONE , 16.f , ORANGE ,
		deltaSeconds , "Light Color = R(%u) G(%u) B(%u) " , debuglightColor.r , debuglightColor.g ,
		debuglightColor.b );
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

	g_theRenderer->SetAmbientLight( m_ambientLightColor , m_lights.ambientLight.w );
	g_theRenderer->EnableLight( 0 , m_lights.lights[ 0 ] );
	
	g_theRenderer->SetSpecularFactor( m_lights.SPECULAR_FACTOR );
	g_theRenderer->SetSpecularPower( m_lights.SPECULAR_POWER );
	
	g_theRenderer->BindShader( m_currentShader );
	g_theRenderer->BindTexture( m_tileDiffuse );
	g_theRenderer->BindTexture( m_tileNormal , eTextureType::TEX_NORMAL );

	g_theRenderer->BindShader( g_theRenderer->GetOrCreateShader( "Data/Shaders/triplanar.hlsl" ) );
	g_theRenderer->BindTexture( m_triplanarShaderTextures[ 0 ] , eTextureType::TEX_USER_TYPE , 0 );
	g_theRenderer->BindTexture( m_triplanarShaderTextures[ 1 ] , eTextureType::TEX_USER_TYPE , 1 );
	g_theRenderer->BindTexture( m_triplanarShaderTextures[ 2 ] , eTextureType::TEX_USER_TYPE , 2 );
	g_theRenderer->BindTexture( m_triplanarShaderTextures[ 3 ] , eTextureType::TEX_USER_TYPE , 3 );
	g_theRenderer->BindTexture( m_triplanarShaderTextures[ 4 ] , eTextureType::TEX_USER_TYPE , 4 );
	g_theRenderer->BindTexture( m_triplanarShaderTextures[ 5 ] , eTextureType::TEX_USER_TYPE , 5 );
	
 	g_theRenderer->SetModelMatrix( m_sphereMeshTransform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_meshSphere );

	//g_theRenderer->BindShader( m_currentShader );
 	g_theRenderer->SetModelMatrix( m_quadTransform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_quadMesh );
	
	g_theRenderer->SetModelMatrix( m_cubeMeshTransform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_cubeMesh );
	
	if ( m_isFresnelShaderActive )													{	RenderFresnelShader2ndPass();	}
	
	g_theRenderer->SetRasterState( FILL_SOLID );

	g_theRenderer->BindShader( nullptr );
	g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->SetDepthTest( COMPARE_ALWAYS , true );
	g_theRenderer->SetCullMode( CULL_NONE );
	g_theRenderer->DisableLight( 0 );
	g_theRenderer->EndCamera( m_gameCamera );

	DebugRenderWorldToCamera( &m_gameCamera );
	DebugRenderScreenTo( nullptr );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderFresnelShader2ndPass() const
{
	g_theRenderer->BindShader( m_lightShaders[ LitShaderTypes::FRESNEL ] );
	g_theRenderer->SetBlendMode( ALPHA );

	g_theRenderer->SetModelMatrix( m_sphereMeshTransform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_meshSphere );

	g_theRenderer->SetModelMatrix( m_quadTransform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_quadMesh );

	g_theRenderer->SetModelMatrix( m_cubeMeshTransform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_cubeMesh );
	g_theRenderer->SetBlendMode( SOLID );
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

void Game::AddLightDevConsoleCommands( DevConsole* devConsole )
{
	EventArgs consoleArgs;
	devConsole->CreateCommand( "ChangeLightColor" , "Ex - ChangeLightColor idx = 0 |color = 255 , 255 , 255 , 255" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "ChangeLightColor" , ChangeLightColorViaConsole );

	devConsole->CreateCommand( "ChangeLightAttenuation" ,
		"Update Light Attenuation of Ambient Light .Ex - ChangeLightAttenuation idx = 0 |attenuation = 1.f , 0.f , 2.3f" ,
		consoleArgs );
	g_theEventSystem->SubscribeToEvent( "ChangeLightAttenuation" , ChangeLightAttenuationViaConsole );

	devConsole->CreateCommand( "ChangeAmbientLightColor" ,
	                           "Update Color of Ambient Light .Ex - ChangeAmbientLightColor color = 255 , 255 , 255 , 255" ,
	                           consoleArgs );
	g_theEventSystem->SubscribeToEvent( "ChangeAmbientLightColor" , ChangeAmbientLightColorViaConsole );

	devConsole->CreateCommand( "ChangeAmbientLightIntensity" ,
		"Update Intensity of Ambient Light .Ex -  ChangeAmbientLightIntensity intensity = 0.3f" ,
		consoleArgs );
	g_theEventSystem->SubscribeToEvent( "ChangeAmbientLightIntensity" , ChangeAmbientLightIntensityViaConsole );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool Game::ChangeLightColorViaConsole( EventArgs& args )
{
	int		lightIndex		= args.GetValue( "idx " , 0 );
	Rgba8	color			= args.GetValue( "color " , WHITE );
	Vec4	normalizedColor = color.GetAsNormalizedFloat4();
			lightIndex	   %= TOTAL_LIGHTS;
			m_lights.lights[ lightIndex ].color = Vec3( normalizedColor.x , normalizedColor.y , normalizedColor.z );

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool Game::ChangeLightAttenuationViaConsole( EventArgs& args )
{
	int		lightIndex	= args.GetValue( "idx " , 0 );
	Vec3	attenuation = args.GetValue( "attenuation " , Vec3::UNIT_VECTOR_ALONG_J_BASIS );
			lightIndex %= TOTAL_LIGHTS;
			m_lights.lights[ lightIndex ].attenuation = attenuation;

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool Game::ChangeAmbientLightColorViaConsole( EventArgs& args )
{
	Rgba8	color			= args.GetValue( "color " , WHITE );
	Vec4	normalizedColor = color.GetAsNormalizedFloat4();

	m_lights.ambientLight	= Vec4( normalizedColor.x , normalizedColor.y , normalizedColor.z , m_lights.ambientLight.w );
	m_ambientLightColor.SetColorFromNormalizedFloat( m_lights.ambientLight );
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool Game::ChangeAmbientLightIntensityViaConsole( EventArgs& args )
{
	float	intensity		= args.GetValue( "intensity " , 1.f );
			intensity		= ClampZeroToOne(intensity);
	m_lights.ambientLight.w = intensity;
	return true;
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
	UpdateLightsFromKeyBoard( deltaSeconds );
	
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

void Game::UpdateLightsFromKeyBoard( float deltaSeconds )
{
	UpdateLightPositionOnUserInput();
	UpdateCurrentShaderFromUserInput();
	UpdateAmbientLightFromUserInput( deltaSeconds );
	UpdateSpecularLightFromUserInput( deltaSeconds );
	UpdateLightAttenuationFromUserInput();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateCurrentShaderFromUserInput()
{
	if ( g_theInput->WasKeyJustPressed( KEY_LEFTARROW ) )
	{
		m_currentShaderIndex -= 1;

		if ( m_currentShaderIndex < 0 )
		{
			m_currentShaderIndex = LitShaderTypes::TOTAL - 1;
		}

		m_currentShaderIndex %= LitShaderTypes::TOTAL;
		m_currentShader = m_lightShaders[ m_currentShaderIndex ];

		if ( LitShaderTypes::FRESNEL == m_currentShaderIndex )
		{
			m_isFresnelShaderActive = true;
			m_currentShader = m_lightShaders[ LitShaderTypes::LIT ];
		}
		else
		{
			m_isFresnelShaderActive = false;
		}
	}

	if ( g_theInput->WasKeyJustPressed( KEY_RIGHTARROW ) )
	{
		m_currentShaderIndex += 1;
		m_currentShaderIndex %= LitShaderTypes::TOTAL;
		m_currentShader = m_lightShaders[ m_currentShaderIndex ];

		if ( LitShaderTypes::FRESNEL == m_currentShaderIndex )
		{
			m_isFresnelShaderActive = true;
			m_currentShader = m_lightShaders[ LitShaderTypes::LIT ];
		}
		else
		{
			m_isFresnelShaderActive = false;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateAmbientLightFromUserInput( float deltaSeconds )
{
	if ( g_theInput->IsKeyHeldDown( '9' ) )
	{
		m_lights.ambientLight.w -= deltaSeconds;
		m_lights.ambientLight.w = ClampZeroToOne( m_lights.ambientLight.w );
	}

	if ( g_theInput->IsKeyHeldDown( '0' ) )
	{
		m_lights.ambientLight.w += deltaSeconds;
		m_lights.ambientLight.w = ClampZeroToOne( m_lights.ambientLight.w );
	}

	if ( g_theInput->IsKeyHeldDown( KEY_MINUS ) )
	{
		m_lights.lights[ m_currentLightIndex ].intensity -= deltaSeconds;
		m_lights.ambientLight.w = ClampZeroToOne( m_lights.ambientLight.w );
	}

	if ( g_theInput->IsKeyHeldDown( KEY_PLUS ) )
	{
		m_lights.lights[ m_currentLightIndex ].intensity += deltaSeconds;
		m_lights.ambientLight.w = ClampZeroToOne( m_lights.ambientLight.w );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateSpecularLightFromUserInput( float deltaSeconds )
{
	if ( g_theInput->IsKeyHeldDown( 'U' ) )
	{
		m_lights.SPECULAR_POWER -= 20.f * deltaSeconds;
		m_lights.SPECULAR_POWER = Clamp( m_lights.SPECULAR_POWER , 1.f , INFINITY );
		return;
	}

	if ( g_theInput->IsKeyHeldDown( 'J' ) )
	{
		m_lights.SPECULAR_POWER += 20.f * deltaSeconds;
		m_lights.SPECULAR_POWER = Clamp( m_lights.SPECULAR_POWER , 1.f , INFINITY );
		return;
	}

	if ( g_theInput->IsKeyHeldDown( KEY_LEFT_SQ_BRACKET ) )
	{
		m_lights.SPECULAR_FACTOR -= deltaSeconds;
		m_lights.SPECULAR_FACTOR = ClampZeroToOne( m_lights.SPECULAR_FACTOR );
	}

	if ( g_theInput->IsKeyHeldDown( KEY_RIGHT_SQ_BRACKET ) )
	{
		m_lights.SPECULAR_FACTOR += deltaSeconds;
		m_lights.SPECULAR_FACTOR = ClampZeroToOne( m_lights.SPECULAR_FACTOR );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateLightAttenuationFromUserInput()
{
	if ( g_theInput->WasKeyJustPressed( 'T' ) )
	{
		m_lights.lights[ 0 ].attenuation = Vec3::ZERO;
	}

	if ( g_theInput->WasKeyJustPressed( 'R' ) )
	{
		m_lights.lights[ 0 ].attenuation = Vec3::UNIT_VECTOR_ALONG_I_BASIS;
	}

	if ( g_theInput->WasKeyJustPressed( 'G' ) )
	{
		m_lights.lights[ 0 ].attenuation = Vec3::UNIT_VECTOR_ALONG_J_BASIS;
	}

	if ( g_theInput->WasKeyJustPressed( 'Y' ) )
	{
		m_lights.lights[ 0 ].attenuation = Vec3::UNIT_VECTOR_ALONG_K_BASIS;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateLightPositionOnUserInput()
{
	if ( g_theInput->WasKeyJustPressed( KEY_F5 ) )
	{
		m_isLightFollowingTheCamera = false;
		m_isLightAnimated = false;
		m_lights.lights[ m_currentLightIndex ].world_position = Vec3::ZERO;
	}

	if ( g_theInput->WasKeyJustPressed( KEY_F6 ) )
	{
		m_isLightFollowingTheCamera = false;
		m_isLightAnimated = false;
		m_lights.lights[ m_currentLightIndex ].world_position = m_gameCamera.GetPosition();
	}

	if ( g_theInput->WasKeyJustPressed( KEY_F7 ) )
	{
		m_isLightFollowingTheCamera = true;
		m_isLightAnimated = false;
	}

	if ( g_theInput->WasKeyJustPressed( KEY_F9 ) )
	{
		m_isLightFollowingTheCamera = false;
		m_isLightAnimated = true;
	}
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