#pragma once

namespace Client
{
	enum ATTACKTYPE
	{
		WEAK,
		NORMAL,
		KNOCKBACK,
	};

	typedef struct tagAttackDesc
	{
		ATTACKTYPE eAttackType;

	}ATTACKDESC;
}
