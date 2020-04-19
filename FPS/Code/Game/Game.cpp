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
STATIC Rgba8			Game::m_ambientLightColor;
STATIC LightType		Game::m_lightType[ TOTAL_LIGHTS ];
STATIC fresnelData_t	Game::m_fresnelShaderData;
STATIC dissolveData_t	Game::m_dissolveShaderData;
STATIC fogDataT			Game::m_fogData;
STATIC Texture*			Game::m_dissolveShaderPatternTexture;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	if ( !s_areDevconsoleCommandsLoaded )
	{
		AddLightDevConsoleCommands( g_theDevConsole );
		AddShaderDevConsoleCommands( g_theDevConsole );
		AddFogCommandsToDevConsole( g_theDevConsole );
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
	
	InitializeLightData();
	InitializeShaderMaterialData();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::InitializeLightData()
{
	m_lights.ambientLight								= Vec4( 1.f , 1.f , 1.f , 0.f );
	m_ambientLightColor.SetColorFromNormalizedFloat( m_lights.ambientLight );
	m_lights.lights[ 0 ].color							= Vec3( 1.f , 1.f , 1.f );
	m_lights.lights[ 0 ].intensity						= 1.0f;
	m_lights.lights[ 0 ].worldPosition					= Vec3( 0.f , 0.f , -5.f );
	m_lights.lights[ 0 ].attenuation					= Vec3::UNIT_VECTOR_ALONG_J_BASIS;
	m_lights.lights[ 0 ].specularAttenuation			= Vec3::UNIT_VECTOR_ALONG_K_BASIS;

	RandomNumberGenerator rng;
	
	for ( uint index = 1 ; index < TOTAL_LIGHTS ; index++ )
	{
		Rgba8 color;
		color.RollRandomColor( rng );
		rng.manuallyIncrementPosition();
		m_lights.lights[ index ].color					= color.GetAsNormalizedFloat3();
		m_lights.lights[ index ].intensity				= 0.f;
		m_lights.lights[ index ].attenuation			= Vec3::UNIT_VECTOR_ALONG_K_BASIS;
		m_lights.lights[ index ].specularAttenuation	= Vec3::UNIT_VECTOR_ALONG_K_BASIS;
		m_lights.lights[ index ].worldPosition			= Vec3( index * 1.5f , 0.f , 10.f );
	}

	for ( uint index = 0 ; index < TOTAL_LIGHTS ; index++ )
	{
		m_lightType[ index ] = POINT_LIGHT;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::InitializeShaderMaterialData()
{
	m_fresnelShaderData.fresnelColor			= Vec3::UNIT_VECTOR_ALONG_J_BASIS;
	m_fresnelShaderData.fresnelfactor			= 1.f;
	m_fresnelShaderData.fresnelPower			= 1.f;

	m_dissolveShaderData.startColor				= CYAN.GetAsNormalizedFloat3();
	m_dissolveShaderData.burnEdgeWidth			= 1.f;
	m_dissolveShaderData.endColor				= ORANGE.GetAsNormalizedFloat3();
	m_dissolveShaderData.burnValue				= 0.f;

	m_fogData.nearFog							= 0.f;
	m_fogData.farFog							= 15.f;
	
	m_fogData.nearFogColor						= GRAY.GetAsNormalizedFloat3();
	m_fogData.farFogColor						= /*WHITE.GetAsNormalizedFloat3();*/ Rgba8( 37 , 70 , 87 , 127 ).GetAsNormalizedFloat3();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadShaders()
{
	m_lightShaders[ LitShaderTypes::LIT ]						= g_theRenderer->GetOrCreateShader( "Data/Shaders/litDefault.hlsl" );
	m_lightShaders[ LitShaderTypes::UNLIT ]						= g_theRenderer->GetOrCreateShader( "Data/Shaders/default.hlsl" );
	m_lightShaders[ LitShaderTypes::UV ]						= g_theRenderer->GetOrCreateShader( "Data/Shaders/uvDebugger.hlsl" );
	m_lightShaders[ LitShaderTypes::NORMAL ]					= g_theRenderer->GetOrCreateShader( "Data/Shaders/normalLit.hlsl" );
	m_lightShaders[ LitShaderTypes::TANGENT ]					= g_theRenderer->GetOrCreateShader( "Data/Shaders/tangentLit.hlsl" );
	m_lightShaders[ LitShaderTypes::BITANGENT ]					= g_theRenderer->GetOrCreateShader( "Data/Shaders/bitangentLit.hlsl" );
	m_lightShaders[ LitShaderTypes::SURFACE_NORMAL ]			= g_theRenderer->GetOrCreateShader( "Data/Shaders/surfaceNormalLit.hlsl" );
	m_lightShaders[ LitShaderTypes::DISSOLVE ]					= g_theRenderer->GetOrCreateShader( "Data/Shaders/dissolve.hlsl" );
	m_lightShaders[ LitShaderTypes::FRESNEL ]					= g_theRenderer->GetOrCreateShader( "Data/Shaders/fresnel.hlsl" );
	m_lightShaders[ LitShaderTypes::TRIPLANAR_UNLIT ]			= g_theRenderer->GetOrCreateShader( "Data/Shaders/triplanarUnlit.hlsl" );
	m_lightShaders[ LitShaderTypes::TRIPLANAR_LIT ]				= g_theRenderer->GetOrCreateShader( "Data/Shaders/triplanarLit.hlsl" );
	//m_lightShaders[ LitShaderTypes::FOG ]						= g_theRenderer->GetOrCreateShader( "Data/Shaders/fog.hlsl" );

	m_currentShader = m_lightShaders[ LitShaderTypes::LIT ];
	m_currentShaderIndex = LitShaderTypes::LIT;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadTextures()
{
	m_dissolveShaderPatternTexture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/noise.png" );
	
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
	std::vector<uint>			cubeMeshIndices;

	AABB3 box( Vec3( -1 , -1 , -1 ) , Vec3( 1 , 1 , 1 ) );
	CreateCuboid( cubeMeshVerts , cubeMeshIndices , box , WHITE );
	
	m_cubeMesh->UpdateVertices( ( uint ) cubeMeshVerts.size() , cubeMeshVerts.data() );
	m_cubeMesh->UpdateIndices( cubeMeshIndices );

	std::vector<VertexMaster>	sphereMeshVerts;
	std::vector<uint>			sphereIndices;

	CreateUVSphere( m_hCuts , m_vCuts , sphereMeshVerts , sphereIndices , 1.f );

	m_meshSphere = new GPUMesh( g_theRenderer );
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
	m_tileDiffuse		= nullptr;
	m_tileNormal		= nullptr;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::InitializeCameras()
{
		m_gameCamera.SetProjectionPerspective( 60.f , CLIENT_ASPECT , -.1f , -100.f );
		m_gameCamera.SetPosition( Vec3( 0.f , 0.f , 0.f ) );
		m_gameCamera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , Rgba8( 37 , 70 , 87 , 127 ) , 1.f , 0 );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	m_ambientLightColor.SetColorFromNormalizedFloat( m_lights.ambientLight );
	
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

	for ( uint index = 0; index < TOTAL_LIGHTS; index++ )
	{
		Rgba8 lightColor;
		lightColor.SetColorFromNormalizedFloat( Vec4( m_lights.lights[ index ].color , m_lights.lights[ index ].intensity ) );

		if ( ( index == m_currentLightIndex ) & m_isLightFollowingTheCamera )
		{
			m_lights.lights[ m_currentLightIndex ].worldPosition = m_gameCamera.GetPosition();
			Vec3 direction = m_gameCamera.GetCameraTransform().GetAsMatrix().GetKBasis3D();
			m_lights.lights[ m_currentLightIndex ].direction = -direction;
			continue;
		}
		if ( POINT_LIGHT == m_lightType[ index ] )
		{
			DebugAddWorldPoint( m_lights.lights[ index ].worldPosition , 0.125f , lightColor , deltaSeconds * 0.5f , DEBUG_RENDER_USE_DEPTH );
		}
		if ( DIRECTIONAL_LIGHT == m_lightType[ index ] )
		{
			DebugAddWorldArrow( m_lights.lights[ index ].worldPosition , m_lights.lights[ index ].worldPosition + m_lights.lights[ index ].direction , lightColor , deltaSeconds * 0.5f );
		}
		if ( SPOT_LIGHT == m_lightType[ index ] )
		{
			DebugAddWorldArrow( m_lights.lights[ index ].worldPosition , m_lights.lights[ index ].worldPosition + m_lights.lights[ index ].direction , lightColor , deltaSeconds * 0.5f );
		}
	}


	if ( m_isLightAnimated )
	{
		m_lights.lights[ m_currentLightIndex ].worldPosition = Vec3( 0.f , 0.f , -5.f ) + Vec3::MakeFromSpericalCoordinates(
			45.f * ( float ) GetCurrentTimeSeconds() , 30.f * SinDegrees( ( float ) GetCurrentTimeSeconds() ) , 5.f );
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
		case LitShaderTypes::DISSOLVE:
			cureentShaderName = "DISSOLVE LIT SHADER";
			break;
		
		case LitShaderTypes::FRESNEL:
			cureentShaderName = "FRESNEL SHADER";
			break;
		
		case LitShaderTypes::TRIPLANAR_LIT:
			cureentShaderName = "TRIPLANAR LIT SHADER";
			break;
		
		case LitShaderTypes::TRIPLANAR_UNLIT:
			cureentShaderName = "TRIPLANAR UNLIT SHADER";
			break;
	}

	float leftVerticalAlignment = ( 1080.f * 0.25f ) / 16.f;
	float normalizedOffset = RangeMapFloat( 0.f , 1080.f , 0.f , 1.f , leftVerticalAlignment );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1.00f ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
		"[ LT-ARRW / RT-ARRW ] : SWITCH SHADERS" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 1 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
		"CURRENT SHADER = %s" , cureentShaderName.c_str() );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 2 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
		"[ F5 ] = Position Light At Origin" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 3 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
		"[ F6 ] = Position Light At Current Camera Position" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 4 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
		"[ F7 ] = Light Follows Camera" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 5 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
		"[ F9 ] = Light Moves in an fixed Path" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 6 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
		"[ 9 / 0 ] = Change Ambient Light Intensity (Clamped 0-1)" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 7 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
		"[ - / + ] = Change Light Intensity (UnClamped)" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 8 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
		"[ [ / ] ] = Change Specular Factor (Clamped 0-1)" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 9 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
		"[ U / J ] = Change Specular Power (Clamped 1-INF)" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 10 * normalizedOffset ) ) , Vec2::ZERO_ONE, 14.5f , PINK , deltaSeconds ,
		"[ T / R / G / Y ] = Change Current Light Attenuation to ZERO / Constant / Linear / Quadratic" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 11 * normalizedOffset ) ) , Vec2::ZERO_ONE, 14.5f , PINK , deltaSeconds ,
		"[ 1 - 8 ] = Change Current Selected Light" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 12 * normalizedOffset ) ) , Vec2::ZERO_ONE, 14.5f , PINK , deltaSeconds ,
		"[ M ] = Cycle Current Selected Light between POINT / DIRECTIONAL / SPOT" );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 13 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
		"[ Q / E ] = ENABLE / DISABLE Currently Selected Light" );

	if ( LitShaderTypes::FRESNEL == m_currentShaderIndex )
	{
		DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 14 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
			"[ Z / X ] = Fresnel Factor = %f", m_fresnelShaderData.fresnelfactor );

		DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 15 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
			"[ C / V ] = Fresnel Power = %f" , m_fresnelShaderData.fresnelPower );

		DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 16 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
			"Fresnel Color = ( %u , %u ,%u )" , (uint)(m_fresnelShaderData.fresnelColor.x * 255), ( uint ) (m_fresnelShaderData.fresnelColor.y * 255), ( uint ) ( m_fresnelShaderData.fresnelColor.z * 255 ) );
	}

	if ( LitShaderTypes::DISSOLVE == m_currentShaderIndex )
	{
		DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 14 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
			"[ Z / X ] = Dissolve Burn Value = %f" , m_dissolveShaderData.burnValue );

		DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 15 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
			"[ C / V ] = Dissolve Burn Edge Width = %f" , m_dissolveShaderData.burnValue );

		DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 16 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
			"Dissolve Start Color = ( %u , %u ,%u )" , ( uint ) ( m_dissolveShaderData.startColor.x * 255 ) , ( uint ) ( m_dissolveShaderData.startColor.y * 255 ) , ( uint ) ( m_dissolveShaderData.startColor.z * 255 ) );

		DebugAddScreenTextf( Vec4( 0.f , 0.f , 0.f , 1 - ( 16 * normalizedOffset ) ) , Vec2::ZERO_ONE , 14.5f , PINK , deltaSeconds ,
			"Dissolve End Color = ( %u , %u ,%u )" , ( uint ) ( m_dissolveShaderData.endColor.x * 255 ) , ( uint ) ( m_dissolveShaderData.endColor.y * 255 ) , ( uint ) ( m_dissolveShaderData.endColor.z * 255 ) );
	}

//--------------------------------------------------------------------------------------------------------------------------------------------

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1.f ) , Vec2::ONE , 14.5f , ORANGE , deltaSeconds ,
		"Global Specular Power = %.2f" , m_lights.SPECULAR_POWER );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 1 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE , deltaSeconds ,
		"Global Specular Factor = %.2f" , m_lights.SPECULAR_FACTOR );
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 2 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE , deltaSeconds , "Ambient Light Intensity = %.2f" , m_lights.ambientLight.w );

	Rgba8 ambientLightColor;
	ambientLightColor.SetColorFromNormalizedFloat( m_lights.ambientLight );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 3 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE , deltaSeconds , "Ambient Light Color = R(%u) G(%u) B(%u) " , ambientLightColor.r , ambientLightColor.g , ambientLightColor.b );

	Rgba8 debuglightColor;
	debuglightColor.SetColorFromNormalizedFloat( Vec4( m_lights.lights[ m_currentLightIndex ].color , 1.f ) );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 4 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
		deltaSeconds , "Current Light Number = %u" , m_currentLightIndex );

	if ( POINT_LIGHT == m_lightType[ m_currentLightIndex ] )
	{
		DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 5 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
			deltaSeconds , "Current Light Type = POINT LIGHT");
	}
	else if ( DIRECTIONAL_LIGHT == m_lightType[ m_currentLightIndex ] )
	{
		DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 5 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
			deltaSeconds , "Current Light Type = DIRECTIONAL LIGHT" );
	}
	else if ( SPOT_LIGHT == m_lightType[ m_currentLightIndex ] )
	{
		DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 5 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
			deltaSeconds , "Current Light Type = SPOT LIGHT" );
	}

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 6 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
		deltaSeconds , "Current Light Intensity = %.2f" , m_lights.lights[ m_currentLightIndex ].intensity );
	
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 7 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE , deltaSeconds ,
		"Diffuse Attenuation = ( %.3f , %.3f , %.3f ) " , m_lights.lights[ m_currentLightIndex ].attenuation.x ,
		m_lights.lights[ m_currentLightIndex ].attenuation.y , m_lights.lights[ m_currentLightIndex ].attenuation.z );
	
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 8 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
		deltaSeconds , "Current Light Color = R(%u) G(%u) B(%u) " , debuglightColor.r , debuglightColor.g ,
		debuglightColor.b );
	
	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 11 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
		deltaSeconds , "Current Light InnerAngle = %f" , m_lights.lights[ m_currentLightIndex ].dotInnerAngle );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 12 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
		deltaSeconds , "Current Light OuterAngle = %f" , m_lights.lights[ m_currentLightIndex ].dotOuterAngle );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 13 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
		deltaSeconds , "Current Light Specular attenuation = ( %.0f , %.0f , %.0f )" ,
		m_lights.lights[ m_currentLightIndex ].specularAttenuation.x ,
		m_lights.lights[ m_currentLightIndex ].specularAttenuation.y ,
		m_lights.lights[ m_currentLightIndex ].specularAttenuation.z );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 14 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
		deltaSeconds , "Current Light direction Factor = %f" , m_lights.lights[ m_currentLightIndex ].directionFactor );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 15 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
		deltaSeconds , "Current Light direction = ( %.0f , %.0f , %.0f )" ,
		m_lights.lights[ m_currentLightIndex ].direction.x ,
		m_lights.lights[ m_currentLightIndex ].direction.y ,
		m_lights.lights[ m_currentLightIndex ].direction.z );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 17 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
		deltaSeconds , "Fog Near Distance = %f" , m_fogData.nearFog );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 18 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
		deltaSeconds , "Fog Far Distance = %f" , m_fogData.farFog );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 19 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
		deltaSeconds , "Fog Near Color = ( %u , %u , %u )" ,
		( uint ) ( m_fogData.nearFogColor.x * 255.f ) , ( uint ) ( m_fogData.nearFogColor.y * 255.f ) , ( uint ) ( m_fogData.nearFogColor.z * 255.f ) );

	DebugAddScreenTextf( Vec4( 0.f , 0.f , 1.f , 1 - ( 20 * normalizedOffset ) ) , Vec2::ONE , 14.5f , ORANGE ,
		deltaSeconds , "Fog Far Color = ( %u , %u , %u )" ,
		( uint ) ( m_fogData.farFogColor.x * 255.f ) , ( uint ) ( m_fogData.farFogColor.y * 255.f ) , ( uint ) ( m_fogData.farFogColor.z * 255.f ) );
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
	//g_theRenderer->EnableLight( 0 , m_lights.lights[ 0 ] );
	//g_theRenderer->EnableLight( 1 , m_lights.lights[ 1 ] );
	//g_theRenderer->EnableLight( 2 , m_lights.lights[ 2 ] );
	//g_theRenderer->EnableLight( 3 , m_lights.lights[ 3 ] );
	g_theRenderer->UpdateLightsData( m_lights );
	g_theRenderer->EnableAllLights();
	g_theRenderer->SetSpecularFactor( m_lights.SPECULAR_FACTOR );
	g_theRenderer->SetSpecularPower( m_lights.SPECULAR_POWER );
	
	g_theRenderer->BindShader( m_currentShader );
	g_theRenderer->BindTexture( m_tileDiffuse );
	g_theRenderer->BindTexture( m_tileNormal , eTextureType::TEX_NORMAL );

	//g_theRenderer->EnableFog( m_fogData.nearFog , m_fogData.farFog , m_fogData.nearFogColor , m_fogData.farFogColor );
	g_theRenderer->EnableFog( m_fogData );

	BindShaderSpecificMaterialData();

 	g_theRenderer->SetModelMatrix( m_sphereMeshTransform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_meshSphere );

	g_theRenderer->DisableFog();

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

void Game::BindShaderSpecificMaterialData() const
{
	if ( LitShaderTypes::TRIPLANAR_LIT == m_currentShaderIndex || LitShaderTypes::TRIPLANAR_UNLIT == m_currentShaderIndex )
	{
		g_theRenderer->BindTexture( m_triplanarShaderTextures[ 0 ] , eTextureType::TEX_USER_TYPE , 0 );
		g_theRenderer->BindTexture( m_triplanarShaderTextures[ 1 ] , eTextureType::TEX_USER_TYPE , 1 );
		g_theRenderer->BindTexture( m_triplanarShaderTextures[ 2 ] , eTextureType::TEX_USER_TYPE , 2 );
		g_theRenderer->BindTexture( m_triplanarShaderTextures[ 3 ] , eTextureType::TEX_USER_TYPE , 3 );
		g_theRenderer->BindTexture( m_triplanarShaderTextures[ 4 ] , eTextureType::TEX_USER_TYPE , 4 );
		g_theRenderer->BindTexture( m_triplanarShaderTextures[ 5 ] , eTextureType::TEX_USER_TYPE , 5 );
	}

	if ( LitShaderTypes::DISSOLVE == m_currentShaderIndex )
	{
		g_theRenderer->BindTexture( m_dissolveShaderPatternTexture , eTextureType::TEX_USER_TYPE , 0 );
		g_theRenderer->BindMaterialData( ( void* ) &m_dissolveShaderData , sizeof( m_dissolveShaderData ) );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderFresnelShader2ndPass() const
{
	g_theRenderer->BindShader( m_lightShaders[ LitShaderTypes::FRESNEL ] );
	g_theRenderer->BindMaterialData( ( void* ) &m_fresnelShaderData , sizeof( m_fresnelShaderData ) );
	g_theRenderer->SetBlendMode( ALPHA );

	g_theRenderer->SetModelMatrix( m_sphereMeshTransform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_meshSphere );

	g_theRenderer->SetModelMatrix( m_cubeMeshTransform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_cubeMesh );
	g_theRenderer->SetBlendMode( SOLID );
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

void Game::AddShaderDevConsoleCommands( DevConsole* devConsole )
{
	EventArgs consoleArgs;
	
	devConsole->CreateCommand( "UpdateFresnelShader" ,
		"Update Fresnel Shader.Ex -  UpdateFresnelShader color = 255, 0 , 255 , 255| power = .9f| factor = .3f" ,
		consoleArgs );
	g_theEventSystem->SubscribeToEvent( "UpdateFresnelShader" , UpdateFresnelShaderMaterialDataViaConsole );

	devConsole->CreateCommand( "UpdateDissolveShader" ,
		"Ex -  UpdateDissolveShader startColor = 255, 0 , 255 , 255| endColor = 255, 0 , 255 , 255| edgeWidth = .9f| burnValue = .3f" ,
		consoleArgs );
	g_theEventSystem->SubscribeToEvent( "UpdateDissolveShader" , UpdateDissolveShaderMaterialViaConsole );

	devConsole->CreateCommand( "UpdateDissolveShaderPattern" ,
		"Ex -  UpdateDissolveShaderPattern patternTexPath = Data\\Images\\..." ,
		consoleArgs );
	g_theEventSystem->SubscribeToEvent( "UpdateDissolveShaderPattern" , UpdateDissolveShaderPatternViaConsole );
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool Game::UpdateFresnelShaderMaterialDataViaConsole( EventArgs& args )
{
	Rgba8 curColor;
	curColor.SetColorFromNormalizedFloat( Vec4( m_fresnelShaderData.fresnelColor , 1.f ) );
	Rgba8 color = args.GetValue( "color " , curColor );
	m_fresnelShaderData.fresnelColor = color.GetAsNormalizedFloat3();

	float power = args.GetValue( "power " , m_fresnelShaderData.fresnelPower );
	m_fresnelShaderData.fresnelPower = power;

	float factor = args.GetValue( "factor " , m_fresnelShaderData.fresnelfactor );
	m_fresnelShaderData.fresnelfactor = factor;

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Game::UpdateDissolveShaderMaterialViaConsole( EventArgs& args )
{
	Rgba8 curStartColor;
	curStartColor.SetColorFromNormalizedFloat( Vec4( m_dissolveShaderData.startColor , 1.f ) );
	Rgba8 startColor = args.GetValue( "startColor " , curStartColor );
	m_dissolveShaderData.startColor = startColor.GetAsNormalizedFloat3();

	Rgba8 curEndColor;
	curEndColor.SetColorFromNormalizedFloat( Vec4( m_dissolveShaderData.endColor , 1.f ) );
	Rgba8 endColor = args.GetValue( "endColor " , curStartColor );
	m_dissolveShaderData.startColor = endColor.GetAsNormalizedFloat3();

	float burnEdgeWidth = args.GetValue( "edgeWidth " , m_dissolveShaderData.burnEdgeWidth );
	m_dissolveShaderData.burnEdgeWidth = burnEdgeWidth;

	float burnValue = args.GetValue( "burnValue " , m_dissolveShaderData.burnValue );
	m_dissolveShaderData.burnValue = burnValue;

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Game::UpdateDissolveShaderPatternViaConsole( EventArgs& args )
{
	std::string patternTexPath		= args.GetValue( "patternTexPath " ,"" );
	m_dissolveShaderPatternTexture	= g_theRenderer->GetOrCreateTextureFromFile( patternTexPath.c_str() );

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::AddFogCommandsToDevConsole( DevConsole* devConsole )
{
	EventArgs consoleArgs;
	devConsole->CreateCommand( "DisableFog" , "Disables the Fog" , consoleArgs );
	g_theEventSystem->SubscribeToEvent( "DisableFog" , DisableFog );

	devConsole->CreateCommand( "EnableFog" ,
		"Ex - EnableFog nearFog = 0.f |farFog = 15.f |nearFogColor = 100,100,100,100 |farFogColor = 255,100,0,100" ,
		consoleArgs );
	g_theEventSystem->SubscribeToEvent( "EnableFog" , UpdateFog );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool Game::UpdateFog( EventArgs& args )
{
	m_fogData.nearFog		= args.GetValue( "nearFog "			, m_fogData.nearFog );
	m_fogData.farFog		= args.GetValue( "farFog "			, m_fogData.farFog );
	 
	Rgba8 nearFogColor		= args.GetValue( "nearFogColor "	, GRAY );
	Rgba8 farFogColor		= args.GetValue( "farFogColor "		, Rgba8( 37 , 70 , 87 , 127 ) );
	
	m_fogData.nearFogColor	= nearFogColor.GetAsNormalizedFloat3();
	m_fogData.farFogColor	= farFogColor.GetAsNormalizedFloat3();

	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

STATIC bool Game::DisableFog( EventArgs& args )
{
	UNUSED( args );
	m_fogData.nearFog			= INFINITY;
	m_fogData.farFog			= INFINITY;
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateFromKeyBoard( float deltaSeconds )
{
	if ( g_theDevConsole->IsOpen() )
	{
		return;
	}
		
	if ( g_theInput->WasKeyJustPressed( 'H' ) )
	{
		m_isHUDEnabled = !m_isHUDEnabled;
	}

	CameraPositionUpdateOnInput( deltaSeconds );
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
	float finalYaw		= m_cameraRotation.z;//Clamp( m_cameraRotation.z , -175.f , 175.f );
	float finalRoll		= m_cameraRotation.y;//Clamp( m_cameraRotation.y , -85.f , 85.f );

	m_gameCamera.SetPitchYawRollRotation( finalPitch , finalRoll , finalYaw );
	//m_gameCamera.SetPitchYawRollRotation( m_cameraRotation.x , m_cameraRotation.z , m_cameraRotation.y );
}


//--------------------------------------------------------------------------------------------------------------------------------------------