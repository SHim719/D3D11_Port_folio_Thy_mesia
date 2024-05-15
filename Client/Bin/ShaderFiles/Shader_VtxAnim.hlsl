
#include "Shader_Defines.hlsli"

// float4x4
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_VTF;

texture2D		g_DiffuseTexture;

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

//matrix Get_BoneMatrix(uint iIdx, texture2D VTFTex)
//{
//    float4 uvCol = float4(((float) ((iIdx % 16) * 4) + 0.5f) / 64.0f, ((float) ((iIdx / 16)) + 0.5f) / 64.0f, 0.0f, 0.0f);
//	
//    matrix boneMatrix =
//    {
//        VTFTex.SampleLevel(DefaultSampler, uvCol, 0),
//		VTFTex.SampleLevel(DefaultSampler, uvCol + float4(1.0f / 64.0f, 0, 0, 0), 0),
//		VTFTex.SampleLevel(DefaultSampler, uvCol + float4(2.0f / 64.0f, 0, 0, 0), 0),
//		VTFTex.SampleLevel(DefaultSampler, uvCol + float4(3.0f / 64.0f, 0, 0, 0), 0)
//    };
//
//    return boneMatrix;
//}


matrix GetBoneMatrix(int idx, texture2D tex2D)
{
    float2 uvCol = float2(((float)(idx % 16 * 4) + 0.5f) / 64.0f, ((float)(idx / 16) + 0.5f) / 64.0f);

    matrix mat =
    {
        tex2D.SampleLevel(DefaultSampler, uvCol, 0),
		tex2D.SampleLevel(DefaultSampler, uvCol + float2(1.0f / 64.0f, 0.f), 0),
		tex2D.SampleLevel(DefaultSampler, uvCol + float2(2.0f / 64.0f, 0.f), 0),
		tex2D.SampleLevel(DefaultSampler, uvCol + float2(3.0f / 64.0f, 0.f), 0)
    };

    return mat;
}


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    matrix BoneMatrix =  GetBoneMatrix(In.vBlendIndex.x, g_VTF) * In.vBlendWeight.x
		+ GetBoneMatrix(In.vBlendIndex.y, g_VTF) * In.vBlendWeight.y
		+ GetBoneMatrix(In.vBlendIndex.z, g_VTF) * In.vBlendWeight.z
		+ GetBoneMatrix(In.vBlendIndex.w, g_VTF) * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
	vPosition = mul(vPosition, matWVP);

	Out.vPosition = vPosition;
	Out.vNormal = mul(vNormal, g_WorldMatrix).xyz;
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
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	//float4		vNormal : SV_TARGET1;
	//float4		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;	

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	if (Out.vDiffuse.a < 0.1f)
		discard;

	return Out;
}



technique11 DefaultTechinque
{
	pass Default
	{
		SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}	
	
}

