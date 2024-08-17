#include "Effect_Group.h"
#include "Effect_Particle.h"
#include "Effect_Mesh.h"

CEffect_Group::CEffect_Group(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect_Group::CEffect_Group(const CEffect_Group& rhs)
	: CGameObject(rhs)
	, m_Effects(rhs.m_Effects)
{
}

HRESULT CEffect_Group::Initialize_Prototype(ifstream& fin)
{
	if (FAILED(Load_EffectGroup(fin)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Group::Initialize(void* pArg)
{
	for (auto& pEffect : m_Effects)
		pEffect = static_cast<CGameEffect*>(pEffect->Clone(pArg));

	return S_OK;
}

void CEffect_Group::Tick(_float fTimeDelta)
{
	for (auto& pEffect : m_Effects)
		pEffect->Tick(fTimeDelta);

}

void CEffect_Group::LateTick(_float fTimeDelta)
{
	for (auto& pEffect : m_Effects)
		pEffect->LateTick(fTimeDelta);

	Check_Destroy();
}


HRESULT CEffect_Group::OnEnter_Layer(void* pArg)
{
	m_bReturnToPool = false;
	m_bUsing = true;

	Start_Effect(pArg);
	
	return S_OK;
}

void CEffect_Group::Check_Destroy()
{
	for (auto& pEffect : m_Effects)
	{
		if (false == pEffect->Is_Complete())
			return;
	}

	End_Effect();
}


void CEffect_Group::Start_Effect(void* pArg)
{
	CGameEffect::EFFECTSPAWNDESC* SpawnDesc = (CGameEffect::EFFECTSPAWNDESC*)pArg;
	
	for (auto& pEffect : m_Effects)
		pEffect->Restart_Effect(SpawnDesc);
}

void CEffect_Group::End_Effect()
{
	for (auto& pEffect : m_Effects)
		pEffect->OnEnd_Effect();

	m_bUsing = false;
	m_bReturnToPool = true;
}

HRESULT CEffect_Group::Load_EffectGroup(ifstream& fin)
{
	_uint iNumEffects = 0;
	fin.read((_char*)&iNumEffects, sizeof(_uint));

	for (_uint i = 0; i < iNumEffects; ++i)
	{
		CGameEffect::EFFECTTYPE eEffectType;
		fin.read((_char*)&eEffectType, sizeof(CGameEffect::EFFECTTYPE));

		CGameEffect* pEffect = nullptr;
		switch (eEffectType)
		{
		case CGameEffect::PARTICLE:
			pEffect = CEffect_Particle::Create(m_pDevice, m_pContext, fin);
			break;
		case CGameEffect::MESH:
			pEffect = CEffect_Mesh::Create(m_pDevice, m_pContext, fin);
			break;
		}
		m_Effects.emplace_back(pEffect);
	}
	return S_OK;
}


CEffect_Group* CEffect_Group::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& fin)
{
	CEffect_Group* pInstance = new CEffect_Group(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(fin)))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Group"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Group::Clone(void* pArg)
{
	CEffect_Group* pInstance = new CEffect_Group(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffect_Group"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Group::Free()
{
	__super::Free();

	for (auto& pEffect : m_Effects)
		Safe_Release(pEffect);
}
