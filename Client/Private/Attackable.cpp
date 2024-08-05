#include "Attackable.h"

CAttackable::CAttackable()
	: CComponent()
{
}

CAttackable::CAttackable(const CAttackable& rhs)
	: CComponent(rhs)
{
}

HRESULT CAttackable::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAttackable::Initialize(void* pArg)
{
	return S_OK;
}

CAttackable* CAttackable::Create()
{
	return new CAttackable;
}

CComponent* CAttackable::Clone(void* pArg)
{
	return new CAttackable(*this);
}

void CAttackable::Free()
{
	__super::Free();
}
