Texture2D HeightMap;
Texture2D BaseMap; // 기본 지면 색상
Texture2D Layer1AlphaMap;
Texture2D Layer1ColorMap;

float screenPosX = -1;
float screenPosY = -1;

float pickingPointx = 0;
float pickingPointy = 1;
float pickingPointz = 1;

struct VertexTerrain
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float4 Color : Color;
    float3 Tangent : Tangent;
};

///////////////////////////////////////////////////////////////////////////////
// Brush
///////////////////////////////////////////////////////////////////////////////
struct BrushDesc
{
    float4 Color;
    float3 Location;
    uint Type;
    uint Range;

    float2 mousePos : MOUSEPOS;
    float2 textureSize : TEXURESIZE;
};

cbuffer CB_TerrainBrush
{
    BrushDesc TerrainBrush;
};



float4 GetBrushColor(float3 wPosition)
{
    [flatten]
    if (TerrainBrush.Type == 1)
    {
        [flatten]
        if ((wPosition.x >= (TerrainBrush.Location.x - TerrainBrush.Range)) &&
            (wPosition.x <= (TerrainBrush.Location.x + TerrainBrush.Range)) &&
            (wPosition.z >= (TerrainBrush.Location.z - TerrainBrush.Range)) &&
            (wPosition.z <= (TerrainBrush.Location.z + TerrainBrush.Range)))
        {
            return TerrainBrush.Color;
        }
    }
    
    [flatten]
    if (TerrainBrush.Type == 2)
    {
        float dx = wPosition.x - TerrainBrush.Location.x;
        float dz = wPosition.z - TerrainBrush.Location.z;
        float dist = sqrt(dx * dx + dz * dz);
        
        [flatten]
        if (dist <= (float) TerrainBrush.Range)
            return TerrainBrush.Color;
    }
    
    return float4(0, 0, 0, 0);
}



/////////////////////////////////////////////////////////////////////////////
// Line
///////////////////////////////////////////////////////////////////////////////
struct LineDesc
{
    float4 Color;
    uint Visible;
    float Thickness;
    float Size;
};

cbuffer CB_TerrainLine
{
    LineDesc TerrainLine;
};

//float4 GetLineColor(float3 wPosition)
//{
//    //Orthographic은 문제가없지만 Perspective는 축소해서 그리기때문에 왜곡이생김
//    [flatten]
//    if(TerrainLine.Visible < 1)
//        return float4(0, 0, 0, 0);
//  
//    float2 grid = wPosition.xz / TerrainLine.Size;
//    grid = frac(grid);
//  
//    float thick = TerrainLine.Thickness / TerrainLine.Size;
//  
// 
//    [flatten]
//    if(grid.x < thick || grid.y < thick)
//        return TerrainLine.Color;
//  
//    return float4(0, 0, 0, 0);
//}

float4 GetLineColor(float3 wPosition)
{
    [flatten]
    if (TerrainLine.Visible < 1)
        return float4(0, 0, 0, 0);
    
    float2 grid = wPosition.xz / TerrainLine.Size;
    float2 range = frac(grid);
    
    //return float4(range, 0, 1);
    
    float2 speed = fwidth(grid);
    //return float4(speed, 0, 1) * 3;
    
    float2 pixel = range / speed;
    //return float4(pixel, 0, 1);
    
    float thick = saturate(min(pixel.x, pixel.y) - TerrainLine.Thickness);
    
    return lerp(TerrainLine.Color, float4(0, 0, 0, 0), thick);
}


///////////////////////////////////////////////////////////////////////////////
// Shader
///////////////////////////////////////////////////////////////////////////////
MeshOutput VS_Terrain(VertexTerrain input)
{
    MeshOutput output = (MeshOutput) 0;
    
    VS_GENERATE
    
    //output.Color = GetBrushColor(output.wPosition);
    return output;
}

MeshDepthOutput VS_Terrain_Depth(VertexTerrain input)
{
    MeshDepthOutput output;
    
    VS_DEPTH_GENERATE
    
    return output;
}

MeshOutput VS_Terrain_Projector(VertexTerrain input)
{
    MeshOutput output = VS_Terrain(input);
    VS_Projector(output.wvpPosition_Sub, input.Position);
    
    return output;
}

MeshOutput VS_Terrain_Reflection(VertexTerrain input)
{
    MeshOutput output = (MeshOutput) 0;
    
    VS_REFLECTION_GENERATE
    
    return output;
}

///////////////////////////////////////////////////////////////////////////
struct TerrainLODDesc
{
    float2 Distance;
    float2 TessFactor;
    
    float Cellspacing;
    float CellspacingU;
    float CellspacingV;
    float HeightScale;
    
    float4 Culling[6];
};

cbuffer CB_TerrainLOD
{
    TerrainLODDesc TerrainLOD;
};

struct VertexInput_TerrainLOD
{
    float4 Position : Position;
    float2 Uv : Uv;
    float2 Bound : Bound;
};

struct VertexOutput_TerrainLOD
{
    float4 Position : Position;
    float2 Uv : Uv;
    float2 Bound : Bound;
};

VertexOutput_TerrainLOD VS_TerrainLOD(VertexOutput_TerrainLOD input)
{
    VertexOutput_TerrainLOD output;
    output.Position = input.Position;
    output.Uv = input.Uv;
    output.Bound = input.Bound;
    
    return output;
}

//MeshOutput VS_TerrainLOD_Reflection(VertexOutput_TerrainLOD input)
//{
//    VertexOutput_TerrainLOD output;
//    output.Position = input.Position;
//    output.Uv = input.Uv;
//    output.Bound = input.Bound;
    
//    return output;
//}



struct CHullOutput
{
    float Edge[4] : SV_TessFactor; //간선
    float Inside[2] : SV_InsideTessFactor; //중심점, 삼각형당 중심축 1개
};

float CalcTessFactor(float3 position)
{
    float d = distance(position, ViewPosition());
    float s = saturate((d - TerrainLOD.Distance.y) / (TerrainLOD.Distance.x - TerrainLOD.Distance.y));

    return lerp(TerrainLOD.TessFactor.x, TerrainLOD.TessFactor.y, s);
}

bool OutFrustumPlane(float3 center, float3 extent, float4 plane)
{
    float3 n = abs(plane.xyz);
    float r = dot(extent, n);
    float s = dot(float4(center, 1), plane);
    
    return (s + r) < 0.0f;
}

bool OutFrustum(float3 center, float3 extent)
{
    [unroll(6)]
    for (int i = 0; i < 6; i++)
    {
        [flatten]
        if (OutFrustumPlane(center, extent, TerrainLOD.Culling[i]))
            return true;
    }
    return false;
}

float2 Distance; // x는 최소거리, y는 최대거리
CHullOutput CHS(InputPatch<VertexOutput_TerrainLOD, 4> input)
{
    float4 position[4];
    position[0] = WorldPosition(input[0].Position);
    position[1] = WorldPosition(input[1].Position);
    position[2] = WorldPosition(input[2].Position);
    position[3] = WorldPosition(input[3].Position);
    
    float minY = input[0].Bound.x;
    float maxY = input[0].Bound.y;
    
    float3 minBox = float3(position[2].x, minY, position[2].z);
    float3 maxBox = float3(position[1].x, maxY, position[1].z);
    
    float3 boxCenter = (minBox + maxBox) * 0.5f;
    float3 boxExtent = abs(maxBox - minBox) * 0.5f;
    
    CHullOutput output;
    
    [flatten]
    if (OutFrustum(boxCenter, boxExtent))
    {
        output.Edge[0] = -1;
        output.Edge[1] = -1;
        output.Edge[2] = -1;
        output.Edge[3] = -1;
        
        output.Inside[0] = -1;
        output.Inside[1] = -1;
        
        return output;
    }
    
    float3 e0 = (position[0] + position[2]).xyz * 0.5f;
    float3 e1 = (position[0] + position[1]).xyz * 0.5f;
    float3 e2 = (position[1] + position[3]).xyz * 0.5f;
    float3 e3 = (position[2] + position[3]).xyz * 0.5f;
    
    float3 center = (position[0].xyz + position[1].xyz + position[2].xyz + position[3].xyz) * 0.25f;

    output.Edge[0] = CalcTessFactor(e0);
    output.Edge[1] = CalcTessFactor(e1);
    output.Edge[2] = CalcTessFactor(e2);
    output.Edge[3] = CalcTessFactor(e3);
    
    output.Inside[0] = CalcTessFactor(center);
    output.Inside[1] = CalcTessFactor(center);
    
    return output;
}

struct HullOutput
{
    float4 Position : Position;
    float2 Uv : Uv;
};

[domain("quad")] //어느모양으로 자르는지
//[partitioning("integer")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")] //어느방향으로 넘겨줄지
[outputcontrolpoints(4)] //몇개를 넘겨줄지
[patchconstantfunc("CHS")] //Constant HullShadr Name

HullOutput HS(InputPatch<VertexOutput_TerrainLOD, 4> input, uint id : SV_OutputControlPointID)
{
    HullOutput output;
    output.Position = input[id].Position;
    output.Uv = input[id].Uv;
    
    return output;
}

struct DomainOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};

[domain("quad")] //어느모양으로 받는지
DomainOutput DS(CHullOutput input, const OutputPatch<HullOutput, 4> patch, float2 uv : SV_DomainLocation)
{
    DomainOutput output;
    
    //최종 정점의 위치
    float4 v1 = lerp(patch[0].Position, patch[1].Position, uv.x); //계산의 편의를 위해 뒤집음
    float4 v2 = lerp(patch[2].Position, patch[3].Position, uv.x);
    float4 position = lerp(v1, v2, uv.y);
    
    float2 uv1 = lerp(patch[0].Uv, patch[1].Uv, uv.x); //계산의 편의를 위해 뒤집음
    float2 uv2 = lerp(patch[2].Uv, patch[3].Uv, uv.x);
    float2 texCoord = lerp(uv1, uv2, uv.y);
    
    position.y = HeightMap.SampleLevel(LinearSampler, texCoord, 0).r * 255.0f / TerrainLOD.HeightScale;
    
    output.Position = float4(position.xyz, 1);
    output.Position = WorldPosition(output.Position);
    output.Position = ViewProjection(output.Position);
    
    output.Uv = texCoord;
    
    
    return output;
}



float4 PS_TerrainLOD(DomainOutput input) : SV_Target
{
    float height = HeightMap.Sample(LinearSampler, input.Uv);
    //return float4(height, height, height, height);
    
    return BaseMap.Sample(LinearSampler, input.Uv);
    float2 left = input.Uv + float2(-TerrainLOD.CellspacingU, 0.0f);
    float2 right = input.Uv + float2(+TerrainLOD.CellspacingU, 0.0f);
    float2 top = input.Uv + float2(0.0f, -TerrainLOD.CellspacingV);
    float2 bottom = input.Uv + float2(0.0f, +TerrainLOD.CellspacingV);

    float leftY = HeightMap.Sample(LinearSampler, left).r * 255.0f / TerrainLOD.HeightScale; // 왼쪽 정점의 높이
    float rightY = HeightMap.Sample(LinearSampler, right).r * 255.0f / TerrainLOD.HeightScale;
    float topY = HeightMap.Sample(LinearSampler, top).r * 255.0f / TerrainLOD.HeightScale;
    float bottomY = HeightMap.Sample(LinearSampler, bottom).r * 255.0f / TerrainLOD.HeightScale;
    
    float3 tangent = normalize(float3(TerrainLOD.Cellspacing * 2.0f, rightY - leftY, 0.0f));
    float3 biTangent = normalize(float3(0.0f, bottomY - topY, TerrainLOD.Cellspacing * -2.0f));
    float3 normal = normalize(cross(tangent, biTangent));

    return float4(normal, 1);
    
}
///////////////////////////////////////////////////////////////////////////

Texture2D worldPosTex;

//float4 TerrainPickingPS(MeshOutput input) : SV_Target
//{
//    float height = BaseMap.Sample(LinearSampler, input.Uv).r;
    
//    uint width, right;
//    BaseMap.GetDimensions(width, right);

//    float2 textureSize = float2(width, right);
//    float4 worldPos = float4(input.Uv * textureSize, height, 1.0);
//    matrix wvp = World * View * Projection;
//    float4 worldPosView = mul(worldPos, wvp);

//    // Compute inverse view-projection matrix for screen-to-world conversion
//    float4x4 invViewProj = mul(transpose(View), transpose(Projection));

//    // Compute screen position from world position
//    float4 worldPosViewUnprojected = worldPosView / worldPosView.w;
//    float3 screenPos = mul(worldPosViewUnprojected, invViewProj).xy;

//    // Sample world position texture to get the exact world position
//    float4 worldPosTexValue = worldPosTex.Sample(LinearSampler, worldPos.xy / worldPos.w);
//    float4 worldPosViewTex = mul(worldPosTexValue, wvp);
//    return worldPosViewTex;
//}



float4 PS_Terrain(MeshOutput input) : SV_Target
{
    float height = BaseMap.Sample(LinearSampler, input.Uv).r;
    
    uint width, right;
    BaseMap.GetDimensions(width, right);

    float2 textureSize = float2(width, right);
    float4 worldPos = float4(input.Uv * textureSize, height, 1.0);
    matrix wvp = World * View * Projection;
    float4 worldPosView = mul(worldPos, wvp);

    // Compute inverse view-projection matrix for screen-to-world conversion
    float4x4 invViewProj = mul(transpose(View), transpose(Projection));

    // Compute screen position from world position
    float4 worldPosViewUnprojected = worldPosView / worldPosView.w;
    float3 screenPos = mul(worldPosViewUnprojected, invViewProj);

    // Sample world position texture to get the exact world position
    float4 worldPosTexValue = worldPosTex.Sample(LinearSampler, worldPos.xy / worldPos.w);
    float4 worldPosViewTex = mul(worldPosTexValue, wvp);
    //return worldPosViewTex;
    TerrainBrush.Location = worldPos.xyz;
    
    
    
    float4 color = BaseMap.Sample(LinearSampler, input.Uv);
    
    float alphaMap = Layer1AlphaMap.Sample(LinearSampler, input.Uv).r;
    float4 colorMap = Layer1ColorMap.Sample(LinearSampler, input.Uv);
   
    if (alphaMap > 0.0f)
        color = lerp(color, colorMap, alphaMap);
   
    Material.Diffuse = color;
    color = PS_Shadow(input.sPosition, PS_AllLight(input));
    color = CalcualteFogColor(PS_Shadow(input.sPosition, PS_AllLight(input)), input.wPosition);
   
    color += GetBrushColor(input.wPosition);
    color += GetLineColor(input.wPosition);
    
   // color = GetLineColor(input.wPosition);
    color += input.Color;
    
    
    return color;
    
}



///////////////////////////////////////////////////////////////////////////


//float4 PS_Terrain(MeshOutput input) : SV_Target
//{
//    float4 color = BaseMap.Sample(LinearSampler, input.Uv);
    
//    float alphaMap = Layer1AlphaMap.Sample(LinearSampler, input.Uv).r;
//    float4 colorMap = Layer1ColorMap.Sample(LinearSampler, input.Uv);
   
//    if (alphaMap > 0.0f)
//        color = lerp(color, colorMap, alphaMap);
   
//    Material.Diffuse = color;
//    color = PS_Shadow(input.sPosition, PS_AllLight(input));
//    color = CalcualteFogColor(PS_Shadow(input.sPosition, PS_AllLight(input)), input.wPosition);
   
//    /////////////////////////////////////////////////////////////
   
//    float2 texCoord = input.Uv;
//    float4 color2 = BaseMap.Sample(LinearSampler, input.Uv);
    
//    uint width, height;
//    BaseMap.GetDimensions(width, height);

//    float2 textureSize = float2(width, height);
//    float2 mousePos = TerrainBrush.mousePos / textureSize;
//    float dist = length(texCoord - mousePos);

//    float4 highlightColor = float4(1.0, 1.0, 0.0, 1.0); // yellow
//    float blendFactor = saturate(10.0f - dist);

//    float4 blendedColor = lerp(color2, highlightColor, blendFactor);
    
//    ////////////////////////////////////////////////////////////////
    
//    //color += finalColor;
//    color += GetBrushColor(input.wPosition);
//    color += GetLineColor(input.wPosition);
    
//   // color = GetLineColor(input.wPosition);
//    color += input.Color;
    
//    blendedColor += GetBrushColor(input.wPosition);
    
//    return blendedColor;
//}
