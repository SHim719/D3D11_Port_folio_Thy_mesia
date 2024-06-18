#pragma once

namespace Engine
{
	class CBone;
	class CTransform;
}


namespace Client
{
	enum ATTACKTYPE
	{
		WEAK,
		NORMAL,
		SEMIKNOCKBACK,
		KNOCKBACK,
		BIG_HIT,
		VERY_BIG_HIT,
	};

	typedef struct tagAttackDesc
	{
		ATTACKTYPE eAttackType;

	}ATTACKDESC;


	typedef struct tagEnemyDesc
	{
		int iMaxHp;
	}ENEMYDESC;

	typedef struct tagAttachDesc
	{
		Engine::CBone*			pAttachBone = nullptr;
		Engine::CTransform*		pParentTransform = nullptr;
	}ATTACHDESC;


	typedef struct tagExecutedDesc
	{
		tagAttachDesc			AttachDesc;
		unsigned int			iExecutedAnimIndex = 0;
	}EXECUTEDDESC;
}
