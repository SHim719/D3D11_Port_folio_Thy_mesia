
#include "Shader_Defines.hlsli"

// float4x4
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_MaskTexture;
texture2D g_NoiseTexture;

float2 g_vMaskUV;
float2 g_vNoiseUV;

float4 g_vColor;
float g_fAlpha;

float g_fNoiseScale;
float g_fMaskScale;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    vector vPosition = mul(float4(In.vPosition, 1.f), matWVP);

    Out.vPosition = vPosition;
    Out.vTexUV = In.vTexUV;
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV) * g_vColor;
    
    float4 vNoise = g_NoiseTexture.Sample(LinearWrapSampler, In.vTexUV + g_vNoiseUV);
    float4 vMask = g_MaskTexture.Sample(LinearWrapSampler, In.vTexUV + g_vMaskUV);
    
    Out.vColor.a *= vNoise.r;
    Out.vColor.a *= vMask.r;
    Out.vColor.rgb *= vMask.rgb;
   
    if (Out.vColor.a < 0.3f)
        discard;
  
    return Out;
}

technique11 DefaultTechinque
{
    pass Default // 0
    {
        SetBlendState(BS_AlphaBlend, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }
	
}

