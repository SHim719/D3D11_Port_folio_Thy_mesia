#include "PlayerState_Sprint.h"


CPlayerState_Sprint::CPlayerState_Sprint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayerState_Base(pDevice, pContext)
{
}

HRESULT CPlayerState_Sprint::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CPlayerState_Sprint::OnState_Start()
{
	m_pOwnerTransform->Set_Speed(m_fSprintSpeed);
	m_pModel->Change_Animation(Corvus_SD1_Sprint);
}

void CPlayerState_Sprint::OnGoing(_float fTimeDelta)
{
	if (KEY_NONE(eKeyCode::W) && KEY_NONE(eKeyCode::A) && KEY_NONE(eKeyCode::S) && KEY_NONE(eKeyCode::D))
	{
		m_pPlayer->Change_State((_uint)PlayerState::State_Idle);
		return;
	}

	_vector vNewLook = Calc_NewLook();

	if (0.f == vNewLook.m128_f32[0] && 0.f == vNewLook.m128_f32[1])
		return;

	if (Check_StateChange(PlayerState::State_Attack))
	{
		m_pOwnerTransform->LookTo(vNewLook);
		m_pPlayer->Change_State((_uint)PlayerState::State_Attack);
		return;
	}

	m_pOwnerTransform->Set_MoveLook(vNewLook);

	Rotate_To_Look(vNewLook, fTimeDelta);

	m_pOwnerTransform->Go_Dir(vNewLook, fTimeDelta);
}

void CPlayerState_Sprint::OnState_End()
{
	m_pOwnerTransform->Set_Speed(m_fJogSpeed);
}

CPlayerState_Sprint* CPlayerState_Sprint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerState_Sprint* pInstance = new CPlayerState_Sprint(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerState_Sprint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_Sprint::Free()
{
	__super::Free();
}
