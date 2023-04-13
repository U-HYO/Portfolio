#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float4 PS(MeshOutput input) : SV_Target
{
    return PS_Shadow(input.sPosition, PS_AllLight(input));
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