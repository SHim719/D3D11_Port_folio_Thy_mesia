
#include "Shader_Defines.hlsli"

// float4x4
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;

float4			g_vColor;
float4			g_vPickingColor;
float			g_fAlpha;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float3		vTangent : TANGENT;
	float3		vBinormal: BINORMAL;

};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector		vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vPosition = vPosition;
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix).xyz);
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix).xyz);
	Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
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
	float3		vTangent : TANGENT;
	float3		vBinormal: BINORMAL;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	//float4		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;	

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV);
	
    if (Out.vDiffuse.a < 0.1f)
        discard;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	
	
	//vector vNormalDesc = g_NormalTexture.Sample(LinearWrapSampler, In.vTexUV);
	//
	//float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	//float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

	//vNormal = mul(vNormal, WorldMatrix);

	//Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MAIN_ALPHABLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV);

    Out.vDiffuse.a *= g_fAlpha;
	
    if (Out.vDiffuse.a < 0.1f)
        discard;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	
	//vector vNormalDesc = g_NormalTexture.Sample(LinearWrapSampler, In.vTexUV);
	//
	//float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	//float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

	//vNormal = mul(vNormal, WorldMatrix);

	//Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
	
   
	

    return Out;
}

PS_OUT PS_MAIN_PLAGUEWEAPON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vColor = float4(134.f, 217.f, 196.f, 1.f) / 255.f;
    vColor.a = 1.f;
	
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV) * vColor;
   
    if (Out.vDiffuse.a < 0.1f)
        discard;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    return Out;
}

struct VS_OUT_PICKING
{
    float4 vPosition : SV_POSITION;
};

VS_OUT_PICKING VS_MAIN_PICKING(VS_IN In)
{
    VS_OUT_PICKING Out = (VS_OUT_PICKING) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

    return Out;
}

struct PS_IN_PICKING
{
    float4 vPosition : SV_POSITION;
};

struct PS_OUT_PICKING
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_PICKING PS_MAIN_PICKING(PS_IN_PICKING In)
{
    PS_OUT_PICKING Out = (PS_OUT_PICKING) 0;

    Out.vColor = g_vPickingColor;

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

	pass Picking // 1
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN_PICKING();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_PICKING();
		ComputeShader = NULL;
	}


    pass AlphaBlend // 2
    {
        SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ALPHABLEND();
		ComputeShader = NULL;
    }

    pass PlagueWeapon // 3
    {
        SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PLAGUEWEAPON();
		ComputeShader = NULL;
    }
	
}

