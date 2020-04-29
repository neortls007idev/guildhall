#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/Shader.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

extern RenderContext* g_theRenderer;

//--------------------------------------------------------------------------------------------------------------------------------------------

ShaderState::ShaderState( XMLElement const& node )
{
	m_depthTest				= ( eCompareOp )			ParseXmlAttribute( node , "DepthTest"		, ( int ) m_depthTest );
	m_blendMode				= ( eBlendMode )			ParseXmlAttribute( node , "BlendMode"		, ( int ) m_blendMode );
	m_writeDepth			=							ParseXmlAttribute( node , "WriteDepth"		, m_writeDepth );

	m_windingOrder			= ( eWindingOrder )			ParseXmlAttribute( node , "WindingOrder"	, ( int ) m_windingOrder );
	m_culling				= ( eCullMode )				ParseXmlAttribute( node , "CullMode"		, ( int ) m_culling );
	m_fillMode				= ( eRasterStateFillMode )	ParseXmlAttribute( node , "FillMode"		, ( int ) m_fillMode );

	std::string shaderPath	=							ParseXmlAttribute( node , "Shader"			, "" );

	m_shader = g_theRenderer->GetOrCreateShader( shaderPath.c_str() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ShaderState::SetupFromXML( XMLElement const& node )
{
	m_depthTest				= ( eCompareOp )			ParseXmlAttribute( node , "DepthTest"		, ( int ) m_depthTest );
	m_blendMode				= ( eBlendMode )			ParseXmlAttribute( node , "BlendMode"		, ( int ) m_blendMode );
	m_writeDepth			=							ParseXmlAttribute( node , "WriteDepth"		, m_writeDepth );

	m_windingOrder			= ( eWindingOrder )			ParseXmlAttribute( node , "WindingOrder"	, ( int ) m_windingOrder );
	m_culling				= ( eCullMode )				ParseXmlAttribute( node , "CullMode"		, ( int ) m_culling );
	m_fillMode				= ( eRasterStateFillMode )	ParseXmlAttribute( node , "FillMode"		, ( int ) m_fillMode );

	std::string shaderPath	=							ParseXmlAttribute( node , "Shader"			, "" );

	m_shader = g_theRenderer->GetOrCreateShader( shaderPath.c_str() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

void ShaderState::SetCurrentShader( Shader* const shader )
{
	m_shader = shader;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
