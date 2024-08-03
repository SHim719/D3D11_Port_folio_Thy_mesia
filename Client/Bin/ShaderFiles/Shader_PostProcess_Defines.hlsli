#ifndef SHADER_FUNCTION
#define SHADER_FUNCTION

float g_fBloomStrength = 0.5f;
float g_fGamma = 2.2f;
float g_fExposure = 2.f;

// RADIAL_BLUR/////////////////////
int     g_iSampleCount = 16;
float    g_fBlurStrength;
float2  g_vBlurCenter;
float   g_fBlurRadius;
///////////////////////////////////


float3 LinearToneMapping(float3 color)
{
    float3 invGamma = float3(1, 1, 1) / g_fGamma;

    color = clamp(g_fExposure * color, 0., 1.);
    color = pow(color, g_fGamma);
    return color;
}

float3 FilmicToneMapping(float3 color)
{
    color = max(float3(0, 0, 0), color);
    color = (color * (6.2 * color + .5)) / (color * (6.2 * color + 1.7) + 0.06);
    return color;
}

float3 Uncharted2ToneMapping(float3 color)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    
    color *= g_fExposure;
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    color /= white;
    color = pow(color, float3(1.0, 1.0, 1.0) / g_fGamma);
    return color;
}

float3 lumaBasedReinhardToneMapping(float3 color)
{
    float3 invGamma = float3(1, 1, 1) / g_fGamma;
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float toneMappedLuma = luma / (1. + luma);
    color *= toneMappedLuma / luma;
    color = pow(color, invGamma);
    return color;
}

#endif