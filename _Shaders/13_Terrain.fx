#include "00_Global.fx"

float3 LightDirection = float3(-1, -1, +1);
Texture2D BaseMap;
uint Selected;

struct VertexInput
{
    float4 Position : Position;
    float3 Normal : Normal;
    float2 Uv : Uv;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 Normal : Normal;
    float2 Uv : Uv;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    
    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);
    
    output.Normal = WorldNormal(input.Normal);
    output.Uv = input.Uv;

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float NdotL = dot(normal, -LightDirection);
    
    float4 diffuse = BaseMap.Sample(LinearSampler, input.Uv);
    
    if (Selected == 0)
        return diffuse;
    
    return diffuse * NdotL;
}

technique11 T0
{
    P_VP(P0, VS, PS)
    P_RS_VP(P1, FillMode_WireFrame, VS, PS)

    //pass P0
    //{
    //    SetVertexShader(CompileShader(vs_5_0, VS()));
    //    SetPixelShader(CompileShader(ps_5_0, PS()));
    //}

    //pass P1
    //{
    //    SetRasterizerState(RS);

    //    SetVertexShader(CompileShader(vs_5_0, VS()));
    //    SetPixelShader(CompileShader(ps_5_0, PS()));
    //}
}