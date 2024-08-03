#pragma once

#include "Engine_Defines.h"

namespace Engine
{
	class CBone;
	class CTransform;
}


namespace Client
{
	enum ENEMYATTACKTYPE
	{
		WEAK,
		NORMAL,
		SEMIKNOCKBACK,
		KNOCKBACK,
		KNOCKDOWN,
		BIG_HIT,
		VERY_BIG_HIT,
		ENEMYATTACKTYPE_END,
	};

	enum PLAYERATTACKTYPE
	{
		ATTACK_NORMAL,
		PLUNDER,
		IGNORE_STANCE,
		PLAYERATTACKTYPE_END,
	};

	typedef struct tagAttackDesc
	{
		class CCharacter*	pAttacker = nullptr;
		ENEMYATTACKTYPE		eEnemyAttackType;
		PLAYERATTACKTYPE	ePlayerAttackType = ATTACK_NORMAL;
		_int				iDamage = 0;
		_int				iPlagueDamage = 0;
	}ATTACKDESC;


	typedef struct tagEnemyDesc
	{
		wstring wstrEnemyName = L"";
		int		iMaxHp = 0;
		_bool   bIsBoss = { false };
	}ENEMYDESC;

	typedef struct tagAttachDesc
	{
		Engine::CBone* pAttachBone = nullptr;
		Engine::CTransform* pParentTransform = nullptr;
	}ATTACHDESC;


	typedef struct tagExecutedDesc
	{
		tagAttachDesc			AttachDesc;
		unsigned int			iExecutedAnimIndex = 0;
	}EXECUTEDDESC;

	typedef struct tagLoadObjectDesc
	{
		OBJTYPE		eObjType;
		_float4x4	WorldMatrix;
		_int		iNaviIdx;
		_int		iTriggerIdx;
		_float3		vColliderSize;
		_tchar		szModelTag[MAX_PATH];
	}LOADOBJDESC;

	typedef struct tagLadderDesc
	{
		_float4x4*	pWorldMatrix;
		_float4		vOffset;
		_uint		iTargetNaviIdx;
		_uint		iLadderHeight;
		_uint		iStartHeight;
		_uint		iPlayerAnimIdx;
	}LADDERDESC;

	typedef struct tagEffectDesc
	{
		_uint			m_eEffectType;

		_float			m_fPlayTime = { 0.f };
		_bool			m_bComplete = { false };

		_int			m_iBaseTextureIdx = { 0 };
		_int			m_iMaskTextureIdx = { -1 };
		_int			m_iNoiseTextureIdx = { -1 };
		_int			m_iEmissiveTextureIdx = { -1 };
		_int			m_iPassIdx = { 0 };

		_int			m_iRenderGroup = 5;

		_float4			m_vTextureFlag = { 0.f, 0.f, 0.f, 0.f };

		_bool			m_bLoop = { true };

		_float4			m_vClipColor = { 0.f, 0.f, 0.f, 0.f };

		_bool			m_bGlow = { false };
		_float4			m_vGlowColor = { 1.f, 1.f, 1.f, 1.f };
		_float			m_fGlowIntensity = { 1.f };

		_bool			m_bBloom = { false };
	}EFFECT_DESC;

	typedef struct tagParticleDesc
	{
		_float3		vSpeedDir;
		_float		fSpeed;
		_float3		vForceDir;
		_float		fForceScale;
		_float3		vRotation;
		_float3		vRotationSpeed;
		_float		fSpawnTime;
	}PARTICLE_DESC;


	typedef struct tagParticleInfo
	{
		_int			iParticleType = { 0 };
		_int			iNumParticles = { 0 };

		_int			iParticleMode = { 0 };
		_bool			bParticleLoop = { false };

		_float3			vStartPosMin = { 0.f, 0.f, 0.f };
		_float3			vStartPosMax = { 0.f, 0.f, 0.f };

		_float2			vStartSizeMin = { 1.f, 1.f };
		_float2			vStartSizeMax = { 1.f, 1.f };

		_float3			vSpeedDirMin = { 0.f, 0.f, 0.f };
		_float3			vSpeedDirMax = { 0.f, 0.f, 0.f };

		_float			fStartSpeedMin = { 0.f };
		_float			fStartSpeedMax = { 0.f };

		_float4			vStartColorMin = { 1.f, 1.f, 1.f, 1.f };
		_float4			vStartColorMax = { 1.f, 1.f, 1.f, 1.f };

		_float3			vStartRotationMin = { 0.f, 0.f, 0.f };
		_float3			vStartRotationMax = { 0.f, 0.f, 0.f };

		_float3			vRotationSpeedMin = { 0.f, 0.f, 0.f };
		_float3			vRotationSpeedMax = { 0.f, 0.f, 0.f };

		_float3			vForceDirMin = { 0.f, 0.f, 0.f };
		_float3			vForceDirMax = { 0.f, 0.f, 0.f };
		_float			fForceScaleMin = { 0.f };
		_float			fForceScaleMax = { 0.f };

		_float			fLifeTimeMin = { 1.f };
		_float			fLifeTimeMax = { 1.f };

		_float			fSpawnTimeMin = { 0.f };
		_float			fSpawnTimeMax = { 0.f };

		_bool			bSpeedLerp = { false };
		_float			fSpeedEnd = { 0.f };

		_bool			bSizeLerp = { false };
		_float2			vSizeEnd = { 0.f, 0.f };

		_bool			bColorLerp = { false };
		_float4			vColorEnd = { 0.f, 0.f, 0.f, 0.f };

		_bool			bSprite = { false };
		_int			iCol = { 0 };
		_int			iRow = { 0 };
	}PARTICLE_INFO;

	typedef struct tagMeshEffectInfo
	{
		_tchar		szModelTag[MAX_PATH];
		_float		fSpawnTime = { 0.f };

		_float4		vColor = { 1.f, 1.f, 1.f, 1.f };

		_bool		bColorLerp = { false };
		_float4		vStartColor = { 1.f, 1.f, 1.f, 1.f };
		_float4		vColorEnd = { 1.f, 1.f, 1.f, 1.f };

		_float3		vStartPosition = { 0.f, 0.f, 0.f };

		_bool		bLocal = { true };
		_float		fTurnSpeed = { 0.f };
		_float4		vRotationAxis = { 0.f, 0.f, 0.f, 0.f };

		_bool		bRotationLerp = { false };
		_float3		vStartRotation = { 0.f, 0.f, 0.f };
		_float3		vRotationEnd = { 0.f, 0.f, 0.f };
	
		_bool		bScaleLerp = { false };
		_float3		vStartScale = { 1.f, 1.f, 1.f };
		_float3		vScaleEnd = { 1.f, 1.f, 1.f };

		_float2		vStartMaskUVOffset = { 0.f, 0.f };
		_float2     vMaskUVSpeed = { 0.f, 0.f };

		_float2		vStartNoiseUVOffset = { 0.f, 0.f };
		_float2		vNoiseUVSpeed = { 0.f, 0.f };

	}MESHEFFECT_INFO;


	typedef struct tagRimLightDesc
	{
		_float4         vRimColor;
		float           fRimPower;
		float           fRimStrength;
		float			fDuration;
		_bool			bColorLerp;
	} RIMLIGHTDESC;
}
