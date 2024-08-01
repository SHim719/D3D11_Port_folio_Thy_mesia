#ifndef SHADER_EFFECT_DEFINE
#define SHADER_EFFECT_DEFINE
#include "Shader_Defines.hlsli"

texture2D g_BaseTexture;
texture2D g_MaskTexture;
texture2D g_NoiseTexture;
texture2D g_DissolveTexture;

float4  g_vTextureFlag;
float4  g_vColor_Offset;
float4  g_vColor_Clip;

float2  g_vMaskUVOffset;
float2  g_vNoiseUVOffset;

float   g_fDissolveAmount;

bool    g_bGlow;
float4  g_vGlowColor;
float   g_fGlowIntensity;

bool    g_bBloom;

int     g_iSpriteCol, g_iSpriteRow;

#endif
