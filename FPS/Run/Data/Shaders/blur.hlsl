#include "ConstantBuffers.hlsl"
// input to the vertex shader - for now, a special input that is the index of the vertex we're drawing

//--------------------------------------------------------------------------------------
// constants
//--------------------------------------------------------------------------------------
// The term 'static' refers to this an built into the shader, and not coming
// from a contsant buffer - which we'll get into later (if you remove static, you'll notice
// this stops working).

static float3 POSITIONS[ 3 ] =
{
   float3( -1.0f , -1.0f , 0.0f ) ,
   float3(  3.0f , -1.0f , 0.0f ) ,
   float3( -1.0f ,  3.0f , 0.0f )
};

//--------------------------------------------------------------------------------------

static float2 UVS[ 3 ] =
{
   float2( 0.0f ,  1.0f ) ,
   float2( 2.0f ,  1.0f ) ,
   float2( 0.0f , -1.0f )
};

//--------------------------------------------------------------------------------------

static float Offsets[ 11 ] =
{
  -5 ,
  -4 ,
  -3 ,
  -2 ,
  -1 ,
  0 ,
  1 ,
  2 ,
  3 ,
  4 ,
  5 ,
};

//--------------------------------------------------------------------------------------

static float Weights[ 11 ][ 11 ] =
{
  
{0	    ,    0	     ,   0	     ,   0	     ,   0.000001,	0.000001,	0.000001,	0	    ,    0	     ,   0	     ,   0       },
{0	    ,    0	     ,   0.000001,	 0.000014,	 0.000055,	0.000088,	0.000055,	0.000014,	 0.000001,	 0	     ,   0       },
{0	    ,    0.000001,	 0.000036,	 0.000362,	 0.001445,	0.002289,	0.001445,	0.000362,	 0.000036,	 0.000001,	 0       },
{0	    ,    0.000014,	 0.000362,	 0.003672,	 0.014648,	0.023204,	0.014648,	0.003672,	 0.000362,	 0.000014,	 0       },
{0.000001,	 0.000055,	 0.001445,	 0.014648,	 0.058433,	0.092564,	0.058433,	0.014648,	 0.001445,	 0.000055,	 0.000001},
{0.000001,	 0.000088,	 0.002289,	 0.023204,	 0.092564,	0.146632,	0.092564,	0.023204,	 0.002289,	 0.000088,	 0.000001},
{0.000001,	 0.000055,	 0.001445,	 0.014648,	 0.058433,	0.092564,	0.058433,	0.014648,	 0.001445,	 0.000055,	 0.000001},
{0	    ,    0.000014,	 0.000362,	 0.003672,	 0.014648,	0.023204,	0.014648,	0.003672,	 0.000362,	 0.000014,	 0       },
{0	    ,    0.000001,	 0.000036,	 0.000362,	 0.001445,	0.002289,	0.001445,	0.000362,	 0.000036,	 0.000001,	 0       },
{0	    ,    0	     ,   0.000001,	 0.000014,	 0.000055,	0.000088,	0.000055,	0.000014,	 0.000001,	 0	     ,   0       },
{0	    ,    0	     ,   0	     ,   0	     ,   0.000001,	0.000001,	0.000001,	0	    ,    0	     ,   0	     ,   0       },

};

//--------------------------------------------------------------------------------------

struct vs_input_t
{
   uint vidx : SV_VERTEXID;                                 // SV_* stands for System Variable (ie, built-in by D3D11 and has special meaning)
                                                            // in this case, is the the index of the vertex coming in.
};

//--------------------------------------------------------------------------------------

struct VertexToFragment_t
{
   // SV_POSITION is a semantic - or a name identifying this variable.
   // Usually a semantic can be any name we want - but the SV_* family
   // usually denote special meaning (SV = System Variable I believe)
   // SV_POSITION denotes that this is output in clip space, and will be
   // use for rasterization.  When it is the input (pixel shader stage), it will
   // actually hold the pixel coordinates.

   float4 position : SV_POSITION;
   float2 uv : UV;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VertexToFragment_t VertexFunction( vs_input_t input )
{
   VertexToFragment_t v2f = (VertexToFragment_t)0;

   // The output of a vertex shader is in clip-space, which is a 4D vector
   // so we need to convert out input to a 4D vector.
    
   v2f.position = float4( POSITIONS[input.vidx], 1.0f );
   v2f.uv = UVS[input.vidx];

   // And return - this will pass it on to the next stage in the pipeline;
    
   return v2f;
}

Texture2D<float4> blurDiffuse : register( t8 );

float4 FragmentFunction( VertexToFragment_t input ) : SV_Target0
{

//--------------------------------------------------------------------------------------
//              SAMPLE THE TEXTURES
//--------------------------------------------------------------------------------------    
  
    float2 textureDimensions; 
    blurDiffuse.GetDimensions( textureDimensions.x , textureDimensions.y );
    
    float PixelWidth = 1.0f / textureDimensions.x;
    float PixelHeight = 1.0f / textureDimensions.y;

    float4 Color = { 0 , 0 , 0 , 0 };

    float2 Blur;
        
    for( int x = 0 ; x < 11 ; x++ )
    {
        Blur.x = input.uv.x + Offsets[ x ] * PixelWidth;
        for( int y = 0 ; y < 11 ; y++ )
        {
            Blur.y = input.uv.y + Offsets[ y ] * PixelHeight;
            Color += blurDiffuse.Sample( sSampler , Blur ) * Weights[ x ][ y ];
        }
    }
     
    //return float4( Blur.x , Blur.y , 0 , 1 );
    return Color;
}

//--------------------------------------------------------------------------------------