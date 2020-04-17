//#ifndef CONSTANT_BUFFERS
    #include "ConstantBuffers.hlsl"
//#define CONSTANT_BUFFERS
//--------------------------------------------------------------------------------------------------------------------------------------------

float2 ComputeLightFactor( light_t light , float3 worldPosition , float3 worldNormal , float3 directionToEye )
{
    float3      vectorToLight           = light.worldPosition - worldPosition;
    //return float2( vectorToLight.xx );
    //return float2( vectorToLight.yy );
    //return float2( vectorToLight.zz );
    
    float       distanceToLight         = length( vectorToLight );
    //return float2( distanceToLight.xx );
    float3      directionToLight        = normalize( vectorToLight );
    
    //float       distanceToPlane         = abs( dot( worldPosition - light.worldPosition , light.direction ) );
    //return float2( distanceToPlane.xx );
    //            distanceToLight         = lerp( distanceToLight , distanceToPlane , light.directionFactor ) + 0.00037695241f; //  little nudge to prevent 0 
    
    float3      lightDirection          = lerp( directionToLight , light.direction , light.directionFactor );
    //return float2( lightDirection.xx );
    //return float2( lightDirection.yy );
    //return float2( lightDirection.zz );
    float3      attenuationVector       = float3( 1.0f , distanceToLight , distanceToLight * distanceToLight );
    
    // how much do these two coincide (cone lights)
    float       dotAngle                = dot( light.direction , directionToLight );
    //return float2( dotAngle.xx );
    float       coneAttenuation         = smoothstep( light.dotInnerAngle , light.dotOuterAngle , dotAngle );
    //return float2( coneAttenuation.xx );
    float       diffuseAttenuation      = light.intensity / dot( attenuationVector , light.attenuation ) * coneAttenuation;
    float       specularAttenuation     = light.intensity / dot( attenuationVector , light.specularAttenuation )* coneAttenuation;

    
   // compute diffuse
   // max prevents lighting from the "back", which would subtract light
    float       dotIncident             = dot( lightDirection , worldNormal );
   // return float2( dotIncident.xx );
    float       facingDirection         = smoothstep( -0.4f , 0.1f , dotIncident );
    float       diffuse                 = max( 0.0f , dotIncident );
      
   // BLINN-PHONG LIGHTING COMPUTATION
    float3      hv                      = normalize( -lightDirection + directionToEye );
    float       specular                = max( 0.0f , dot( normalize( worldNormal ) , hv ) );
    
    // SPECULAR LIGHTING
                specular                = facingDirection * specular;   
                specular                = SPECULAR_FACTOR * pow( specular , SPECULAR_POWER );
 
    return float2( diffuseAttenuation * diffuse , specularAttenuation * specular );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

float3 ComputeLightingAt( float3 worldPos , float3 worldNormal , float3 surfaceColor , float3 emmisiveColor , float specularFactor )
{
    float3 directionToEye           = normalize( CAMERA_POSITION - worldPos );
    //return directionToEye; - match found
    float3 diffuse                  = pow( AMBIENT.xyz * AMBIENT.w , GAMMA.xxx );                                       // assumes ambient is set from a user - so sRGB space
    // return diffuse; - match found
    float3 specular                 = float3( 0.0f.xxx );

   // add up contribution of all lights
    for( uint index = 0 ; index < TOTAL_LIGHTS ; index++ )
    {
        float3 lightColor           = LIGHTS[ index ].color.xyz;
        // return lightColor; - match found
        lightColor                  = pow( lightColor , GAMMA.xxx );                                                    // assumes light color is set by a user - so sRGB space
        // return lightColor; - match found
        float2 lightFactors         = ComputeLightFactor( LIGHTS[ index ] , worldPos , worldNormal , directionToEye );
        //return float3( lightFactors.xxx ); - match found
        //return float3( lightFactors.yyy ); - match found
        //return float3( lightFactors , 0 );
        diffuse                    += lightFactors.x * lightColor;
        specular                   += lightFactors.y * lightColor;
    }
   
   // limit it
    diffuse                         = min( DIFFUSE_FACTOR * diffuse , float3( 1.0f.xxx ) );
    specular                       *= specularFactor;                                                                   // scale back specular based on spec factor

   // returns light color (in linear space)
    return diffuse * surfaceColor + specular + emmisiveColor;
}