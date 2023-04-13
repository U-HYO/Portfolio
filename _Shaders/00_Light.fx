struct LightDesc
{
    float4 Ambient;
    float4 Specular;
    float3 Direction;
    float Padding;
    float3 Position;
};

cbuffer CB_Light
{
    LightDesc GlobalLight;
};

Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;
Texture2D ProjectorMap;
Texture2D ShadowMap; // DSV(����)�� ���� SRV

Texture2D RefractionMap;
Texture2D ReflectionMap;

TextureCube EnvCubeMap;
TextureCube SkyCubeMap;

struct MaterialDesc
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
};

float3 MaterialToColor(MaterialDesc result)
{
    return float4(result.Ambient + result.Diffuse + result.Specular + result.Emissive).rgb;
}

void AddMaterial(inout MaterialDesc result, MaterialDesc val)
{
    result.Ambient += val.Ambient;
    result.Diffuse += val.Diffuse;
    result.Emissive += val.Emissive;
    result.Specular += val.Specular;
    
}

cbuffer CB_Material
{
    MaterialDesc Material;
};

///////////////////////////////////////////////////////////////////////

void Texture(inout float4 color, Texture2D t, float2 uv, SamplerState samp)
{
    float4 sampling = t.Sample(samp, uv);
    
    [flatten]
    if (any(sampling.rgb))
        color = color * sampling;
}

void Texture(inout float4 color, Texture2D t, float2 uv)
{
    Texture(color, t, uv, LinearSampler);
}

///////////////////////////////////////////////////////////////////////

void ComputeLight(out MaterialDesc output, float3 normal, float3 wPosition)
{
    output = (MaterialDesc) 0;
    
    output.Ambient = GlobalLight.Ambient * Material.Ambient; // Calc Ambient
    
    
    float3 direction = -GlobalLight.Direction;
    float NdotL = dot(normalize(normal), direction);
    
    float3 E = normalize(ViewPosition() - wPosition);
    
    [flatten]
    if(NdotL > 0.0f)
    {
        output.Diffuse = Material.Diffuse * NdotL; // Calc Diffuse
        
        [flatten]
        if(Material.Specular.a > 0.0f) //���� ������ �ƴ� ����
        {
            float3 E = normalize(ViewPosition() - wPosition);
            
            float3 R = normalize(reflect(-direction, normal));
            float RdotE = saturate(dot(R, E));
            
            float specular = pow(RdotE, Material.Specular.a);
            output.Specular = Material.Specular * specular * GlobalLight.Specular;
        }
    }

    //Emssive�� ��ο� �κп��� �����ϱ� ������ NdotL�� ������ ���� �ʴ´�.
    [flatten]
    if(Material.Emissive.a > 0.0f)
    {
        float NdotE = dot(normalize(normal), E);
        float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));

        output.Emissive = Material.Emissive * emissive; // calc Emissive
    }
}

///////////////////////////////////////////////////////////////////////

void NormalMapping(float2 uv, float3 normal, float3 tangent, SamplerState samp)
{
    float3 map = NormalMap.Sample(samp, uv).rgb; // uv rasterizing, pixel samp
    
    [flatten]
    if (any(map.rgb) == false)
        return;
    
    //��ָ� ���ø���� --> ��ֺ��ͷ� ��ȯ
    float3 coord = map.rgb * 2.0f - 1.0f;
    
    //ź��Ʈ(�̹���) ���� ����
    float3 N = normalize(normal); //Z
    float3 T = normalize(tangent - dot(tangent, N) * N); //X
    float3 B = cross(N, T); // Y / T�� ���������� BiNormal
    float3x3 TBN = float3x3(T, B, N);
    
    coord = mul(coord, TBN); // Image Space -> 3D Space
    
    Material.Diffuse *= saturate(dot(coord, -GlobalLight.Direction));
    
}

void NormalMapping(float2 uv, float3 normal, float3 tangent)
{
    NormalMapping(uv, normal, tangent, LinearSampler);
}

////////////////////////////////////////////////////////////////////////

#define MAX_POINT_LIGHTS 256

struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
    
    float3 Position;
    float Range;
    
    float Intensity;
    float3 Padding;
};

cbuffer CB_PointLights
{
    uint PointLightCount;
    float3 CB_PointLights_Padding;
    
    PointLight PointLights[MAX_POINT_LIGHTS];
};


void ComputePointLight(out MaterialDesc output, float3 normal, float3 wPosition)
{
    output = (MaterialDesc) 0;
    MaterialDesc result = (MaterialDesc) 0;
    
    for (uint i = 0; i < PointLightCount; i++)
    {
        float3 light = PointLights[i].Position - wPosition;
        float dist = length(light);
        
        [flatten]
        if (dist > PointLights[i].Range)
            continue; //�Ÿ��Ǵ�
        
        light /= dist;
        
        float NdotL = dot(light, normalize(normal));
        float3 E = normalize(ViewPosition() - wPosition);
        
        result.Ambient = PointLights[i].Ambient * Material.Ambient; // Calc Ambient
    
    
        [flatten]
        if (NdotL > 0.0f)
        {
            result.Diffuse = Material.Diffuse * NdotL * PointLights[i].Diffuse; // Calc Diffuse
        
            [flatten]
            if (Material.Specular.a > 0.0f) //���� ������ �ƴ� ����
            {
                float3 R = normalize(reflect(-light, normal));
                float RdotE = saturate(dot(R, E));
            
                float specular = pow(RdotE, Material.Specular.a);
                result.Specular = Material.Specular * specular * PointLights[i].Specular;
            }
        }

    //Emssive�� ��ο� �κп��� �����ϱ� ������ NdotL�� ������ ���� �ʴ´�.
        [flatten]
        if (Material.Emissive.a > 0.0f)
        {
            float NdotE = dot(normalize(normal), E);
            float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));

            result.Emissive = Material.Emissive * emissive * PointLights[i].Emissive; // calc Emissive
        }

        float temp = 1.0f / saturate(dist / PointLights[i].Range);
        //float att = temp * temp * (1.0f / max(1.0f - PointLights[i].Intensity, 1e-8f));
        float att = temp * temp * PointLights[i].Intensity;
        
        output.Ambient  += result.Ambient * temp;
        output.Diffuse  += result.Diffuse * att;
        output.Emissive += result.Emissive * att;
        output.Specular += result.Specular * att;
        
    
    }//for i
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_SPOT_LIGHTS 256

struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
    
    float3 Position;
    float Range;
    
    float3 Direction;
    float Angle;
    
    float Intensity;
    float3 Padding;
};

cbuffer CB_SpotLights
{
    uint SpotLightCount;
    float3 CB_SpotLights_Padding;
    
    SpotLight SpotLights[MAX_POINT_LIGHTS];
};


void ComputeSpotLight(out MaterialDesc output, float3 normal, float3 wPosition)
{
    output = (MaterialDesc) 0;
    MaterialDesc result = (MaterialDesc) 0;
    
    for (uint i = 0; i < SpotLightCount; i++)
    {
        float3 light = SpotLights[i].Position - wPosition;
        float dist = length(light);
        
        [flatten]
        if (dist > SpotLights[i].Range)
            continue; //�Ÿ��Ǵ�
        
        light /= dist;
        
        float NdotL = dot(light, normalize(normal));
        float3 E = normalize(ViewPosition() - wPosition);
        
        result.Ambient = SpotLights[i].Ambient * Material.Ambient; // Calc Ambient
    
    
        [flatten]
        if (NdotL > 0.0f)
        {
            result.Diffuse = Material.Diffuse * NdotL * SpotLights[i].Diffuse; // Calc Diffuse
        
            [flatten]
            if (Material.Specular.a > 0.0f) //���� ������ �ƴ� ����
            {
                float3 R = normalize(reflect(-light, normal));
                float RdotE = saturate(dot(R, E));
            
                float specular = pow(RdotE, Material.Specular.a);
                result.Specular = Material.Specular * specular * SpotLights[i].Specular;
            }
        }

    //Emssive�� ��ο� �κп��� �����ϱ� ������ NdotL�� ������ ���� �ʴ´�.
        [flatten]
        if (Material.Emissive.a > 0.0f)
        {
            float NdotE = dot(normalize(normal), E);
            float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));

            result.Emissive = Material.Emissive * emissive * SpotLights[i].Emissive; // calc Emissive
        }

        float temp = pow(saturate(dot(-light, SpotLights[i].Direction)), 90.0f - SpotLights[i].Angle);
        float att = temp * (1.0f / max(1.0f - SpotLights[i].Intensity, 1e-8f));
        //float att = temp * SpotLights[i].Intensity;
        
        output.Ambient += result.Ambient * temp;
        output.Diffuse += result.Diffuse * att;
        output.Emissive += result.Emissive * att;
        output.Specular += result.Specular * att;
        
    
    } //for i
}

//======================================================================================

float4 PS_AllLight(MeshOutput input)
{
    NormalMapping(input.Uv, input.Normal, input.Tangent);
    Texture(Material.Diffuse, DiffuseMap, input.Uv);

    Texture(Material.Specular, SpecularMap, input.Uv);
    
    MaterialDesc output = (MaterialDesc) 0;
    MaterialDesc result = (MaterialDesc) 0;
    
    ComputeLight(output, input.Normal, input.wPosition);
    AddMaterial(result, output);
    
    ComputePointLight(output, input.Normal, input.wPosition);
    AddMaterial(result, output);
    
    ComputeSpotLight(output, input.Normal, input.wPosition);
    AddMaterial(result, output);
    
    return float4(MaterialToColor(result), 1.0f);
}


/////////////////////////////////////////////////////////////////////
// Projector 
/////////////////////////////////////////////////////////////////////
struct ProjectorDesc
{
    float4 Color;
    
    matrix View;
    matrix Projection;
};

cbuffer CB_Projector
{
    ProjectorDesc Projector;
};

void VS_Projector(inout float4 wvp, float4 oPosition)
{
    wvp = WorldPosition(oPosition);
    wvp = mul(wvp, Projector.View);
    wvp = mul(wvp, Projector.Projection);
}

/////////////////////////////////////////////////////////////////////
// Shadow
/////////////////////////////////////////////////////////////////////
float4 PS_Shadow_Depth(MeshDepthOutput input) : SV_Target
{
    float depth = input.sPosition.z / input.sPosition.w; // Lighting ������ ViewProjection���� ��ȯ
    
    return float4(depth, depth, depth, 1);
}

float4 PS_Shadow(float4 sPosition, float4 color)
{
    float4 position = sPosition; //Shadow�������� view Projection�� �� position
    position.xyz /= position.w;
    
    [flatten]
    if (position.x < -1.0f || position.x > 1.0f &&
        position.y < -1.0f || position.y > 1.0f &&
        position.z < 0.0f || position.z > 1.0f) //���� �ȿ� �����°�
    {
        return color; //�����ȿ� ������ ������� 
    }
    
    position.x = position.x * 0.5f + 0.5f;
    position.y = -position.y * 0.5f + 0.5f; //UV�������� ��ȯ
    
    float depth = 0.0f;
    float factor = 0;
    float z = position.z - Shadow.Bias;
    
    if (Shadow.Quality == 0)
    {
        depth = ShadowMap.Sample(LinearSampler, position.xy).r;
        factor = (float) (depth >= z);
    }
    else if (Shadow.Quality == 1)
    {
        depth = position.z;
        factor = ShadowMap.SampleCmp(ShadowPcfSampler, position.xy, depth).r;
    }
    else if (Shadow.Quality == 2)
    { //�ֺ��ȼ��� ��ó��
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