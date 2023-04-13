
float4 PS_Sky(MeshOutput input) : SV_Target
{
    return SkyCubeMap.Sample(LinearSampler, input.oPosition);
}

//=================================================================

#define VS_GENERATE \
output.oPosition = input.Position.xyz;\
\
output.Position = WorldPosition(input.Position); \
output.wPosition = output.Position.xyz; \
\
output.Position = ViewProjection(output.Position); \
output.wvpPosition = output.Position; \
output.wvpPosition_Sub = output.Position; \
\
output.sPosition = WorldPosition(input.Position);\
output.sPosition = mul(output.sPosition, Shadow.View);\
output.sPosition = mul(output.sPosition, Shadow.Projection);\
\
output.Normal = WorldNormal(input.Normal);\
output.Tangent = WorldTangent(input.Tangent);\
output.Uv = input.Uv;\
output.Color = input.Color;\
\
//output.ColorAmb = input.ColorAmb;\
//output.ColorDiff = input.ColorDiff;\
//output.ColorSpec = input.ColorSpec;\
//output.ColorEmi = input.ColorEmi;\
\
output.Culling.x = dot(float4(output.wPosition, 1), Culling[0]);\
output.Culling.y = dot(float4(output.wPosition, 1), Culling[1]);\
output.Culling.z = dot(float4(output.wPosition, 1), Culling[2]);\
output.Culling.w = dot(float4(output.wPosition, 1), Culling[3]);\
\
output.Clipping = float4(0,0,0,0);\
output.Clipping.x = dot(float4(output.wPosition, 1), Clipping);

//=================================================================
//깊이만 렌더
#define VS_DEPTH_GENERATE \
output.Position = WorldPosition(input.Position); \
output.Position = mul(output.Position, Shadow.View); \
output.Position = mul(output.Position, Shadow.Projection); \
\
output.sPosition = output.Position;

//=================================================================

//=================================================================
// 반사 용
#define VS_REFLECTION_GENERATE \
output.oPosition = input.Position.xyz; \
\
output.Position = WorldPosition(input.Position); \
output.wPosition = output.Position.xyz; \
\
output.Position = mul(output.Position, ReflectionView); \
output.Position = mul(output.Position, Projection); \
output.wvpPosition = output.Position; \
output.wvpPosition_Sub = output.Position; \
\
output.sPosition = WorldPosition(input.Position); \
output.sPosition = mul(output.sPosition, Shadow.View); \
output.sPosition = mul(output.sPosition, Shadow.Projection); \
\
output.Normal = WorldNormal(input.Normal); \
output.Tangent = WorldTangent(input.Tangent); \
output.Uv = input.Uv; \
output.Color = input.Color;\
\
//output.ColorAmb = input.ColorAmb;\
//output.ColorDiff = input.ColorDiff;\
//output.ColorSpec = input.ColorSpec;\
//output.ColorEmi = input.ColorEmi;\
\
output.Culling.x = dot(float4(output.wPosition, 1), Culling[0]); \
output.Culling.y = dot(float4(output.wPosition, 1), Culling[1]); \
output.Culling.z = dot(float4(output.wPosition, 1), Culling[2]); \
output.Culling.w = dot(float4(output.wPosition, 1), Culling[3]); \
\
output.Clipping = float4(0, 0, 0, 0); \
output.Clipping.x = dot(float4(output.wPosition, 1), Clipping);

//=================================================================

struct VertexMesh
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    
    matrix Transform : Inst1_Transform;
    float4 Color : Inst2_Color; //Material 개별적용하려면 MaterialDesc컬러4개 만들어서 받음
    
    //float4 ColorAmb : Inst3_ColorAmb;
    //float4 ColorDiff : Inst4_ColorDiffuse;
    //float4 ColorSpec : Inst5_ColorSpecular;
    //float4 ColorEmi : Inst6_ColorEmissive;
};

void SetMeshWorld(inout matrix world, VertexMesh input)
{
    world = input.Transform;
}

MeshOutput VS_Mesh(VertexMesh input)
{
    MeshOutput output = (MeshOutput) 0;
    SetMeshWorld(World, input);
    
    VS_GENERATE
    
    return output;
}

MeshDepthOutput VS_Mesh_Depth(VertexMesh input)
{
    MeshDepthOutput output = (MeshDepthOutput) 0;
    SetMeshWorld(World, input);
    
    VS_DEPTH_GENERATE
    
    return output;
}

MeshOutput VS_Mesh_Projector(VertexMesh input)
{
    MeshOutput output = VS_Mesh(input);
    VS_Projector(output.wvpPosition_Sub, input.Position);

    return output;
}

MeshOutput VS_Mesh_Reflection(VertexMesh input)
{
    MeshOutput output = (MeshOutput) 0;
    SetMeshWorld(World, input);
    
    VS_REFLECTION_GENERATE
    
    return output;
}

//=================================================================
Texture2DArray TransformsMap;

struct VertexModel
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 BlendIndices : BlendIndices;
    float4 BlendWeights : BlendWeights;
    
    matrix Transform : Inst1_Transform;
    float4 Color : Inst2_Color;
    
    uint InstanceId : SV_InstanceID;
};

#define MAX_MODEL_TRANSFORMS 250

cbuffer CB_Bones
{
    //matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
    
    uint BoneIndex;
};

void SetModelWorld(inout matrix world, VertexModel input)
{
    //World = mul(BoneTransforms[BoneIndex], World);
    
    float4 m0 = TransformsMap.Load(int4(BoneIndex * 4 + 0, input.InstanceId, 0, 0));
    float4 m1 = TransformsMap.Load(int4(BoneIndex * 4 + 1, input.InstanceId, 0, 0));
    float4 m2 = TransformsMap.Load(int4(BoneIndex * 4 + 2, input.InstanceId, 0, 0));
    float4 m3 = TransformsMap.Load(int4(BoneIndex * 4 + 3, input.InstanceId, 0, 0));
    
    matrix transform = matrix(m0, m1, m2, m3);
    world = mul(transform, input.Transform);
}

MeshOutput VS_Model(VertexModel input)
{
    MeshOutput output = (MeshOutput) 0;
    SetModelWorld(World, input);
    VS_GENERATE

    return output;
}

MeshDepthOutput VS_Model_Depth(VertexModel input)
{
    MeshDepthOutput output = (MeshDepthOutput) 0;
    SetModelWorld(World, input);
    VS_DEPTH_GENERATE

    return output;
}

MeshOutput VS_Model_Projector(VertexModel input)
{
    MeshOutput output = VS_Model(input);
    VS_Projector(output.wvpPosition_Sub, input.Position);

    return output;
}

MeshOutput VS_Model_Reflection(VertexModel input)
{
    MeshOutput output = (MeshOutput) 0;
    SetModelWorld(World, input);
    
    VS_REFLECTION_GENERATE
    
    return output;
}

//=================================================================

#define MAX_MODEL_KEYFRAMES 500
#define MAX_MODEL_INSTANCE 500

struct AnimationFrame
{
    int Clip;

    uint CurrFrame;
    uint NextFrame;

    float Time;
    float Running;

    float3 Padding;
};

struct TweenFrame
{
    float TakeTime;
    float TweenTime;
    float RunningTime;
    float Padding;

    AnimationFrame Curr;
    AnimationFrame Next;
    
};

cbuffer CB_AnimationFrame
{
    TweenFrame TweenFrames[MAX_MODEL_INSTANCE];
};


void SetAnimationWorld(inout matrix world, VertexModel input)
{
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };


    int clip[2];
    int currFrame[2], nextFrame[2];
    float time[2];
    
    clip[0] = TweenFrames[input.InstanceId].Curr.Clip;
    currFrame[0] = TweenFrames[input.InstanceId].Curr.CurrFrame;
    nextFrame[0] = TweenFrames[input.InstanceId].Curr.NextFrame;
    time[0] = TweenFrames[input.InstanceId].Curr.Time;
    
    clip[1] = TweenFrames[input.InstanceId].Next.Clip;
    currFrame[1] = TweenFrames[input.InstanceId].Next.CurrFrame;
    nextFrame[1] = TweenFrames[input.InstanceId].Next.NextFrame;
    time[1] = TweenFrames[input.InstanceId].Next.Time;
    
    
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    matrix transform = 0;
    matrix curr = 0, currAnim = 0;
    matrix next = 0, nextAnim = 0;
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame[0], clip[0], 0));
        c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame[0], clip[0], 0));
        c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame[0], clip[0], 0));
        c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame[0], clip[0], 0));
        curr = matrix(c0, c1, c2, c3);
        
        n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], clip[0], 0));
        n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], clip[0], 0));
        n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], clip[0], 0));
        n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], clip[0], 0));
        next = matrix(n0, n1, n2, n3);
        
        currAnim = lerp(curr, next, time[0]);
        
        
        [flatten]
        if (clip[1] > -1)
        {
            c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame[1], clip[1], 0));
            c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame[1], clip[1], 0));
            c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame[1], clip[1], 0));
            c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame[1], clip[1], 0));
            curr = matrix(c0, c1, c2, c3);
        
            n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], clip[1], 0));
            n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], clip[1], 0));
            n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], clip[1], 0));
            n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], clip[1], 0));
            next = matrix(n0, n1, n2, n3);
        
            nextAnim = lerp(curr, next, time[1]);
            
            currAnim = lerp(currAnim, nextAnim, TweenFrames[input.InstanceId].TweenTime);
        }
        
        
        transform += mul(weights[i], currAnim);
    }
    
    world = mul(transform, world);
}


struct BlendingFrame
{
    uint Mode;
    float Alpha;
    float2 Padding;
    
    AnimationFrame Clip[3];
};

cbuffer CB_BlendingFrame
{
    BlendingFrame BlendingFrames[MAX_MODEL_INSTANCE];
};

void SetBlendingWorld(inout matrix world, VertexModel input)
{
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };


    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    matrix transform = 0;
    matrix curr = 0;
    matrix next = 0;
    matrix anim = 0;
    matrix currAnim[3];
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        [unroll(3)]
        for (int k = 0; k < 3; k++)
        {
            c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, BlendingFrames[input.InstanceId].Clip[k].CurrFrame, BlendingFrames[input.InstanceId].Clip[k].Clip, 0));
            c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, BlendingFrames[input.InstanceId].Clip[k].CurrFrame, BlendingFrames[input.InstanceId].Clip[k].Clip, 0));
            c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, BlendingFrames[input.InstanceId].Clip[k].CurrFrame, BlendingFrames[input.InstanceId].Clip[k].Clip, 0));
            c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, BlendingFrames[input.InstanceId].Clip[k].CurrFrame, BlendingFrames[input.InstanceId].Clip[k].Clip, 0));
            curr = matrix(c0, c1, c2, c3);
        
            n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, BlendingFrames[input.InstanceId].Clip[k].NextFrame, BlendingFrames[input.InstanceId].Clip[k].Clip, 0));
            n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, BlendingFrames[input.InstanceId].Clip[k].NextFrame, BlendingFrames[input.InstanceId].Clip[k].Clip, 0));
            n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, BlendingFrames[input.InstanceId].Clip[k].NextFrame, BlendingFrames[input.InstanceId].Clip[k].Clip, 0));
            n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, BlendingFrames[input.InstanceId].Clip[k].NextFrame, BlendingFrames[input.InstanceId].Clip[k].Clip, 0));
            next = matrix(n0, n1, n2, n3);
        
            currAnim[k] = lerp(curr, next, BlendingFrames[input.InstanceId].Clip[k].Time);
        }
        
        
        float alpha = BlendingFrames[input.InstanceId].Alpha;
        int clipIndex[2] = { 0, 1 };
        if (alpha > 1)
        {
            clipIndex[0] = 1;
            clipIndex[1] = 2;
            
            alpha -= 1.0f;

        }
    
        anim = lerp(currAnim[clipIndex[0]], currAnim[clipIndex[1]], alpha);

        transform += mul(weights[i], anim);
    }
    
    world = mul(transform, world);
}

MeshOutput VS_Animation(VertexModel input)
{
    MeshOutput output = (MeshOutput) 0;
    
    World = input.Transform;
    
    if (BlendingFrames[input.InstanceId].Mode == 0)
        SetAnimationWorld(World, input);
    else
        SetBlendingWorld(World, input);
    
    VS_GENERATE
    
    return output;
}

MeshDepthOutput VS_Animation_Depth(VertexModel input)
{
    MeshDepthOutput output = (MeshDepthOutput) 0;
    
    World = input.Transform;
    
    if (BlendingFrames[input.InstanceId].Mode == 0)
        SetAnimationWorld(World, input);
    else
        SetBlendingWorld(World, input);
    
    VS_DEPTH_GENERATE
    
    return output;
}

MeshOutput VS_Animation_Projector(VertexModel input)
{
    MeshOutput output = VS_Animation(input);
    VS_Projector(output.wvpPosition_Sub, input.Position);

    return output;
}

MeshOutput VS_Animation_Reflection(VertexModel input)
{
    MeshOutput output = (MeshOutput) 0;
    
    World = input.Transform;
    
    if (BlendingFrames[input.InstanceId].Mode == 0)
        SetAnimationWorld(World, input);
    else
        SetBlendingWorld(World, input);
    
    VS_REFLECTION_GENERATE
    
    return output;
}

struct EnvCubeMapDesc
{
    uint Type;
    float Alpha;
    float RefractAmount;
    float Padding;
    
    float FresnelAmount;
    float FresnelBias;
    float FresnelScale;
    float Padding2;
    
    matrix Views[6]; // 해당물체에 카메라가 있다 생각하고 6면으로 찍음
    matrix Projection;
};

cbuffer CB_EnvCube
{
    EnvCubeMapDesc EnvCube;
};

////////////////////////////////////////////////////////////////////////
// flatten 문장을 합쳐버림 --> 주로 if 만 사용할 때 사용
// unroll(n) n만큼 컴파일러가 복사함 --> unroll이 없으면 만들면서 복사
// 변수를 만들면 중복선언이 되기때문에 변수선언은 밖에서
[maxvertexcount(18)]
void GS_EnvCube_PreRender(triangle MeshOutput input[3], inout TriangleStream<MeshOutput> stream)
{
    int vertex = 0;
    MeshOutput output = (MeshOutput) 0;
    
    [unroll(6)]
    for (int i = 0; i < 6; i++)
    {
        output.TargetIndex = i;
        
        [unroll(3)]
        for (vertex = 0; vertex < 3; vertex ++)
        {
            output.Position = mul(float4(input[vertex].wPosition, 1), EnvCube.Views[i]);
            output.Position = mul(output.Position, EnvCube.Projection);
            
            output.oPosition = input[vertex].oPosition;
            output.wPosition = input[vertex].wPosition;
            output.Normal = input[vertex].Normal;
            output.Tangent = input[vertex].Tangent;
            output.Uv = input[vertex].Uv;
            output.Color = input[vertex].Color;
            
            stream.Append(output);
        }
        
        stream.RestartStrip();
    }

}

float4 PS_EnvCube(MeshOutput input) : SV_Target
{
    float4 diffuse = 0;
    
    float3 view = normalize(input.wPosition - ViewPosition());
    float3 normal = normalize(input.Normal);
    float3 reflection = reflect(view, normal);  //반사
    float3 refraction = refract(view, normal, EnvCube.RefractAmount); // 굴절 
    
    if (EnvCube.Type == 0)
    {
        diffuse = EnvCubeMap.Sample(LinearSampler, input.oPosition);
        diffuse.a = 1.0f;
    }
    else if (EnvCube.Type == 1)
    {
        diffuse = EnvCubeMap.Sample(LinearSampler, reflection);
        diffuse.a = EnvCube.Alpha;
    }
    else if (EnvCube.Type == 2)
    {
        diffuse = EnvCubeMap.Sample(LinearSampler, -refraction);
        diffuse.a = EnvCube.Alpha;
    }
    else if (EnvCube.Type == 3)
    {
        float4 color = PS_AllLight(input);
        diffuse = EnvCubeMap.Sample(LinearSampler, reflection);
        
        float4 fresnel = EnvCube.FresnelBias + (1.0f - EnvCube.FresnelScale) * pow(abs(1.0f - dot(view, normal)), EnvCube.FresnelAmount);
        diffuse = EnvCube.FresnelAmount * diffuse + lerp(color, diffuse, fresnel);
        diffuse *= 0.75f;
        
        diffuse.a = EnvCube.Alpha;
    }
    
    return diffuse;
}
