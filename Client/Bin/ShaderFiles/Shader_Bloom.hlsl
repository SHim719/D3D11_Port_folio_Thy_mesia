#include "Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

int     g_iDiv;

texture2D g_PrevBrightPassTexture;
texture2D g_ToSampleDownTexture;
texture2D g_ToSampleUpTexture;

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

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_BrightPass(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vOriginColor = g_PrevBrightPassTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    float4 vPassedColor = (float4) 0.f;

    if (vOriginColor.a < 0.3f)
        discard;
    
    float fBrightness = dot(vOriginColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    if (fBrightness > 0.9f)
        vPassedColor = vOriginColor;
    else
        discard;
   
    Out.vColor = vPassedColor;
    
    return Out;
}


PS_OUT PS_MAIN_DownSample(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float fX = In.vTexcoord.x;
    float fY = In.vTexcoord.y;
    float fdx = 1.f / (1280.f / float(g_iDiv)); //1280.f;   
    float fdy = 1.f / (720.f / float(g_iDiv)); //720.f;     

    float3 a = g_ToSampleDownTexture.Sample(LinearClampSampler, float2(fX - 2 * fdx, fY + 2 * fdy)).rgb;
    float3 b = g_ToSampleDownTexture.Sample(LinearClampSampler, float2(fX, fY + 2 * fdy)).rgb;
    float3 c = g_ToSampleDownTexture.Sample(LinearClampSampler, float2(fX + 2 * fdx, fY + 2 * fdy)).rgb;

    float3 d = g_ToSampleDownTexture.Sample(LinearClampSampler, float2(fX - 2 * fdx, fY)).rgb;
    float3 e = g_ToSampleDownTexture.Sample(LinearClampSampler, float2(fX, fY)).rgb;
    float3 f = g_ToSampleDownTexture.Sample(LinearClampSampler, float2(fX + 2 * fdx, fY)).rgb;

    float3 g = g_ToSampleDownTexture.Sample(LinearClampSampler, float2(fX - 2 * fdx, fY - 2 * fdy)).rgb;
    float3 h = g_ToSampleDownTexture.Sample(LinearClampSampler, float2(fX, fY - 2 * fdy)).rgb;
    float3 i = g_ToSampleDownTexture.Sample(LinearClampSampler, float2(fX + 2 * fdx, fY - 2 * fdy)).rgb;

    float3 j = g_ToSampleDownTexture.Sample(LinearClampSampler, float2(fX - fdx, fY + fdy)).rgb;
    float3 k = g_ToSampleDownTexture.Sample(LinearClampSampler, float2(fX + fdx, fY + fdy)).rgb;
    float3 l = g_ToSampleDownTexture.Sample(LinearClampSampler, float2(fX - fdx, fY - fdy)).rgb;
    float3 m = g_ToSampleDownTexture.Sample(LinearClampSampler, float2(fX + fdx, fY - fdy)).rgb;

    float3 color = e * 0.125;
    color += (a + c + g + i) * 0.03125;
    color += (b + d + f + h) * 0.0625;
    color += (j + k + l + m) * 0.125;

    Out.vColor = float4(color, 1.0);
   
    return Out;
}

PS_OUT PS_MAIN_UpSample(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
   
    float fX = In.vTexcoord.x;
    float fY = In.vTexcoord.y;
    float fdx = 1.f / (1280.f / float(g_iDiv)); //1280.f;//   
    float fdy = 1.f / (720.f / float(g_iDiv)); // 720.f;//    

    float3 a = g_ToSampleUpTexture.Sample(LinearClampSampler, float2(fX - fdx, fY + fdy)).rgb;
    float3 b = g_ToSampleUpTexture.Sample(LinearClampSampler, float2(fX, fY + fdy)).rgb;
    float3 c = g_ToSampleUpTexture.Sample(LinearClampSampler, float2(fX + fdx, fY + fdy)).rgb;

    float3 d = g_ToSampleUpTexture.Sample(LinearClampSampler, float2(fX - fdx, fY)).rgb;
    float3 e = g_ToSampleUpTexture.Sample(LinearClampSampler, float2(fX, fY)).rgb;
    float3 f = g_ToSampleUpTexture.Sample(LinearClampSampler, float2(fX + fdx, fY)).rgb;

    float3 g = g_ToSampleUpTexture.Sample(LinearClampSampler, float2(fX - fdx, fY - fdy)).rgb;
    float3 h = g_ToSampleUpTexture.Sample(LinearClampSampler, float2(fX, fY - fdy)).rgb;
    float3 i = g_ToSampleUpTexture.Sample(LinearClampSampler, float2(fX + fdx, fY - fdy)).rgb;

    float3 color = e * 4.0;
    color += (b + d + f + h) * 2.0;
    color += (a + c + g + i);
    color *= 1.0 / 16.0;

    Out.vColor = float4(color, 1.0);

    return Out;

}


technique11 DefaultTechnique
{
    pass BrightPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BrightPass();
    }

    pass DownSample // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DownSample();
    }


    pass UpSample // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NoZTest_And_Write, 0);
        SetBlendState(BS_None, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        ComputeShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UpSample();
    }

}

