#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

///////////////////////////////////////////////////////////////////////////////
// Mesh, Model, Animation
///////////////////////////////////////////////////////////////////////////////
float4 PS(MeshOutput input) : SV_Target
{
    float3 position = input.wvpPosition.xyz / input.wvpPosition.w; //NDC°ø°£ -1~1
    //float depth = position.z;
    
    float n = 0.1f;
    float f = 1000.0f;
    
    float depth = (2.0f * n) / (f + n - position.z * (f - n));
    return float4(depth, depth, depth, 1);
    
    
}



technique11 T0
{
    //Main Render
    P_VP(P0, VS_Mesh, PS)
    P_VP(P1, VS_Model, PS)
    P_VP(P2, VS_Animation, PS)
}