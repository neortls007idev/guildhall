//--------------------------------------------------------------------------------------
// Stream Input
// ------
// Stream Input is input that is walked by the vertex shader.
// If you say "Draw(3,0)", you are telling to the GPU to expect '3' sets, or
// elements, of input data.  IE, 3 vertices.  Each call of the VertxShader
// we be processing a different element.
//--------------------------------------------------------------------------------------

// inputs are made up of internal names (ie: uv) and semantic names
// (ie: TEXCOORD).  "uv" would be used in the shader file, where
// "TEXCOORD" is used from the client-side (cpp code) to attach ot.
// The semantic and internal names can be whatever you want,
// but know that semantics starting with SV_* usually denote special
// inputs/outputs, so probably best to avoid that naming.
struct vs_input_t
{
   // we are not defining our own input data;
	// Name : SEMANTIC NAME  - SEMANTIC NAME NECESSARY to bind it with the input

   float3 position      : POSITION;
   float4 color         : COLOR;
   float2 uv            : TEXCOORD;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------

float RangeMap( float val , float inMin , float inMax , float outMin , float outMax )
{
	float domain = inMax - inMin;
	float range = outMax - outMin;
	return ( ( val - inMin ) / domain ) * range + outMin;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// constant built into the shader
static float SHIFT = 5.0f;
//

cbuffer time_constants : register( b0 ) // index 0 is now time
{
	float SYSTEM_TIME_SECONDS;
	float SYSTEM_TIME_DELTA_SECONDS;
}

cbuffer camera_constants : register( b1 ) // index 1 is now camera
{
	// MVP - Model - View - Projection
	// float2 orthoMin;
	// float2 orthoMax;
	float4x4 CAMERA_TO_CLIP_TRANSFORM; // PROJECTION MATRIX
	float4x4 VIEW;
}

// Texture & Samplers are also a form of constants

Texture2D <float4> tDiffuse : register( t0 );			// Color of surface
SamplerState eSampler : register( s0 );					// Sampler are rules on hoe to sample

//--------------------------------------------------------------------------------------
// Programmable Shader Stages
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// for passing data from vertex to fragment (v-2-f)
struct v2f_t
{
   float4 position : SV_POSITION;
   float4 color : COLOR;
   float2 uv : UV;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction( vs_input_t input )
{
   v2f_t v2f = (v2f_t)0;

   // forward vertex input onto the next stage
   v2f.position = float4( input.position, 1.0f );
   v2f.color = input.color;
   v2f.uv = input.uv;

   float4 worldPos = float4( input.position , 1 );
 //  worldPos.x += cos( SYSTEM_TIME_SECONDS );
  // worldPos.y += sin( SYSTEM_TIME_SECONDS );

   //float4 clipPos = worldPos; // might have a w( usually 1 for now )
   float4 cameraPos = mul( VIEW , worldPos );
   float4 clipPos = mul( CAMERA_TO_CLIP_TRANSFORM , cameraPos );


   //float clipPos;
//   clipPos.x = RangeMap( worldPos.x , orthoMin.x , orthoMax.x , -1.0f , 1.0f );
  // clipPos.y = RangeMap( worldPos.y , orthoMin.y , orthoMax.y , -1.0f , 1.0f );
   //clipPos.z = 0.f;
   //clipPos.w = 1.f;

   // SYSTEM_TIME_SECONDS = 4.0f // ERROR SINCE ITS A CONSTANT BUFFER
   //v2f.position.x += cos( SYSTEM_TIME_SECONDS );
   //v2f.position.y += sin( SYSTEM_TIME_SECONDS );

   v2f.position = clipPos;
   return v2f;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
//
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
	// we'll outoupt our UV coordinates as color here
	// to make sure they're being passed correctly.
	// Very common rendering debugging method is to
	// use color to portray information;

	//	float4 uvAsColor = float4( input.uv, 0.0f, 1.0f );
	//	float4 finalColor = uvAsColor * input.color;

	//return float4 ( input.uv , 0 , 1 );

		//input.uv[1] = 0.5f * ( sin( input.uv.x * 40.0f + SYSTEM_TIME_SECONDS * 10.f ) );

		float4 color = tDiffuse.Sample( eSampler, input.uv );
		return color * input.color;


	// float r = 0.5f * ( sin( input.uv.x * 40.0f + SYSTEM_TIME_SECONDS * 10.f ) + 1.0f );
	//	float  b = 0.5f * ( cos( input.uv.y * 40.0f + SYSTEM_TIME_SECONDS * 10.f ) + 1.0f );
	//	return float4( r , 0 , b , 1 );

//   return finalColor;
}
