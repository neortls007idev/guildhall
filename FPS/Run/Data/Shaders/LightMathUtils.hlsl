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

float3 ComputeLightingAt( float3 worldPos , float3 worldNormal , float3 surfaceColor , float3 emmisiveColor , float specularFactor )
{
    float3 directionToEye           = normalize( CAMERA_POSITION - worldPos );

    float3 diffuse                  = pow( AMBIENT.xyz * AMBIENT.w , GAMMA.xxx );                                       // assumes ambient is set from a user - so sRGB space
    float3 specular                 = float3( 0.0f.xxx );

   // add up contribution of all lights
    for( uint index = 0 ; index < TOTAL_LIGHTS ; index++ )
    {
        float3 lightColor           = LIGHTS[ index ].color.xyz;
        lightColor                  = pow( lightColor , GAMMA.xxx );                                                    // assumes light color is set by a user - so sRGB space
        float2 lightFactors         = ComputeLightFactor( LIGHTS[ index ] , worldPos , worldNormal , directionToEye );
      
        diffuse                    += lightFactors.x * lightColor;
        specular                   += lightFactors.y * lightColor;
    }
    
   // limit it
    diffuse                         = min( DIFFUSE_FACTOR * diffuse , float3( 1.0f.xxx ) );
    specular                       *= specularFactor;                                                                   // scale back specular based on spec factor

   // returns light color (in linear space)
    return diffuse * surfaceColor + specular + emmisiveColor;
}