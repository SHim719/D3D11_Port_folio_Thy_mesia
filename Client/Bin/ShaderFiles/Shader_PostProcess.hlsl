#include "Shader_Defines.hlsli"
#include "Shader_PostProcess_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_ForCopyTexture;

texture2D g_PrevBloomTexture;
texture2D g_BloomTexture;

texture2D g_OriginTexture;
texture2D g_GlowTextureAfterBlur;

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

PS_OUT PS_MAIN_Copy(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vCopy = g_ForCopyTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    Out.vColor = vCopy;
    
    return Out;
}

PS_OUT PS_MAIN_Glow(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vOrigin = g_OriginTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vAfterBlur = g_GlowTextureAfterBlur.Sample(LinearClampSampler, In.vTexcoord);
    
    Out.vColor = vOrigin + vAfterBlur;
    
    return Out;
}


PS_OUT PS_MAIN_Bloom(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vPrevBloom = g_PrevBloomTexture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vBloom = g_BloomTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    float4 vCombined = vPrevBloom + g_fBloomStrength * vBloom;
    
    float4 vOrigin = g_OriginTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    vOrigin.rgb += vCombined.rgb;
    Out.vColor = float4(LinearToneMapping(vOrigin.rgb), 1.f);
    
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
    pass Copy // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Copy();
    }

    pass Glow // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Glow();
    }

    pass Bloom // 2
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


    pass Final // 3
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

