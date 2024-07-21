#include "Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_fWeights[5] = { 0.0545, 0.2442, 0.4026, 0.2442, 0.0545 };

float g_fPixelWidth = 1.f / 1280.f;
float g_fPixelHeight = 1.f / 720.f;
float g_fThreshold = 0.5f;
float g_fStrength = 0.5f;

texture2D g_ReadyBrightPassTexture;
texture2D g_BrightPassedTexture; 
texture2D g_BlurXTexture;
texture2D g_BlurYTexture;

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


PS_OUT PS_MAIN_BRIGHTPASS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vColor = g_ReadyBrightPassTexture.Sample(LinearClampSampler, In.vTexcoord);
  
   if (vColor.a < 0.1f)
       discard;
    
   if (vColor.r + vColor.g + vColor.b / 3.f > g_fThreshold)
   {
        Out.vColor = vColor;
   }  
    
    return Out;
    /*
      float4 BrightColor = (float4) 0.f;

    float brightness = dot(vOriginalColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    if (brightness > 0.85)
        BrightColor = float4(vOriginalColor.rgb, 1.0);
    else
    {
        discard;
    }
    Out.vExtractBloom = BrightColor;
    
    */
}

/* ÇÈ¼¿ÀÇ »öÀ» °áÁ¤ÇÏ³®. */
PS_OUT PS_MAIN_BlurX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float3 vColor = float3(0.f, 0.f, 0.f);

    for (int i = 0; i < 5; ++i)
    {
        vColor += g_fWeights[i] * g_BlurXTexture.Sample(LinearClampSampler, In.vTexcoord +
            float2(g_fPixelWidth, 0.f) * float(i - 2)).rgb;
    }
    Out.vColor = float4(vColor, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_BlurY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float3 vColor = float3(0.f, 0.f, 0.f);
    
    for (int i = 0; i < 5; ++i)
    {
        vColor += g_fWeights[i] * g_BlurYTexture.Sample(LinearClampSampler, In.vTexcoord +
            float2(0.f, g_fPixelHeight) * float(i - 2)).rgb;
    }
    
    Out.vColor = float4(vColor, 1.f);

    return Out;
}

PS_OUT PS_MAIN_Bloom(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vOriginColor = g_BrightPassedTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    if (vOriginColor.a < 0.1f)
        discard;
    
    float4 vBlurredColor = g_BlurXTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    Out.vColor.rgb = saturate(vOriginColor.rgb + g_fStrength * vBlurredColor.rgb);
    Out.vColor.a = vOriginColor.a;

    return Out;
}


technique11 DefaultTechnique
{
    pass BrightPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BRIGHTPASS();
    }


    pass BlurX // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BlurX();
    }

    pass BlurY // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BlurY();
    }

    pass Bloom // 3
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


}

