#include "00_Global.fx"
#include "00_Light.fx"

cbuffer CB_Rain
{
    float4 Color;
    
    float3 Velocity;
    float DrawDistance;
    
    float3 Origin;
    float CB_Rain_Padding;
    
    float3 Extent;
};

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
    float2 Scale : Scale;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    float Alpha : Alpha; // 거리에 따른 투명도 조절
};

VertexOutput VS(VertexInput _input)
{
    VertexOutput output;
    
    float3 displace = Velocity;
    displace.xz /= _input.Scale.y * 0.1f;
    displace *= Time;
    
    _input.Position.xyz = Origin + (Extent + (_input.Position.xyz + displace) % Extent) % Extent - (Extent * 0.5f);
    //_input.Position.y -= Extent + Time * 10 % Extent;
    
    float4 position = _input.Position;
    
    float3 up = float3(0, 1, 0);
    float3 forward = normalize(position.xyz - ViewPosition());
    
    float3 right = normalize(cross(up, forward));
    
    position.xyz += (_input.Uv.x - 0.5f) * right * _input.Scale.x;
    position.xyz += (1.0f - _input.Uv.y - 0.5f) * up * _input.Scale.y;
    position.w = 1.0f;
    
    output.Position = WorldPosition(position);
    output.Position = ViewProjection(output.Position);
    
    output.Uv = _input.Uv;
    output.Alpha = 0;
    
    float4 view = mul(position, View);
    output.Alpha = saturate(1 - view.z / DrawDistance) * 0.5f;
    
    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    diffuse.rgb = Color.rgb * input.Alpha * 2.0f;
    diffuse.a = diffuse.a * input.Alpha * 1.5f;

    return diffuse;
}

technique11 T0
{
    //P_VP(P0, VS, PS)
    P_BS_VP(Alpha1, AlphaBlend, VS, PS)
    P_BS_VP(Alpha2, AlphaBlend_AlphaToCoverageEnable, VS, PS)
    P_BS_VP(Alpha3, AdditiveBlend, VS, PS)
    P_BS_VP(Alpha4, AdditiveBlend_AlphaToCoverageEnable, VS, PS)

}
