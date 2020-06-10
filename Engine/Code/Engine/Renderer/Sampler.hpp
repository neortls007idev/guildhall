#pragma once
#include "RendererCommon.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

struct ID3D11SamplerState;
class RenderContext;

//--------------------------------------------------------------------------------------------------------------------------------------------

class Sampler
{
public:
	Sampler( RenderContext* ctx , eSamplerType type );
	~Sampler();

	inline ID3D11SamplerState*	GetHandle() const					{ return m_handle;  }
	inline eSamplerType			GetType() const						{ return m_type; }
public:
	RenderContext*		m_owner;
	ID3D11SamplerState* m_handle;
	eSamplerType		m_type;			// DEBUG INFO 

private:

};
