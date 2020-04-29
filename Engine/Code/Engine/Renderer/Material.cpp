#include "Engine/Renderer/Material.hpp"
//#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"

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
	materialDefinition = materialDefinition->FirstChildElement( "Shader" );

	m_shaderState = new ShaderState();
	m_shaderState->SetupFromXML( *materialDefinition );
// 	while ( materialDefinition )
// 	{
// 		std::string key = ParseXmlAttribute( *materialDefinition , "name" , "Invalid Name" );
// 		s_definitions[ key ] = currentActorDefiniton;
// 		materialDefinition = materialDefinition->NextSiblingElement();
// 	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Material::SetData( void const* data , size_t dataSize )
{
	m_uboCPUData.resize( dataSize );
	memcpy( &m_uboCPUData[ 0 ] , data , dataSize );
	m_uboIsDirty = true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void Material::SetShader( Shader* const shader )
{
	m_shaderState->SetCurrentShader( shader );
}

//--------------------------------------------------------------------------------------------------------------------------------------------