cbuffer CB_PerFrame
{
    matrix View;
    matrix ViewInverse;
    matrix Projection;
    matrix VP;
    
    float4 Culling[4];
    float4 Clipping;
    
    float Time;
    matrix ProjectionInverse;
};

cbuffer CB_World
{
    matrix World;
};

cbuffer CB_Reflection
{
    matrix ReflectionView;
};


struct ShadowDesc
{
    matrix View;
    matrix Projection;
    
    float2 MapSize;
    float Bias;

    uint Quality;
};

cbuffer CB_Shadow
{
    ShadowDesc Shadow;    
};

SamplerComparisonState ShadowPcfSampler;

//===================================================

float4 WorldPosition(float4 _position)
{
    return mul(_position, World);
}

float4 ViewProjection(float4 _position)
{
    return mul(_position, VP);
}

float3 WorldNormal(float3 _normal)
{
    return mul(_normal, (float3x3) World);
}

float3 WorldTangent(float3 _tangent)
{
    return mul(_tangent, (float3x3) World);
}


float3 ViewPosition()
{
    return ViewInverse._41_42_43;
}

//===================================================
struct MeshOutput
{
    float4 Position : SV_Position; // Rasterizing position (-1~1) �����Ҽ�����
    float3 oPosition : Position1; // original Position ����������
    float3 wPosition : Position2; // world Position ���� ��ȯ 
    float4 wvpPosition : Position3; // WVP ��ȯ �Ϸ�(ȭ�鿵������ ��ȯ)
    float4 wvpPosition_Sub : Position4; // WVP SUB position
    float4 sPosition : Position5; // shadow position
    
    
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float2 Uv : Uv;
    float4 Color : Color;
    
    uint TargetIndex : SV_RenderTargetArrayIndex;
    
    float4 Culling : SV_CullDistance; // left, right, up, down �Ǵ�
    float4 Clipping : SV_ClipDistance; // ���ļ� �ִ� 2��(float8��)���� Cull+clip Cull2 Clip2
    
};

///////////////////////////////////////////////////////////////////////////////

cbuffer CB_Fog
{
    float4 FogColor;
    float2 FogDistance;
    float FogDensity;
    uint FogType;
};

float4 LinearFogBlend(float4 color, float3 wPosition)
{
    float dist = saturate((distance(wPosition, ViewPosition()) - FogDistance.x) / (FogDistance.y + FogDistance.x));

    return float4(lerp(color.rgb, FogColor.rgb, dist), 1); // �Ÿ� ������ ���� ���������� �Ȱ������� ����
}

float4 ExpFogBlend(float4 color, float3 wPosition)
{
    float dist = distance(wPosition, ViewPosition());
    dist = dist / FogDistance.y * FogDistance.x;
    
    float factor = exp(-dist * FogDensity); // �����Ÿ����� ���ϰ� �Ȱ�
    return float4(lerp(FogColor.rgb, color.rgb, factor), 1);
}

float4 Exp2FogBlend(float4 color, float3 wPosition)
{
    float dist = distance(wPosition, ViewPosition());
    dist = dist / FogDistance.y * FogDistance.x;
    
    float factor = exp(-(dist * FogDensity) * (dist * FogDensity)); // �Ȱ��� ���ʿ� ����
    return float4(lerp(FogColor.rgb, color.rgb, factor), 1);
}

float4 CalcualteFogColor(float4 color, float3 wPosition)
{
    if (FogType == 0)
        color = LinearFogBlend(color, wPosition);
    else if (FogType == 1)
        color = ExpFogBlend(color, wPosition);
    else if (FogType == 2)
        color = Exp2FogBlend(color, wPosition);
    
    return color;
}

///////////////////////////////////////////////////////////////////////////////


//===================================================

struct MeshDepthOutput
{
    float4 Position : SV_Position; // Rasterizing position (-1~1) �����Ҽ�����
    float4 sPosition : Position1; // Lighting Position;
};

//===================================================

struct Vertex
{
    float4 Position : Position;
};

struct VertexNormal
{
    float4 Position : Position;
    float3 Normal : Normal;
};

struct VertexColor
{
    float4 Position : Position;
    float4 Color : Color;
};

struct VertexColorNormal
{
    float4 Position : Position;
    float4 Color : Color;
    float3 Noraml : Normal;
};

struct VertexTexture
{
    float4 Position : Position;
    float2 UV : UV;
};

struct VertexTextureNormal
{
    float4 Position : Position;
    float2 UV : UV;
    float3 Normal : Normal;
};

//===================================================


SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

RasterizerState FillMode_WireFrame
{
    FillMode = Wireframe;
};

RasterizerState CullMode_None
{
    CullMode = None;
};

RasterizerState FrontCounterClockwise_True
{
    FrontCounterClockwise = true;
};

DepthStencilState DepthEnable_False
{
    DepthEnable = false;
};

BlendState AlphaBlend //������
{
    BlendEnable[0] = true;
    SrcBlend[0] = SRC_ALPHA;        // �׷��� ���
    DestBlend[0] = INV_SRC_ALPHA;   // �׷��� ���
    BlendOp[0] = ADD;

    SrcBlendAlpha[0] = ONE;
    DestBlendAlpha[0] = ZERO;
    BlendOpAlpha[0] = ADD;

    RenderTargetWriteMask[0] = 0x0F; //15 / 0�̸� �ȵ� 15�� ���� ���
};   

BlendState AlphaBlend_AlphaToCoverageEnable 
{
    AlphaToCoverageEnable = true;

    BlendEnable[0] = true;
    SrcBlend[0] = SRC_ALPHA; 
    DestBlend[0] = INV_SRC_ALPHA;
    BlendOp[0] = ADD;

    SrcBlendAlpha[0] = ONE;
    DestBlendAlpha[0] = ZERO;
    BlendOpAlpha[0] = ADD;

    RenderTargetWriteMask[0] = 0x0F;
};

BlendState AdditiveBlend
{
    BlendEnable[0] = true;
    SrcBlend[0] = SRC_ALPHA;
    DestBlend[0] = ONE;
    BlendOp[0] = ADD;

    SrcBlendAlpha[0] = ONE;
    DestBlendAlpha[0] = ZERO;
    BlendOpAlpha[0] = ADD;

    RenderTargetWriteMask[0] = 0x0F;
};

BlendState AdditiveBlend_AlphaToCoverageEnable
{
    AlphaToCoverageEnable = true;

    BlendEnable[0] = true;
    SrcBlend[0] = SRC_ALPHA;
    DestBlend[0] = ONE;
    BlendOp[0] = ADD;

    SrcBlendAlpha[0] = ONE;
    DestBlendAlpha[0] = ZERO;
    BlendOpAlpha[0] = ADD;

    RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState PixelDepthEnable_False    //�ȼ��� ���� ����
{
    DepthEnable = true;
    DepthWritemask = Zero;
    DepthFunc = LESS_EQUAL;
};

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Vertex / Pixel
///////////////////////////////////////////////////////////////////////////////
#define P_VP(name, vs, ps) \
pass name \
{ \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_VP(name, rs, vs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_BS_VP(name, bs, vs, ps) \
pass name \
{ \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_VP(name, dss, vs, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_VP(name, rs, dss, vs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_BS_VP(name, rs, bs, vs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_BS_VP(name, dss, bs, vs, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

///////////////////////////////////////////////////////////////////////////////
// Vertex / Geometry / Pixel
///////////////////////////////////////////////////////////////////////////////
#define P_VGP(name, vs, gs, ps) \
pass name \
{ \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_VGP(name, rs, vs, gs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_BS_VGP(name, bs, vs, gs, ps) \
pass name \
{ \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_VGP(name, dss, vs, gs, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_VGP(name, rs, dss, vs, gs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_BS_VGP(name, rs, bs, vs, gs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_BS_VGP(name, dss, bs, vs, gs, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}


///////////////////////////////////////////////////////////////////////////////
// Vertex / Tessellation / Pixel
///////////////////////////////////////////////////////////////////////////////
#define P_VTP(name, vs, hs, ds, ps) \
pass name \
{ \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_VTP(name, rs, vs, hs, ds, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_BS_VTP(name, bs, vs, hs, ds, ps) \
pass name \
{ \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_VTP(name, dss, vs, hs, ds, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_VTP(name, rs, dss, vs, hs, ds, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 1); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_BS_VTP(name, rs, bs, vs, hs, ds, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_BS_VTP(name, dss, bs, vs, hs, ds, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 1); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_BS_VTP(name, rs, dss, bs, vs, hs, ds, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 1); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetHullShader(CompileShader(hs_5_0, hs())); \
    SetDomainShader(CompileShader(ds_5_0, ds())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}