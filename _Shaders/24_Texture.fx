Texture2DArray<float4> Input;
RWTexture2DArray<float4> Output;

[numthreads(32, 32, 1)]
void CS(uint3 id : SV_DispatchThreadID)
{
    float4 pixel = Input.Load(int4(id, 0));
    
    Output[id] = float4(pixel.r, 0, 0, 1);
}

[numthreads(32, 32, 1)]
void CS2(uint3 id : SV_DispatchThreadID)
{
    float4 pixel = Input.Load(int4(id, 0));
    
    Output[id] = 1 - pixel;
}

[numthreads(32, 32, 1)]
void CS3(uint3 id : SV_DispatchThreadID)
{
    float4 pixel = Input.Load(int4(id, 0));
    
    Output[id] = (pixel.r + pixel.g + pixel.b) / 3.0f;
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS()));
    }

    pass P1
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS2()));
    }

    pass P2
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS3()));
    }
}
