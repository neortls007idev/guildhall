#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Time/Time.hpp"
//#include "Engine/Renderer/RenderBuffer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------
//				THIRD PARTY LIBRARIES
//--------------------------------------------------------------------------------------------------------------------------------------------
#pragma warning( push )
#pragma warning ( disable: 4100 )
#include "ThirdParty/stb/stb_image.h"
#pragma warning( pop )

#include <string>
#include <corecrt_wstring.h>

#include <shobjidl.h>
#include <shobjidl_core.h>

#include <d3d11sdklayers.h>
#include <dxgidebug.h>

//--------------------------------------------------------------------------------------------------------------------------------------------
//				D3D11 specific includes and Macros
//--------------------------------------------------------------------------------------------------------------------------------------------

#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Primitives/GPUMesh.hpp"

#pragma comment( lib, "d3d11.lib" )         // needed a01
#pragma comment( lib, "dxgi.lib" )          // needed a01
#pragma comment( lib, "d3dcompiler.lib" )   // needed when we get to shaders
//--------------------------------------------------------------------------------------------------------------------------------------------

BitmapFont* g_bitmapFont = nullptr;
extern char const* g_errorShaderCode;

//--------------------------------------------------------------------------------------------------------------------------------------------

// TODO :- move to D3D11 common

void SetDebugName( ID3D11DeviceChild* child , const std::string* name )
{
	if ( child != nullptr && name != nullptr )
		child->SetPrivateData( WKPDID_D3DDebugObjectName , ( UINT ) name->size() , name->c_str() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

RenderContext::~RenderContext()
{
	delete g_bitmapFont;
	g_bitmapFont = nullptr;

	m_LoadedTextures.clear();
	m_LoadedBitMapFonts.clear();
	m_LoadedShaders.clear();

	delete m_swapChain;
	m_swapChain = nullptr;

	DX_SAFE_RELEASE( m_lastBoundIBO );
	DX_SAFE_RELEASE( m_lastBoundVBO );
	DX_SAFE_RELEASE( m_context );
	DX_SAFE_RELEASE( m_device );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::Startup( Window* window )
{
	// ID3D11Device
	// ID3D11DeviceContext
#if defined(RENDER_DEBUG)
	CreateDebugModule();
#endif

	IDXGISwapChain* swapchain = nullptr;  // Create Swap Chain

	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined(RENDER_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	memset( &swapChainDesc , 0 , sizeof( swapChainDesc ) );
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;

	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // on swap, the old buffer is discarded
	swapChainDesc.Flags = 0; // additional flags - see docs.  Used in special cases like for video buffers

	// how swap chain is to be used
	HWND hwnd = ( HWND ) window->m_hwnd;
	swapChainDesc.OutputWindow = hwnd; // HWND for the window to be used
	swapChainDesc.SampleDesc.Count = 1; // how many samples per pixel (1 so no MSAA)
										 // note, if we're doing MSAA, we'll do it on a secondary target

	// describe the buffer
	swapChainDesc.Windowed = TRUE;                                    // windowed/full-screen mode
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color RGBA8 color
	swapChainDesc.BufferDesc.Width = window->GetClientWidth();
	swapChainDesc.BufferDesc.Height = window->GetClientHeight();
	// save data as member variable when window is created.

	// create
	HRESULT result = D3D11CreateDeviceAndSwapChain( nullptr ,
		D3D_DRIVER_TYPE_HARDWARE ,
		nullptr ,
		flags ,// Controls the type of device we make
		nullptr ,
		0 ,
		D3D11_SDK_VERSION ,
		&swapChainDesc ,
		&swapchain ,
		&m_device ,
		nullptr ,
		&m_context );

	//HRESULT SetProgressState( HWND hwnd ,TBPFLAG TBPF_NORMAL );

	// Instance - singleton
	// Device - create resources like textures buffers etc.
	// Context - issue commands

	//~ swapchain
	GUARANTEE_OR_DIE( SUCCEEDED( result ) , "FAILED TO CREATE RESOURCES" );
	m_swapChain = new SwapChain( this , swapchain );

	/*Shader::s_errorShader->CreateFromString( this , g_errorShaderCode );*/

	m_defaultShader = GetOrCreateShader( "Data/Shaders/default.hlsl" );

	m_immediateVBO = new VertexBuffer( this , MEMORY_HINT_DYNAMIC );
	m_frameUBO = new RenderBuffer( this , UNIFORM_BUFFER_BIT , MEMORY_HINT_DYNAMIC );
	m_modelMatrixUBO = new RenderBuffer( this , UNIFORM_BUFFER_BIT , MEMORY_HINT_DYNAMIC );

	m_defaultSampler = new Sampler( this , SAMPLER_POINT );
	m_textureDefault = CreateTextureFromColor( WHITE );

	CreateBlendStates();

	if ( g_bitmapFont == nullptr )
	{
		g_bitmapFont = GetOrCreateBitmapFontFromFile( "Data/Fonts/SquirrelFixedFont" ); // TO DO PASS IN THE FONT ADDRESS AND THE TEXTURE POINTER TO IT.
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::BeginFrame()
{

}

void RenderContext::UpdateFrameTime( float deltaSeconds )
{
	FrameDataT frameData;
	frameData.m_systemTime = ( float ) GetCurrentTimeSeconds();
	frameData.m_systemDeltaTime = deltaSeconds;

	m_frameUBO->Update( &frameData , sizeof( frameData ) , sizeof( frameData ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::EndFrame()
{
	m_swapChain->Present();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::Shutdown()
{

	delete g_bitmapFont;
	g_bitmapFont = nullptr;


	for ( int index = 0; index < BlendMode::TOTAL; index++ )
	{
		DX_SAFE_RELEASE( m_blendStates[ index ] );
	}

	delete m_textureDefault;
	m_textureDefault = nullptr;

	//delete m_defaultShader;
	m_defaultShader = nullptr;

 	for ( auto& shaderIndex : m_LoadedShaders )
 	{
 		if ( shaderIndex.second != nullptr )
 		{
 			delete shaderIndex.second;
 			shaderIndex.second = nullptr;
 		}
 	}

	delete m_defaultSampler;
	m_defaultSampler = nullptr;



	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	delete m_modelMatrixUBO;
	m_modelMatrixUBO = nullptr;

	delete m_frameUBO;
	m_frameUBO = nullptr;

	m_lastBoundIBO = nullptr;
	m_lastBoundVBO = nullptr;
	m_currentCamera = nullptr;

	delete m_swapChain;
	m_swapChain = nullptr;


	DX_SAFE_RELEASE( m_context );
	DX_SAFE_RELEASE( m_device );

	ReportLiveObjects();    // do our leak reporting just before shutdown to give us a better detailed list;
	DestroyDebugModule();
}

//--------------------------------------------------------------------------------------------------------------------------------------------


void RenderContext::ClearScreen( const Rgba8& clearColor )
{
	float clearFloats[ 4 ];
	float scaleToFloat = 1 / 255.f;

	clearFloats[ 0 ] = ( float ) clearColor.r * scaleToFloat;
	clearFloats[ 1 ] = ( float ) clearColor.g * scaleToFloat;
	clearFloats[ 2 ] = ( float ) clearColor.b * scaleToFloat;
	clearFloats[ 3 ] = ( float ) clearColor.a * scaleToFloat;

	// can be put under clear Texture function

	Texture* backbuffer = m_swapChain->GetBackBuffer();
	TextureView* backbuffer_rtv = backbuffer->GetOrCreateRenderTargetView();

	ID3D11RenderTargetView* rtv = backbuffer_rtv->GetRTVHandle();
	m_context->ClearRenderTargetView( rtv , clearFloats );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::BeginCamera( const Camera& camera )
{
	m_currentCamera = const_cast< Camera* >( &camera );

#if defined(RENDER_DEBUG)
	m_context->ClearState();
#endif

	if ( camera.GetClearMode() & CLEAR_COLOR_BIT )
	{
		ClearScreen( camera.GetClearColor() );
	}

// TEMPORARY - this will be moved
//Set up the GPU for a draw

	m_textureTarget = camera.GetColorTarget();

	if ( m_textureTarget  == nullptr )
	{
		m_textureTarget = m_swapChain->GetBackBuffer();
	}

	IntVec2 output = m_textureTarget->GetDimensions();

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = ( float ) output.x;
	viewport.Height = ( float ) output.y;
	viewport.MinDepth = 0.0;
	viewport.MaxDepth = 1.f;

	m_context->RSSetViewports( 1 , &viewport );

	ID3D11RenderTargetView* rtv = m_textureTarget->GetOrCreateRenderTargetView()->GetRTVHandle();
	m_context->OMSetRenderTargets( 1 , &rtv , nullptr );

	ClearScreen( camera.GetClearColor() );
	BindShader( "" );
	m_lastBoundVBO = nullptr;
	BindUniformBuffer( UBO_FRAME_SLOT , m_frameUBO );
	m_currentCamera->UpdateUBO( this );
	BindUniformBuffer( UBO_CAMERA_SLOT , m_currentCamera->UpdateUBO( this ) );

	SetModelMatrix( Mat44::IDENTITY );
	//BindUniformBuffer( UBO_MODEL_SLOT , m_modelMatrixUBO );

	BindTexture( m_textureDefault );
	BindSampler( m_defaultSampler );
	SetBlendMode( BlendMode::ALPHA );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::EndCamera( const Camera& camera )
{
	m_currentCamera = nullptr;
	UNUSED(camera);
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::SetBlendMode( BlendMode blendMode )
{
	float const zeroes[] = { 0 , 0 , 0 , 0 };

	switch ( blendMode )
	{
	case BlendMode::ALPHA:
					m_context->OMSetBlendState( m_blendStates[ BlendMode::ALPHA ] , zeroes , ~0U );
					break;
	case BlendMode::ADDITIVE:
					m_context->OMSetBlendState( m_blendStates[ BlendMode::ADDITIVE ] , zeroes , ~0U );
					break;
	case BlendMode::SOLID:
					m_context->OMSetBlendState( m_blendStates[ BlendMode::SOLID ] , zeroes , ~0U );
					break;
	default:
		break;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::CreateDebugModule()
{
	// load the dll
 	m_debugModule = ::LoadLibraryA( "Dxgidebug.dll" );
 	if ( m_debugModule == nullptr )
 	{
 		DebuggerPrintf( "gfx" , "Failed to find dxgidebug.dll.  No debug features enabled." );
 	}
 	else
 	{
 		// find a function in the loaded dll
 		typedef HRESULT( WINAPI* GetDebugModuleCB )( REFIID , void** );
		HMODULE temp = ( HMODULE ) m_debugModule;
		GetDebugModuleCB cb = ( GetDebugModuleCB ) ::GetProcAddress( temp , "DXGIGetDebugInterface" );

 		// create our debug object
 		HRESULT hr = cb( __uuidof( IDXGIDebug ) , ( void** ) &m_debug );
 		ASSERT_OR_DIE( SUCCEEDED( hr ), "" );
 	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DestroyDebugModule()
{
	if ( nullptr != m_debug )
	{
		DX_SAFE_RELEASE( m_debug );   // release our debug object
		FreeLibrary( ( HMODULE ) m_debugModule ); // unload the dll

		m_debug = nullptr;
		m_debugModule = ( HMODULE )nullptr;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::ReportLiveObjects()
{
	if ( nullptr != m_debug )
	{
		m_debug->ReportLiveObjects( DXGI_DEBUG_ALL , DXGI_DEBUG_RLO_DETAIL );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Shader* RenderContext::GetOrCreateShader( char const* filename )
{
	Shader* Temp = m_LoadedShaders[ filename ];

	if (Temp == nullptr)
	{
		Temp = CreateShaderFromFile( filename );
	}

	return Temp;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Shader* RenderContext::CreateShaderFromFile( char const* shaderFilePath )
{
	Shader* temp = new Shader(this);
	temp->CreateFromFile( this , shaderFilePath );
	m_LoadedShaders[ shaderFilePath ] = temp;
	return temp;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Texture* RenderContext::CreateTextureFromFile( const char* imageFilePath )
{
		//unsigned int textureID = 0;
		int imageTexelSizeX = 0; // This will be filled in for us to indicate image width
		int imageTexelSizeY = 0; // This will be filled in for us to indicate image height
		int numComponents = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
		int numComponentsRequested = 4; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

		stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT ,  0 for D3D11
		unsigned char* imageData = stbi_load( imageFilePath , &imageTexelSizeX , &imageTexelSizeY , &numComponents , numComponentsRequested );

		// Check if the load was successful
		GUARANTEE_OR_DIE( imageData , Stringf( "Failed to load image \"%s\"" , imageFilePath ));
		GUARANTEE_OR_DIE( numComponents == 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0 , Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)" , imageFilePath , numComponents , imageTexelSizeX , imageTexelSizeY ) );

		// describe the texture
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = imageTexelSizeX;
		desc.Height = imageTexelSizeY;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;							// Multi sampling Anti-Aliasing
		desc.SampleDesc.Quality = 0;						// Multi sampling Anti-Aliasing
		desc.Usage = D3D11_USAGE_IMMUTABLE;					//  if we do mip-chains, we change this to GPU/DEFAULT
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;							// does the CPU write to this? 0  = no
		desc.MiscFlags = 0;									// extension features like cube maps

		D3D11_SUBRESOURCE_DATA initialData;
		initialData.pSysMem = imageData;
		initialData.SysMemPitch = imageTexelSizeX * 4;
		initialData.SysMemSlicePitch = 0;					// for 3D texturing

		// DirectX Creation
		ID3D11Texture2D* texHandle = nullptr;
		m_device->CreateTexture2D( &desc , &initialData , &texHandle );


		stbi_image_free( imageData );
		Texture* temp = new Texture( imageFilePath , this , texHandle );
		m_LoadedTextures[ imageFilePath ] = temp;
		//delete temp;
		//temp = nullptr;
		return m_LoadedTextures[ imageFilePath ];
}

//--------------------------------------------------------------------------------------------------------------------------------------------

BitmapFont* RenderContext::CreateBitMapFontFromFile( std::string bitmapFontFilePath )
{
	UNUSED( bitmapFontFilePath );
	Texture* bitmapFontTexture = GetOrCreateTextureFromFile( bitmapFontFilePath.c_str() );
	BitmapFont* newBitmapFont = new BitmapFont( "BitMapFont" , bitmapFontTexture );
	m_LoadedBitMapFonts[ bitmapFontFilePath ] = newBitmapFont;
	return newBitmapFont;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::ReCompileAllShaders()
{
	for ( auto& shaderIndex : m_LoadedShaders )
	{
		shaderIndex.second->RecompileShader( shaderIndex.first );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Texture* RenderContext::GetOrCreateTextureFromFile( const char* imageFilePath )
{
	Texture* Temp = m_LoadedTextures[ imageFilePath ];

	if (Temp == nullptr)
	{
		Temp = CreateTextureFromFile( imageFilePath );
	}

	return Temp;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::BindTexture( const Texture* constTexture )
{
	if ( nullptr == constTexture )
	{
		TextureView* shaderResourceView = m_textureDefault->GetOrCreateShaderResourceView();
		ID3D11ShaderResourceView* shaderResourceViewHandle = shaderResourceView->GetSRVHandle();
		m_context->PSSetShaderResources( 0 , 1 , &shaderResourceViewHandle );
		return;
	}

	Texture* texture = const_cast< Texture* >( constTexture );
	TextureView* shaderResourceView =  texture->GetOrCreateShaderResourceView();
	ID3D11ShaderResourceView* shaderResourceViewHandle = shaderResourceView->GetSRVHandle();
	m_context->PSSetShaderResources( 0 , 1 , &shaderResourceViewHandle );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::CreateBlendStates()
{
	D3D11_BLEND_DESC alphaDesc;

	alphaDesc.AlphaToCoverageEnable = false;
	alphaDesc.IndependentBlendEnable = false;
	alphaDesc.RenderTarget[ 0 ].BlendEnable = true;
	alphaDesc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	alphaDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;

	// render all output
	alphaDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_device->CreateBlendState( &alphaDesc , &m_blendStates[ BlendMode::ALPHA ] );

	D3D11_BLEND_DESC additiveDesc;

	additiveDesc.AlphaToCoverageEnable = false;
	additiveDesc.IndependentBlendEnable = false;
	additiveDesc.RenderTarget[ 0 ].BlendEnable = true;
	additiveDesc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
	additiveDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_ONE;
	additiveDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_ONE;

	additiveDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	additiveDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
	additiveDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;

	// render all output
	additiveDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_device->CreateBlendState( &additiveDesc , &m_blendStates[ BlendMode::ADDITIVE ] );

	D3D11_BLEND_DESC opaqueDesc;

	opaqueDesc.AlphaToCoverageEnable = false;
	opaqueDesc.IndependentBlendEnable = false;
	opaqueDesc.RenderTarget[ 0 ].BlendEnable = false;
	opaqueDesc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
	opaqueDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	opaqueDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	opaqueDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	opaqueDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
	opaqueDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;

	// render all output
	opaqueDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	m_device->CreateBlendState( &additiveDesc , &m_blendStates[ BlendMode::SOLID ] );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::Draw( int numVertexes , int vertexOffset )
{
	m_context->VSSetShader( m_currentShader->m_vertexStage.m_vertexShader , nullptr , 0 );
	m_context->RSSetState( m_currentShader->m_rasterState );
	m_context->PSSetShader( m_currentShader->m_fragmentStage.m_fragmentShader , nullptr , 0 );


	// So at this, I need to describe the vertex format to the shader
	ID3D11InputLayout* inputLayout = m_currentShader->GetOrCreateInputLayout( Vertex_PCU::LAYOUT );
	// do similar to last bound VBO
	m_context->IASetInputLayout( inputLayout );
	m_context->Draw( numVertexes , vertexOffset );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

BitmapFont* RenderContext::GetOrCreateBitmapFontFromFile( std::string bitmapFontFilePath )
{
 	bitmapFontFilePath.append( ".png" );
 	BitmapFont* Temp = m_LoadedBitMapFonts[ bitmapFontFilePath ];
 	if ( Temp == nullptr )
 	{
 		Temp = CreateBitMapFontFromFile( bitmapFontFilePath );
 	}
 	return Temp;
// 	UNUSED( bitmapFontFilePath );
// 	GUARANTEE_OR_DIE( false , "Starting Stuff replace with D3D11" );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes )
{

	// Update a vertex buffer
	// RenderBUffer* m_immediateVBO // VBO - vertex buffer object - a buffer of memomry on the GPU.
	// void* cpuBuffer = ( void* ) vertexes;
	// stride - the number of bytes we need to move in an array or buffer of a memory


	size_t  bufferTotalByteSize	= numVertexes * sizeof( Vertex_PCU );
	size_t	elementSize			= sizeof( Vertex_PCU );
	m_immediateVBO->Update( vertexes , bufferTotalByteSize , elementSize );


	// Bind the Shader

 	BindVertexBuffer( m_immediateVBO );

	// Index Buffers - to be covered later

	// Draw
	Draw( numVertexes , 0 );

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawVertexArray( const std::vector<Vertex_PCU>& vertexArray )
{
	DrawVertexArray( ( int ) vertexArray.size() , &vertexArray[ 0 ] );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext:: DrawLine(const Vec2& start, const Vec2& end, const Rgba8& color, float thickness)
{
	Vec2 displacement = end - start;
	Vec2 forward = displacement.GetNormalized();
	forward.SetLength( thickness / 2.f );
	Vec2 left = forward.GetRotated90Degrees();

	Vec2 endLeft	= end	+ forward + left;
	Vec2 endRight	= end	+ forward - left;
	Vec2 startLeft  = start - forward + left;
	Vec2 startRight = start - forward - left;

	Vec3 endLeftVec3( endLeft.x , endLeft.y , 0.f );
	Vec3 endRightVec3( endRight.x , endRight.y , 0.f );
	Vec3 startLeftVec3( startLeft.x , startLeft.y , 0.f );
	Vec3 startRightVec3( startRight.x , startRight.y , 0.f );

	const Vertex_PCU lineVerts[6] = { Vertex_PCU(startRightVec3, color, Vec2(0.f, 0.f)),
								Vertex_PCU(endRightVec3  , color, Vec2(0.f, 0.f)),
								Vertex_PCU(endLeftVec3   , color, Vec2(0.f, 0.f)),
								Vertex_PCU(endLeftVec3   , color, Vec2(0.f, 0.f)),
								Vertex_PCU(startLeftVec3 , color, Vec2(0.f, 0.f)),
								Vertex_PCU(startRightVec3, color, Vec2(0.f, 0.f)) };


	DrawVertexArray(6, lineVerts);
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawMesh( const GPUMesh* mesh )
{
	/*
	BindVertexBuffer( 0 , mesh->GetVertexBuffer() );
	UpdateLayoutIfNeeded(); // based on currentVertex buffer & CurrentShader
	{
		// buffer_attribute_t const* m_currentLayout;
		// buffer_attribute_t const* m_prevoiuslyBoundLayout;

		if( m_currentlyBoundLayout != m_currentLayout = m_currentVBO -> GetLayout() )
		|| ( m_hasShaderChanged )
		{
			ID3D11InputLayout* layout = m_currentShader->GetOrCreateInputLayout( m_currentLayout );
			m_context->IASetInputLayput ( layout );
			m_previouslyBoundLayout = m_currentlLayout;
			m_chaderHasChanged = false;
		}
	}
	

	bool hasIndices = mesh->GetIndexCount();

	if ( hasIndices )
	{
		BindIndexBuffer( mesh->GetIndexBuffer() );
		DrawIndexed( mesh->GetIndexCount() , 0 , 0 );
	}
	else
	{
		Draw( mesh->GetVertexCount() , 0 , 0 , 0 );

	}*/
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawRing(const Vec2& center, float radius, const Rgba8& color, float thickness)
{
	constexpr float degreePerSide = 360.f / 64.f;

	for (int NumberOfLines = 0 ; NumberOfLines <= 64 ; NumberOfLines++ )
	{
		float theta1Degrees = degreePerSide * ( float ) NumberOfLines;
		float theta2Degrees = degreePerSide * ( float ) ( NumberOfLines + 1 );

		Vec2 start = center + Vec2( radius * CosDegrees( theta1Degrees ) , radius * SinDegrees( theta1Degrees ) );
		Vec2 end   = center + Vec2( radius * CosDegrees( theta2Degrees ) , radius * SinDegrees( theta2Degrees ) );

			DrawLine( start , end , color , thickness );
	}

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawAABB2( const AABB2& box , const Rgba8& tint )
{
	const Vertex_PCU AABB2Verts[ 6 ] = {
								Vertex_PCU( Vec3( box.m_mins.x,box.m_mins.y,0.f ) , tint, Vec2( 0.f, 0.f ) ),
								Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,0.f ) , tint, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,0.f ) , tint, Vec2( 0.f, 1.f ) ),

								Vertex_PCU( Vec3( box.m_maxs.x,box.m_mins.y,0.f ) , tint, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( box.m_maxs.x,box.m_maxs.y,0.f ) , tint, Vec2( 1.f, 1.f ) ),
								Vertex_PCU( Vec3( box.m_mins.x,box.m_maxs.y,0.f ) , tint, Vec2( 0.f, 1.f ) ) };

	DrawVertexArray( 6 , AABB2Verts );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawDisc( const Disc2D& disc , const Rgba8& tint )
{
	constexpr int  NUMBER_OF_DISC_VERTS = 120;
	Vertex_PCU discVerts[ NUMBER_OF_DISC_VERTS ];
	const Vec2 UVCoordsAtCenter = Vec2( 0.5f , 0.5f );
	const float UVRadius = 0.5f;

	float angleInDegreesBetweenDiscTriangles = 0.f;

	discVerts[ 0 ] = Vertex_PCU( ( Vec3( 0.f , 0.f , 0.f ) ) , tint , Vec2( 0.5f , 0.5f ) );
	discVerts[ 1 ] = Vertex_PCU( ( Vec3( disc.m_radius , 0.f , 0.f ) ) , tint , Vec2( 1.f , 0.5f ) );
	angleInDegreesBetweenDiscTriangles = ( 360.f * 3.f ) / static_cast< float >( NUMBER_OF_DISC_VERTS );

	//-----------------------------------------------------------------------------------------------------------------
	float costheta = CosDegrees( angleInDegreesBetweenDiscTriangles );
	float intialXVertex = disc.m_radius * costheta;

	float sintheha = SinDegrees( angleInDegreesBetweenDiscTriangles );
	float initialYVertex = disc.m_radius * sintheha;

	discVerts[ 2 ] = Vertex_PCU( ( Vec3( intialXVertex , initialYVertex , 0.f ) ) , tint , Vec2( UVRadius + UVRadius * costheta , UVRadius + UVRadius * sintheha ) );

	//-----------------------------------------------------------------------------------------------------------------
	int discVertIndex = 3;
	for ( discVertIndex = 3; discVertIndex < NUMBER_OF_DISC_VERTS; discVertIndex += 3 )
	{
		angleInDegreesBetweenDiscTriangles = angleInDegreesBetweenDiscTriangles + ( ( 360.f * 3.f ) / ( NUMBER_OF_DISC_VERTS ) );
		discVerts[ discVertIndex ] = discVerts[ discVertIndex - 3 ];
		discVerts[ discVertIndex + 1 ] = discVerts[ discVertIndex - 1 ];

		discVerts[ discVertIndex + 2 ].m_position = Vec3( disc.m_radius * CosDegrees( angleInDegreesBetweenDiscTriangles ) ,
			disc.m_radius * SinDegrees( angleInDegreesBetweenDiscTriangles ) , 0.f );
		discVerts[ discVertIndex + 2 ].m_color = tint;
		discVerts[ discVertIndex + 2 ].m_uvTexCoords = Vec2( UVRadius + UVRadius * CosDegrees( angleInDegreesBetweenDiscTriangles ) ,
															 UVRadius + UVRadius * SinDegrees( angleInDegreesBetweenDiscTriangles ) );
	}
	discVerts[ NUMBER_OF_DISC_VERTS - 1 ] = discVerts[ 1 ];


	// MOVE  THIS LINE OF CODE INTO A SEPARATE FUNCTION LATER
	TransformVertexArray2D( NUMBER_OF_DISC_VERTS , discVerts , 1 , 0.f , disc.m_center );
	DrawVertexArray( NUMBER_OF_DISC_VERTS , discVerts );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawOBB2( const OBB2& box , const Rgba8& tint )
{
	float orientationDegrees = box.m_iBasis.GetAngleDegrees();

	AABB2 newbox = AABB2::ZERO_TO_ONE;
	newbox.SetDimensions( box.GetDimensions() );

	Vertex_PCU OBB2Verts[ 6 ] = {
								Vertex_PCU( Vec3( newbox.m_mins.x,newbox.m_mins.y,0.f ) , tint, Vec2( 0.f, 0.f ) ),
								Vertex_PCU( Vec3( newbox.m_maxs.x,newbox.m_mins.y,0.f ) , tint, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( newbox.m_mins.x,newbox.m_maxs.y,0.f ) , tint, Vec2( 0.f, 1.f ) ),

								Vertex_PCU( Vec3( newbox.m_maxs.x,newbox.m_mins.y,0.f ) , tint, Vec2( 1.f, 0.f ) ),
								Vertex_PCU( Vec3( newbox.m_maxs.x,newbox.m_maxs.y,0.f ) , tint, Vec2( 1.f, 1.f ) ),
								Vertex_PCU( Vec3( newbox.m_mins.x,newbox.m_maxs.y,0.f ) , tint, Vec2( 0.f, 1.f ) ) };

	TransformVertexArray2D( 6 , OBB2Verts , 1.f , orientationDegrees , box.GetCenter() );
	DrawVertexArray( 6 , OBB2Verts );

// ALTERNATE WAY
//
// 	Vec2 cornerPoints[ 4 ];
// 	box.GetCornerPositions( cornerPoints );
// 	Vertex_PCU obbVertices[ 6 ];
// 	obbVertices[ 0 ] = Vertex_PCU( Vec3( cornerPoints[ 0 ].x , cornerPoints[ 0 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
// 	obbVertices[ 1 ] = Vertex_PCU( Vec3( cornerPoints[ 1 ].x , cornerPoints[ 1 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
// 	obbVertices[ 2 ] = Vertex_PCU( Vec3( cornerPoints[ 2 ].x , cornerPoints[ 2 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
//
// 	obbVertices[ 3 ] = Vertex_PCU( Vec3( cornerPoints[ 0 ].x , cornerPoints[ 0 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
// 	obbVertices[ 4 ] = Vertex_PCU( Vec3( cornerPoints[ 3 ].x , cornerPoints[ 3 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
// 	obbVertices[ 5 ] = Vertex_PCU( Vec3( cornerPoints[ 2 ].x , cornerPoints[ 2 ].y , 0.f ) , Rgba8( 100 , 0 , 0 , 150 ) , Vec2( 0.f , 0.f ) );
//
// 	DrawVertexArray( 6 , obbVertices );


}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawCapsule2D( const Vec2& capsuleMidStart , const Vec2& capsuleMidEnd , const float capsuleRadius , const Rgba8& tint )
{
	//DrawLine( capsuleMidStart , capsuleMidEnd , RED , capsuleRadius * 2.f );

	Vec2 displacement = capsuleMidEnd - capsuleMidStart;
	Vec2 forward = displacement.GetNormalized();
	forward.SetLength( capsuleRadius );
	Vec2 left = forward.GetRotated90Degrees();

	Vec2 endLeft	= capsuleMidEnd + left;
	Vec2 endRight	= capsuleMidEnd - left;
	Vec2 startLeft  = capsuleMidStart + left;
	Vec2 startRight = capsuleMidStart - left;

	Vec3 endLeftVec3( endLeft.x , endLeft.y , 0.f );
	Vec3 endRightVec3( endRight.x , endRight.y , 0.f );
	Vec3 startLeftVec3( startLeft.x , startLeft.y , 0.f );
	Vec3 startRightVec3( startRight.x , startRight.y , 0.f );

	const Vertex_PCU lineVerts[ 6 ] = { Vertex_PCU( startRightVec3, tint, Vec2( 0.f, 0.f ) ),
										Vertex_PCU( endRightVec3  , tint, Vec2( 0.f, 0.f ) ),
										Vertex_PCU( endLeftVec3   , tint, Vec2( 0.f, 0.f ) ),
										Vertex_PCU( endLeftVec3   , tint, Vec2( 0.f, 0.f ) ),
										Vertex_PCU( startLeftVec3 , tint, Vec2( 0.f, 0.f ) ),
										Vertex_PCU( startRightVec3, tint, Vec2( 0.f, 0.f ) ) };

	DrawVertexArray( 6 , lineVerts );

	DrawDiscFraction( Disc2D( capsuleMidStart , capsuleRadius ) , 0.5f , tint, (capsuleMidEnd-capsuleMidStart).GetRotated90Degrees().GetAngleDegrees() );
	DrawDiscFraction( Disc2D( capsuleMidEnd , capsuleRadius ) , 0.5f , tint , 180.f + ( capsuleMidEnd - capsuleMidStart ).GetRotated90Degrees().GetAngleDegrees() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::DrawDiscFraction(const Disc2D& disc, const float drawFraction, const Rgba8& tint, const float OrientationDegrees)
{
	constexpr int  NUMBER_OF_DISC_VERTS = 120 ;
	Vertex_PCU discVerts[ NUMBER_OF_DISC_VERTS ];
	const int NUMBER_OF_VERTS_IN_DISC_FRACTION = static_cast< int >( 120 * drawFraction );
	const Vec2 UVCoordsAtCenter = Vec2( 0.5f , 0.5f );
	const float UVRadius = 0.5f;

	float angleInDegreesBetweenDiscTriangles = 0.f;

	discVerts[ 0 ] = Vertex_PCU( ( Vec3( 0.f , 0.f , 0.f ) ) , tint , Vec2( 0.5f , 0.5f ) );
	discVerts[ 1 ] = Vertex_PCU( ( Vec3( disc.m_radius , 0.f , 0.f ) ) , tint , Vec2( 1.f , 0.5f ) );
	angleInDegreesBetweenDiscTriangles = ( 360.f * 3.f ) / static_cast< float >( NUMBER_OF_DISC_VERTS );

	//-----------------------------------------------------------------------------------------------------------------
	float costheta = CosDegrees( angleInDegreesBetweenDiscTriangles );
	float intialXVertex = disc.m_radius * costheta;

	float sintheha = SinDegrees( angleInDegreesBetweenDiscTriangles );
	float initialYVertex = disc.m_radius * sintheha;

	discVerts[ 2 ] = Vertex_PCU( ( Vec3( intialXVertex , initialYVertex , 0.f ) ) , tint , Vec2( UVRadius + UVRadius * costheta , UVRadius + UVRadius * sintheha ) );

	//-----------------------------------------------------------------------------------------------------------------
	int discVertIndex = 3;
	for ( discVertIndex = 3; discVertIndex < NUMBER_OF_VERTS_IN_DISC_FRACTION; discVertIndex += 3 )
	{
		angleInDegreesBetweenDiscTriangles = angleInDegreesBetweenDiscTriangles + ( ( 360.f * 3.f ) / ( NUMBER_OF_DISC_VERTS ) );
		discVerts[ discVertIndex ] = discVerts[ discVertIndex - 3 ];
		discVerts[ discVertIndex + 1 ] = discVerts[ discVertIndex - 1 ];

		discVerts[ discVertIndex + 2 ].m_position = Vec3( disc.m_radius * CosDegrees( angleInDegreesBetweenDiscTriangles ) ,
			disc.m_radius * SinDegrees( angleInDegreesBetweenDiscTriangles ) , 0.f );
		discVerts[ discVertIndex + 2 ].m_color = tint;
		discVerts[ discVertIndex + 2 ].m_uvTexCoords = Vec2( UVRadius + UVRadius * CosDegrees( angleInDegreesBetweenDiscTriangles ) ,
			UVRadius + UVRadius * SinDegrees( angleInDegreesBetweenDiscTriangles ) );
	}
	discVerts[ NUMBER_OF_DISC_VERTS - 1 ] = discVerts[ 1 ];


	// MOVE  THIS LINE OF CODE INTO A SEPARATE FUNCTION LATER
	TransformVertexArray2D( NUMBER_OF_VERTS_IN_DISC_FRACTION , discVerts , 1 , OrientationDegrees , disc.m_center );
	DrawVertexArray( NUMBER_OF_VERTS_IN_DISC_FRACTION , discVerts );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool RenderContext::IsDrawing() const
{
	if ( m_currentCamera )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool RenderContext::BindShader( Shader* shader )
{
	ASSERT_OR_DIE( IsDrawing(),"No active Camera" );					// -> IsDrawing() -> Do I have a camera?
	m_currentShader = shader;
	if ( m_currentShader == nullptr )
	{
		m_currentShader = m_defaultShader;
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::BindShader( std::string shaderFileName )
{
	Shader* temp = nullptr;
	if ( shaderFileName == "" )
	{
		BindShader( temp );
		return;
	}
	temp = GetOrCreateShader( shaderFileName.c_str() );
	BindShader( temp );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool RenderContext::HasAnyShaderChangedAtPath( const wchar_t* relativePath )
{
	TCHAR path[ MAX_PATH + 1 ] = L"";

	::GetCurrentDirectory( MAX_PATH , path );
	wcscat_s( path , relativePath );

 	HANDLE result = FindFirstChangeNotification( path , false , FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE );

	const HANDLE* ptrResult = &result;

	if ( result == INVALID_HANDLE_VALUE )
	{
		FindCloseChangeNotification( result );
		return false;
	}

	DWORD waitResult = WaitForMultipleObjects( 1 , ptrResult , FALSE , 5 );

// 	if ( FindNextChangeNotification( result ) )
// 	{
// 		FindCloseChangeNotification( result );
// 		return true;
// 	}

	if ( waitResult == WAIT_OBJECT_0 )
	{
		ReCompileAllShaders();
		FindCloseChangeNotification( result );
		return true;
	}
	else if ( waitResult ==  WAIT_ABANDONED_0 || waitResult == WAIT_FAILED ||  waitResult == WAIT_TIMEOUT )
	{
		FindCloseChangeNotification( result );
		return false;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::BindVertexBuffer( VertexBuffer* vbo )
{
	ID3D11Buffer* vboHandle = vbo->m_handle;
	UINT stride = ( UINT ) sizeof( Vertex_PCU );	//	how far from one vertex to next
	UINT offset = 0;								//  how far into buffer we start

	if (m_lastBoundVBO != vboHandle )
	{
		m_context->IASetVertexBuffers( 0 , 1 , &vboHandle , &stride , &offset );
		m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		m_lastBoundVBO = vboHandle;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::BindIndexBuffer( IndexBuffer* ibo )
{
	ID3D11Buffer* iboHandle = ibo->m_handle;
	UINT stride = ( UINT ) sizeof( Vertex_PCU );	//	how far from one vertex to next
	UINT offset = 0;								//  how far into buffer we start

	if ( m_lastBoundIBO != iboHandle )
	{
		//m_context->IASetIndexBuffer( 0 , 1 , &iboHandle , &stride , &offset );
		m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		m_lastBoundIBO = iboHandle;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::BindUniformBuffer( unsigned int slot , RenderBuffer* ubo )
{

	ID3D11Buffer* uboHandle = ubo->m_handle; /*ubo->GetHandle();*/

	m_context->VSSetConstantBuffers( slot , 1 , &uboHandle );
	m_context->PSSetConstantBuffers( slot , 1 , &uboHandle );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::SetModelMatrix( Mat44 modelMat )
{
	ModelDataT modelData;
	modelData.model = modelMat;
	m_modelMatrixUBO->Update( &modelData , sizeof( modelData ) , sizeof( modelData ) );
	BindUniformBuffer( UBO_MODEL_SLOT , m_modelMatrixUBO );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void RenderContext::BindSampler( const Sampler* sampler )
{

	if ( nullptr == sampler )
	{
		ID3D11SamplerState* samplerHandle = m_defaultSampler->GetHandle();
		m_context->PSSetSamplers( 0 , 1 , &samplerHandle );
		return;
	}
	ID3D11SamplerState* samplerHandle = sampler->GetHandle();
	m_context->PSSetSamplers( 0 , 1 , &samplerHandle );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Texture* RenderContext::CreateTextureFromColor( Rgba8 color )
{
	// make a 1x1 texture of that color, and return it;
	int imageTexelSizeX = 1; // This will be filled in for us to indicate image width
	int imageTexelSizeY = 1; // This will be filled in for us to indicate image height
	//int numComponents = 4; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	//int numComponentsRequested = 4; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

	unsigned char* imageData = &color.r;

	// describe the texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = imageTexelSizeX;
	desc.Height = imageTexelSizeY;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;							// Multi sampling Anti-Aliasing
	desc.SampleDesc.Quality = 0;						// Multi sampling Anti-Aliasing
	desc.Usage = D3D11_USAGE_IMMUTABLE;					//  if we do mip-chains, we change this to GPU/DEFAULT
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;							// does the CPU write to this? 0  = no
	desc.MiscFlags = 0;									// extension features like cube maps

	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = imageData;
	initialData.SysMemPitch = imageTexelSizeX * 4;
	initialData.SysMemSlicePitch = 0;					// for 3D texturing

	// DirectX Creation
	ID3D11Texture2D* texHandle = nullptr;
	m_device->CreateTexture2D( &desc , &initialData , &texHandle );

// 	delete imageData;
// 	imageData = nullptr;

	Texture* temp = new Texture( this , texHandle );

	return temp;
}

//---------------------------------------------------------------------------------------------------------------------------------------------