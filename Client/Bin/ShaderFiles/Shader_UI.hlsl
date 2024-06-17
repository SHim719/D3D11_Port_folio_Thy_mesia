#include "Shader_Defines.hlsli"

matrix  g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;

float4  g_vColor;
float   g_fAlpha;

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
    float3 vPosition : POSITION;
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
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = vector(In[0].vPosition, 1.f) + vector(-0.5f, 0.5f, 0.f, 0.f);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vTexcoord = float2(0.f, 0.f);
    
    OutputStream.Append(Out);
    
    Out.vPosition = vector(In[0].vPosition, 1.f) + vector(0.5f, 0.5f, 0.f, 0.f);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vTexcoord = float2(1.f, 0.f);
  
    OutputStream.Append(Out);
    
    Out.vPosition = vector(In[0].vPosition, 1.f) + vector(-0.5f, -0.5f, 0.f, 0.f);
    Out.vPosition = mul(Out.vPosition, matWVP);
    Out.vTexcoord = float2(0.f, 1.f);
  
    OutputStream.Append(Out);
    
    Out.vPosition = vector(In[0].vPosition, 1.f) + vector(0.5f, -0.5f, 0.f, 0.f);
    Out.vPosition = mul(Out.vPosition, matWVP);
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
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vColor.a = Out.vColor.a * g_fAlpha;

    return Out;
}

PS_OUT PS_MAIN_REDTOALPHA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vColor.a = Out.vColor.r;
    
    ////Out.vColor.a = 1.f;
    //Out.vColor.a = Out.vColor.a * g_fAhlpaScale;
	//
    
    if (Out.vColor.a < 0.3f)
	    discard;

    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultUI //0
    {
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass REDTOALPHA //1
    {
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_REDTOALPHA();
    }

	
}