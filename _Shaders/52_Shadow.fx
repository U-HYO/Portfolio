#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

///////////////////////////////////////////////////////////////////////////////
// Mesh, Model, Animation
///////////////////////////////////////////////////////////////////////////////
float4 PS(MeshOutput input) : SV_Target
{
    //float4 color = float4(1, 1, 1, 1);
    float4 color = PS_AllLight(input);
    
    float4 position = input.sPosition; //Shadow공간에서 view Projection이 된 position
    position.xyz /= position.w;
    
    [flatten]
    if(position.x < -1.0f || position.x > 1.0f && 
        position.y < -1.0f || position.y > 1.0f &&
        position.z < 0.0f || position.z > 1.0f) //영역 안에 들어오는가
    {
        return color; //영역안에 없으면 흰색으로 
    }
    
    position.x = position.x * 0.5f + 0.5f;
    position.y = -position.y * 0.5f + 0.5f; //UV공간으로 변환
    
    float depth = 0.0f;
    float factor = 0;
    float z = position.z - Shadow.Bias;
    
    if(Shadow.Quality == 0)
    {
        depth = ShadowMap.Sample(LinearSampler, position.xy).r;
        factor = (float)(depth >= z);
    }
    else if (Shadow.Quality == 1)
    {
        depth = position.z;
        factor = ShadowMap.SampleCmp(ShadowPcfSampler, position.xy, depth).r;
    }
    else if (Shadow.Quality == 2)
    {//주변픽셀로 블러처리
        depth = position.z;
        
        float2 size = 1.0f / Shadow.MapSize;
        float2 offsets[] =
        {
            float2(-size.x, -size.y), float2(0.0f, -size.y), float2(+size.x, -size.y),
            float2(-size.x, 0.0f), float2(0.0f, 0.0f), float2(+size.x, 0.0f),
            float2(-size.x, +size.y), float2(0.0f, +size.y), float2(+size.x, +size.y),
        };
        
        
        float2 uv = 0;
        float sum = 0;
        
        [unroll(9)]
        for (int i = 0; i < 9; i++)
        {
            uv = position.xy + offsets[i];
            sum += ShadowMap.SampleCmpLevelZero(ShadowPcfSampler, uv, depth).r;
        }
        
        factor = sum / 9.0f;
    }
    
    factor = saturate(factor + depth);
    
    return float4(color.rgb * factor, 1.0f);
    
}

technique11 T0
{
    // 1Pass Depth Render
    // z-fighting ==> culling을 뒤집어서 해결(뒤집으면 깊이차이가 많이생김) 
    P_RS_VP(P0, FrontCounterClockwise_True, VS_Mesh_Depth, PS_Shadow_Depth)
    P_RS_VP(P1, FrontCounterClockwise_True, VS_Model_Depth, PS_Shadow_Depth)
    P_RS_VP(P2, FrontCounterClockwise_True, VS_Animation_Depth, PS_Shadow_Depth)


    //2Pass Main Render
    P_RS_DSS_VP(P3, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, PS_Sky)
    P_VP(P4, VS_Mesh, PS)
    P_VP(P5, VS_Model, PS)
    P_VP(P6, VS_Animation, PS)
}