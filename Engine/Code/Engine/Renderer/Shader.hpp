#pragma once
#include "Engine/Platform/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <string.h>

//--------------------------------------------------------------------------------------------------------------------------------------------

class RenderContext;
struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D10Blob;

struct ID3D11InputLayout;
struct ID3D11RasterizerState;
struct buffer_attribute_t;

//--------------------------------------------------------------------------------------------------------------------------------------------

enum SHADER_STAGE_TYPE
{
	SHADER_STAGE_VERTEX ,
	SHADER_STAGE_FRAGMENT ,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class ShaderStage
{
public:
	ShaderStage();
	~ShaderStage();

	bool Compile( RenderContext* ctx , std::string const& filename , void const* source ,
				  size_t const sourceByteLen , SHADER_STAGE_TYPE stageType );

	inline bool IsValid() const { return nullptr != m_handle; }

	void const* GetByteCode() const;
	size_t GetByteCodeLength() const;


public:
	// a stage could be any one of these types,
	// so we use a enum to identify it, and a union to store it; 
	SHADER_STAGE_TYPE		m_stage;
	ID3D10Blob*				m_byteCode = nullptr ;

	union
	{
		ID3D11Resource*		m_handle;
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader*	m_fragmentShader;
	};
};

//--------------------------------------------------------------------------------------------------------------------------------------------

class Shader
{
public:
	Shader( RenderContext* context );
	~Shader();
	bool CreateFromFile( RenderContext* ctx , std::string const& filename );    
	bool CreateFromString( RenderContext* ctx , std::string const& filename );   
	// for hooking IA (input assembler) to the VS (vertex shader), 
	// needs to vertex shader and vertex format to make the binding
	//ID3D11InputLayout* GetOrCreateInputLayout( VertexBuffer* vbo );            

	ID3D11RasterizerState* GetRasterState();                                   
	void CreateRasterSate();

	ID3D11InputLayout* GetOrCreateInputLayout( buffer_attribute_t const* attribs );
	   
public:
	//static Shader*				s_errorShader;
	ShaderStage					m_vertexStage;
	ShaderStage					m_fragmentStage;
	ID3D11RasterizerState*		m_rasterState		  = nullptr;
	buffer_attribute_t const*	m_lastBufferAttribute = nullptr;

	// A02 temp
	RenderContext*				m_owner;
	ID3D11InputLayout*			m_inputLayout		 = nullptr; // for now, we'll have 1, but in the future you could have one for each different vertex type you use with this; 
	ID3D11RasterizerState*		m_defaultRasterState = nullptr;
};

//--------------------------------------------------------------------------------------------------------------------------------------------