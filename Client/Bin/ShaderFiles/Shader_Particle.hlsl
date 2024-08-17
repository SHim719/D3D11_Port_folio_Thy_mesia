#include "Shader_Defines.hlsli"
#include "Shader_Effect_Defines.hlsli"

#define BILLBOARD 0
#define ALIGNVELOCITY 1
#define FOLLOW_BONE 2

matrix  g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4  g_vCamPosition;

int     g_iParticleMode;
    
struct VS_IN
{
    float3      vPosition : POSITION;

    float4      vRight : TEXCOORD0;
    float4      vUp : TEXCOORD1;
    float4      vLook : TEXCOORD2;
    float4      vTranslation : TEXCOORD3;
    
    float4      vColor : COLOR0;
    float4      vVelocity : VELOCITY;
    float       fLifeTime : LIFETIME;
    int         iSpriteIdx : INDEX;
};

struct VS_OUT
{
    float3      vPosition       : POSITION;

    float4      vRight          : TEXCOORD0;
    float4      vUp             : TEXCOORD1;
    float4      vLook           : TEXCOORD2;
    float4      vTranslation    : TEXCOORD3;
    float4      vColor          : COLOR0;
    float3      vVelocity       : VELOCITY;
    float       fLifeTime       : LIFETIME;
    int         iSpriteIdx      : INDEX;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;

    Out.vPosition = In.vPosition;
    
    Out.vRight = In.vRight;
    Out.vUp = In.vUp;
    Out.vLook = In.vLook;
    Out.vTranslation = In.vTranslation;
    
    Out.vColor = In.vColor;
    Out.vVelocity = In.vVelocity;
    Out.fLifeTime = In.fLifeTime;
    Out.iSpriteIdx = In.iSpriteIdx;
    
    return Out;
}

struct GS_IN
{
    float3              vPosition : POSITION;

    float4              vRight : TEXCOORD0;
    float4              vUp : TEXCOORD1;
    float4              vLook : TEXCOORD2;
    float4              vTranslation : TEXCOORD3;
    
    float4              vColor : COLOR0;
    float3              vVelocity : VELOCITY;
    float               fLifeTime : LIFETIME;
    int                 iSpriteIdx : INDEX;
};

struct GS_OUT
{
    float4      vPosition : SV_POSITION;
    float2      vTexcoord : TEXCOORD0;
    float4      vColor : TEXCOORD1;
    float       fLifeTime : TEXCOORD2;  
};


[maxvertexcount(4)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutputStream)
{
    GS_OUT Out = (GS_OUT)0;
    
    float4 vLook = float4(0.f, 0.f, 0.5f, 0.f);
    float4 vRight = float4(0.5f, 0.f, 0.f, 0.f);
    float4 vUp = float4(0.f, 0.5f, 0.f, 0.f);
    
    float2 vPSize = float2(length(In[0].vRight), length(In[0].vUp));
  
    matrix TransformMatrix = float4x4(In[0].vRight, In[0].vUp, In[0].vLook, In[0].vTranslation);
    matrix WorldMatrix = g_WorldMatrix;
    
    if (g_iParticleMode & (1 << BILLBOARD))
    {
        float4 vWorldPosition = mul(float4(In[0].vPosition, 1.f), g_WorldMatrix);
        
        vLook = normalize(g_vCamPosition - vWorldPosition);
        vRight = float4(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * 0.5f, 0.f) * vPSize.x;
        vUp = float4(normalize(cross(vLook.xyz, vRight.xyz)) * 0.5f, 0.f) * vPSize.y;
        
        TransformMatrix[0] = float4(1.f, 0.f, 0.f, 0.f);
        TransformMatrix[1] = float4(0.f, 1.f, 0.f, 0.f);
        TransformMatrix[2] = float4(0.f, 0.f, 1.f, 0.f);
        WorldMatrix[0] = float4(1.f, 0.f, 0.f, 0.f);
        WorldMatrix[1] = float4(0.f, 1.f, 0.f, 0.f);
        WorldMatrix[2] = float4(0.f, 0.f, 1.f, 0.f);
    }
    else if ((g_iParticleMode & (1 << ALIGNVELOCITY)) && 0.f != length(In[0].vVelocity))
    {
        float3 vVelocityDir = normalize(In[0].vVelocity);
    
        vRight = float4(vVelocityDir * 0.5f, 0.f) * vPSize.x;
        vLook = float4(normalize(cross(float3(0.f, 1.f, 0.f), vRight.xyz)) * 0.5f , 0.f);
        vUp = float4(normalize(cross(vLook.xyz, vRight.xyz)) * 0.5f, 0.f) * vPSize.y;
        
        TransformMatrix[0] = float4(1.f, 0.f, 0.f, 0.f);
        TransformMatrix[1] = float4(0.f, 1.f, 0.f, 0.f);
        TransformMatrix[2] = float4(0.f, 0.f, 1.f, 0.f);
    }
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    matrix matWVP = mul(WorldMatrix, matVP);
    
    float4 vPosition = float4(In[0].vPosition, 1.f);

    Out.vPosition = vPosition - vRight + vUp;
    Out.vPosition = mul(Out.vPosition, TransformMatrix);
    Out.vPosition = (g_iParticleMode & (1 << FOLLOW_BONE)) ? mul(Out.vPosition, matVP) : mul(Out.vPosition, matWVP);
    Out.fLifeTime = In[0].fLifeTime;
    Out.vColor = In[0].vColor;
    Out.vTexcoord = float2(0.f, 0.f);

    OutputStream.Append(Out);

    Out.vPosition = vPosition + vRight + vUp;
    Out.vPosition = mul(Out.vPosition, TransformMatrix);
    Out.vPosition = (g_iParticleMode & (1 << FOLLOW_BONE)) ? mul(Out.vPosition, matVP) : mul(Out.vPosition, matWVP);
    Out.vTexcoord = float2(1.f, 0.f);

    OutputStream.Append(Out);

    Out.vPosition = vPosition - vRight - vUp;
    Out.vPosition = mul(Out.vPosition, TransformMatrix);
    Out.vPosition = (g_iParticleMode & (1 << FOLLOW_BONE)) ? mul(Out.vPosition, matVP) : mul(Out.vPosition, matWVP);
    Out.vTexcoord = float2(0.f, 1.f);

    OutputStream.Append(Out);

    Out.vPosition = vPosition + vRight - vUp;
    Out.vPosition = mul(Out.vPosition, TransformMatrix);
    Out.vPosition = (g_iParticleMode & (1 << FOLLOW_BONE)) ? mul(Out.vPosition, matVP) : mul(Out.vPosition, matWVP);
    Out.vTexcoord = float2(1.f, 1.f);

    OutputStream.Append(Out);
}

struct PS_IN
{
    float4      vPosition : SV_POSITION;
    float2      vTexcoord : TEXCOORD0;
    float4      vColor : TEXCOORD1;
    float       fLifeTime : TEXCOORD2;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vGlow : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.fLifeTime <= 0.f)
        discard;
    
    float fMask = 1.f;
    float fNoise = 1.f;
    float fDistortion = 0.f;
    
    float4 vColor = g_BaseTexture.Sample(LinearWrapSampler, In.vTexcoord);
    
    if (0.f != g_vTextureFlag.x)    // Noise
    {
        fNoise = g_NoiseTexture.Sample(LinearWrapSampler, In.vTexcoord).r;
    }
       
    if (0.f != g_vTextureFlag.y) // Mask
    {
        fMask = g_MaskTexture.Sample(LinearWrapSampler, In.vTexcoord).r;
    }
    
    vColor *= fMask * In.vColor;
    vColor.a *= fNoise;

    if (vColor.r <= g_vColor_Clip.r && vColor.g <= g_vColor_Clip.g && vColor.b <= g_vColor_Clip.b)
        discard;
    
    if (vColor.a <= g_vColor_Clip.a)
        discard;
    
    Out.vColor = vColor;
   
    if (g_bGlow)
    {
        float fGlowIntensity = g_fGlowIntensity * vColor.a;
        float3 vGlow = g_vGlowColor.rgb * fGlowIntensity;
        Out.vGlow = Out.vColor;
        Out.vGlow.rgb *= vGlow;
    }
  

   
    return Out;
}


technique11 DefaultTechnique
{
    pass AlphaBlend //0
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Effect, 0);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

    pass OneBlend // 1
    {
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }
}
