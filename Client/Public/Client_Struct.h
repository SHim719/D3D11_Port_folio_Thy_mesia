#pragma once

namespace Client
{
	enum ATTACKTYPE
	{
		WEAK,
		NORMAL,
		SEMIKNOCKBACK,
		KNOCKBACK,
	};

	typedef struct tagAttackDesc
	{
		ATTACKTYPE eAttackType;

	}ATTACKDESC;


	typedef struct tagEnemyDesc
	{
		int iMaxHp;
	}ENEMYDESC;
}
