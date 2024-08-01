#include "Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_BoneMatrices[256];

float			g_fAlpha;

texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;

texture2D		g_DissolveTexture;
float			g_fDissolveAmount;

float4			g_vGlowColor;
float			g_fGlowRange;
float			g_fGlowFalloff;

float4          g_vCamPosition;
float4          g_vRimColor;
float           g_fRimPower;
float           g_fRimStrength;


struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};




VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x
		+ g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y
		+ g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z
		+ g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

	vector		vBonePosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
    vector      vPosition = mul(vBonePosition, matWVP);

	Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix).xyz);
	Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(vBonePosition, g_WorldMatrix);
	Out.vProjPos = vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;	

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV);
	
    if (Out.vDiffuse.a < 0.1f)
        discard;
	
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MAIN_ALPHABLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV);
	
    if (Out.vDiffuse.a < 0.1f)
        discard;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
	
    Out.vDiffuse.a *= g_fAlpha;

    return Out;
}

struct PS_OUT_GLOW
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vGlow : SV_TARGET3;
};


PS_OUT_GLOW PS_MAIN_DISSOLVE(PS_IN In)
{
    PS_OUT_GLOW Out = (PS_OUT_GLOW) 0;

    float fDissolve = g_DissolveTexture.Sample(LinearWrapSampler, In.vTexUV).r;
    float fClip = fDissolve - g_fDissolveAmount;

    float4 vEdgeColor = float4(1.0f, 1.0f, 1.f, 1.0f);
    float fEdgeWidth = 0.05f;
	
    if (fClip < 0.001f)
        discard;
	
    else if (fClip < fEdgeWidth)
    {
        float fEdgeFactor = smoothstep(0.f, fEdgeWidth, fClip);
        Out.vDiffuse = lerp(vEdgeColor, g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV), fEdgeFactor);
        Out.vGlow = Out.vDiffuse;
        Out.vGlow.rgb *= float4(0.7f, 0.45f, 0.f, 1.f);
    }
    else
    {
        Out.vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV);
    }
	
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

    return Out;
}

PS_OUT_GLOW PS_MAIN_RIMLIGHT(PS_IN In)
{
    PS_OUT_GLOW Out = (PS_OUT_GLOW) 0;

    Out.vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV);
	
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
    float3 vToCamera = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    
    float fRim = abs(1.f - dot(In.vNormal, vToCamera));
    fRim = smoothstep(0.f, 1.f, fRim);
    fRim = pow(fRim, g_fRimPower);
    
    Out.vGlow = fRim * g_vRimColor * g_fRimStrength;
    
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

    pass AlphaBlend // 1
    {
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ALPHABLEND();
		ComputeShader = NULL;
    }

    pass Dissolve // 2
    {
        SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
        ComputeShader = NULL;
    }

    pass RimLight // 3
    {
        SetBlendState(BS_None, vector(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RIMLIGHT();
        ComputeShader = NULL;
    }
	
	
}

