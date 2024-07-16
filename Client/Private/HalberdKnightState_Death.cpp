#include "HalberdKnightState_Death.h"

#include "Player.h"

CHalberdKnightState_Death::CHalberdKnightState_Death(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CHalberdKnightState_Base(pDevice, pContext)
{
}

HRESULT CHalberdKnightState_Death::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CHalberdKnightState_Death::OnState_Start(void* pArg)
{
	m_pHalberdKnight->Set_LookTarget(false);
	m_pHalberdKnight->Set_Active_Colliders(false);
	m_pHalberdKnight->InActive_StunnedMark();

	m_pModel->Change_Animation(HArmorLV1_Halberds_Dead, 0.1f, false);
}

void CHalberdKnightState_Death::Update(_float fTimeDelta)
{
	if (false == m_pModel->Is_AnimComplete())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CHalberdKnightState_Death::Late_Update(_float fTimeDelta)
{
}

void CHalberdKnightState_Death::OnState_End()
{
}

void CHalberdKnightState_Death::OnHit(const ATTACKDESC& AttackDesc)
{
}

CHalberdKnightState_Death* CHalberdKnightState_Death::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CHalberdKnightState_Death* pInstance = new CHalberdKnightState_Death(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHalberdKnightState_Death"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHalberdKnightState_Death::Free()
{
	__super::Free();
}
