#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};

VertexOutput VS(float4 position : Position)
{
    VertexOutput output;
    
    output.Position = position;
    output.Uv.x = position.x * 0.5f + 0.5f;
    output.Uv.y = -position.y * 0.5f + 0.5f;
    
    return output;
}

struct PixelOutput
{
    float4 Color0 : SV_Target0;
    float4 Color1 : SV_Target1;
    float4 Color2 : SV_Target2;
};

float4 PS_Diffuse(VertexOutput input) : SV_Target
{
    return DiffuseMap.Sample(LinearSampler, input.Uv);
}
float2 PixelSize;

uint BlurCount = 8;
float4 PS_Blur(VertexOutput input) : SV_Target
{
    float2 arr[9] =
    {
        float2(-1, -1), float2(+0, -1), float2(+1, -1),
        float2(-1, +0), float2(+0, +0), float2(+1, +0),
        float2(-1, +1), float2(+0, +1), float2(+1, +1),
    };
    
    float3 color = 0;
    for (uint blur = 0; blur < BlurCount; blur++)
    {
        for (int i = 0; i < 9; i++)
        {
            float x = arr[i].x * PixelSize.x * (float) blur;
            float y = arr[i].y * PixelSize.y * (float) blur;
        
            float2 uv = input.Uv + float2(x, y);
            
            color += DiffuseMap.Sample(LinearSampler, uv).rgb;
        }
    }
    
    color /= BlurCount * 9;
    
    return float4(color, 1.0f);
}

//¼Óµµ°¨ blur
uint RadialBlurCount = 8;
float RadialBlurRadius = 0.0f;
float RadialBlurAmount = 0.04f;
float2 RadialCenter = float2(0.5f, 0.5f);

float4 PS_RadialBlur(VertexOutput input) : SV_Target
{
    float2 radius = input.Uv - RadialCenter;
    float r = length(radius);
    radius /= r;
    r = r * 2 / RadialBlurRadius;
    r = saturate(r);
    
    float2 delta = radius * r * r * RadialBlurAmount / RadialBlurCount;
    delta = -delta;

    float4 color = 0;
    
    for (uint i = 0; i < RadialBlurCount; i++)
    {
        color += DiffuseMap.Sample(LinearSampler, input.Uv);
        input.Uv += delta;
    }
    color /= RadialBlurCount;
    
    return float4(color.rgb, 1.0f);
    
}


technique11 T0
{
    P_VP(P0, VS, PS_Diffuse)
    P_VP(P1, VS, PS_Blur)
    P_VP(P2, VS, PS_RadialBlur)
}