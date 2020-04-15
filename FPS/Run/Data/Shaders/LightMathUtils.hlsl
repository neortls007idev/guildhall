//#ifndef CONSTANT_BUFFERS
    #include "ConstantBuffers.hlsl"
//#define CONSTANT_BUFFERS
//--------------------------------------------------------------------------------------------------------------------------------------------

float2 ComputeLightFactor( light_t light , float3 worldPosition , float3 worldNormal , float3 DirectionToEye )
{
    float3 vectorToLight = light.world_position - worldPosition;
//return float2( vectorToLight.xx );
//return float2( vectorToLight.yy );
//return float2( vectorToLight.zz );

    float distance = length( vectorToLight );
//return float2( distance.xx ); 

    float3 directionToLight = normalize( vectorToLight );
    float3 attenuationVector = float3( 1.0f , distance , distance * distance );
    
    float diffuseAttenuation = light.intensity / dot( attenuationVector , light.attenuation );
    float specularAttenuation = light.intensity / dot( attenuationVector , light.spec_attenuation );
//return float2( diffuseAttenuation.xx );
//return float2( specularAttenuation.xx );
    
   // compute diffuse
   // max prevents lighting from the "back", which would subtract light
    float diffuse = max( 0.0f , dot( directionToLight , worldNormal ) );
//return float2( diffuse.xx );
      
   // BLINN-PHONG LIGHTING COMPUTATION
    float3 hv = normalize( directionToLight + DirectionToEye );
    float specular = max( 0.0f , dot( normalize( worldNormal ) , hv ) );
//return float2( specular.xx );
    
    // SPECULAR LIGHTING
    specular = SPECULAR_FACTOR * pow( specular , SPECULAR_POWER );
//return float2 ( specular.xx );
 
    return float2( diffuseAttenuation * diffuse , specularAttenuation * specular );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float3 ComputeLightingAt( float3 world_pos , float3 world_normal ,
   float3 surf_color , float3 emsv_color , float spec_factor )
{
    float3 dir_to_eye = normalize( CAMERA_POSITION - world_pos );

    float3 diffuse = pow( AMBIENT.xyz * AMBIENT.w , GAMMA.xxx ); // assumes ambient is set from a user - so sRGB space
    float3 spec = float3( 0.0f.xxx );

   // add up contribution of all lights
//    for( uint i = 0 ; i < MAX_LIGHTS ; ++i )
//    {
//        float3 light_color = LIGHTS[ i ].color.xyz;
//        light_color = pow( light_color , GAMMA.xxx ); // assumes light color is set by a user - so sRGB space
//        float3 light_factors = ComputeLightFactor( LIGHTS[ i ] , world_pos , world_normal , dir_to_eye );
//      
//        diffuse += light_factors.x * light_color;
//        spec += light_factors.y * light_color;
//    }

    float3 light_color = LIGHTS.color.xyz;
    light_color = pow( light_color , GAMMA.xxx ); // assumes light color is set by a user - so sRGB space
    float2 light_factors = ComputeLightFactor( LIGHTS , world_pos , world_normal , dir_to_eye );
      
    diffuse += light_factors.x * light_color;
    spec += light_factors.y * light_color;
    
   // limit it
    diffuse = min( DIFFUSE_FACTOR * diffuse , float3( 1.0f.xxx ) );
    spec *= spec_factor; // scale back specular based on spec factor

   // returns light color (in linear space)
    return diffuse * surf_color + spec + emsv_color;
}