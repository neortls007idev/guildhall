#include "Engine/Renderer/Material.hpp"

#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

Material::~Material()
{
	SAFE_RELEASE_POINTER( m_ubo );
	SAFE_RELEASE_POINTER( m_shaderState );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Material::CreateFromFile( const char* xmlFilePath )
{
	tinyxml2::XMLDocument xmlDocument;
	xmlDocument.LoadFile( xmlFilePath );

	if ( xmlDocument.ErrorID() != tinyxml2::XML_SUCCESS )
	{
		ERROR_AND_DIE( "XML FILE DID NOT LOAD" );
		return;
	}

	tinyxml2::XMLElement* materialDefinition = xmlDocument.RootElement();
	//materialDefinition = materialDefinition->FirstChildElement( "Shader" );

	m_shaderState = g_theRenderer->GetOrCreateShaderState( xmlFilePath );
	//m_shaderState = new ShaderState();
	//m_shaderState->SetupFromXML( *materialDefinition );
 	while ( materialDefinition )
 	{
 		std::string key = ParseXmlAttribute( *materialDefinition , "name" , "Invalid Name" );
 		materialDefinition = materialDefinition->NextSiblingElement();
 	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Material::SetData( void const* data , size_t dataSize )
{
	m_uboCPUData.resize( dataSize );
	memcpy( &m_uboCPUData[ 0 ] , data , dataSize );
	m_uboIsDirty = true;

	if ( nullptr == m_ubo )
	{
		m_ubo = new RenderBuffer( g_theRenderer , UNIFORM_BUFFER_BIT , MEMORY_HINT_DYNAMIC );
	}
	
	m_ubo->Update( data , dataSize , dataSize );
//	void* nonconstData = const_cast<void*>( data );
//	g_theRenderer->BindMaterialData( nonconstData , dataSize );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Material::SetShader( Shader* const shader )
{
	m_shaderState->SetCurrentShader( shader );
}

//--------------------------------------------------------------------------------------------------------------------------------------------