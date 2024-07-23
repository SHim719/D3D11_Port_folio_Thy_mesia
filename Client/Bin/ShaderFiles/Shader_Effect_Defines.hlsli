#ifndef SHADER_EFFECT_DEFINE
#define SHADER_EFFECT_DEFINE
#include "Shader_Defines.hlsli"

texture2D g_BaseTexture;
texture2D g_MaskTexture;
texture2D g_NoiseTexture;
texture2D g_EmissiveTexture;
texture2D g_DissolveTexture;

float4  g_vTextureFlag;
float4  g_vColor_Mul;
float4  g_vColor_Offset;
float4  g_vColor_Clip;

int     g_iSpriteCol, g_iSpriteRow;

float4 CalculateEffectColor(inout float2 vUV, in float2 vOriginUV, inout float fDistortion, in int iSamplerState)
{
    float fMask = 1.f;
    float2 vNewUV = vUV;
    
    sampler Sampler = LinearWrapSampler;
    
    if (0.f != g_vTextureFlag.x)    // Noise
    {
        float2 vNoiseUV = g_NoiseTexture.Sample(Sampler, vOriginUV).rg;
        vNewUV += ((vNoiseUV - 0.5f) * 2.f) * 0.1f;
    }
    
    if (0.f != g_vTextureFlag.y)   // Mask
    {
        fMask = g_MaskTexture.Sample(Sampler, vNewUV).r;
        if (fMask < 0.01f) discard;
    }
 
    float4 vColor = g_BaseTexture.Sample(Sampler, vNewUV);
    vColor += g_vColor_Offset;
    vColor *= g_vColor_Mul;
    
    vColor.a *= fMask;
    
    if (vColor.r <= g_vColor_Clip.r && vColor.g <= g_vColor_Clip.g && vColor.b <= g_vColor_Clip.b)
       discard;
    
    if (vColor.a <= g_vColor_Clip.a)
       discard;
    

    //if (EPSILON < fIntensity_Distortion)
    //{
    //    if (bDistortionOnBaseMaterial)
    //        fDistortion = vColor.r * fIntensity_Distortion;
    //    else
    //        fDistortion = fMask * fIntensity_Distortion;
    //}
    
    //if (EPSILON < g_vTextureFlag.w)	// Dissolve
    //{
    //    float fDissolve = g_DissolveTexture.Sample(Sampler, vNewUV).x;
    //    
	//    //Discard the pixel if the value is below zero
    //    clip(fDissolve - fDissolve_Amount);
	//    //Make the pixel emissive if the value is below ~f
    //    if (fDissolve - fDissolve_Amount < 0.25f)/*0.08f*/
    //    {
    //        vColor.xyz += vColor.xyz * float3(0.3f, 0.3f, 0.3f);
    //    }
    //}
    
    return vColor;
}



#endif
