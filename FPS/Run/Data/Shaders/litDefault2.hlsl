#include "ShaderMathUtils.hlsl"
#include "ConstantBuffers.hlsl"
#include "defaultLitStageStructs.hlsl"

//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//                      PROGRAMMABLE SHADER STAGES FUNCTIONS
//--------------------------------------------------------------------------------------
//
// VERTEX SHADER
//
//--------------------------------------------------------------------------------------

v2f_t VertexFunction(vs_input_t input)
{
    v2f_t v2f = (v2f_t) 0;

   // move the vertex through the spaces
    float4 local_pos        = float4( input.position , 1.0f );                                  // passed in position is usually inferred to be "local position", ie, local to the object
    float4 world_pos        = mul( MODEL , local_pos );                                         // world pos is the object moved to its place int he world by the model, not used yet
    float4 camera_pos       = mul( VIEW , world_pos );                                          
    float4 clip_pos         = mul( PROJECTION , camera_pos );                                   
                                                                                                
   // normal is currently in model/local space                                                  
    float4 local_normal     = float4( input.normal , 0.0f );                                    
    float4 world_normal     = mul( MODEL , local_normal );                                      
                                                                                                
   // tangent & bitangent                                                                       
                                                                                                
    float4 local_tangent    = float4( input.tangent.xyz , 0.0f );                               
    float4 world_tangent    = mul( MODEL , local_tangent );                                     
                                                                                                
    v2f.position            = clip_pos;                                                         // we want to output the clip position to raster (a perspective point)
    v2f.color               = input.color * TINT;
    v2f.uv                  = input.uv;
    v2f.world_position      = world_pos.xyz;
    v2f.world_normal        = world_normal.xyz;
    v2f.world_tangent       = float4( world_tangent.xyz , input.tangent.w );
    return v2f;
}


//--------------------------------------------------------------------------------------
//                  FRAGMENT SHADER
//--------------------------------------------------------------------------------------
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
//--------------------------------------------------------------------------------------

/*

float4 FragmentFunction(v2f_t input) : SV_Target0
{

   // use the uv to sample the texture
    
    //return float4(AMBIENT.xyz * AMBIENT.w, 1);
    //return float4(input.uv , 0, 1);
    
    float4 texture_color = tDiffuse.Sample(sSampler, input.uv);
    //return texture_color;
    
    float3 surface_color = (input.color * texture_color).xyz; // multiply our tint with our texture color to get our final color; 
    float surface_alpha = (input.color.a * texture_color.a);
    //return float4(surface_color, 1);
   // return float4(surface_alpha.xxxx);
    float3 diffuse = AMBIENT.xyz * AMBIENT.w; // ambient color * ambient intensity
 
    //return float4(diffuse.zzz, 1);
   
    // get my surface normal - this comes from the vertex format
   // We now have a NEW vertex format
    float3 normal_color = tNormal.Sample(sSampler, input.uv);
    float3 surface_normal = ConvertNormalizedVector3ToColor( normal_color );
    //convert color tovector; // (0 to 1) space to (-1, -1, 0),(1, 1, 1) space
    surface_normal = mul(surface_normal, normalize(input.world_normal));
    
    //float3 surface_normal = normalize(input.world_normal);
   //return float4(surface_normal , 1);
   // for each light, we going to add in dot3 factor it
    float3 light_position = LIGHT.world_position;
    float3 dir_to_light = normalize(light_position - input.world_position);
    //return float4(dir_to_light, 1); 
    float dot3 = max(0.0f, dot(dir_to_light, surface_normal));
   
    //return float4(dot3.xxxx );
    diffuse += dot3;
 
   // just diffuse lighting
   // return float4(diffuse, 1);
    diffuse = min(float3(1, 1, 1), diffuse.xyz);
    diffuse = saturate(diffuse); // saturate is clamp01(v)
    //return float4(surface_color, 1);
    
    float3 final_color = diffuse * surface_color;
    return float4(final_color, surface_alpha);
   
   //return float4(diffuse, 1);
}
*/

float4 FragmentFunction( v2f_t input ) : SV_Target0
{
   // use the uv to sample the texture
    float4 texture_color = tDiffuse.Sample( sSampler , input.uv );
    float3 surface_color = ( input.color * texture_color ).xyz; // multiply our tint with our texture color to get our final color; 
    float surface_alpha = ( input.color.a * texture_color.a );

    float3 diffuse = AMBIENT.xyz * AMBIENT.w; // ambient color * ambient intensity

    float3 tangent = normalize( input.world_tangent.xyz );
    float3 normal = normalize( input.world_normal );
    float3 bitangent = normalize( cross( normal , tangent ) ) * input.world_tangent.w;
    float3x3 TBN = float3x3( tangent , bitangent , normal );
    
   // get my surface normal - this comes from the vertex format
   // We now have a NEW vertex format
    float3 normal_color = tNormal.Sample( sSampler , input.uv );
    float3 surface_normal = NormalColorToVector3( normal_color ); // (0 to 1) space to (-1, -1, 0),(1, 1, 1) space
    surface_normal = mul( surface_normal , TBN );

   // float3 surface_normal = normalize( input.world_normal ); 

   // for each light, we going to add in dot3 factor it
    float3 light_position = LIGHT.world_position;
    float3 dir_to_light = normalize( light_position - input.world_position );
    float dot3 = max( 0.0f , dot( dir_to_light , surface_normal ) );

    diffuse += dot3;

   // just diffuse lighting
    diffuse = min( float3( 1 , 1 , 1 ) , diffuse );
    diffuse = saturate( diffuse ); // saturate is clamp01(v)
    float3 final_color = diffuse * surface_color;

   
    return float4( final_color , surface_alpha );
}
//--------------------------------------------------------------------------------------