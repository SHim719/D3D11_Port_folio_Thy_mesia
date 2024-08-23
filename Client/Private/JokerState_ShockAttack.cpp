#include "JokerState_ShockAttack.h"
#include "Effect_Hit.h"
#include "Effect_Manager.h"
#include "Main_Camera.h"

CJokerState_ShockAttack::CJokerState_ShockAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CJokerState_Base(pDevice, pContext)
{
}

HRESULT CJokerState_ShockAttack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("Joker_Impact", bind(&CJokerState_ShockAttack::Shock_Attack, this));

	return S_OK;
}

void CJokerState_ShockAttack::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pOwnerTransform->LookAt2D(m_pTargetTransform->Get_Position());
	m_pJoker->Update_AttackDesc();

	PLAY_SOUND(L"Joker_Vocal_ShockAttack", false, 1.f);

	m_pModel->Change_Animation(Joker_ShockAttack);
}

void CJokerState_ShockAttack::Update(_float fTimeDelta)
{

}

void CJokerState_ShockAttack::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		m_pJoker->Change_State((_uint)JokerState::State_Walk);
}

void CJokerState_ShockAttack::OnState_End()
{
}

void CJokerState_ShockAttack::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	ATTACKDESC AttackDesc;
	AttackDesc.eEnemyAttackType = AIR;
	AttackDesc.iDamage = 120;
	AttackDesc.pAttacker = m_pJoker;

	m_AttackDescs.emplace_back(CJoker::HAMMER, AttackDesc);
}

void CJokerState_ShockAttack::Shock_Attack()
{
	CEffect_Hit::EFFECT_HITDESC EffectHitDesc{};
	CCollider::COLLIDERDESC ColliderDesc{};
	ColliderDesc.eType = CCollider::SPHERE;
	ColliderDesc.pOwner = m_pJoker;
	ColliderDesc.strCollisionLayer = "Enemy_Weapon";
	ColliderDesc.vSize = { 8.f, 5.f, 1.f };
	ColliderDesc.bActive = true;

	EffectHitDesc.pColliderDesc = &ColliderDesc;
	EffectHitDesc.AttackDesc = m_AttackDescs[0].second;

	CBone* pSpawnBone = m_pModel->Get_Bone("weapon_r_end");
	CALC_TF->Attach_To_Bone(pSpawnBone, m_pOwnerTransform);

	EffectHitDesc.EffectSpawnDesc.pParentModel = m_pModel;
	EffectHitDesc.EffectSpawnDesc.pParentTransform = m_pOwnerTransform;
	EffectHitDesc.iTag = TAG_ENEMY_WEAPON;
	XMStoreFloat4(&EffectHitDesc.vSpawnPos, CALC_TF->Get_Position());

	EFFECTMGR->Create_Effect_Hit("Effect_Joker_Impact", &EffectHitDesc);

	static_cast<CMain_Camera*>(GET_CAMERA)->Play_CameraShake("Shaking_Joker_Impact");

	PLAY_SOUND(L"Joker_Shock_Impact", false, 1.f);
}

CJokerState_ShockAttack* CJokerState_ShockAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CJokerState_ShockAttack* pInstance = new CJokerState_ShockAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CJokerState_ShockAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJokerState_ShockAttack::Free()
{
	__super::Free();
}
