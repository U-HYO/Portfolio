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

float4 PS_Diffuse(VertexOutput input) : SV_Target
{
    return DiffuseMap.Sample(LinearSampler, input.Uv);
}

float4 PS_Inverse(VertexOutput input) : SV_Target
{
    return 1.0f - DiffuseMap.Sample(LinearSampler, input.Uv);
}

float4 PS_Grayscale(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    float average = (diffuse.r + diffuse.g + diffuse.b) / 3.0f;
    
    return float4(average, average, average, 1.0f);
}

float4 PS_Grayscale2(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    float3 grayscale = float3(0.2627f, 0.6780f, 0.0593f);
    float average = dot(diffuse.rgb, grayscale);
    
    return float4(average, average, average, 1.0f);
}


float3x3 ColorToSepiaMatrix = float3x3 //색상공간
(
    0.393, 0.769, 0.189, //R
    0.349, 0.686, 0.168, //G
    0.272, 0.534, 0.131  //B
);

float4 PS_Sepia(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    float3 result = mul(diffuse.rgb, ColorToSepiaMatrix);
    
    return float4(result, 1.0f);
}


//Saturation = 0 : grayscale
//0 < Saturation < 1 : saturation
//Saturation = 1 : original
//Saturation > 1 : desatuaration
float Saturation = 0;
float4 PS_Saturation(VertexOutput input) : SV_Target
{
    float3 grayscale = float3(0.2627f, 0.6780f, 0.0593f);
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    
    float temp = dot(diffuse.rgb, grayscale);
    
    //A : 0, B : 1
    //A + (B - A) * t
    diffuse.rgb = lerp(temp, diffuse.rgb, Saturation);
    diffuse.a = 1.0f;
    
    return diffuse;
}

float Sharpness = 0.0f;
float colorR = 0.0f;
float colorG = 0.0f;
float colorB = 0.0f;
float2 PixelSize;   
float4 PS_Sharpness(VertexOutput input) : SV_Target //외곽선
{
    float4 center = DiffuseMap.Sample(LinearSampler, input.Uv); // 기준픽셀
    float4 top = DiffuseMap.Sample(LinearSampler, input.Uv + float2(0, -PixelSize.y));      // 기준 위
    float4 bottom = DiffuseMap.Sample(LinearSampler, input.Uv + float2(0, +PixelSize.y));   //기준 아래 
    float4 left = DiffuseMap.Sample(LinearSampler, input.Uv + float2(-PixelSize.x, 0));     //기준 왼쪽 
    float4 right = DiffuseMap.Sample(LinearSampler, input.Uv + float2(+PixelSize.x, 0));    //기준 오른쪽

    
    //float4 edge = center * 4 - left - right - top - bottom;
    float4 edge = center * 2 - left - right;
    
    //return edge;
    
    //float3 grayscale = float3(1.0f, 1.0f, 1.0f);
    float3 grayscale = float3(0.2627f, 0.6780f, 0.0593f);
    //float average = dot(edge.rgb, grayscale);
    
    float average = dot(edge.rgb, grayscale);
    
    float3 edgeColor = float3(average * colorR, average * colorG, average * colorB);

    
    return center + Sharpness * float4(edgeColor, 1.0f);
    
}

float2 WiggleOffset = float2(10, 10);
float2 WiggleAmount = float2(0.01f, 0.01f);
float4 PS_Wiggle(VertexOutput input) : SV_Target
{
    float2 uv = input.Uv;
    uv.x += sin(Time + uv.x * WiggleOffset.x) * WiggleAmount.x;
    uv.y += cos(Time + uv.y * WiggleOffset.y) * WiggleAmount.y;
    
    return DiffuseMap.Sample(LinearSampler, uv);
}

float Power = 2;
float2 Scale = float2(2, 2);
float4 PS_Vignette(VertexOutput input) : SV_Target //
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    
    float radius = length((input.Uv - 0.5f) * 2 / Scale);
    float vignette = pow(abs(radius + 1e-6f), Power);
    
    return saturate(1 - vignette) * diffuse;
}

float Strength = 1.0f;
int InteraceValue = 2;
float4 PS_Interace(VertexOutput input) : SV_Target
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);
    
    float height = 1.0f / PixelSize.y;
    int value = (int) ((floor(input.Uv.y * height) % InteraceValue) / (InteraceValue / 2));
    
    [flatten]
    if (value)
    {
        float3 grayScale = float3(0.2126f, 0.7152f, 0.0722f);
        float luminance = dot(color.rgb, grayScale);
        
        luminance = min(0.999f, luminance);
        
        color.rgb = lerp(color.rgb, color.rgb * luminance, Strength);
    }
    
    return color;
}

float LensPower = 1;
float3 Distortion = -0.02f;
float4 PS_LensDistortion(VertexOutput input) : SV_Target //색상별 채널 분리
{
    float2 uv = input.Uv * 2 - 1;
    
    float2 vpSize = float2(1.0f / PixelSize.x, 1.0f / PixelSize.y);
    float aspect = vpSize.x / vpSize.y;
    float radiusSquared = aspect * aspect + uv.x * uv.x + uv.y * uv.y;
    float radius = sqrt(radiusSquared);

    float3 f = Distortion * pow(abs(radius + 0.0001f), LensPower) + 1;
    
    float2 r = (f.r * uv + 1) * 0.5f;
    float2 g = (f.g * uv + 1) * 0.5f;
    float2 b = (f.b * uv + 1) * 0.5f;
    
    float4 color = 0;
    color.r = DiffuseMap.Sample(LinearSampler, r).r;
    color.g = DiffuseMap.Sample(LinearSampler, g).g;
    color.b = DiffuseMap.Sample(LinearSampler, b).b;
    
    return color;
}

technique11 T0
{
    P_VP(P0, VS, PS_Diffuse)
    P_VP(P1, VS, PS_Inverse)
    P_VP(P2, VS, PS_Grayscale)
    P_VP(P3, VS, PS_Grayscale2)
    P_VP(P4, VS, PS_Sepia)
    P_VP(P5, VS, PS_Saturation)
    P_VP(P6, VS, PS_Sharpness)
    P_VP(P7, VS, PS_Wiggle)
    P_VP(P8, VS, PS_Vignette)
    P_VP(P9, VS, PS_Interace)
    P_VP(P10, VS, PS_LensDistortion)
}