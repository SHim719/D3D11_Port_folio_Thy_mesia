#include "Shader_Defines.hlsli"
#include "Shader_PostProcess_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DeferredTexture;
texture2D g_PrevBloomTexture;
texture2D g_EffectAlphaBlendTexture;
texture2D g_BloomTexture;

texture2D g_FinalTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    Out.vPosition = float4(In.vPosition, 1.f);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_MAIN_Bloom(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vColor = g_DeferredTexture.Sample(LinearWrapSampler, In.vTexcoord);
    float4 vEffectAlphaBlend = g_EffectAlphaBlendTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    if (vEffectAlphaBlend.a > 0.001f)
    {
        vColor = float4(vEffectAlphaBlend.rgb * vEffectAlphaBlend.a + vColor.rgb * (1.f - vEffectAlphaBlend.a), 1.f);
    }
    
    float4 vPrevBloom = g_PrevBloomTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vBloom = g_BloomTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    float3 vCombined = (1.0 - g_fBloomStrength) * vPrevBloom.rgb + g_fBloomStrength * vBloom.rgb;
    
    vColor.rgb += Uncharted2ToneMapping(vCombined);
 
    Out.vColor = vColor;
 
    return Out;
}

PS_OUT PS_MAIN_Final(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_FinalTexture.Sample(PointClampSampler, In.vTexcoord);
 
    return Out;
}


technique11 DefaultTechnique
{
    pass Bloom // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Bloom();
    }

    pass Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Final();
    }
}

