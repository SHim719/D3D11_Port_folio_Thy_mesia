#pragma once


namespace Engine
{
	typedef struct tagKeyFrame
	{
		float		fTime;

		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vPosition;
	}KEYFRAME;

	typedef struct
	{
		HWND			hWnd;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		bool			isWindowed;
	}GRAPHIC_DESC;


	typedef struct
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOT, TYPE_END };
		TYPE		eType;
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;

		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

	}LIGHT_DESC;

	typedef ENGINE_DLL struct tagParticleDesc
	{
		XMFLOAT3		vVelocity;
		float			fLifeTime;
	} PARTICLE_DESC;

	typedef ENGINE_DLL struct tagTrailDesc
	{
		_float3		vStartPos;
		_float3		vEndPos;
		_uint		iMaxVertexCount;
		_uint		iCatmullRomCount;
		_uint		iRemoveCount;
	} TRAIL_DESC;

	typedef struct tagLineIndices16
	{
		unsigned short		_0, _1;
	}LINEINDICES16;

	typedef struct tagMaterialDesc
	{
		class CTexture*		pTexture[TEXTURE_TYPE_MAX];
	}MATERIALDESC;

	typedef struct tagLineIndices32
	{
		unsigned long		_0, _1;
	}LINEINDICES32;

	typedef struct tagFaceIndices16
	{
		unsigned short		_0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_0, _1, _2;
	}FACEINDICES32;

	typedef struct tagVertexTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexture;
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertexTexture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXTEX_DECLARATION;

	typedef struct tagVertexNorTex
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexNorTexture_Declaration
	{
		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXNORTEX_DECLARATION;

	typedef struct tagVertexModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
		XMFLOAT3		vTangent;
	}VTXMODEL;

	typedef struct ENGINE_DLL tagVertexModel_Declaration
	{
		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXMODEL_DECLARATION;

	typedef struct ENGINE_DLL tagVertexModelInstance_Declaration
	{
		static const unsigned int	iNumElements = { 8 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMODELINSTANCE_DECLARATION;


	typedef struct tagVertexAnimModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndex; /* 이 정점에 영향을 주는 뼈의 인덱스 네개. */
		XMFLOAT4		vBlendWeight; /* 영향르 주고 있는 각 뼈대의 영향 비율 */
	}VTXANIMMODEL;

	typedef struct ENGINE_DLL tagVertexAnimModel_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXANIMMODEL_DECLARATION;

	typedef struct tagVertexPoint
	{
		XMFLOAT3		vPosition;
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertexPoint_Declaration
	{
		static const unsigned int iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPOINT_DECLARATION;


	typedef struct ENGINE_DLL tagVertexParticle_Declaration
	{
		static const unsigned int iNumElements = 9;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPARTICLE_DECLARATION;

	typedef struct tagVertexMatrix
	{
		XMFLOAT4		vRight, vUp, vLook, vTranslation;
	}VTXMATRIX;

	typedef struct tagVertexParticle
	{
		XMFLOAT4		vRight, vUp, vLook, vTranslation;
		XMFLOAT4		vColor;
		XMFLOAT3		vVelocity;
		float			fLifeTime;
		int				iSpriteIdx;
	}VTXPARTICLE;
}
