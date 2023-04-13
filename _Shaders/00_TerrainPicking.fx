cbuffer ConstantBuffer : register(b0)
{
    float4x4 worldViewProj;
};

SamplerState textureSampler;
Texture2D inputTexture;

float4 main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD) : SV_Target
{
    // Convert texture coordinates to pixel coordinates
    float2 pixelCoord = texCoord * float2(inputTexture.GetDimensions().xy);

    // Convert mouse position to pixel coordinates
    float2 mousePixelCoord = GetMousePosInPixelCoordSpace();

    // Calculate distance between pixel and mouse positions
    float dist = length(pixelCoord - mousePixelCoord);

    // Blend highlight color based on distance
    float4 highlightColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
    float blendFactor = saturate(10.0f - dist);
    float4 blendedColor = lerp(float4(inputTexture.Sample(textureSampler, texCoord)), highlightColor, blendFactor);

    return blendedColor;
}
