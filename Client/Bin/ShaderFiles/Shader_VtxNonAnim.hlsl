
#include "Shader_Defines.hlsli"

// float4x4
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;

float4			g_vPickingColor;
float			g_fAlpha;

texture2D       g_BaseTexture;
texture2D       g_MaskTexture;
texture2D       g_NoiseTexture;
texture2D       g_DissolveTexture;
float2          g_vMaskUVOffset;
float2          g_vNoiseUVOffset;
float           g_fDissolveAmount;

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
	float4		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;	

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearWrapSampler, In.vTexUV);
	
    if (Out.vDiffuse.a < 0.1f)
        discard;
	
	vector vNormalDesc = g_NormalTexture.Sample(LinearWrapSampler, In.vTexUV);
	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	vNormal = mul(vNormal, WorldMatrix);
    
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 100.f, 0.f, 0.f);

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

struct VS_IN_GLOW
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT_GLOW
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT_GLOW VS_MAIN_AISEMY(VS_IN_GLOW In)
{
    VS_OUT_GLOW Out = (VS_OUT_GLOW) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    vector vPosition = mul(float4(In.vPosition, 1.f), matWVP);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    return Out;
}

struct PS_IN_GLOW
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_GLOW
{
    float4 vColor : SV_TARGET0;
    float4 vGlow : SV_TARGET1;
    //float4 vBloom : SV_TARGET2;
};

PS_OUT_GLOW PS_MAIN_AISEMY(PS_IN_GLOW In)
{
    PS_OUT_GLOW Out = (PS_OUT_GLOW) 0;
    
    float fNoise = 1.f;
    float fDissolve = 0.f;

    fDissolve = g_DissolveTexture.Sample(LinearWrapSampler, In.vTexcoord);
    float fClip = fDissolve - g_fDissolveAmount;
    if (fClip < 0.001f)
        discard;
  
    float2 vNoiseUV = In.vTexcoord + g_vNoiseUVOffset;
    fNoise = g_NoiseTexture.Sample(LinearWrapSampler, vNoiseUV).r;
       
    float4 vColor = g_BaseTexture.Sample(LinearWrapSampler, In.vTexcoord);
    float4 vColor_Mul = float4(0.f, 1.f, 0.5f, 1.f);
    
    vColor *= fNoise * vColor_Mul;

    if (vColor.a <= 0.f)
        discard;
    
    Out.vColor = vColor;
   
    float4 vGlowColor = float4(0.f, 0.3f, 0.5f, 1.f);
    
    float3 vGlow = vGlowColor.rgb;
    Out.vGlow = Out.vColor;
    Out.vGlow.rgb *= vGlow;
    
    return Out;
}


PS_OUT_GLOW PS_MAIN_GLOW(PS_IN_GLOW In)
{
    PS_OUT_GLOW Out = (PS_OUT_GLOW) 0;
    
    float4 vColor = float4(0.5f, 0.5f, 0.f, 1.f);

    if (vColor.a <= 0.f)
        discard;
    
    Out.vColor = vColor;
   
    float4 vGlowColor = float4(1.f, 1.f, 0.f, 1.f);
    
    float3 vGlow = vGlowColor.rgb;
    Out.vGlow = Out.vColor;
    Out.vGlow.rgb *= vGlow * 5.f;
    
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

    pass Aisemy // 3
    {
        SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN_AISEMY();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_AISEMY();
        ComputeShader = NULL;
    }

    pass Glow // 4
    {
        SetBlendState(BS_None, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN_AISEMY();
        HullShader = NULL;
        GeometryShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLOW();
        ComputeShader = NULL;
    }

 
	
}

