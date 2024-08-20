#include "VillagerMState_Attack.h"

CVillagerMState_Attack::CVillagerMState_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVillagerMState_Base(pDevice, pContext)
{
}

HRESULT CVillagerMState_Attack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("Sound_AttackVoice", bind(&CGameInstance::Play_RandomSound, m_pGameInstance, L"VillagerM_Voice_Attack", 1, 3, false, 1.f));
	m_pModel->Bind_Func("Sound_AxeWoosh", bind(&CGameInstance::Play_RandomSound, m_pGameInstance, L"Villager_WhooshOneHandAxe", 1, 2, false, 1.f));

	return S_OK;
}

void CVillagerMState_Attack::OnState_Start(void* pArg)
{
	Reset_AttackIdx();
	m_pVillager_M->Set_Stanced(true);
	m_pVillager_M->Set_LookTarget(true);
	m_pVillager_M->Update_AttackDesc();

	Decide_Attack();
}

void CVillagerMState_Attack::Update(_float fTimeDelta)
{
	if (!m_pVillager_M->Is_CollPlayer())
		m_pOwnerTransform->Move_Root(m_pModel->Get_DeltaRootPos(), m_pNavigation);
}

void CVillagerMState_Attack::Late_Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
		Decide_State();
}

void CVillagerMState_Attack::OnState_End()
{

}

void CVillagerMState_Attack::Init_AttackDesc()
{
	m_AttackDescs.reserve(1);

	ATTACKDESC AttackDesc;
	AttackDesc.iDamage = 70;
	AttackDesc.eEnemyAttackType = NORMAL;
	AttackDesc.pAttacker = m_pVillager_M;

	m_AttackDescs.emplace_back(CVillager_M::AXE, AttackDesc);
}

void CVillagerMState_Attack::Decide_Attack()
{
	_int iRandNum = JoRandom::Random_Int(0, 3);

	m_pModel->Change_Animation(LV1Villager_M_Attack01 + iRandNum);
}

CVillagerMState_Attack* CVillagerMState_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CVillagerMState_Attack* pInstance = new CVillagerMState_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CVillagerMState_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillagerMState_Attack::Free()
{
	__super::Free();
}
