#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

///////////////////////////////////////////////////////////////////////////////
// Mesh, Model, Animation
///////////////////////////////////////////////////////////////////////////////
float4 PS(MeshOutput input) : SV_Target
{
    float4 color = PS_AllLight(input);
    
    float4 wvp = input.wvpPosition_Sub;
    
    float3 uvw = 0;
    uvw.x = wvp.x / wvp.w * 0.5f + 0.5f;    //0~1
    uvw.y = -wvp.y / wvp.w * 0.5f + 0.5f;   //0~1
    uvw.z = wvp.z / wvp.w * 0.5f + 0.5f;    //0~1
    
    [flatten]
    if (saturate(uvw.x) == uvw.x && saturate(uvw.y) == uvw.y && saturate(uvw.z) == uvw.z)
    {
        float4 map = ProjectorMap.Sample(LinearSampler, uvw.xy); // 투영되면 xy값만 유의미
        
        map.rgb += Projector.Color.rgb;
        color = lerp(color, map, map.a);
        
    } //화면 안에 있는것들을 찾아냄
    
    return color;
}



technique11 T0
{
    //Main Render
    P_RS_DSS_VP(P0, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, PS_Sky)
    P_VP(P1, VS_Mesh_Projector, PS)
    P_VP(P2, VS_Model_Projector, PS)
    P_VP(P3, VS_Animation_Projector, PS)
}