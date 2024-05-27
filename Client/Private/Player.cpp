#include "Player.h"


CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->GetKeyDown(eKeyCode::Q))
	{
		_uint iNextAnimIdx = (m_pModels[PART_BODY]->Get_CurrentAnimIndex() + 1) % m_pModels[PART_BODY]->Get_NumAnimations();
		//m_pModels[PART_BODY]->Set_AnimIndex(iNextAnimIdx);
	}
}

void CPlayer::LateTick(_float fTimeDelta)
{
	m_pModels[PART_BODY]->Play_Animation(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer::Render()
{

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	//m_pModels[PART_BODY]->Update_VTF(0);
	//m_pModels[PART_BODY]->Bind_VTF(m_pShader, "g_VTF", 0);

	for (_uint i = 0; i < PART_END; ++i)
	{
		if (nullptr == m_pModels[i])
			continue;

		_uint		iNumMeshes = m_pModels[i]->Get_NumMeshes();

		for (_uint j = 0; j < iNumMeshes; ++j)
		{
			if (FAILED(m_pModels[i]->SetUp_OnShader(m_pShader, j, aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;


			/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModel->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
				return E_FAIL;*/


			if (FAILED(m_pModels[i]->Render(m_pShader, j, 0)))
				return E_FAIL;
		}
	}
	

	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = To_Radian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Player_Head"), TEXT("Model_Head"), (CComponent**)&m_pModels[PART_HEAD])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Player_Body"), TEXT("Model_Body"), (CComponent**)&m_pModels[PART_BODY])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Puppetwstring"), TEXT("Model_Puppetwstring"), (CComponent**)&m_pModels[PART_REGIONARM])))
		return E_FAIL;

	return S_OK;
}


CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& pModel : m_pModels)
		Safe_Release(pModel);

	Safe_Release(m_pShader);
}
