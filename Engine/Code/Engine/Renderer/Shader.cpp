#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/FileUtils.hpp"
#include <d3dcompiler.h>

#include <stdio.h>
#include "Engine/Core/ErrorWarningAssert.hpp"
//#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

void ConvertBufferAttributeToID3DX11Attribute( D3D11_INPUT_ELEMENT_DESC* d3d11VertxDescription, buffer_attribute_t const* attribute )
{
	for ( int index = 0; attribute[ index ].type != BUFFER_FORMAT_INVALID; index++ )
	{
		d3d11VertxDescription[ index ].SemanticName			= attribute[ index ].name.c_str();
		d3d11VertxDescription[ index ].SemanticIndex		= 0;													// Array element
		switch ( attribute[index].type )
		{
			case BUFFER_FORMAT_INVALID			: ERROR_AND_DIE( "INVALID FORMAT" );
												  break;
			case BUFFER_FORMAT_VEC2				: d3d11VertxDescription[ index ].Format = DXGI_FORMAT_R32G32_FLOAT;
												  break;
			case BUFFER_FORMAT_VEC3				: d3d11VertxDescription[ index ].Format = DXGI_FORMAT_R32G32B32_FLOAT; 
												  break;
			case BUFFER_FORMAT_R8G8B8A8_UNORM	: d3d11VertxDescription[ index ].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
												  break;
		default:
			break;
		}
		d3d11VertxDescription[ index ].InputSlot			= 0;													// interlaced or parallel IA format
		d3d11VertxDescription[ index ].AlignedByteOffset	= attribute[ index ].offset;
		d3d11VertxDescription[ index ].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;							// Vertex data = drawing a tree, Instance data = drawing a million tree
		d3d11VertxDescription[ index ].InstanceDataStepRate = 0;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

ShaderStage::ShaderStage()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------

ShaderStage::~ShaderStage()
{
	DX_SAFE_RELEASE( m_byteCode );
	DX_SAFE_RELEASE( m_handle );
	DX_SAFE_RELEASE( m_vertexShader );
	DX_SAFE_RELEASE( m_fragmentShader );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

static char const* GetDefaultEntryPointForStage( SHADER_STAGE_TYPE type )
{
	switch ( type )
	{
		case SHADER_STAGE_VERTEX: return "VertexFunction";
		case SHADER_STAGE_FRAGMENT: return "FragmentFunction";
		default:__debugbreak();
	}
	return "main";
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//			What version of the Shader language do we want to use -  similar to say c++11 vs c++14
//--------------------------------------------------------------------------------------------------------------------------------------------

static char const* GetShaderModelForStage( SHADER_STAGE_TYPE type )
{
	switch ( type )
	{
	case SHADER_STAGE_VERTEX: return "vs_5_0";
	case SHADER_STAGE_FRAGMENT: return "ps_5_0";
	default:__debugbreak(); return "Error";
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool ShaderStage::Compile( RenderContext* ctx , std::string const& filename , void const* source , size_t const sourceByteLen , SHADER_STAGE_TYPE stageType )
{
	// HLSL - high level sahder language
	// Compile - HLSL -  Byte Code
	// Link ByteCode - Device Assembly ( what we need to get to ) - this is device specific

	char const* entryPoint  = GetDefaultEntryPointForStage( stageType );
	char const* shaderModel = GetShaderModelForStage( stageType );

	DWORD compileFlags = 0U;
#if defined(DEBUG_SHADERS)
	compile_flags |= D3DCOMPILE_DEBUG;
	compile_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	compile_flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;  // cause, FIX YOUR WARNINGS
#else 
	// compile_flags |= D3DCOMPILE_SKIP_VALIDATION;   // Only do this if you know for a fact this shader works with this device (so second run through of a game)
	compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Yay, fastness (default is level 1)
#endif

	ID3DBlob* byteCode	= nullptr;
	ID3DBlob* errors	= nullptr;

	HRESULT result = ::D3DCompile( source,
		sourceByteLen ,								// plain text source code
		filename.c_str() ,							// optional, used for error messages (If you HLSL has includes - it will not use the includes names, it will use this name)
		nullptr ,									// pre-compiler defines - used more for compiling multiple versions of a single shader (different quality specs, or shaders that are mostly the same outside some constants)
		D3D_COMPILE_STANDARD_FILE_INCLUDE ,			// include rules - this allows #includes in the shader to work relative to the src_file path or my current working directly
		entryPoint ,								// Entry Point for this shader
		shaderModel ,								// Compile Target (MSDN - "Specifying Compiler Targets")
		compileFlags ,								// Flags that control compilation
		0 ,											// Effect Flags (we will not be doing Effect Files)
		&byteCode ,									// [OUT] ID3DBlob (buffer) that will store the byte code.
		&errors	);									// [OUT] ID3DBlob (buffer) that will store error information

	if ( FAILED( result ) )
	{
		// report errors
		if ( errors != nullptr )
		{
			char* errorString = ( char* ) errors->GetBufferPointer();
			DebuggerPrintf( "Failed to compile [%s].  Compiler gave the following output;\n%s" ,
				filename.c_str() ,
				errorString );
			DEBUGBREAK();
		}
	}
		else
		{
			ID3D11Device*	device = ctx->m_device;
			void const*		byteCodePtr = byteCode->GetBufferPointer();
			size_t			byteCodeSize = byteCode->GetBufferSize();

			switch ( stageType )
			{
			case SHADER_STAGE_VERTEX: 
			{
				result = device->CreateVertexShader( byteCodePtr , byteCodeSize , nullptr , &m_vertexShader );
				GUARANTEE_OR_DIE( SUCCEEDED( result ) , "FAILED TO LINK VERTEX SHADER STAGE" );

			} break;

			case SHADER_STAGE_FRAGMENT:
			{
				result = device->CreatePixelShader( byteCodePtr , byteCodeSize , nullptr , &m_fragmentShader );
				GUARANTEE_OR_DIE( SUCCEEDED( result ) , "FAILED TO LINK VERTEX SHADER STAGE" );
			} break;

			default: GUARANTEE_OR_DIE( false , "Unimplemented Shader Stage." ); 
					break;
			
		}
	}
		DX_SAFE_RELEASE( errors );

	// link the stage

	if ( stageType == SHADER_STAGE_VERTEX )
	{
		m_byteCode = byteCode;
	}
	else
	{
		DX_SAFE_RELEASE( byteCode );
		m_byteCode = nullptr;
	}

	m_stage = stageType;

	// byte 
	return IsValid();

}

//--------------------------------------------------------------------------------------------------------------------------------------------

void const* ShaderStage::GetByteCode() const
{
	return m_byteCode->GetBufferPointer();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

size_t ShaderStage::GetByteCodeLength() const
{
	return m_byteCode->GetBufferSize();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Shader::Shader( RenderContext* context ) :
											m_owner( context )
{
	CreateRasterSate();
}

//--------------------------------------------------------------------------------------------------------------------------------------------

bool Shader::CreateFromFile( RenderContext* ctx , std::string const& filename )
{
	UNUSED( ctx );
	size_t fileSize = 0;
	void* source = FileReadToNewBuffer( filename, &fileSize );
	if ( nullptr == source )
	{
		return false;
	}

	m_vertexStage.Compile( m_owner , filename , source , fileSize , SHADER_STAGE_VERTEX );
	m_fragmentStage.Compile( m_owner , filename , source , fileSize , SHADER_STAGE_FRAGMENT );

	delete[] source;

	return m_vertexStage.IsValid() && m_fragmentStage.IsValid();
// 
// 	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

Shader::~Shader()
{
	DX_SAFE_RELEASE( m_rasterState );
	DX_SAFE_RELEASE( m_defaultRasterState );
	DX_SAFE_RELEASE( m_inputLayout );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Shader::CreateRasterSate()
{
	D3D11_RASTERIZER_DESC desc;

	desc.FillMode = D3D11_FILL_SOLID; // full triangle swithc for wireframe
	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = TRUE;
	desc.DepthBias = 0U;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	ID3D11Device* device = m_owner->m_device;
	device->CreateRasterizerState( &desc , &m_rasterState );
}

ID3D11InputLayout* Shader::GetOrCreateInputLayout( buffer_attribute_t const* attribs )
{
	D3D11_INPUT_ELEMENT_DESC vertexDescription[ 3 ];
	if ( m_inputLayout != nullptr )
	{
		return m_inputLayout;
	}

	if ( m_lastBufferAttribute != attribs )
	{
		ConvertBufferAttributeToID3DX11Attribute( vertexDescription , attribs );
		m_lastBufferAttribute = attribs;
	}

	ID3D11Device* device = m_owner->m_device;
	device->CreateInputLayout(
		vertexDescription , 3 ,
		m_vertexStage.GetByteCode() , m_vertexStage.GetByteCodeLength() ,
		&m_inputLayout );
																			// describe the Vertex PCU
	return m_inputLayout;
}

//--------------------------------------------------------------------------------------------------------------------------------------------