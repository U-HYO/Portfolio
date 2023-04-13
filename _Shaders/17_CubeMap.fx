#include "00_Global.fx"

TextureCube CubeMap;

struct VertexInput
{
    float4 Position : Position;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 oPosition : Position1;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    
    output.oPosition = input.Position.xyz;
    
    input.Position.x += cos(Time * 3.0f) * 2.0f;
    input.Position.y += sin(Time * 3.0f);
    
    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    float4 diffuse = CubeMap.Sample(LinearSampler, input.oPosition);
    
    float4 color = float4(1, 0, 0, 1) * (sin(Time * 10.0f) * 0.5f + 0.5f);
    
    return diffuse * color;
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