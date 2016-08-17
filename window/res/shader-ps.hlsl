Texture2D tex;
SamplerState samplerState;

struct PixelShaderInput {
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    return tex.Sample(samplerState, input.texCoord);
}
