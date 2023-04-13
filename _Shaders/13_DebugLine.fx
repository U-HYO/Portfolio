#include "00_Global.fx"

struct VertexInput
{
    float4 Position : Position;
    float4 Color : Color;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float4 Color : Color;
};

VertexOutput VS(VertexInput _input)
{
    VertexOutput output;
    output.Position = WorldPosition(_input.Position);
    output.Position = ViewProjection(output.Position);
    
    output.Color = _input.Color;
    
    return output;   
}

float4 PS(VertexOutput _input) : SV_Target
{
    return _input.Color;
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

}