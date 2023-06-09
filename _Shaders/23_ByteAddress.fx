ByteAddressBuffer Input;
RWByteAddressBuffer Output;

struct Group
{
    uint3 GroupId;
    uint3 GroupThreadId;
    uint3 DispatchThreadId;
    uint GroupIndex;
};

struct InputData
{
    float RetValue;
};

struct ComputeInput
{
    uint3 GroupId : SV_GroupID;
    uint3 GroupThreadId : SV_GroupThreadID;
    uint3 DispatchThreadId : SV_DispatchThreadID;
    uint GroupIndex : SV_GroupIndex;
};

[numthreads(10, 8, 3)]
void CS(ComputeInput input)
{
    Group group;
    group.GroupId = asuint(input.GroupId);
    group.GroupThreadId = asuint(input.GroupThreadId);
    group.DispatchThreadId = asuint(input.DispatchThreadId);
    group.GroupIndex = asuint(input.GroupIndex);

    uint inAddress = (input.GroupId.x * 240 + input.GroupIndex) * 4;
    float value = asfloat(Input.Load(inAddress)) * 10;
    
    uint outAddress = (input.GroupId.x * 240 + input.GroupIndex) * 11 * 4;
    Output.Store3(outAddress + 0, asuint(group.GroupId));
    Output.Store3(outAddress + 12, asuint(group.GroupThreadId));
    Output.Store3(outAddress + 24, asuint(group.DispatchThreadId));
    Output.Store(outAddress + 36, asuint(group.GroupIndex));
    Output.Store(outAddress + 40, asuint(value));
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}
