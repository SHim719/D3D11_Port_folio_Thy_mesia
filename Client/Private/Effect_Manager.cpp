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

			m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Effect", L"Prototype_Effect_Hit");
			return;
		}
	}
}


HRESULT CEffect_Manager::Init_Effects()
{
#pragma region Corvus_Default
	if (FAILED(Add_EffectGroups("Effect_Corvus_Parry_Success", 3, L"Prototype_Effect_Corvus_Parry_Particle")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_Healing", 1, L"Prototype_Effect_Corvus_HealingCurve")))
		assert(false);
#pragma endregion

#pragma region Corvus_Attack
	if (FAILED(Add_EffectGroups("Effect_Corvus_LAttack1", 1, L"Prototype_Effect_Corvus_LAttack1")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_LAttack2", 1, L"Prototype_Effect_Corvus_LAttack2")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_LAttack3", 1, L"Prototype_Effect_Corvus_LAttack3")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_LAttack4", 1, L"Prototype_Effect_Corvus_LAttack4")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_Claw_Long1", 1, L"Prototype_Effect_Claw_Long1")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_Claw_Long2", 1, L"Prototype_Effect_Claw_Long2")))
		assert(false);
#pragma endregion

#pragma region Plunder

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

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Hammer_Slash", 1, L"Prototype_Corvus_PW_Hammer_Slash")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Hammer_Impact", 1, L"Prototype_Effect_Corvus_PW_Hammer_Impact")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Corvus_PW_Hammer_Disappear", 1, L"Prototype_Effect_Corvus_PW_Hammer_Disappear")))
		assert(false);

#pragma endregion

#pragma region HitParticle
	if (FAILED(Add_EffectGroups("Effect_Enemy_Hit_Particle", 5, L"Prototype_Effect_Enemy_Hit_Particle")))
		assert(false);
#pragma endregion

#pragma region Blood
	if (FAILED(Add_EffectGroups("Effect_Blood_L_TwinBladeKnight", 3, L"Prototype_Effect_Blood_L_TwinBladeKnight")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_R_TwinBladeKnight", 3, L"Prototype_Effect_Blood_R_TwinBladeKnight")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_L_Vill_F", 3, L"Prototype_Effect_Blood_L_Vill_F")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_R_Vill_F", 3, L"Prototype_Effect_Blood_R_Vill_F")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_L_Vill_M", 3, L"Prototype_Effect_Blood_L_Vill_M")))
		assert(false);

	if (FAILED(Add_EffectGroups("Effect_Blood_R_Vill_M", 3, L"Prototype_Effect_Blood_R_Vill_M")))
		assert(false);

#pragma endregion

	return S_OK;
};

HRESULT CEffect_Manager::Add_EffectGroups(const string& strEffectGroupTags, _uint iNumGroups, const wstring& wstrPrototypeTag)
{
	vector<CEffect_Group*> vecGroups;
	vecGroups.reserve(iNumGroups);

	for (_uint i = 0; i < iNumGroups; ++i)
		vecGroups.emplace_back(static_cast<CEffect_Group*>(m_pGameInstance->Clone_GameObject(wstrPrototypeTag)));

	m_EffectGroups.emplace(strEffectGroupTags, vecGroups);

	return S_OK;
}

void CEffect_Manager::Release()
{
	for (auto& Pair : m_EffectGroups)
		for (auto& pEffect_Group : Pair.second)
			Safe_Release(pEffect_Group);
		

}
