#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float3 vTangent : TANGENT;
    
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matVP, matWVP;

    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    matrix  TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    
    Out.vPosition = mul(vPosition, matVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), TransformMatrix).xyz);
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), TransformMatrix);
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	//float4		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV);
  
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);


    return Out;
}



technique11 DefaultTechinque
{
    pass Default // 0
    {
        SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }


}



