#pragma once
#include "Engine/Renderer/RendererCommon.hpp"
#include "Engine/Core/CoreUtils.hpp"
#include "Engine/Renderer/D3D11Utils.hpp"
//#include "Engine/Renderer/RenderContext.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

class ShaderState
{
public:
	ShaderState() {};
	ShaderState( XMLElement const& node );

	void SetupFromXML( XMLElement const& node );

	inline Shader* GetCurrentShader() const															{ return m_shader; }

	inline Shader* SetCurrentShader( Shader* const shader );

public:
	Shader*						m_shader;

	eBlendMode					m_blendMode			= eBlendMode::SOLID;
	eCompareOp					m_depthTest			= COMPARE_LEQUAL;

	bool						m_writeDepth		= true;
	eWindingOrder				m_windingOrder		= eWindingOrder::COUNTERCLOCKWISE;
	eCullMode					m_culling			= eCullMode::CULL_BACK;
	eRasterStateFillMode		m_fillMode			= eRasterStateFillMode::FILL_SOLID;
};

//--------------------------------------------------------------------------------------------------------------------------------------------