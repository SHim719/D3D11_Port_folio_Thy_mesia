#include "Ladder_Up.h"

#include "Player.h"

CLadder_Up::CLadder_Up(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLadder_Up::CLadder_Up(const CLadder_Up& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLadder_Up::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLadder_Up::Initialize(void* pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (m_pCollider)
		m_pCollider->Update(m_pTransform->Get_WorldMatrix());

	return S_OK;
}


void CLadder_Up::LateTick(_float fTimeDelta)
{
	if (m_bInstancing)
		return;

	if (m_pGameInstance->In_WorldFrustum(m_pTransform->Get_Position(), m_fCullingRadius))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pGameInstance->Add_RenderComponent(m_pCollider);
#endif
	}
}

HRESULT CLadder_Up::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 0)))
			return E_FAIL;
	}

	return S_OK;
}

void CLadder_Up::OnCollisionStay(CGameObject* pOther)
{
	if (KEY_DOWN(eKeyCode::E))
	{
		static_cast<CPlayer*>(pOther)->SetState_ClimbStart(&m_LadderDesc);
	}

}

HRESULT CLadder_Up::Ready_Components(void* pArg)
{
	LOADOBJDESC* pLoadDesc = (LOADOBJDESC*)pArg;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	CCollider::COLLIDERDESC Desc;
	Desc.eType = CCollider::AABB;
	Desc.pOwner = this;
	Desc.vCenter = { 0.f, 1.f, 0.f };
	Desc.vSize = { 0.5f, 0.5f, 0.5f };
	Desc.vRotation = { 0.f, 0.f, 0.f };
	Desc.bActive = true;
	Desc.strCollisionLayer = "MapObject";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_AABB"), TEXT("Collider"), (CComponent**)&m_pCollider, &Desc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, pLoadDesc->szModelTag, TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pLoadDesc->WorldMatrix));

	m_LadderDesc.iTargetNaviIdx = pLoadDesc->iNaviIdx;
	m_LadderDesc.iLadderHeight = (_uint)pLoadDesc->iTriggerIdx;
	m_LadderDesc.iStartHeight = m_LadderDesc.iLadderHeight - 1 - 5;
	m_LadderDesc.iPlayerAnimIdx = Corvus_SD_Ladder_Descend;
	XMStoreFloat4(&m_LadderDesc.vOffset, m_pTransform->Get_GroundLook() * 0.45f + m_pTransform->Get_Up() * 0.42f);
	m_LadderDesc.pWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

	return S_OK;
}


CLadder_Up* CLadder_Up::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLadder_Up* pInstance = new CLadder_Up(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLadder_Up"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLadder_Up::Clone(void* pArg)
{
	CLadder_Up* pInstance = new CLadder_Up(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CLadder_Up"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLadder_Up::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
}
