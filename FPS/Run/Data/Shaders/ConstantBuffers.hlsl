
//--------------------------------------------------------------------------------------
// Constant Input
// ------
// Constant Input are buffers bound that all shaders share/read.
// It can't be changed from the shader, but can be
// changed in-between draw calls on the CPU
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// buffer holding time information from our game
//--------------------------------------------------------------------------------------

cbuffer frame_constants : register( b0 )                                                    // constant buffer slot 0
{
    float           SYSTEM_TIME;
    float           SYSTEM_FRAME_TIME;
    float           GAME_TIME;
    float           GAME_DELTA_TIME;
};

//--------------------------------------------------------------------------------------
// buffer telling us information about our camera
//--------------------------------------------------------------------------------------

cbuffer camera_constants : register( b1 )                                                   // constant buffer slot 1
{
    float4x4        PROJECTION;                                                             // aka, CameraToClipTransform
    float4x4        VIEW;                                                                   // aka, WorldToCameraTransform
};

//--------------------------------------------------------------------------------------
// information that might change per model/object
//--------------------------------------------------------------------------------------

cbuffer model_constants : register( b2 )                                                    // constant buffer slot 2
{
    float4x4        MODEL;
    float4          TINT;
};

//--------------------------------------------------------------------------------------

struct light_t
{
    float3          world_position;
    float           pad00;                                                                  // this is not required, but know the GPU will add this padding to make the next variable 16-byte aligned

    float3          color;
    float           intensity;                                                              // rgb and an intensity
};

//--------------------------------------------------------------------------------------
// buffer telling us information about our lights
//--------------------------------------------------------------------------------------

cbuffer light_constants : register( b3 )                                                    // constant buffer slot 3
{
    float4          AMBIENT;
    light_t         LIGHT;
};

//--------------------------------------------------------------------------------------
// Textures & Samplers are also a form of constant
// data - uniform/constant across the entire call
//--------------------------------------------------------------------------------------

Texture2D<float4>   tDiffuse    : register( t0 );                                           // color of the surface
Texture2D<float4>   tNormal     : register( t1 );                                           // normal Map of the surface
SamplerState        sSampler    : register( s0 );                                           // sampler are rules on how to sample (read) from the texture.

//--------------------------------------------------------------------------------------