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
    float Edge[4] : SV_TessFactor;  //간선
    float Inside[2] : SV_InsideTessFactor;   //중심점, 삼각형당 중심축 1개
};

float2 Distance; // x는 최소거리, y는 최대거리
CHullOutput CHS(InputPatch<VertexOutput, 4> input)
{
    float4 position = 0;
    
    for (int i = 0; i < 4; i++)
        position += input[i].Position;
    
    position *= 0.25f;
    position.w = 1.0f;
    
    float dist = distance(position.xyz, ViewPosition());
    float factor = saturate((Distance.y - dist) / (Distance.y - Distance.x)) * 64;
    //가까우면 1개, 멀면 최대 64개
    
    CHullOutput output;
    output.Edge[0] = factor;
    output.Edge[1] = factor;
    output.Edge[2] = factor;
    output.Edge[3] = factor;
    
    output.Inside[0] = factor;
    output.Inside[1] = factor;
    
    return output;    
}

struct HullOutput
{
    float4 Position : Position;
};

[domain("quad")]                 //어느모양으로 자르는지
[partitioning("integer")]
[outputtopology("triangle_cw")] //어느방향으로 넘겨줄지
[outputcontrolpoints(4)]        //몇개를 넘겨줄지
[patchconstantfunc("CHS")]      //Constant HullShadr Name

HullOutput HS(InputPatch<VertexOutput, 4> input, uint id : SV_OutputControlPointID)
{
    HullOutput output;
    output.Position = input[id].Position;
 
    return output;
}

struct DomainOutput
{
    float4 Position : SV_Position;
};

[domain("quad")]         //어느모양으로 받는지
DomainOutput DS(CHullOutput input, const OutputPatch<HullOutput, 4> patch, float2 uv : SV_DomainLocation)
{
    DomainOutput output;
    
    //최종 정점의 위치
    float4 v1 = lerp(patch[0].Position, patch[1].Position, 1 - uv.y); //계산의 편의를 위해 뒤집음
    float4 v2 = lerp(patch[2].Position, patch[3].Position, 1 - uv.y);
    float4 position = lerp(v1, v2, uv.x);
    
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