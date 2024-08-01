#include "Archive_Chair.h"

#include "Player.h"

#include "UI_Manager.h"

CArchive_Chair::CArchive_Chair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CArchive_Chair::CArchive_Chair(const CArchive_Chair& rhs)
	: CGameObject(rhs)
{
}

HRESULT CArchive_Chair::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CArchive_Chair::Initialize(void* pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (m_pCollider)
		m_pCollider->Update(m_pTransform->Get_WorldMatrix());

	return S_OK;
}

void CArchive_Chair::Tick(_float fTimeDelta)
{
}


void CArchive_Chair::LateTick(_float fTimeDelta)
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

HRESULT CArchive_Chair::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModel->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, 0)))
			return E_FAIL;
	}

	return S_OK;
}

void CArchive_Chair::Active_Chair()
{
	UIMGR->Active_UI("UI_BeaconFound");
	CGameObject* pAisemy = m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Effect", L"Prototype_Aisemy");

	CTransform* pAisemyTransform = pAisemy->Get_Transform();
	pAisemyTransform->Set_WorldMatrix(m_pTransform->Get_WorldMatrix());
	pAisemyTransform->Add_Position(XMVectorSet(1.f, 0.f, -1.f, 0.f), true);
	pAisemyTransform->LookTo(m_pTransform->Get_GroundRight());
}

void CArchive_Chair::OnCollisionStay(CGameObject* pOther)
{
	if (KEY_DOWN(eKeyCode::E))
	{
		CPlayer* pPlayer = GET_PLAYER;

		_float fScalar = 1.f;
		_vector vPos, vLook = m_pTransform->Get_GroundLook();

		if (false == m_bFound)
		{
			pPlayer->SetState_FoundBeacon(this);
			m_bFound = true;
		}
		else
		{
			fScalar = 0.7f;
			vLook = -vLook;
			pPlayer->SetState_Sit(nullptr);
		}

		pPlayer->Get_Transform()->LookTo(vLook);
		vPos = m_pTransform->Get_Position() - m_pTransform->Get_GroundLook() * fScalar;
		pPlayer->Get_Transform()->Set_Position(vPos);
	}

}

HRESULT CArchive_Chair::Ready_Components(void* pArg)
{
	LOADOBJDESC* pLoadDesc = (LOADOBJDESC*)pArg;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	CCollider::COLLIDERDESC Desc;
	Desc.eType = CCollider::SPHERE;
	Desc.pOwner = this;
	Desc.vCenter = { 0.f, 1.f, 0.f };
	Desc.vSize = { 2.f, 1.f, 1.f };
	Desc.vRotation = { 0.f, 0.f, 0.f };
	Desc.bActive = true;
	Desc.strCollisionLayer = "MapObject";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Sphere"), TEXT("Collider"), (CComponent**)&m_pCollider, &Desc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, pLoadDesc->szModelTag, TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pLoadDesc->WorldMatrix));

	return S_OK;
}


CArchive_Chair* CArchive_Chair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CArchive_Chair* pInstance = new CArchive_Chair(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CArchive_Chair"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CArchive_Chair::Clone(void* pArg)
{
	CArchive_Chair* pInstance = new CArchive_Chair(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CArchive_Chair"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CArchive_Chair::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
}
