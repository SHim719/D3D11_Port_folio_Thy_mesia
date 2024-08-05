#include "Shader_Defines.hlsli"
#include "Shader_Effect_Defines.hlsli"

// float4x4
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vColor;

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

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    vector vPosition = mul(float4(In.vPosition, 1.f), matWVP);

    Out.vPosition = vPosition;
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
    float4 vGlow : SV_TARGET1;
    //float4 vBloom : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fMask = 1.f;
    float fNoise = 1.f;
    float fDissolve = 0.f;
    float fDistortion = 0.f;
    
    float4 vColor = g_BaseTexture.Sample(LinearWrapSampler, In.vTexcoord);
   
    if (0.f != g_vTextureFlag.x)    // Noise
    {
        float2 vNoiseUV = In.vTexcoord;
        fNoise = g_NoiseTexture.Sample(LinearWrapSampler, vNoiseUV).r;
    }
       
    if (0.f != g_vTextureFlag.y) // Mask
    {
        float2 vMaskUV = In.vTexcoord;
        fMask = g_MaskTexture.Sample(LinearWrapSampler, vMaskUV).r;
    }
    
    vColor *= fMask * fNoise * g_vColor;

    if (vColor.r <= g_vColor_Clip.r && vColor.g <= g_vColor_Clip.g && vColor.b <= g_vColor_Clip.b)
        discard;
    
    if (vColor.a <= g_vColor_Clip.a)
       discard;
    
    Out.vColor = vColor;
   
    if (g_bGlow)
    {
        float fGlowIntensity = g_fGlowIntensity * g_vColor.a;
        float3 vGlow = g_vGlowColor.rgb * fGlowIntensity;
        Out.vGlow = Out.vColor;
        Out.vGlow.rgb *= vGlow;
    }
    
  //if (g_bBloom)
  //{
  //     Out.vBloom = Out.vColor;
  //}
    
    return Out;
}

PS_OUT PS_MAIN_CLAMP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fMask = 1.f;
    float fNoise = 1.f;
    float fDistortion = 0.f;
    
    float4 vColor = g_BaseTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    if (0.f != g_vTextureFlag.x)    // Noise
    {
        float2 vNoiseUV = In.vTexcoord;
        fNoise = g_NoiseTexture.Sample(LinearClampSampler, vNoiseUV).r;
    }
       
    if (0.f != g_vTextureFlag.y) // Mask
    {
        float2 vMaskUV = In.vTexcoord;
        fMask = g_MaskTexture.Sample(LinearClampSampler, vMaskUV).r;
    }
    
    vColor *= fMask * fNoise * g_vColor;

    if (vColor.r <= g_vColor_Clip.r && vColor.g <= g_vColor_Clip.g && vColor.b <= g_vColor_Clip.b)
        discard;
    
    if (vColor.a <= g_vColor_Clip.a)
        discard;
    
    Out.vColor = vColor;
   
    if (g_bGlow)
    {
        float fGlowIntensity = g_fGlowIntensity * g_vColor.a;
        float3 vGlow = g_vGlowColor.rgb * fGlowIntensity;
        Out.vGlow = Out.vColor;
        Out.vGlow.rgb *= vGlow;
    }
    
    //if (g_bBloom)
    //{
    //    
    //}
    
    return Out;
}


technique11 DefaultTechinque
{
    pass DefaultWrap // 0
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

    pass DefaultClamp // 1
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CLAMP();
        ComputeShader = NULL;
    }
}

