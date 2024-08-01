#include "Shader_Defines.hlsli"

matrix  g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4x4 g_PlayerWorldMatrix;
float4 g_vTargetPosition;

texture2D g_MaskTexture;
float2 g_vMaskUVOffset; 


struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float3 vPosition : POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Out.vPosition = In.vPosition;

    return Out;
}

struct GS_IN
{
    float3 vPosition : POSITION0;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


[maxvertexcount(64)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutputStream)
{
    GS_OUT Out = (GS_OUT)0;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
   
    float3 vStartPos = g_PlayerWorldMatrix[3].xyz; //mul(float4(In[0].vPosition, 1.f), g_WorldMatrix).xyz;
    float3 vEndPos = g_vTargetPosition.xyz;
    float3 vRight = g_PlayerWorldMatrix[0].xyz;
    float3 vControlPos = (vStartPos + vEndPos) * 0.5f - vRight * 2.f + float3(0.f, 1.f, 0.f) * 3.f;
    float fThickness = 0.1f;
    
    int iNumPoints = 32;
    for (int i = 0; i <= iNumPoints; ++i)
    {
        float d = i / (float) iNumPoints;
        float3 vNewPoint = lerp(lerp(vStartPos, vControlPos, d), lerp(vControlPos, vEndPos, d), d);

        float3 vTangent = normalize(lerp(vControlPos - vStartPos, vEndPos - vControlPos, d));
        float3 vNormal = normalize(cross(vTangent, float3(0.f, 1.f, 0.f))) * fThickness * 0.5f;
        
        Out.vPosition = mul(float4(vNewPoint - vNormal, 1.f), matVP);
        Out.vTexcoord = float2(d, 0.f);
        OutputStream.Append(Out);
        
        Out.vPosition = mul(float4(vNewPoint + vNormal, 1.f), matVP);
        Out.vTexcoord = float2(d, 1.f);
        OutputStream.Append(Out);
    }
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

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float fMask = g_MaskTexture.Sample(LinearClampSampler, In.vTexcoord + g_vMaskUVOffset) * 0.6f;
    
    Out.vColor = float4(1.f, 1.f, 1.f, fMask);
    
    clip(Out.vColor.a < 0.1f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass LockOnCurve
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Effect, 0);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}