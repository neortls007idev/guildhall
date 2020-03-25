#include "Engine/Renderer/D3D11Utils.hpp"


//--------------------------------------------------------------------------------------------------------------------------------------------

D3D11_COMPARISON_FUNC GetD3D11ComparisonFunc( eCompareOp compareFunc )
{
	switch ( compareFunc )
	{
	case COMPARE_NEVER:
		return D3D11_COMPARISON_NEVER;
	case COMPARE_ALWAYS:
		return D3D11_COMPARISON_ALWAYS;
	case COMPARE_EQUAL:
		return D3D11_COMPARISON_EQUAL;
	case COMPARE_NOTEQUAL:
		return D3D11_COMPARISON_NOT_EQUAL;
	case COMPARE_LESS:
		return D3D11_COMPARISON_LESS;
	case COMPARE_LEQUAL:
		return D3D11_COMPARISON_LESS_EQUAL;
	case COMPARE_GREATER:
		return D3D11_COMPARISON_GREATER;
	case COMPARE_GEQUAL:
		return D3D11_COMPARISON_GREATER_EQUAL;
	default:
		return D3D11_COMPARISON_NEVER;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------

D3D11_DEPTH_WRITE_MASK GetD3D11DepthWriteMask( bool writeOnPass )
{
	if ( writeOnPass )
	{
		return D3D11_DEPTH_WRITE_MASK_ALL;
	}

	return D3D11_DEPTH_WRITE_MASK_ZERO;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

D3D11_FILL_MODE GetD3D11FillMode( eRasterState rasterFillMode )
{
	switch ( rasterFillMode )
	{
	case FILL_SOLID:
		return D3D11_FILL_SOLID;
	case WIREFRAME:
		return D3D11_FILL_WIREFRAME;
	}
		
	return D3D11_FILL_SOLID;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

D3D11_CULL_MODE GetD3D11CullMode( eCullMode cullMode )
{
	switch ( cullMode )
	{
	case CULL_NONE:
					return D3D11_CULL_NONE;
	case CULL_FRONT:
					return D3D11_CULL_FRONT;
	case CULL_BACK:
					return D3D11_CULL_BACK;
	}

	return D3D11_CULL_NONE;
}

//--------------------------------------------------------------------------------------------------------------------------------------------