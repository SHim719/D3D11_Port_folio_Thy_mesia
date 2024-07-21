#include "Shader_Defines.hlsli"

matrix  g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_MaskTexture;

float4  g_vColor;
float4  g_vCamPosition;

bool    g_bBillBoard;

//int     g_iSpriteIdx;
//int     g_iSpriteWidth;
//int     g_iSpriteHeight;

struct VS_IN
{
    float3      vPosition : POSITION;

    float4      vRight : TEXCOORD0;
    float4      vUp : TEXCOORD1;
    float4      vLook : TEXCOORD2;
    float4      vTranslation : TEXCOORD3;
};

struct VS_OUT
{
    float3      vPosition : POSITION;

    float4      vRight : TEXCOORD0;
    float4      vUp : TEXCOORD1;
    float4      vLook : TEXCOORD2;
    float4      vTranslation : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;

    Out.vPosition = In.vPosition;
    
    Out.vRight = In.vRight;
    Out.vUp = In.vUp;
    Out.vLook = In.vLook;
    Out.vTranslation = In.vTranslation;
    
    return Out;
}

struct GS_IN
{
    float3              vPosition : POSITION;

    float4              vRight : TEXCOORD0;
    float4              vUp : TEXCOORD1;
    float4              vLook : TEXCOORD2;
    float4              vTranslation : TEXCOORD3;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


[maxvertexcount(4)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutputStream)
{
    GS_OUT Out = (GS_OUT)0;

    matrix TransformMatrix = float4x4(In[0].vRight, In[0].vUp, In[0].vLook, In[0].vTranslation);
    
    float4 vPosition = float4(In[0].vPosition, 1.f);
    
    float4 vLook = float4(0.f, 0.f, 0.f, 0.5f);
    float4 vRight = float4(0.5f, 0.f, 0.f, 0.f);
    float4 vUp = float4(0.f, 0.5f, 0.f, 0.f);

    if (true == g_bBillBoard)
    {
        vLook = normalize(g_vCamPosition - vPosition);
        vRight = float4(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * 0.5f, 0.f) ;
        vUp = float4(normalize(cross(vLook.xyz, vRight.xyz)) * 0.5f, 0.f);
    }

    matrix matWVP = mul(mul(g_WorldMatrix, g_ViewMatrix), g_ProjMatrix);

    Out.vPosition = vPosition - vRight + vUp;
    Out.vPosition = mul(Out.vPosition, TransformMatrix);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vTexcoord = float2(0.f, 0.f);

    OutputStream.Append(Out);

    Out.vPosition = vPosition + vRight + vUp;
    Out.vPosition = mul(Out.vPosition, TransformMatrix);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vTexcoord = float2(1.f, 0.f);

    OutputStream.Append(Out);

    Out.vPosition = vPosition - vRight - vUp;
    Out.vPosition = mul(Out.vPosition, TransformMatrix);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vTexcoord = float2(0.f, 1.f);

    OutputStream.Append(Out);

    Out.vPosition = vPosition + vRight - vUp;
    Out.vPosition = mul(Out.vPosition, TransformMatrix);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vTexcoord = float2(1.f, 1.f);

    OutputStream.Append(Out);
}

struct GS_OUT_SOFT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

[maxvertexcount(4)]
void GS_MAIN_SOFT(point GS_IN In[1], inout TriangleStream<GS_OUT_SOFT> OutputStream)
{
    GS_OUT_SOFT Out = (GS_OUT_SOFT) 0;
    
    float4 vPosition = float4(In[0].vPosition, 1.f);

    float4	vLook = In[0].vLook;
    float4  vRight = In[0].vRight;
    float4  vUp = In[0].vUp;

    if (true == g_bBillBoard)
    {
        vLook = g_vCamPosition - vPosition;
        vRight = float4(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * length(vRight) * 0.5f, 0.f);
        vUp = float4(normalize(cross(vLook.xyz, vRight.xyz)) * length(vUp) * 0.5f, 0.f);
    }

    matrix TransformMatrix = float4x4(In[0].vRight, In[0].vUp, vLook, In[0].vTranslation);

    matrix  matWVP = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWVP, g_ProjMatrix);

    Out.vPosition = vPosition - vRight + vUp;
    Out.vPosition = mul(Out.vPosition, TransformMatrix);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vProjPos = Out.vPosition;
    Out.vTexcoord = float2(0.f, 0.f);

    OutputStream.Append(Out);

    Out.vPosition = vPosition + vRight + vUp;
    Out.vPosition = mul(Out.vPosition, TransformMatrix);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vProjPos = Out.vPosition;
    Out.vTexcoord = float2(1.f, 0.f);

    OutputStream.Append(Out);

    Out.vPosition = vPosition - vRight - vUp;
    Out.vPosition = mul(Out.vPosition, TransformMatrix);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vProjPos = Out.vPosition;
    Out.vTexcoord = float2(0.f, 1.f);

    OutputStream.Append(Out);

    Out.vPosition = vPosition + vRight - vUp;
    Out.vPosition = mul(Out.vPosition, TransformMatrix);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vProjPos = Out.vPosition;
    Out.vTexcoord = float2(1.f, 1.f);

    OutputStream.Append(Out);
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
    PS_OUT Out = (PS_OUT)0;

    Out.vColor = g_MaskTexture.Sample(LinearWrapSampler, In.vTexcoord);
    Out.vColor.gba = Out.vColor.r;
   
    Out.vColor.rgb *= g_vColor;
    Out.vColor.rgb /= Out.vColor.a;
    
    
    if (Out.vColor.a <= 0.1f)
        discard;
    return Out;
}

struct PS_IN_SOFT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

PS_OUT PS_MAIN_SOFT(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    Out.vColor = g_MaskTexture.Sample(LinearWrapSampler, In.vTexcoord) * g_vColor;

    return Out;
}


technique11 DefaultTechnique
{
    pass Default //0
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

    pass Soft //1
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_CullNone);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN_SOFT();
        PixelShader = compile ps_5_0 PS_MAIN_SOFT();
        ComputeShader = NULL;
    }
}
