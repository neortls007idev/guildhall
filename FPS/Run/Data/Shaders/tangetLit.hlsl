#include "ShaderMathUtils.hlsl"
#include "ConstantBuffers.hlsl"
#include "defaultLitStageStructs.hlsl"

//--------------------------------------------------------------------------------------

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

    float3 world_position : WORLD_POSITION;
    float3 world_normal : WORLD_NORMAL;
    float3 world_tangent : WORLD_TANGENT;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction(vs_input_t input)
{
    v2f_t v2f = (v2f_t) 0;

   // move the vertex through the spaces
    float4 local_pos    = float4(input.position, 1.0f); // passed in position is usually inferred to be "local position", ie, local to the object
    float4 world_pos    = mul(MODEL, local_pos); // world pos is the object moved to its place int he world by the model, not used yet
    float4 camera_pos   = mul(VIEW, world_pos);
    float4 clip_pos     = mul(PROJECTION, camera_pos);

   // normal is currently in model/local space
    float4 local_tangent = float4(input.tangent, 0.0f);
    float4 world_tangent = mul(MODEL, local_tangent);
    
   // tangent & bitangent

    v2f.position        = clip_pos; // we want to output the clip position to raster (a perspective point)
    v2f.color           = input.color * TINT;
    v2f.uv              = input.uv;
    v2f.world_position  = world_pos.xyz;
    v2f.world_tangent   = world_tangent.xyz;

    return v2f;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
// 
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.

float4 ConvertVectorToColor( float3 vec )
{
    float4 color;
    color.x = RangeMap(vec.x, -1.f, 1.f, 0.f, 1.f);
    color.y = RangeMap(vec.y, -1.f, 1.f, 0.f, 1.f);
    color.z = RangeMap(vec.z, -1.f, 1.f, 0.f, 1.f);
    color.w = 1;
    return color;
}

//--------------------------------------------------------------------------------------

float4 FragmentFunction(v2f_t input) : SV_Target0
{

    float4 color = ConvertVectorToColor(input.world_tangent);
    color = normalize(color);
    return color;

}

//--------------------------------------------------------------------------------------