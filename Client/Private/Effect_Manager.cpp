#include "Effect_Manager.h"
#include "Effect_Group.h"
#include "Effect_Hit.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
}

CEffect_Manager::~CEffect_Manager()
{
	Release();
}

HRESULT CEffect_Manager::Initialize()
{
	if (FAILED(Init_Effects()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Manager::Active_Effect(const string& strTag, void* pArg)
{
	auto it = m_EffectGroups.find(strTag);
	if (m_EffectGroups.end() == it)
		return;

	vector<CEffect_Group*>& EffectGroups = it->second;

	for (auto& pEffect_Group : EffectGroups)
	{
		if (false == pEffect_Group->Is_Using())
		{
			pEffect_Group->OnEnter_Layer(pArg);
			Safe_AddRef(pEffect_Group);

			m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Effect", pEffect_Group);
			//ADD_EVENT(bind(&CGameInstance::Insert_GameObject, m_pGameInstance, GET_CURLEVEL, L"Effect", pEffect_Group));
			return;
		}
	}

}

void CEffect_Manager::Inactive_Effect(const string& strTag, size_t iIdx)
{
	auto it = m_EffectGroups.find(strTag);
	if (m_EffectGroups.end() == it)
		return;

	vector<CEffect_Group*>& EffectGroups = it->second;

	EffectGroups[iIdx]->End_Effect();
}

void CEffect_Manager::Create_Effect_Hit(const string& strTag, void* pArg)
{
	if (nullptr == pArg)
		return;

	auto it = m_EffectGroups.find(strTag);
	if (m_EffectGroups.end() == it)
		return;

	vector<CEffect_Group*>& EffectGroups = it->second;

	for (auto& pEffect_Group : EffectGroups)
	{
		if (false == pEffect_Group->Is_Using())
		{
			CEffect_Hit::EFFECT_HITDESC* pEffectHitDesc = (CEffect_Hit::EFFECT_HITDESC*)pArg;

			pEffectHitDesc->pEffect_Group = pEffect_Group;
			Safe_AddRef(pEffect_Group);

			m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Effect", L"Prototype_Effect_Hit", pEffectHitDesc);
			return;
		}
	}
}

CEffect_Group* CEffect_Manager::Get_EffectGroup(const string& strTag)
{
	auto it = m_EffectGroups.find(strTag);
	if (m_EffectGroups.end() == it)
		return nullptr;

	vector<CEffect_Group*>& EffectGroups = it->second;

	for (auto& pEffect_Group : EffectGroups)
	{
		if (false == pEffect_Group->Is_Using())
			return pEffect_Group;
	}

	return nullptr;
}


HRESULT CEffect_Manager::Init_Effects()
{
#pragma region Corvus_Default
	if (FAILED(Add_EffectGroups("Effect_Corvus_Parry_Success", 4, L"Prototype_Effect_Corvus_Parry_Particle")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_Healing", 2, L"Prototype_Effect_Corvus_HealingCurve")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_Foot_Particle", 1, L"Prototype_Effect_Corvus_Foot_Particle")))
		assert(false);
#pragma endregion

#pragma region Corvus_Attack
	if (FAILED(Add_EffectGroups("Effect_Corvus_LAttack1", 2, L"Prototype_Effect_Corvus_LAttack1")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_LAttack2", 2, L"Prototype_Effect_Corvus_LAttack2")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_LAttack3", 2, L"Prototype_Effect_Corvus_LAttack3")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_LAttack4", 2, L"Prototype_Effect_Corvus_LAttack4")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_Claw_Long1", 2, L"Prototype_Effect_Claw_Long1")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_Claw_Long2", 2, L"Prototype_Effect_Claw_Long2")))
		assert(false);
#pragma endregion

#pragma region Plunder
	if (FAILED(Add_EffectGroups("Effect_Corvus_Charge_Start", 1, L"Prototype_Effect_Corvus_Charge_Start")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_Charge_Complete", 1, L"Prototype_Effect_Corvus_Charge_Complete")))
		assert(false);
	
	if (FAILED(Add_EffectGroups("Effect_Corvus_Plunder_Rush_Trail", 1, L"Prototype_Effect_Corvus_Plunder_Rush_Trail")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_Plunder_Start", 1, L"Prototype_Effect_Corvus_Plunder_Start")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_Plunder_Effect", 1, L"Prototype_Effect_Corvus_Plunder_Effect")))
		assert(false);

#pragma endregion

#pragma region PW_Axe
	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Axe", 1, L"Prototype_Effect_Corvus_PW_Axe")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Axe_StartParticle", 1, L"Prototype_Effect_Corvus_PW_Axe_StartParticle")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Axe_Impact", 1, L"Prototype_Effect_Corvus_PW_Axe_Impact")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Axe_Slash", 1, L"Prototype_Effect_Corvus_PW_Axe_Slash")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Axe_Disappear", 1, L"Prototype_Effect_Corvus_PW_Axe_Disappear")))
		assert(false);
#pragma endregion

#pragma region PW_Spear
	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Spear", 1, L"Prototype_Effect_Corvus_PW_Spear")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Spear_Start", 1, L"Prototype_Effect_Corvus_PW_Spear_Start")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Spear_Slash", 1, L"Prototype_Effect_Corvus_PW_Spear_Slash")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Spear_Particle", 1, L"Prototype_Effect_Corvus_PW_Spear_Particle")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Spear_Disappear", 1, L"Prototype_Effect_Corvus_PW_Spear_Disappear")))
		assert(false);
#pragma endregion

#pragma region PW_TwinBlade
	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_TwinBlade", 1, L"Prototype_Effect_Corvus_PW_TwinBlade")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_TwinBlade_Start", 1, L"Prototype_Effect_Corvus_PW_TwinBlade_Start")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_TwinBlade_Slash0", 1, L"Prototype_Effect_Corvus_PW_TwinBlade_Slash0")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_TwinBlade_Slash1", 1, L"Prototype_Effect_Corvus_PW_TwinBlade_Slash1")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_TwinBlade_Slash2", 1, L"Prototype_Effect_Corvus_PW_TwinBlade_Slash2")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_TwinBlade_Slash3", 1, L"Prototype_Effect_Corvus_PW_TwinBlade_Slash3")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_TwinBlade_Slash4", 1, L"Prototype_Effect_Corvus_PW_TwinBlade_Slash4")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_TwinBlade_Slash5", 1, L"Prototype_Effect_Corvus_PW_TwinBlade_Slash5")))
		assert(false);
	

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_TwinBlade_Slash_End", 1, L"Prototype_Effect_Corvus_PW_TwinBlade_Slash_End")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_TwinBlade_Disappear", 1, L"Prototype_Effect_Corvus_PW_TwinBlade_Disappear")))
		assert(false);

#pragma endregion

#pragma region PW_Hammer

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Hammer", 1, L"Prototype_Effect_Corvus_PW_Hammer")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Hammer_Start", 1, L"Prototype_Effect_Corvus_PW_Hammer_Start")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Hammer_Slash", 1, L"Prototype_Effect_Corvus_PW_Hammer_Slash")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Hammer_Impact", 1, L"Prototype_Effect_Corvus_PW_Hammer_Impact")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Hammer_Disappear", 1, L"Prototype_Effect_Corvus_PW_Hammer_Disappear")))
		assert(false);

#pragma endregion

#pragma region HitParticle
	if (FAILED(Add_EffectGroups("Effect_Enemy_Hit_Particle", 5, L"Prototype_Effect_Enemy_Hit_Particle")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Enemy_Parry_Particle", 1, L"Prototype_Effect_Enemy_Parry_Particle")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_TwinBlade_Hit", 1, L"Prototype_Effect_TwinBlade_Hit")))
		assert(false);
#pragma endregion

#pragma region Blood
	if (FAILED(Add_EffectGroups("Effect_Blood_L_Vill_F", 5, L"Prototype_Effect_Blood_L_Vill_F")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_R_Vill_F", 5, L"Prototype_Effect_Blood_R_Vill_F")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_L_Vill_M", 10, L"Prototype_Effect_Blood_L_Vill_M")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_R_Vill_M", 10, L"Prototype_Effect_Blood_R_Vill_M")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_L_Joker", 3, L"Prototype_Effect_Blood_L_Joker")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_R_Joker", 3, L"Prototype_Effect_Blood_R_Joker")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_L_Odur", 3, L"Prototype_Effect_Blood_L_Odur")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_R_Odur", 3, L"Prototype_Effect_Blood_R_Odur")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_VS_TwinBlade_Blood", 1, L"Prototype_Effect_Corvus_VS_TwinBlade_Blood")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_Execution_Vill_M", 2, L"Prototype_Effect_Blood_Execution_Vill_M")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_Execution_Vill_F", 2, L"Prototype_Effect_Blood_Execution_Vill_F")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_Execution_Halberd", 2, L"Prototype_Effect_Blood_Execution_Halberd")))
		assert(false);

#pragma endregion

#pragma region Joker

	if (FAILED(Add_EffectGroups("Effect_Joker_Impact", 1, L"Prototype_Effect_Joker_Impact")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Joker_StrongAttack_Impact", 1, L"Prototype_Effect_Joker_StrongAttack_Impact")))
		assert(false);

#pragma endregion

#pragma region Odur
	if (FAILED(Add_EffectGroups("Effect_Odur_Cane_Attack1_Cloak_Particle", 1, L"Prototype_Effect_Odur_Cane_Attack1_Cloak_Particle")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Odur_Cane_Attack1_Particle", 1, L"Prototype_Effect_Odur_Cane_Attack1_Particle")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Odur_Cane_Kick_Effect", 1, L"Prototype_Effect_Odur_Cane_Kick_Effect")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Odur_Cane_Kick_Particle", 1, L"Prototype_Effect_Odur_Cane_Kick_Particle")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Odur_Disappear_Particle", 1, L"Prototype_Effect_Odur_Disappear_Particle")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Odur_Execution_Blood", 1, L"Prototype_Effect_Odur_Execution_Blood")))
		assert(false);
	
	if (FAILED(Add_EffectGroups("Effect_Corvus_Execution_Odur_Blood", 1, L"Prototype_Effect_Corvus_Execution_Odur_Blood")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Odur_Execution_Knee", 1, L"Prototype_Effect_Odur_Execution_Knee")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Odur_Card_Explosion", 3, L"Prototype_Effect_Odur_Card_Explosion")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Odur_SP_Start", 1, L"Prototype_Effect_Odur_SP_Start")))
		assert(false);

#pragma endregion

#pragma region Urd
	if (FAILED(Add_EffectGroups("Effect_Urd_Cutscene_Sword_Impact", 1, L"Prototype_Effect_Urd_Cutscene_Sword_Impact")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Cutscene_Particle", 1, L"Prototype_Effect_Urd_Cutscene_Particle")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Sword_Effect", 10, L"Prototype_Effect_Urd_Sword_Effect")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Sword_Impact", 10, L"Prototype_Effect_Urd_Sword_Impact")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_MagicCircle_Appear", 10, L"Prototype_Effect_Urd_MagicCircle_Appear")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_MagicCircle_Impact", 10, L"Prototype_Effect_Urd_MagicCircle_Impact")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Ready_SP01", 1, L"Prototype_Effect_Urd_Ready_SP01")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Sword_Default", 1, L"Prototype_Effect_Urd_Sword_Default")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Step_Particle", 3, L"Prototype_Effect_Urd_Step_Particle")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_DefaultAttack_Particle1", 1, L"Prototype_Effect_Urd_DefaultAttack_Particle1")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_DefaultAttack_Particle2", 1, L"Prototype_Effect_Urd_DefaultAttack_Particle2")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Pierce", 5, L"Prototype_Effect_Urd_Pierce")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Pierce_Start", 2, L"Prototype_Effect_Urd_Pierce_Start")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Slash_Horizontal", 1, L"Prototype_Effect_Urd_Slash_Horizontal")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Slash_Vertical", 1, L"Prototype_Effect_Urd_Slash_Vertical")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Dash_Stab", 1, L"Prototype_Effect_Urd_Dash_Stab")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Strong_Stab", 1, L"Prototype_Effect_Urd_Strong_Stab")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Strong_Stab_Particle", 1, L"Prototype_Effect_Urd_Strong_Stab_Particle")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_VS_Urd_Spark", 1, L"Prototype_Effect_Corvus_VS_Urd_Spark")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Execution_Blood", 1, L"Prototype_Effect_Urd_Execution_Blood")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Urd_Execution_Blood_Phase2", 1, L"Prototype_Effect_Urd_Execution_Blood_Phase2")))
		assert(false);
	
#pragma endregion

	return S_OK;
};

HRESULT CEffect_Manager::Add_EffectGroups(const string& strEffectGroupTags, _uint iNumGroups, const wstring& wstrPrototypeTag)
{
	vector<CEffect_Group*> vecGroups;
	vecGroups.reserve(iNumGroups);

	for (_uint i = 0; i < iNumGroups; ++i)
	{
		CEffect_Group* pEffectGroup = static_cast<CEffect_Group*>(m_pGameInstance->Clone_GameObject(wstrPrototypeTag));
		if (nullptr == pEffectGroup)
			return E_FAIL;
		vecGroups.emplace_back(pEffectGroup);
	}

	m_EffectGroups.emplace(strEffectGroupTags, vecGroups);

	return S_OK;
}

void CEffect_Manager::Release()
{
	for (auto& Pair : m_EffectGroups)
		for (auto& pEffect_Group : Pair.second)
			Safe_Release(pEffect_Group);
		

}
