#include "00_Global.fx"
#include "00_Render.fx"
TextureCube SkyCubeMap;

//struct VertexOutput
//{
//    float4 Position : SV_Position;
//    float3 oPosition : Position1; //original Position
//};

//VertexOutput VS(Vertex _input)
//{
//    VertexOutput output;
    
//    output.oPosition = _input.Position.xyz;
//    output.Position = WorldPosition(_input.Position);
//    output.Position = ViewProjection(output.Position);
        
//    return output;
//}

//float4 PS(VertexOutput _input) : SV_Target
//{
//    return SkyCubeMap.Sample(LinearSampler, _input.oPosition);
//}

float4 PS(MeshOutput _input) : SV_Target
{
    return SkyCubeMap.Sample(LinearSampler, _input.oPosition);
}

technique11 T0
{

    //P_RS_DSS_VP(P0, FrontCounterClockwise_True, DepthEnable_False, VS, PS)
    P_RS_DSS_VP(P0, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, PS)
    
}