#ifndef SHADER_DEFINE
#define SHADER_DEFINE

sampler LinearWrapSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler LinearClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};

sampler LinearBorderSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
};

sampler PointWrapSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};


sampler PointClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = clamp;
    AddressV = clamp;
};


RasterizerState RS_Default
{
	FillMode = solid;
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState RS_CullNone
{
    FillMode = solid;
    CullMode = none;
    FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
	FillMode = wireframe;
	CullMode = back;
	FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_NoZTest_And_Write
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

DepthStencilState DSS_Effect
{
    DepthEnable = true;
    DepthWriteMask = zero;
};

DepthStencilState DSS_WriteStencil
{
    DepthEnable = true;
    DepthWriteMask = zero; 
    DepthFunc = always;

    StencilEnable = true;
    StencilReadMask = 0xff;
    StencilWriteMask = 0xff;

    FrontFaceStencilFunc = always;
    FrontFaceStencilPass = replace;
    FrontFaceStencilFail = keep;

    BackFaceStencilFunc = always;
    BackFaceStencilPass = replace;
    BackFaceStencilFail = keep;
};

BlendState BS_None
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    SrcBlend[0] = SRC_ALPHA;
    DestBlend[0] = INV_SRC_ALPHA;
    BlendOp[0] = Add;
};

BlendState BS_EffectBlend
{
    BlendEnable[0] = true;
	// BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = one;
    BlendOp = Add;
};

BlendState BS_OneBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};




#endif
