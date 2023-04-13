#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

uint Selected = 0;
float4 PS(MeshOutput input) : SV_Target
{
    Material.Diffuse = float4(1, 1, 1, 1);
    
    if(Selected == 0)
    {
        
    }
    else if (Selected == 1)
    {
        NormalMapping(input.Uv, input.Normal, input.Tangent);
    }
    else if (Selected == 2)
    {
        Texture(Material.Diffuse, DiffuseMap, input.Uv);
    }
    else if (Selected == 3)
    {
        NormalMapping(input.Uv, input.Normal, input.Tangent);
        Texture(Material.Diffuse, DiffuseMap, input.Uv);
    }
    
    //return Material.Diffuse;
    
    Texture(Material.Specular, SpecularMap, input.Uv);
    
    MaterialDesc output;
    ComputeLight(output, input.Normal, input.wPosition);
    
    return float4(MaterialToColor(output), 1.0f);
}

technique11 T0
{
    P_VP(P0, VS_Mesh, PS)
    P_VP(P1, VS_Model, PS)
    P_VP(P2, VS_Animation, PS)
    P_RS_VP(P3, FillMode_WireFrame, VS_Model, PS)
}