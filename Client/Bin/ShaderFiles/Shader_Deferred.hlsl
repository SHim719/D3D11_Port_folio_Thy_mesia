#include "Shader_Defines.hlsli"

float4x4    g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4    g_ViewMatrixInv, g_ProjMatrixInv;
float4      g_vCamPosition;

vector      g_vLightDir;
vector      g_vLightPos;
float       g_fLightRange;
float4      g_vLightAmbient;
float4      g_vLightDiffuse;
float4      g_vLightSpecular;
float       g_fLightStrength;

texture2D   g_NormalTexture;

texture2D   g_DiffuseTexture;
texture2D   g_DepthTexture;
texture2D   g_ShadeTexture;

texture2D g_Texture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    Out.vPosition = float4(In.vPosition, 1.f);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

VS_OUT VS_MAIN_DEBUG(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix MatWVP = mul(g_WorldMatrix, mul(g_ViewMatrix, g_ProjMatrix));
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), MatWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
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

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearWrapSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointClampSampler, In.vTexcoord);

	/* 0 ~ 1 -> -1 ~ 1 */
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
   
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient);
    Out.vShade *= g_fLightStrength;
    Out.vShade.a = 1.f;

    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(PointClampSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(PointClampSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1/w == 투영스페이스 상의 위치를 구하낟.*/
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos = vWorldPos * (vDepthDesc.y * 100.f);

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightDir = vWorldPos - g_vLightPos;

    float fDistance = length(vLightDir);

	/* 픽셀의 월드위치과 광원의 위치가 가까우면 1에 가깝게. 광원의 범위보다 멀어지면 0으로. */
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + g_vLightAmbient) * fAtt;
    Out.vShade *= g_fLightStrength;

    //vector vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	
   // vector vLook = vWorldPos - g_vCamPosition;

    //Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f) * fAtt;

    return Out;
}


PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord);
 
    vector vShade = g_ShadeTexture.Sample(LinearClampSampler, In.vTexcoord);

    Out.vColor = vDiffuse * vShade;
    
    float fViewZ = g_DepthTexture.Sample(LinearClampSampler, In.vTexcoord).y * 100.f;
        
    float3 vFogColor = float3(0.1f, 0.1f, 0.1f);
    float fFogMin = 5.f;
    float fFogMax = 30.f;
    
    float distFog = saturate((fViewZ - fFogMin) / (fFogMax - fFogMin));
    float fogFactor = exp(-distFog * 2.5f);

    float3 color = lerp(vFogColor, Out.vColor.rgb, fogFactor);
    
    Out.vColor.rgb = color;
  
    return Out;
}


technique11 DefaultTechnique
{
    pass RenderTarget_Debug // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN_DEBUG();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_OneBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_OneBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Deferred //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }

}

