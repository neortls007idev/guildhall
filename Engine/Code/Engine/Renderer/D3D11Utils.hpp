#pragma once
#include "Engine/Renderer/D3D11Common.hpp"

//--------------------------------------------------------------------------------------------------------------------------------------------

enum  eRasterState
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
D3D11_FILL_MODE GetD3D11FillMode( eRasterState rasterFillMode );
D3D11_COMPARISON_FUNC GetD3D11ComparisonFunc( eCompareOp compareFunc );
D3D11_DEPTH_WRITE_MASK GetD3D11DepthWriteMask( bool writeOnPass );