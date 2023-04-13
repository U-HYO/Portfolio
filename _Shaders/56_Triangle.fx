#include "00_Global.fx"

struct VertexOutput
{
    float4 Position : Position;
};

VertexOutput VS(VertexOutput input)
{
    VertexOutput output;
    output.Position = input.Position;

    return output;
}

struct CHullOutput
{
    float Edge[3] : SV_TessFactor;  //간선
    float Inside : SV_InsideTessFactor;   //중심점
};

uint Edge[3];
uint Inside;

CHullOutput CHS(InputPatch<VertexOutput, 3> input)
{
    CHullOutput output;
    output.Edge[0] = Edge[0];
    output.Edge[1] = Edge[1];
    output.Edge[2] = Edge[2];
    
    output.Inside = Inside;
    
    return output;    
}

struct HullOutput
{
    float4 Position : Position;
};

[domain("tri")]                 //어느모양으로 자르는지
[partitioning("integer")]
[outputtopology("triangle_cw")] //어느방향으로 넘겨줄지
[outputcontrolpoints(3)]        //몇개를 넘겨줄지
[patchconstantfunc("CHS")]      //Constant HullShadr Name

HullOutput HS(InputPatch<VertexOutput, 3> input, uint id : SV_OutputControlPointID)
{
    HullOutput output;
    output.Position = input[id].Position;
 
    return output;
}

struct DomainOutput
{
    float4 Position : SV_Position;
};

[domain("tri")]         //어느모양으로 받는지
DomainOutput DS(CHullOutput input, const OutputPatch<HullOutput, 3> patch, float3 uvw : SV_DomainLocation) // uvw : 자른 간선의 비율
{
    DomainOutput output;
    
    //최종 정점의 위치
    float4 position = uvw.x * patch[0].Position + uvw.y * patch[1].Position + uvw.z * patch[2].Position;
    output.Position = float4(position.xyz, 1);
    
    return output;
}



float4 PS(DomainOutput input) : SV_Target
{
    return float4(1, 0, 0, 1);
}

technique11 T0
{
    P_RS_VTP(P0, FillMode_WireFrame, VS, HS, DS, PS)
}