#pragma once
#include "Engine/Core/CoreUtils.hpp"
#include "Engine/Renderer/D3D11Common.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  eRasterStateFillMode
{
	FILL_SOLID ,
	WIREFRAME ,
	TOTAL_RASTER_STATES
};

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  eCullMode
{
	CULL_NONE = 1 ,
	CULL_FRONT ,
	CULL_BACK ,
};

//--------------------------------------------------------------------------------------------------------------------------------------------

D3D11_CULL_MODE GetD3D11CullMode( eCullMode cullMode );
D3D11_FILL_MODE GetD3D11FillMode( eRasterStateFillMode rasterFillMode );

eCullMode		GetCullModeForD3D11CullMode( D3D11_CULL_MODE D3D11CullMode );
eRasterStateFillMode	GetFillModeForD3D11RasterState( D3D11_FILL_MODE D3D11RasterFillMode );
eWindingOrder	GetWindingOrderForD3D11WindingOrder( BOOL D3D11WindingOrder );

D3D11_COMPARISON_FUNC GetD3D11ComparisonFunc( eCompareOp compareFunc );
D3D11_DEPTH_WRITE_MASK GetD3D11DepthWriteMask( bool writeOnPass );