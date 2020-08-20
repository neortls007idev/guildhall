//--------------------------------------------------------------------------------------


RWTexture2D<float4> gOutput : register( u0 );



[numthreads(1, 1, 1)]
void ComputeFunction( uint3 DTid : SV_DispatchThreadID )
{
    float4 pixelColor = gOutput.Load( DTid );

    pixelColor.rgb *= 0.5f;

	
    

}

