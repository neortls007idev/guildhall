﻿#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/OBJUtils.hpp"
#include "Engine/Core/VertexMaster.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/DebugUI/ImGUISystem.hpp"
#include "Engine/Input/VirtualKeyboard.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/ParticleSystem/ParticleEmitter3D.hpp"
#include "Engine/ParticleSystem/ParticleSystem3D.hpp"
#include "Engine/Primitives/GPUMesh.hpp"
#include "Engine/Profilling/D3D11PerformanceMarker.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Time/Time.hpp"
#include "Game/Game.hpp"

#include "Engine/Math/MatrixUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "ThirdParty/ImGUI/imgui.h"
#include "ThirdParty/ImGUI/ImGuiFileDialog.h"
#include "Engine/ParticleSystem/Particle3D.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext*		g_theRenderer;
extern TheApp*				g_theApp;
extern DevConsole*			g_theDevConsole;
extern ImGUISystem*			g_debugUI;
extern ParticleSystem3D*	g_theParticleSystem3D;

//--------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------
//				PROFILING D3D POINTER FOR THE SPECIFIC CONFIGURATIONS
//--------------------------------------------------------------------------------------------------------------------------------------------

extern	D3D11PerformanceMarker* g_D3D11PerfMarker;

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	LoadShaders();
	LoadTextures();
	LoadModels();
	LoadScene();
	
	m_tileDiffuse	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/tile_diffuse.png" );
	m_tileDiffuse	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	m_tileNormal	= g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/tile_normal.png" );
		   	
	InitializeCameras();
	IntializeGameObjects();
		
	m_cubeMeshTransform.SetPosition( 5.f , 0.0f , -10.0f );
	m_sphereMeshTransform.SetPosition( -5.f , 0.0f , -10.0f );
	m_quadTransform.SetPosition( 0.f , 0.5f , 0.0f );
	m_quadTransform.SetRotation( -90.f , 0.0f , 0.0f );
	m_objSciFiShipTransform.SetPosition( 0.f , 10.0f , -10.0f );

	InitializeLightData();
	InitializeShaderMaterialData();
	
	g_theInput->PushCursorSettings( CursorSettings( RELATIVE_MODE , MOUSE_IS_WINDOWLOCKED , false ) );

	m_unitCubeMesh = new GPUMesh( g_theRenderer );
	
	m_cubeMapex = g_theRenderer->GetOrCreateTextureCubeFromFile( "Data/Images/CubeMaps/galaxy2.png" );
	m_cubeMapTest = g_theRenderer->GetOrCreateShader( "Data/Shaders/CubeMap.hlsl" );

	std::vector<Vertex_PCU>		cubeMeshVerts;
	std::vector<uint>			cubeMeshIndices;

	AABB3 box( Vec3( -0.5f , -0.5f , -0.5f ) , Vec3( 0.5f , 0.5f , 0.5f ) );
	CreateCuboid( cubeMeshVerts , cubeMeshIndices , box , WHITE );

	m_unitCubeMesh->UpdateVertices( ( uint ) cubeMeshVerts.size() , cubeMeshVerts.data() );
	m_unitCubeMesh->UpdateIndices( cubeMeshIndices );

	m_cubeSampler	= new Sampler( g_theRenderer , SAMPLER_CUBEMAP );
	m_bilinear		= new Sampler( g_theRenderer , SAMPLER_BILINEAR );
	m_linear		= new Sampler( g_theRenderer , SAMPLER_LINEAR , COMPARE_LESS );
	m_debugSwitchs[ GAME_CAMERA_VIEW_FRUSTUM_CULLING ] = true;

	InitializeParticleEmitters();
	InitializeShadowTestTrasforms();
	//InitializeShadowMapTextures();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadPlanetaryTextures()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

Game::~Game()
{
	SaveScene();

	//for ( uint index = 0; index < TOTAL_LIGHTS; index++ )
	//{
	//	g_theRenderer->ReleaseRenderTarget( m_shadowMap[ index ] );
	//}
	
	for( int index = 0 ; index < NUM_GAME_MODELS ; index++ )
	{
		m_ModelInstances[ index ].clear();
	}

	for ( int index = 0 ; index < NUM_GAME_MODELS ; index++ )
	{
		delete m_gameModels[ index ];
		m_gameModels[ index ] = nullptr;
	}
	delete m_unitCubeMesh;
	m_unitCubeMesh = nullptr;
	
	delete m_cubeMesh;
	m_cubeMesh			= nullptr;

	delete m_meshSphere;
	m_meshSphere		= nullptr;

	delete m_quadMesh;
	m_quadMesh			= nullptr;

	delete m_objSciFiShipMesh;
	m_objSciFiShipMesh			= nullptr;

	m_meshTex_D			= nullptr;
	m_meshTex_N			= nullptr;
	m_tileDiffuse		= nullptr;
	m_tileNormal		= nullptr;

	delete m_cubeSampler;
	m_cubeSampler = nullptr;
	
	delete	m_bilinear;
	m_bilinear = nullptr;

	delete m_linear;
	m_linear = nullptr;

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::InitializeCameras()
{
		m_gameCamera.SetProjectionPerspective( GAME_CAM_FOV , CLIENT_ASPECT , -GAME_CAM_NEAR_Z , -GAME_CAM_FAR_Z );
		m_gameCamera.SetPosition( Vec3( 0.f , 0.f , 0.f ) );
		m_gameCamera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , BLACK , 1.f , 0 );
		float height = GAME_CAM_NEAR_Z * -tanf( GAME_CAM_FOV * 0.5f );
			  height = ( GAME_CAM_FAR_Z * height ) / GAME_CAM_NEAR_Z;
		//m_lightsCamera.SetProjectionPerspective( GAME_CAM_FOV , CLIENT_ASPECT , -GAME_CAM_NEAR_Z , -GAME_CAM_FAR_Z );
		m_lightsCamera.SetOrthoView3D( 10.f , CLIENT_ASPECT , -GAME_CAM_NEAR_Z , -GAME_CAM_FAR_Z );
		//m_lightsCamera.SetOrthoView( 540.f , CLIENT_ASPECT );
		m_lightsCamera.SetPosition( Vec3( 0.f , 0.f , 0.f ) );
		m_lightsCamera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT | CLEAR_STENCIL_BIT , BLACK , 1.f , 0 );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::InitializeShadowMapTextures()
{
	for( uint index = 0 ; index < TOTAL_LIGHTS ; index++ )
	{
		m_shadowMap[index] = g_theRenderer->CreateRenderTarget(IntVec2(4096, 4096), D3D_DXGI_FORMAT_R32_FLOAT,
		                                                       "ShadowMapRTV");		
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Update( float deltaSeconds )
{
	m_frameRate = 1.f / deltaSeconds;
	//m_frameRates.push_back( m_frameRate );
	m_currentFrameInBuffer++;
	if( m_currentFrameInBuffer == FRAME_RATE_BUFFER_SIZE )
	{
		m_currentFrameInBuffer = 0;
	}
	m_frameRates[ m_currentFrameInBuffer ] = m_frameRate;
	m_frameTimes[ m_currentFrameInBuffer ] = deltaSeconds;
	
	if( m_worstFrame > m_frameRate )	{	m_worstFrame = m_frameRate;		}
	if( m_bestFrame < m_frameRate )		{	m_bestFrame = m_frameRate;		}

	m_rollingAvgFPS += m_frameRate;
	m_currentFrame++;
		
	m_rollingAvgFPS /= m_currentFrame;
		
	if ( m_debugSwitchs[ GAME_CAMERA_VIEW_FRUSTUM_CULLING ] )
	{
		m_currentlyDrawingMeshes = 0;
		UpdateViewFrustumCulling();
	}
	else
	{
		m_currentlyDrawingMeshes = m_totalDrawableMeshes;
	}
	
	for( size_t instanceIndex = 0 ; instanceIndex < m_ModelInstances[ SPACESHIP ].size(); instanceIndex++ )
	{
		if( nullptr == m_ModelInstances[ SPACESHIP ][ instanceIndex ] )
		{
			continue;
		}

		if ( m_debugSwitchs[ VIEW_FRUSTUM_DEBUG_DRAW ] )
		{
			DebugAddWorldWireSphere( m_ModelInstances[ SPACESHIP ][ instanceIndex ]->m_position ,
			                         m_gameModels[ SPACESHIP ]->m_boundingSphereRadius , GREEN , deltaSeconds );
		}
	}
	
	
	m_ambientLightColor.SetColorFromNormalizedFloat( m_lights.ambientLight );
	
	UpdateLightPosition( deltaSeconds );	
	UpdateFromKeyBoard( deltaSeconds );

	DebugUI();

	UpdateAllStarEmitters();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateAllStarEmitters()
{	
	for ( int index = 0 ; index < NUM_STARS_EMITTERS ; index++ )
	{
		m_starEmitters[ index ].m_emitter->UpdateTargetPos( m_gameCamera.GetPosition() );
	
		int direction = 1;
		Vec3 emitterPos = Vec3( 0.f , 0.f , -5.f );
		
		if ( index % 2 == 0 )
		{
			direction = -1;
		}

		emitterPos = m_starEmitters[ index ].m_center + Vec3::MakeFromSpericalCoordinates(
 			direction * 45.f * ( float ) GetCurrentTimeSeconds() , 30.f * SinDegrees( ( float ) GetCurrentTimeSeconds() ) , m_starEmitters[ index ].m_movementRadius );

		if( index == 0 )
		{
			//emitterPos = Vec3::ZERO;
		
			//Vec3 plaentPos = m_starEmitters[ index ].m_center + Vec3::MakeFromSpericalCoordinates(
			//direction * 45.f * ( float ) GetCurrentTimeSeconds() , 0.f , 5.f );
			//
			////m_sphereMeshTransform.SetPosition( plaentPos );
			//m_sphereMeshTransform.SetPosition( Vec3::ZERO );
			//m_sphereMeshTransform.SetRotation( 0.f , ( float ) GetCurrentTimeSeconds() * 10.f , 0.f );
		}

		m_starEmitters[ index ].m_emitter->UpdatePosition( emitterPos );

		uint numNewSpawns = ( ( uint ) m_starEmitters[ index ].m_emitter->m_totalSpawnableParticles ) - m_starEmitters[ index ].m_emitter->m_numAliveParticles;
		
		numNewSpawns = numNewSpawns <= m_starEmitters[ index ].m_numParticlesToSpawnPerFrame ? numNewSpawns : m_starEmitters[ index ].m_numParticlesToSpawnPerFrame;

	
		for( uint particleSpawned = 0 ; particleSpawned <= numNewSpawns ; particleSpawned++ )
		{			
			Vec3 position = g_RNG->RollRandomInUnitSphere();
			Vec3 deviation = g_RNG->RollRandomUnitVec3() * m_starEmitters[ index ].m_particleVelocity;
			//float scale = g_RNG->RollRandomFloatInRange( 0.f , 1.5f );
			int maxAge = g_RNG->RollRandomIntInRange( m_starEmitters[ index ].m_particleMinLifeTime , m_starEmitters[ index ].m_particleMaxLifeTime );

			m_starEmitters[ index ].m_emitter->SpawnNewRandomParticleFromSpriteSheet( AABB2( -m_starEmitters[ index ].m_particleSize , m_starEmitters[ index ].m_particleSize )
																					 , position , 1.f , deviation , static_cast< uint16_t >( maxAge ) ,
																					  m_starEmitters[ index ].m_particleStartColor , m_starEmitters[ index ].m_particleEndColor );
																					 // WHITE , WHITE );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateEmitterOfType( GameStarEmitters emitterType )
{
	UNUSED( emitterType );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateLightPosition( float deltaSeconds )
{
	for ( uint index = 0; index < TOTAL_LIGHTS; index++ )
	{
		Rgba8 lightColor;
		lightColor.SetColorFromNormalizedFloat( Vec4( m_lights.lights[ index ].color , m_lights.lights[ index ].intensity ) );

		if( ( index == ( uint ) m_currentLightIndex ) & m_isLightFollowingTheCamera )
		{
			m_lights.lights[ m_currentLightIndex ].worldPosition = m_gameCamera.GetPosition();
			Vec3 direction = m_gameCamera.GetCameraTransform().GetAsMatrix().GetKBasis3D();
			m_lights.lights[ m_currentLightIndex ].direction = -direction;

			if ( m_lights.lights[ m_currentLightIndex ].lightType != POINT_LIGHT )
			{
				Transform cameraTransform = m_gameCamera.GetCameraTransform();
				m_lightsPitchYawRoll[ m_currentLightIndex ] = Vec3( cameraTransform.GetPitch() , cameraTransform.GetYaw() , 0.f );
			}
			
			continue;
		}
		
		if ( POINT_LIGHT == m_lights.lights[ index ].lightType )
		{
			DebugAddWorldPoint( m_lights.lights[ index ].worldPosition , 0.125f , lightColor , deltaSeconds * 0.5f , DEBUG_RENDER_USE_DEPTH );
		}
		if ( DIRECTIONAL_LIGHT == m_lights.lights[ index ].lightType )
		{
			DebugAddWorldArrow( m_lights.lights[ index ].worldPosition , m_lights.lights[ index ].worldPosition + m_lights.lights[ index ].direction , lightColor , deltaSeconds * 0.5f );
		}
		if ( SPOT_LIGHT == m_lights.lights[ index ].lightType )
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

void Game::UpdateViewFrustumCulling()
{
	Frustum cameraViewFrustum = m_gameCamera.GetCameraViewFrustum();
	
	for ( int modelIndex = 0 ; modelIndex < NUM_GAME_MODELS ; modelIndex++ )
	{
		if( nullptr == m_gameModels[ modelIndex ] )
		{
			continue;
		}
		
		m_ModelDrawableInstances[ modelIndex ].clear();
		float currentModelRadius = m_gameModels[ modelIndex ]->m_boundingSphereRadius;
		
		for( size_t instanceIndex = 0 ; instanceIndex < m_ModelInstances[ modelIndex ].size() ; instanceIndex++ )
		{
			if( nullptr == m_ModelInstances[ modelIndex ][ instanceIndex ] )
			{
				continue;
			}
			
			Vec3& pos = m_ModelInstances[ modelIndex ][ instanceIndex ]->m_position;

			if( cameraViewFrustum.IsSphereInsideFrustum( pos , currentModelRadius ) )
			{
				m_ModelDrawableInstances[ modelIndex ].emplace_back( m_ModelInstances[ modelIndex ][ instanceIndex ] );
				m_currentlyDrawingMeshes++;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::Render() const
{
	RenderShadowMapPass();
	
	Texture* backBuffer		= g_theRenderer->m_swapChain->GetBackBuffer();
	Texture* colorTarget	= g_theRenderer->GetOrCreatematchingRenderTarget( backBuffer , "realColorTarget" );
	Texture* bloomTarget	= g_theRenderer->GetOrCreatematchingRenderTarget( backBuffer , "BloomColorTarget" );
	Texture* finalImage		= g_theRenderer->GetOrCreatematchingRenderTarget( colorTarget );
	
	m_gameCamera.SetColorTarget( 0 , colorTarget );
	m_gameCamera.SetColorTarget( 1 , bloomTarget );

g_D3D11PerfMarker->BeginPerformanceMarker( L"Game Render Start" );
	
	g_D3D11PerfMarker->BeginPerformanceMarker( L"Initializing State" );
	g_theRenderer->BeginCamera( m_gameCamera );
	m_gameCamera.CreateMatchingDepthStencilTarget(); 
	g_theRenderer->BindDepthStencil( m_gameCamera.GetDepthStencilTarget() );
	g_theRenderer->BindShader( nullptr );
	g_theRenderer->BindCubeMapTexture( nullptr );
	g_theRenderer->BindSampler( m_bilinear );
	g_theRenderer->BindSampler( m_linear , 2 );
	
	g_theRenderer->SetRasterState( FILL_SOLID );
	
	g_theRenderer->SetCullMode( CULL_BACK );
	g_theRenderer->SetDepthTest( COMPARE_LEQUAL , true );

	g_theRenderer->SetAmbientLight( m_ambientLightColor , m_lights.ambientLight.w );

	g_theRenderer->UpdateLightsData( m_lights );
	g_theRenderer->EnableAllLights();
	g_theRenderer->SetSpecularFactor( m_lights.SPECULAR_FACTOR );
	g_theRenderer->SetSpecularPower( m_lights.SPECULAR_POWER );

	for( uint lightIndex = 0 ; lightIndex < TOTAL_LIGHTS ; lightIndex++  )
	{
		m_lightsCamera.SetPosition( m_lights.lights[ lightIndex ].worldPosition );
		m_lightsCamera.SetPitchYawRollRotation( m_lightsPitchYawRoll[ lightIndex ].x , m_lightsPitchYawRoll[ lightIndex ].y , 0.f );
		g_theRenderer->SetLightsView( m_currentLightIndex , m_lightsCamera.GetProjectionMatrix() , m_lightsCamera.GetViewMatrix() );
	}
	//g_theRenderer->SetLightsView( m_currentLightIndex , m_gameCamera.GetProjectionMatrix() );
	
	g_theRenderer->BindShader( m_currentShader );
	g_theRenderer->BindTexture( m_tileDiffuse );
	g_theRenderer->BindTexture( m_tileNormal , eTextureType::TEX_NORMAL );

	for( uint index = 0 ; index < TOTAL_LIGHTS ; index++  )
	{
		g_theRenderer->BindTexture( m_shadowMap[ index ] , 8 + index );
	}

	g_theRenderer->EnableFog( m_fogData );

	BindShaderSpecificMaterialData();
	g_D3D11PerfMarker->EndPerformanceMarker();
	
	g_D3D11PerfMarker->BeginPerformanceMarker( L"Test Meshes" );

	Mat44 sph = m_sphereMeshTransform.GetAsMatrix();
	Transform emitT;
	emitT.SetPosition( m_starEmitters->m_emitter->m_position );
	Mat44 emiM = emitT.GetAsMatrix();
	//emiM.TransformBy( sph );
	
 	g_theRenderer->SetModelMatrix( m_sphereMeshTransform.GetAsMatrix() );
 	//g_theRenderer->SetModelMatrix( emiM );
 	g_theRenderer->DrawMesh( m_meshSphere );

	g_theRenderer->DisableFog();
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindTexture( nullptr , TEX_NORMAL );

	g_theRenderer->SetModelMatrix( m_quadTransform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_quadMesh );
	
	//for ( uint index = 0; index < 8; index++ )
	//{
	//	g_theRenderer->SetModelMatrix( m_shadowTestCubes[ index ].GetAsMatrix() );
	//	g_theRenderer->DrawMesh( m_cubeMesh );
	//}
	//g_theRenderer->SetModelMatrix( m_cubeMeshTransform.GetAsMatrix() );
	g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	g_theRenderer->DrawMesh( m_cubeMesh );

	g_D3D11PerfMarker->EndPerformanceMarker();

	g_D3D11PerfMarker->BeginPerformanceMarker( L"Render all OBJ Models" );
	RenderAllInstancesOfType( SPACESHIP );
	RenderAllInstancesOfType( LUMINARIS_SHIP );

	for ( uint index = 0; index < TOTAL_LIGHTS; index++ )
	{
		g_theRenderer->BindTexture( nullptr , 8 + index );
		g_theRenderer->ReleaseRenderTarget( m_shadowMap[ index ] );
	}
	
	g_D3D11PerfMarker->EndPerformanceMarker();
	
	if ( m_isFresnelShaderActive )
	{
		g_D3D11PerfMarker->BeginPerformanceMarker( L"Fresnel Shader Pass" );
		RenderFresnelShader2ndPass();
		g_D3D11PerfMarker->EndPerformanceMarker();
	}
		
	
	g_theRenderer->SetRasterState( FILL_SOLID );

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindShader( nullptr );
 
 	g_theRenderer->BindMaterial( nullptr );

	g_D3D11PerfMarker->BeginPerformanceMarker( L"Skybox" );
	g_theRenderer->BindShader( m_cubeMapTest );
	g_theRenderer->BindCubeMapTexture( m_cubeMapex );
	g_theRenderer->SetCullMode( CULL_NONE );
	g_theRenderer->BindSampler( m_cubeSampler );

	g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
	g_theRenderer->DrawMesh( m_unitCubeMesh );
	g_theRenderer->SetCullMode( CULL_BACK );


	g_theRenderer->BindCubeMapTexture( nullptr );
	g_theRenderer->BindShader( nullptr );
	g_D3D11PerfMarker->EndPerformanceMarker();
	
	g_theRenderer->BindTexture( nullptr );

	g_D3D11PerfMarker->BeginPerformanceMarker( L"Particle System 3D" );
		g_theParticleSystem3D->Render();
	g_D3D11PerfMarker->EndPerformanceMarker();

	
	g_theRenderer->SetDepthTest( COMPARE_ALWAYS , true );
	g_theRenderer->SetCullMode( CULL_NONE );
	g_theRenderer->DisableLight( 0 );

 	if( m_isToneMapShaderActive && m_isToneMapComputeShaderActive )
 	{
 	g_D3D11PerfMarker->BeginPerformanceMarker( L"Tone Map Post Process via CS" );
 
 		Texture* toneMapTarget = g_theRenderer->GetOrCreatematchingUAVTarget( colorTarget , "ToneMapCSTarget" );
 		Texture* currentView = g_theRenderer->GetOrCreatematchingRenderTarget( backBuffer );
 		g_theRenderer->CopyTexture( currentView , colorTarget );
 		g_theRenderer->BindShader( m_toneMapComputeShader );
 		g_theRenderer->BindMaterialData( ( void* ) &m_toneMapTransform , sizeof( m_toneMapTransform ) );
 		g_theRenderer->BindTexture( currentView , 0 , 0 , SHADER_STAGE_COMPUTE );
 		g_theRenderer->BindUAVTexture( toneMapTarget , 1 );
 
 		g_theRenderer->ExecuteComputeShader( m_toneMapComputeShader , 352 , 180 , 1 );
 		g_theRenderer->CopyTexture( colorTarget , toneMapTarget );
 		g_theRenderer->ReleaseRenderTarget( currentView );
 		g_theRenderer->ReleaseUAVTarget( toneMapTarget );
 		g_theRenderer->BindShader( nullptr );
 
 	g_D3D11PerfMarker->EndPerformanceMarker();
 	}
 	
	g_theRenderer->EndCamera( m_gameCamera );
	
	if ( m_isblurShaderActive )
	{
		g_D3D11PerfMarker->BeginPerformanceMarker( L"Blurred Bloom" );
		Shader* blurShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/blur.hlsl" );;
		Texture* blurredBloom = g_theRenderer->GetOrCreatematchingRenderTarget( bloomTarget , "BlurBloomTarget" );
		g_theRenderer->StartEffect( blurredBloom , bloomTarget , blurShader );
		g_theRenderer->BindTexture( bloomTarget , TEX_USER_TYPE );
		g_theRenderer->EndEffect();
		
		Shader* combineImageShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/combineImage.hlsl" );;
		//Texture* finalImage = g_theRenderer->GetOrCreatematchingRenderTarget( colorTarget );
		g_theRenderer->StartEffect( finalImage , colorTarget , combineImageShader );
		g_theRenderer->BindTexture( blurredBloom , TEX_USER_TYPE );
		g_theRenderer->BindTexture( colorTarget , TEX_USER_TYPE , 1 );
		g_theRenderer->EndEffect();
		g_theRenderer->CopyTexture( backBuffer , finalImage );
		g_theRenderer->ReleaseRenderTarget( blurredBloom );
		g_theRenderer->ReleaseRenderTarget( finalImage );

		g_D3D11PerfMarker->EndPerformanceMarker();
	}
	else
	{
		Shader* combineImageShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/combineImage.hlsl" );;
		//Texture* finalImage = g_theRenderer->GetOrCreatematchingRenderTarget( colorTarget );
		g_theRenderer->StartEffect( finalImage , colorTarget , combineImageShader );
		g_theRenderer->BindTexture( bloomTarget , TEX_USER_TYPE );
		g_theRenderer->BindTexture( colorTarget , TEX_USER_TYPE , 1 );
		g_theRenderer->EndEffect();
		g_theRenderer->CopyTexture( backBuffer , finalImage );
		g_theRenderer->ReleaseRenderTarget( finalImage );
	}

	g_theRenderer->ReleaseRenderTarget( bloomTarget );
	g_theRenderer->ReleaseRenderTarget( colorTarget );

	if( m_isToneMapShaderActive && !m_isToneMapComputeShaderActive )
	{
		Texture* toneMapTarget = g_theRenderer->GetOrCreatematchingRenderTarget( colorTarget );
		Texture* currentView = g_theRenderer->GetOrCreatematchingRenderTarget( backBuffer );
		g_theRenderer->CopyTexture( currentView , backBuffer );
		g_theRenderer->StartEffect( toneMapTarget , currentView , m_toneMapShader );
		g_theRenderer->BindMaterialData( ( void* ) &m_toneMapTransform , sizeof( m_toneMapTransform ) );
		g_theRenderer->EndEffect();
		g_theRenderer->CopyTexture( backBuffer , toneMapTarget );
		g_theRenderer->ReleaseRenderTarget( currentView );
		g_theRenderer->ReleaseRenderTarget( toneMapTarget );
	}

	m_gameCamera.SetColorTarget( backBuffer );

	g_D3D11PerfMarker->EndPerformanceMarker();
	
	GUARANTEE_OR_DIE( g_theRenderer->GetTotalRenderTargetPoolSize() < 16 , "LEAKING RENDER TARGETS" );
	GUARANTEE_OR_DIE( g_theRenderer->GetTotalUAVTargetPoolSize() < 16 , "LEAKING UAV TEX TARGETS" );

	g_D3D11PerfMarker->BeginPerformanceMarker( L"Debug Render" );
	DebugRenderWorldToCamera( &m_gameCamera );
	DebugRenderScreenTo( nullptr );
	g_D3D11PerfMarker->EndPerformanceMarker();

	g_D3D11PerfMarker->BeginPerformanceMarker( L"ImGUI Debug UI" );
	g_debugUI->Render();
	g_D3D11PerfMarker->EndPerformanceMarker();

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

	g_theRenderer->SetModelMatrix( m_quadTransform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_quadMesh );

	g_theRenderer->SetModelMatrix( m_cubeMeshTransform.GetAsMatrix() );
	g_theRenderer->DrawMesh( m_cubeMesh );
	g_theRenderer->SetBlendMode( SOLID );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderShadowMapPass() const
{
	g_D3D11PerfMarker->BeginPerformanceMarker( L"Shadow Map Render Pass Start" );

	for ( uint lightIndex = 0; lightIndex < TOTAL_LIGHTS; lightIndex++ )
	{
		Texture* backBuffer = g_theRenderer->m_swapChain->GetBackBuffer();
		m_shadowMap[ lightIndex ] = g_theRenderer->GetOrCreatematchingRenderTargetOfSize( IntVec2(m_shadowMapDimension , m_shadowMapDimension ) ,
																				"lightDepthTarget" , D3D_DXGI_FORMAT_R32_FLOAT );

		m_lightsCamera.SetColorTarget( 0 , m_shadowMap[ lightIndex ] );

		std::wstring passName = L"Light " + std::to_wstring( lightIndex ) + L" Shadow Map Render Pass Start";

		m_lightsCamera.SetPosition( m_lights.lights[ lightIndex ].worldPosition );
		m_lightsCamera.SetPitchYawRollRotation( m_lightsPitchYawRoll[ lightIndex ].x , m_lightsPitchYawRoll[ lightIndex ].y , 0.f );

		//Vec3 lightCameraMin = m_lightsCamera.GetOrthoMin();
		//Vec3 lightCameraMax = m_lightsCamera.GetOrthoMax();
		//
		//DebugAddWorldWireBounds(OBB3(lightCameraMin, lightCameraMax, m_lightsPitchYawRoll[lightIndex]), GREEN,
		//                        0.25f);
		
		g_D3D11PerfMarker->BeginPerformanceMarker( passName.c_str() );
		g_D3D11PerfMarker->BeginPerformanceMarker( L"Initializing State" );
		g_theRenderer->BeginCamera( m_lightsCamera );

		//if( m_lights.lights[ lightIndex ].lightType == DIRECTIONAL_LIGHT )
		//{
		//	m_lightsCamera.m_view.Kx = -m_lights.lights[ lightIndex ].direction.x;
		//	m_lightsCamera.m_view.Ky = -m_lights.lights[ lightIndex ].direction.y;
		//	m_lightsCamera.m_view.Kz = -m_lights.lights[ lightIndex ].direction.z;
		//	//m_lightsCamera.ForceUpdateUBO( g_theRenderer );
		//}

		m_lightsCamera.CreateMatchingDepthStencilTarget();
		g_theRenderer->BindDepthStencil( m_lightsCamera.GetDepthStencilTarget() );
		g_theRenderer->BindShader( nullptr );
		g_theRenderer->BindCubeMapTexture( nullptr );
		g_theRenderer->BindSampler( m_bilinear );
		g_theRenderer->BindSampler( m_linear , 2 );

		g_theRenderer->SetRasterState( FILL_SOLID );

		g_theRenderer->SetCullMode( CULL_BACK );
		g_theRenderer->SetDepthTest( COMPARE_LEQUAL , true );

		g_theRenderer->SetAmbientLight( m_ambientLightColor , m_lights.ambientLight.w );

		g_theRenderer->UpdateLightsData( m_lights );
		g_theRenderer->EnableAllLights();
		g_theRenderer->SetSpecularFactor( m_lights.SPECULAR_FACTOR );
		g_theRenderer->SetSpecularPower( m_lights.SPECULAR_POWER );
		g_theRenderer->SetLightsView( m_currentLightIndex , m_lightsCamera.GetProjectionMatrix() , m_lightsCamera.GetViewMatrix() );

		g_theRenderer->BindShader( m_depthShader );
		g_theRenderer->BindTexture( m_tileDiffuse );
		g_theRenderer->BindTexture( m_tileNormal , eTextureType::TEX_NORMAL );

		g_theRenderer->EnableFog( m_fogData );

		BindShaderSpecificMaterialData();
		g_D3D11PerfMarker->EndPerformanceMarker();

		g_D3D11PerfMarker->BeginPerformanceMarker( L"Test Meshes" );

		g_theRenderer->SetModelMatrix( m_sphereMeshTransform.GetAsMatrix() );
		g_theRenderer->DrawMesh( m_meshSphere );

		g_theRenderer->DisableFog();

		g_theRenderer->SetModelMatrix( m_quadTransform.GetAsMatrix() );
		g_theRenderer->DrawMesh( m_quadMesh );

		//for ( uint index = 0; index < 8; index++ )
		//{
		//	g_theRenderer->SetModelMatrix( m_shadowTestCubes[ index ].GetAsMatrix() );
		//	g_theRenderer->DrawMesh( m_cubeMesh );
		//}
		//g_theRenderer->SetModelMatrix( m_cubeMeshTransform.GetAsMatrix() );
		g_theRenderer->SetModelMatrix( Mat44::IDENTITY );
		g_theRenderer->DrawMesh( m_cubeMesh );

		g_D3D11PerfMarker->EndPerformanceMarker();

		g_D3D11PerfMarker->BeginPerformanceMarker( L"Render all OBJ Models" );
		RenderAllInstancesOfType( SPACESHIP );
		RenderAllInstancesOfType( LUMINARIS_SHIP );
		g_D3D11PerfMarker->EndPerformanceMarker();
		//g_theRenderer->CopyTexture( m_shadowMap[ lightIndex ] , m_lightsCamera.GetDepthStencilTarget() );
		//g_theRenderer->ReleaseRenderTarget( m_shadowMap[ lightIndex ] );
		//g_theRenderer->ReleaseRenderTarget( CurrentShadowMap );
		g_theRenderer->EndCamera( m_lightsCamera );

		//for ( int index = 0; index < 1 ; index++ )
		//{
		//
		//	g_D3D11PerfMarker->BeginPerformanceMarker( L"Blurred Shadow" );
		//	
		//	Shader* blurShader = g_theRenderer->GetOrCreateShader( "Data/Shaders/blur.hlsl" );
		//	Texture* blurredShadow = g_theRenderer->GetOrCreatematchingRenderTarget( m_shadowMap[ lightIndex ] , "BlurBloomTarget" , D3D_DXGI_FORMAT_R32_FLOAT );
		//	g_theRenderer->StartEffect( blurredShadow , m_shadowMap[ lightIndex ] , blurShader );
		//	g_theRenderer->BindTexture( m_shadowMap[ lightIndex ] , TEX_USER_TYPE );
		//	g_theRenderer->EndEffect();
		//	
		//	g_theRenderer->CopyTexture( m_shadowMap[ lightIndex ] , blurredShadow );
		//	g_theRenderer->ReleaseRenderTarget( blurredShadow );
		//	
		//	g_D3D11PerfMarker->EndPerformanceMarker();
		//}
		
		g_D3D11PerfMarker->EndPerformanceMarker();
	}

g_D3D11PerfMarker->EndPerformanceMarker();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderAllInstancesOfType( eGameObjModels modelType ) const
{
	g_theRenderer->BindTexture( m_gameModelsDiffuse[ modelType ] );
	g_theRenderer->BindTexture( m_gameModelsNormals[ modelType ] , TEX_NORMAL );

	if( m_debugSwitchs[ GAME_CAMERA_VIEW_FRUSTUM_CULLING ] )
	{
		for( size_t instanceIndex = 0 ; instanceIndex < m_ModelDrawableInstances[ modelType ].size(); instanceIndex++ )
		{
			if( nullptr == m_ModelDrawableInstances[ modelType ][ instanceIndex ] )
			{
				continue;
			}
			g_theRenderer->SetModelMatrix( m_ModelDrawableInstances[ modelType ][ instanceIndex ]->GetAsMatrix() );
			g_theRenderer->DrawMesh( m_gameModels[ modelType ] );
		}
	}
	else
	{
		for( size_t instanceIndex = 0 ; instanceIndex < m_ModelInstances[ modelType ].size(); instanceIndex++ )
		{
			if( nullptr == m_ModelInstances[ modelType ][ instanceIndex ] )
			{
				continue;
			}
			g_theRenderer->SetModelMatrix( m_ModelInstances[ modelType ][ instanceIndex ]->GetAsMatrix() );
			g_theRenderer->DrawMesh( m_gameModels[ modelType ] );
		}
	}

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateFromKeyBoard( float deltaSeconds )
{
	if( g_theInput->IsKeyHeldDown( KEY_SHIFT ) && g_theInput->WasKeyJustPressed( 'T' ) )
	{
		m_isMouseUnlocked = !m_isMouseUnlocked;

		if( m_isMouseUnlocked )
		{
			g_theInput->PopCursorSettings();
		}
		else
		{
			g_theInput->PushCursorSettings( CursorSettings( RELATIVE_MODE , MOUSE_IS_UNLOCKED , false ) );
		}
	}
	
	if ( g_theDevConsole->IsOpen() || m_isMouseUnlocked )
	{
		return;
	}
		
	CameraPositionUpdateOnInput( deltaSeconds );
	UpdateLightsFromKeyBoard( deltaSeconds );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateLightsFromKeyBoard( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	UpdateLightPositionOnUserInput();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateLightData( int lightIndex )
{
	switch( m_lights.lights[ lightIndex ].lightType )
	{
		case POINT_LIGHT:
			{
				m_lights.lights[ lightIndex ].directionFactor = 0.f;
				m_lights.lights[ lightIndex ].dotInnerAngle = -1.f;
				m_lights.lights[ lightIndex ].dotOuterAngle = -1.f;
			}	break;

		case DIRECTIONAL_LIGHT:
			{
				m_lights.lights[ lightIndex ].directionFactor = 1.f;
				//Vec3 direction = m_gameCamera.GetCameraTransform().GetAsMatrix().GetKBasis3D();
				//m_lights.lights[ lightIndex ].direction = -direction;
				m_lights.lights[ lightIndex ].dotInnerAngle = -1.f;
				m_lights.lights[ lightIndex ].dotOuterAngle = -1.f;
			}	break;

		case SPOT_LIGHT:
			{
				m_lights.lights[ lightIndex ].directionFactor = 0.f;
				Vec3 direction = m_gameCamera.GetCameraTransform().GetAsMatrix().GetKBasis3D();
				m_lights.lights[ lightIndex ].direction = -direction;
				m_lights.lights[ lightIndex ].dotInnerAngle = CosDegrees( 15.f );
				m_lights.lights[ lightIndex ].dotOuterAngle = CosDegrees( 30.f );
			}	break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateLightPositionOnUserInput()
{
// 	if ( g_theInput->WasKeyJustPressed( KEY_F5 ) )
// 	{
// 		m_isLightFollowingTheCamera = false;
// 		m_isLightAnimated = false;
// 		m_lights.lights[ m_currentLightIndex ].worldPosition = Vec3::ZERO;
// 
// 		if ( m_lightType[m_currentLightIndex] != POINT_LIGHT )
// 		{
// 			Vec3 direction = m_gameCamera.GetCameraTransform().GetAsMatrix().GetKBasis3D();
// 			m_lights.lights[ m_currentLightIndex ].direction = -direction;
// 		}
// 	}

	if ( g_theInput->WasKeyJustPressed( KEY_F6 ) )
	{
		m_isLightFollowingTheCamera = false;
		m_isLightAnimated = false;
		m_lights.lights[ m_currentLightIndex ].worldPosition = m_gameCamera.GetPosition();

		if ( m_lights.lights[ m_currentLightIndex ].lightType != POINT_LIGHT )
		{
			Vec3 direction = m_gameCamera.GetCameraTransform().GetAsMatrix().GetKBasis3D();
			m_lights.lights[ m_currentLightIndex ].direction = -direction;
			//m_lightsCamera.SetCameraTransform( m_gameCamera.GetCameraTransform() );
			//CreatePerpsectiveProjectionMatrixD3D( GAME_CAM_FOV , CLIENT_ASPECT , -GAME_CAM_NEAR_Z , -GAME_CAM_FAR_Z );

			Transform cameraTransform = m_gameCamera.GetCameraTransform();
			m_lightsPitchYawRoll[ m_currentLightIndex ] = Vec3( cameraTransform.GetPitch() , cameraTransform.GetYaw() , 0.f );
			
		}		
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

void Game::CameraPositionUpdateOnInput( float deltaSeconds )
{
	Vec3 rotation = Vec3::ZERO;

	Mat44 cameraTransform	= m_gameCamera.GetCameraTransform().GetAsMatrix();
	Vec3 forwardVector		= -cameraTransform.GetKBasis3D();
	Vec3 rightVector		= cameraTransform.GetIBasis3D();
	Vec3 UpVector			= Vec3::UNIT_VECTOR_ALONG_J_BASIS;

	float speed = 4.0f;

	if ( g_theInput->IsKeyHeldDown( KEY_SHIFT ) )
	{
		speed = 20.f;
	}

	if ( g_theInput->IsKeyHeldDown( 'A' ) )
	{
		m_gameCamera.SetPosition( m_gameCamera.GetPosition() - rightVector * speed * deltaSeconds );
		m_dirtyUBOs[ GAME_CAMERA ] = true;
	}
	if ( g_theInput->IsKeyHeldDown( 'D' ) )
	{
		m_gameCamera.SetPosition( m_gameCamera.GetPosition() + rightVector * speed * deltaSeconds );
		m_dirtyUBOs[ GAME_CAMERA ] = true;
	}
	if ( g_theInput->IsKeyHeldDown( 'W' ) )
	{
		m_gameCamera.SetPosition( m_gameCamera.GetPosition() + forwardVector * speed * deltaSeconds );
		m_dirtyUBOs[ GAME_CAMERA ] = true;
	}
	if ( g_theInput->IsKeyHeldDown( 'S' ) )
	{
		m_gameCamera.SetPosition( m_gameCamera.GetPosition() - forwardVector * speed * deltaSeconds );
		m_dirtyUBOs[ GAME_CAMERA ] = true;
	}
	if ( g_theInput->IsKeyHeldDown( 'Q' ) )
	{
		m_gameCamera.SetPosition( m_gameCamera.GetPosition() - UpVector * speed * deltaSeconds );
		m_dirtyUBOs[ GAME_CAMERA ] = true;
	}
	if ( g_theInput->IsKeyHeldDown( 'E' ) )
	{
		m_gameCamera.SetPosition( m_gameCamera.GetPosition() + UpVector * speed * deltaSeconds );
		m_dirtyUBOs[ GAME_CAMERA ] = true;
	}

	if ( g_theInput->WasKeyJustPressed( 'O' ) )
	{
		m_dirtyUBOs[ GAME_CAMERA ] = true;
		m_gameCamera.SetPosition( Vec3::ZERO );
		m_yaw				= 0.f;
		m_pitch				= 0.f;
	}

	Vec2 mousePos		= g_theInput->GetRelativeMovement();
	
	m_pitch -= mousePos.y * speed * deltaSeconds;
	m_yaw	-= mousePos.x * speed * deltaSeconds;

	m_pitch = Clamp( m_pitch , -90.f , 90.f );

	m_gameCamera.SetPitchYawRollRotation( m_pitch , m_yaw , 0.f );
	m_gameCamera.ConstructCameraViewFrustum();
}

//--------------------------------------------------------------------------------------------------------------------------------------------