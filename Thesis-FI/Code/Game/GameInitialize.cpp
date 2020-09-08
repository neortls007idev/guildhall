#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/OBJUtils.hpp"
#include "Engine/Core/VertexLit.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Primitives/GPUMesh.hpp"
#include "Game/Game.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

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
	m_lightShaders[ LitShaderTypes::LIT_SHADOW ]				= g_theRenderer->GetOrCreateShader( "Data/Shaders/litShadows.hlsl" );
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

	m_blurShader												= g_theRenderer->GetOrCreateShader( "Data/Shaders/blur.hlsl" );
	m_toneMapShader												= g_theRenderer->GetOrCreateShader( "Data/Shaders/toneMap.hlsl" );

	m_currentShader												= m_lightShaders[ LitShaderTypes::LIT ];
	m_currentShaderIndex										= LitShaderTypes::LIT;
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

	m_objSciFiShipMeshTex_D		   = g_theRenderer->GetOrCreateTextureFromFile( "Data/Models/scifiFighter/diffuse.jpg" );
	m_objSciFiShipMeshTex_N		   = g_theRenderer->GetOrCreateTextureFromFile( "Data/Models/scifiFighter/normal.png" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::LoadModels()
{
	tinyxml2::XMLDocument xmlDocument;
	xmlDocument.LoadFile( "Data/Models/Models.xml" );

	if( xmlDocument.ErrorID() == tinyxml2::XML_SUCCESS )
	{
		tinyxml2::XMLElement* modelsRoot = xmlDocument.RootElement();
		int numModelsListed = 0;
		
		tinyxml2::XMLElement* model = modelsRoot->FirstChildElement( "Model" );
		while( model != nullptr )
		{
			numModelsListed++;
			model = model->NextSiblingElement( "Model" );
		}

		GUARANTEE_OR_DIE( ( numModelsListed <= ( int ) NUM_GAME_MODELS ) , " Models listed in XML are more than defined in the eGameOBJModels in GameCommon.hpp" );

		model = modelsRoot->FirstChildElement( "Model" );
		while( model != nullptr )
		{
			bool isDataBad = false;
			
			std::string modelPath		= ParseXmlAttribute( *model , "path" , "" );
			std::string modelDiffuseTex = ParseXmlAttribute( *model , "diffuse" , "" );
			std::string modelNormalTex  = ParseXmlAttribute( *model , "normal" , "" );
			int enumValue				= ParseXmlAttribute( *model , "enumValue" , -1 );

			if( ( enumValue < 0 ) && ( enumValue >= NUM_GAME_MODELS ) )
			{
				isDataBad = true;
			}

			if ( modelPath == "" )
			{
				isDataBad = true;
			}

			if ( !isDataBad )
			{
				m_gameModels[ enumValue ] = new GPUMesh( g_theRenderer );
				MeshBuilderOptions objMeshOptions;
				objMeshOptions.generateTangents = true;
				objMeshOptions.generateNormals = true;
				objMeshOptions.clean = true;
				m_gameModels[ enumValue ] = LoadObjFileIntoGpuMesh( objMeshOptions , "Data/Models/scifiFighter/mesh.obj" );

				if ( modelDiffuseTex != "" )
				{
					m_gameModelsDiffuse[ enumValue ] = g_theRenderer->GetOrCreateTextureFromFile( modelDiffuseTex.c_str() );
				}
				
				if ( modelNormalTex != "" )
				{
					m_gameModelsNormals[ enumValue ] = g_theRenderer->GetOrCreateTextureFromFile( modelNormalTex.c_str() );
				}
			}
			model = model->NextSiblingElement( "Model" );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Game::IntializeGameObjects()
{
	m_cubeMesh = new GPUMesh( g_theRenderer ); 
	std::vector<VertexMaster>	cubeMeshVerts;
	std::vector<VertexLit>		cubeMeshLitVerts;
	std::vector<uint>			cubeMeshIndices;

	AABB3 box( Vec3( -1 , -1 , -1 ) , Vec3( 5 , 1 , 1 ) );
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
	m_meshSphere->UpdateVertices( ( uint ) sphereMeshVerts.size() , sphereMeshVerts.data() );
	m_meshSphere->UpdateIndices( sphereIndices );

	std::vector<VertexMaster>	quadMeshVerts;
	std::vector<VertexLit>		quadMeshLitVerts;
	std::vector<uint>			quadIndices;

	
	CreateQuad( quadMeshVerts , quadIndices , AABB2::ZERO_TO_ONE );
	m_quadMesh = new GPUMesh( g_theRenderer );
	m_quadMesh->UpdateVertices( ( uint ) quadMeshVerts.size() , quadMeshVerts.data() );
	m_quadMesh->UpdateIndices( quadIndices );
	
	MeshBuilderOptions objMeshOptions1;
	objMeshOptions1.generateTangents = true;
	objMeshOptions1.generateNormals = true;
	//objMeshOptions1.clean = true;
	m_objSciFiShipMesh = new GPUMesh( g_theRenderer );
	m_objSciFiShipMesh = LoadObjFileIntoGpuMesh( objMeshOptions1 , "Data/Models/scifiFighter/mesh.obj" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------