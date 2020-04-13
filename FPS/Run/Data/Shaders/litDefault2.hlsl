#include "ShaderMathUtils.hlsl"
#include "ConstantBuffers.hlsl"
#include "defaultLitStageStructs.hlsl"

//--------------------------------------------------------------------------------------

float2 ComputeLightFactor( light_t light , float3 worldPosition , float3 worldNormal , float3 DirectionToEye )
{
    float3 vectorToLight            = light.world_position - worldPosition;
//return float2( vectorToLight.xx );
//return float2( vectorToLight.yy );
//return float2( vectorToLight.zz );

    float distance                  = length( vectorToLight );
//return float2( distance.xx ); 

    float3 directionToLight         = normalize( vectorToLight );
    float3 attenuationVector        = float3( 1.0f , distance , distance * distance );
    
    float diffuseAttenuation        = light.intensity / dot( attenuationVector , light.attenuation );
    float specularAttenuation       = light.intensity / dot( attenuationVector , light.spec_attenuation );
//return float2( diffuseAttenuation.xx );
//return float2( specularAttenuation.xx );
    
   // compute diffuse
   // max prevents lighting from the "back", which would subtract light
    float diffuse                   = max( 0.0f , dot( directionToLight , worldNormal ) );
//return float2( diffuse.xx );
      
   // BLINN-PHONG LIGHTING COMPUTATION
    float3 hv                       = normalize( directionToLight + DirectionToEye );
    float specular                  = max( 0.0f , dot( normalize( worldNormal ) , hv ) );
//return float2( specular.xx );
    
    // SPECULAR LIGHTING
    specular = SPECULAR_FACTOR * pow( specular , SPECULAR_POWER );
//return float2 ( specular.xx );
 
    return float2( diffuseAttenuation * diffuse , specularAttenuation * specular );
}

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
    float4 local_pos        = float4( input.position , 1.0f );                                      // passed in position is usually inferred to be "local position", ie, local to the object
    float4 world_pos        = mul( MODEL , local_pos );                                             // world pos is the object moved to its place int he world by the model, not used yet
    float4 camera_pos       = mul( VIEW , world_pos );                                          
    float4 clip_pos         = mul( PROJECTION , camera_pos );                                   
                                                                                                
   // normal is currently in model/local space                                                  
    float4 local_normal     = float4( input.normal , 0.0f );                                    
    float4 world_normal     = mul( MODEL , local_normal );                                      
                                                                                                
   // tangent & bitangent                                                                       
                                                                                                
    float4 local_tangent    = float4( input.tangent.xyz , 0.0f );                               
    float4 world_tangent    = mul( MODEL , local_tangent );                                     
                                                                                                
    v2f.position            = clip_pos;                                                             // we want to output the clip position to raster (a perspective point)
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

float4 FragmentFunction( v2f_t input ) : SV_Target0
{
    
//--------------------------------------------------------------------------------------
//              SAMPLE THE TEXTURES
//--------------------------------------------------------------------------------------    
    float4   diffuseColor        = tDiffuse.Sample( sSampler , input.uv );
    float4   normalColor         = tNormal.Sample( sSampler , input.uv );

//--------------------------------------------------------------------------------------
//              COMPUTE SURFACE COLOR
//--------------------------------------------------------------------------------------
    
    float3   surfaceColor        = pow( diffuseColor.xyz , INVERSE_GAMMA.xxx );
             surfaceColor        = surfaceColor * input.color.xyz;
    float    alpha               = diffuseColor.w * input.color.w;
  
    float3   tangent             = normalize( input.world_tangent.xyz );
    float3   normal              = normalize( input.world_normal );
    float3   bitangent           = normalize( cross( normal , tangent ) ) * input.world_tangent.w;
    float3x3 TBN                 = float3x3( tangent , bitangent , normal );
    float3   directionToCamera   = normalize( CAMERA_POSITION - input.world_position );                 // As the Camera IS OUR EYE
    
    float3   diffuse             = AMBIENT.xyz * AMBIENT.w;
    float3   surfaceNormal       = NormalColorToVector3( normalColor.xyz );
    float3   worldNormal         = mul( surfaceNormal , TBN );
  
    
    float3   specular            = float3( 0.0f.xxx );
   
//--------------------------------------------------------------------------------------
//              COMPUTE LIGHT FACTOR
//--------------------------------------------------------------------------------------
    
    float3   lightColor          = LIGHTS.color.xyz;
    float2   lightFactor         = ComputeLightFactor( LIGHTS , input.world_position , worldNormal , directionToCamera );
    
      
             diffuse            += lightFactor.x * lightColor;
             specular           += lightFactor.y * lightColor;
    
    
    // clamp diffuse to max of 1 (can't wash out a surface)
             diffuse             = min( diffuse , 1.0f );
    
    // spec does not get clamped - allowed to "blow out" for HDR purposes
//           spec               *= spec_color.xyz;

    // compute final color; 
    float3 finalColor            = diffuse * surfaceColor + specular;
    //finalColor                 = pow( finalColor.xyz , GAMMA.xxx );
        
    return float4( finalColor , alpha );

//--------------------------------------------------------------------------------------
//              THE END - DEBUG CODE BELOW
//--------------------------------------------------------------------------------------
    
//    float3 surface_color = ( input.color * diffuse_color ).xyz; // multiply our tint with our texture color to get our final color; 
//    float surface_alpha = ( input.color.a * diffuse_color.a );
//
//    float3 diffuse = AMBIENT.xyz * AMBIENT.w; // ambient color * ambient intensity
//    float3 surf_color = pow( diffuse_color.xyz , INVERSE_GAMMA.xxx );
//    float3 tangent = normalize( input.world_tangent.xyz );
//    float3 normal = normalize( input.world_normal );
//    float3 bitangent = normalize( cross( normal , tangent ) ) * input.world_tangent.w;
//    float3x3 TBN = float3x3( tangent , bitangent , normal );
//    
//   // get my surface normal - this comes from the vertex format
//   // We now have a NEW vertex format
//    float3 surface_normal = NormalColorToVector3( normal_color ); // (0 to 1) space to (-1, -1, 0),(1, 1, 1) space
//    surface_normal = mul( surface_normal , TBN );
//
//   // float3 surface_normal = normalize( input.world_normal ); 
//
//   // for each light, we going to add in dot3 factor it
//    float3 light_position = LIGHT.world_position;
//    float3 dir_to_light = normalize( light_position - input.world_position );
//    float dot3 = max( 0.0f , dot( dir_to_light , surface_normal ) );
//
//    diffuse += dot3;
//
//   // just diffuse lighting
//    diffuse = min( float3( 1 , 1 , 1 ) , diffuse );
//    diffuse = saturate( diffuse ); // saturate is clamp01(v)
//    float3 final_color = diffuse * surface_color;
//
//   
//    return float4( final_color , surface_alpha );
}
//--------------------------------------------------------------------------------------