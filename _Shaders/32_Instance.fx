#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float4 PS(MeshOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float NdotL = dot(normal, -GlobalLight.Direction);
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    
    return diffuse * NdotL + input.Color;
}

technique11 T0
{
    P_VP(P0, VS_Mesh, PS)
    P_VP(P1, VS_Model, PS)
    P_VP(P2, VS_Animation, PS)
    P_RS_VP(P3, FillMode_WireFrame, VS_Model, PS)
}