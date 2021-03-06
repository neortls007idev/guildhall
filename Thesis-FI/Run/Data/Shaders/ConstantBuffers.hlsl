//#define CONSTANT_BUFFERS
//--------------------------------------------------------------------------------------
// Constant Input
// ------
// Constant Input are buffers bound that all shaders share/read.
// It can't be changed from the shader, but can be
// changed in-between draw calls on the CPU
//--------------------------------------------------------------------------------------

static const uint TOTAL_LIGHTS = 4;

//--------------------------------------------------------------------------------------
// buffer holding time information from our game
//--------------------------------------------------------------------------------------

cbuffer frame_constants : register( b0 )                                                    // constant buffer slot 0
{
    float           SYSTEM_TIME;
    float           SYSTEM_FRAME_TIME;
 //   float           GAME_TIME;
 //   float           GAME_DELTA_TIME;
    
    float           GAMMA;
    float           INVERSE_GAMMA;
};

//--------------------------------------------------------------------------------------
// buffer telling us information about our camera
//--------------------------------------------------------------------------------------

cbuffer camera_constants : register( b1 )                                                   // constant buffer slot 1
{
    float4x4        PROJECTION;                                                             // aka, CameraToClipTransform
    float4x4        VIEW;                                                                   // aka, WorldToCameraTransform
    
    float3          CAMERA_POSITION;
    float           padding00;
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
    float3          worldPosition;
    unsigned int    lightType;                                                              // this is not required, but know the GPU will add this padding to make the next variable 16-byte aligned
 //   unsigned int    pad00;                                                                  // this is not required, but know the GPU will add this padding to make the next variable 16-byte aligned

    float3          color;
    float           intensity;                                                              // rgb and an intensity
    
    float3          direction;                                                              // direction light is point, default (0,0,1)
    float           directionFactor;                                                        // do we consider this a point light (0.0f) or a directional light (1.0f).  Determine how we calcualte the incident light vector, default (0.0f)
    
    float3          attenuation;                                                            // attentuation for diffuse light, default (0,0,1)
    float           dotInnerAngle;                                                          // cone light inner angle (default -1.0f) - angle at which cone lights begin to stop affecting an object
  
    float3          specularAttenuation;                                                    // attenuation for specular lighting (constant,linear,quadratic), default (0,0,1)
    float           dotOuterAngle;                                                          // cone light outer angle (default -1.0f) - angle at which cone lights stop affecting the object completely
    
    float           shadowFlag;					                                            // 1.0f Enable Shadows for this light 0.0f Disable Shadows for this light.
    float           shadowBias;					                                            // Shadow bias for the light
	float2          pad00;						            								// padding to keep 16-byte structs
};

//--------------------------------------------------------------------------------------

struct lightView_t
{
    float4x4 LIGHT_PROJECTION;
    float4x4 LIGHT_VIEW;
};

//--------------------------------------------------------------------------------------
// buffer telling us information about our lights
//--------------------------------------------------------------------------------------

cbuffer light_constants : register( b3 )                                                    // constant buffer slot 3
{
    float4          AMBIENT;
    light_t         LIGHTS[TOTAL_LIGHTS];
    
     // all 0 to 1 and help influence the lighting equation
    float           DIFFUSE_FACTOR;                                                         // default: 1  - scales diffuse lighting in equation (lower values make an object absorb light
    float           SPECULAR_FACTOR;                                                        // default: 1  - scales specular lighting in the equation (higher values create a "shinier" surface)
    float           SPECULAR_POWER;                                                         // default: 32 - controls the specular falloff (higher values create a "smoother" looking surface)

    float           padding01;
};

//--------------------------------------------------------------------------------------

cbuffer fog_constants : register( b4 )                                                      // constant buffer slot 5
{
    float nearFog;
    float3 fogNearColor;
    
    float farFog;
    float3 fogFarColor;
};

//--------------------------------------------------------------------------------------

cbuffer lightView_constants : register( b5 )                                                // constant buffer slot 6
{
	//  light matrices to transform the vertex based on the light's view point

	lightView_t LIGHT_VIEW[ TOTAL_LIGHTS ];
    //float4x4 LIGHT_PROJECTION[ TOTAL_LIGHTS ];                                              // aka, CameraToClipTransform
    //float4x4 LIGHT_VIEW[ TOTAL_LIGHTS ];                                                    // aka, WorldToCameraTransform
};

//--------------------------------------------------------------------------------------
// Textures & Samplers are also a form of constant
// data - uniform/constant across the entire call
//--------------------------------------------------------------------------------------

Texture2D<float4>   tDiffuse    : register( t0 );                                           // color of the surface
Texture2D<float4>   tNormal     : register( t1 );                                           // normal Map of the surface

//--------------------------------------------------------------------------------------

//Texture2D<float4> depthMapTexture1 : register( t9 );
//Texture2D<float4> depthMapTexture2 : register( t10 );
//Texture2D<float4> depthMapTexture3 : register( t11 );
//Texture2D<float4> depthMapTexture4 : register( t12 );
//Texture2D<float4> depthMapTexture5 : register( t13 );
//Texture2D<float4> depthMapTexture6 : register( t14 );
//Texture2D<float4> depthMapTexture7 : register( t15 );

//--------------------------------------------------------------------------------------

SamplerState        sSampler    : register( s0 );                                           // sampler are rules on how to sample (read) from the texture.
//sampler2D        sSampler    : register( s0 );                                           // sampler are rules on how to sample (read) from the texture.

//--------------------------------------------------------------------------------------

float3 ApplyLinearFog( float3 worldPosition , float3 color )
{
    float distance          = length( worldPosition - CAMERA_POSITION );
    float alpha             = smoothstep( nearFog , farFog , distance );
    return lerp( color , fogFarColor , alpha.xxx );
}