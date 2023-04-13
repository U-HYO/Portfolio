//Texture2D<float4> worldPosTex : register(t0);
//Texture2D heightmap;
//struct VertexOutput
//{
//    float4 position : POSITIONT;
//    float2 texCoord : UV;
//};
//SamplerState heightmapSampler
//{
//    Filter = MIN_MAG_MIP_LINEAR;
//    AddressU = Wrap;
//    AddressV = Wrap;
//};

//float4 TerrainPickingVS(VertexOutput input) : SV_Target
//{
//    float height = heightmap.Sample(heightmapSampler, input.texCoord).r;
//    float4 worldPos = float4(input.texCoord * terrainSize, height, 1.0);
//    float4 worldPosView = mul(worldPos, worldViewProj);
//    return worldPosView;
//}

//float3 GetWorldPosition(float2 screenPos, float depth, float4x4 invProj, float4x4 invView)
//{
//    // Compute clip space position
//    float4 clipPos = float4(screenPos, depth, 1.0f);

//    // Compute normalized device coordinates (NDC) position
//    float4 ndcPos = mul(clipPos, invProj);

//    // Divide by w to get world-space position in homogeneous coordinates
//    float4 worldPosHomogeneous = mul(ndcPos, invView);
//    worldPosHomogeneous /= worldPosHomogeneous.w;

//    // Return world position
//    return worldPosHomogeneous.xyz;
//}

//float4 TerrainPickingPS(VertexOutput input) : SV_Target
//{
//    float4 worldPosView = TerrainPickingVS(input);

//    // Compute inverse view-projection matrix for screen-to-world conversion
//    float4x4 invViewProj = transpose(worldViewProj);
//    invViewProj = mul(invViewProj, transpose(projection));

//    // Compute screen position and depth from world position
//    float3 worldPos = GetWorldPosition(input.texCoord, worldPosView.z, invViewProj, inverse(worldView));

//    // Sample world position texture to get the exact world position
//    float4 worldPosTexValue = worldPosTex.Sample(worldPosTexSampler, worldPos.xy);
//    float4 worldPosViewTex = mul(worldPosTexValue, worldViewProj);
//    return worldPosViewTex;
//}


Texture2D<float4> worldPosTex : register(t0);

float4 TerrainPickingPS(VertexOutput input) : SV_Target
{
    float height = heightmap.Sample(heightmapSampler, input.texCoord).r;
    float4 worldPos = float4(input.texCoord * terrainSize, height, 1.0);
    float4 worldPosView = mul(worldPos, worldViewProj);

// Compute inverse view-projection matrix for screen-to-world conversion
    float4x4 invViewProj = inverse(mul(worldViewProj, projection));

// Compute screen position and depth from world position
    float3 worldPosViewUnprojected = worldPosView / worldPosView.w;
    float3 screenPos = mul(worldPosViewUnprojected, invViewProj).xy;
    float depth = worldPosViewUnprojected.z;

// Sample world position texture to get the exact world position
    float4 worldPosTexValue = worldPosTex.Sample(worldPosTexSampler, worldPos.xy / worldPos.w);
    float4 worldPosViewTex = mul(worldPosTexValue, worldViewProj);
    return worldPosViewTex;
}

float4 TerrainPickingPS(VertexOutput input) : SV_Target
{
    float4 worldPosView = TerrainPickingPS(input);

    // Compute inverse view-projection matrix for screen-to-world conversion
    float4x4 invViewProj = invert(mul(projection, worldView));

    // Compute screen position and depth from world position
    float4 worldPosViewUnprojected = float4(input.texCoord * terrainSize, worldPosView.z, 1.0);
    float3 screenPos = mul(worldPosViewUnprojected, invViewProj).xy;

    // Sample world position texture to get the exact world position
    float4 worldPosTexValue = worldPosTex.Sample(worldPosTexSampler, screenPos);
    float4 worldPosViewTex = mul(worldPosTexValue, worldViewProj);

    return worldPosViewTex;
}
