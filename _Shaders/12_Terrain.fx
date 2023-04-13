matrix World, View, Projection;
float3 LightDirection = float3(-1, -1, +1);

struct VertexInput
{
    float4 Position : Position;
    float3 Normal : Normal;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 Normal : Normal;
    float4 Color : Color;
};

float4 GetHeightColor(float y)
{
    if(y > 20.0f)
        return float4(1, 0, 0, 1);
    
    if (y > 10.0f)
        return float4(0, 1, 0, 1);
    
    if (y > 5.0f)
        return float4(0, 0, 1, 1);
    
    return float4(1, 1, 1, 1);
}

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    
    //GetHeightColor(input.Position.y); //Local
    output.Position = mul(input.Position, World);
    output.Color = GetHeightColor(output.Position.y);
    
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.Normal = mul(input.Normal, (float3x3) World);

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float NdotL = dot(normal, -LightDirection);
    
    return input.Color * NdotL;
}

RasterizerState RS
{
    FillMode = Wireframe;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(RS);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}