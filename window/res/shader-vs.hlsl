struct VertexShaderInput {
    float2 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct PixelShaderInput {
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    output.position = float4(input.position.x, input.position.y, 0.0, 1.0);
    output.texCoord = input.texCoord;
    return output;
}
