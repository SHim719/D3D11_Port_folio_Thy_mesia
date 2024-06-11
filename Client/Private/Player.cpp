#include "Player.h"

#include "Player_States.h"
#include "PlayerStat.h"

#include "Weapon.h"



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

	if (FAILED(Ready_States()))
		return E_FAIL;

	if (FAILED(Ready_Weapons()))
		return E_FAIL;

	m_pTransform->Set_MoveLook(m_pTransform->Get_Look());

	m_pModel->Set_AnimPlay();

	m_pModel->Change_Animation(Corvus_SD_Idle, 0.f);
	m_eState = PlayerState::State_Idle;

	//Change_State(PlayerState::State_SprintStart);


	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->GetKeyNone(eKeyCode::RButton))
	{
		m_States[(_uint)m_eState]->OnGoing(fTimeDelta);
	}

	m_pModel->Play_Animation(fTimeDelta);
}

void CPlayer::LateTick(_float fTimeDelta)
{
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

	m_pModel->SetUp_BoneMatrices(m_pShader);

	_uint		iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint j = 0; j < iNumMeshes; ++j)
	{
		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, j, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;


		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModel->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/


		if (FAILED(m_pModel->Render(m_pShader, j, 0)))
			return E_FAIL;
	}
	
	

	return S_OK;
}

void CPlayer::Change_State(PlayerState eState)
{
	m_States[(_uint)m_eState]->OnState_End();

	m_eState = eState;

	m_States[(_uint)eState]->OnState_Start();
}

void CPlayer::Active_Weapons()
{
	m_pDagger->Set_Active(true);
	m_pSaber->Set_Active(true);
}

void CPlayer::InActive_Weapons()
{
	m_pDagger->Set_Active(false);
	m_pSaber->Set_Active(false);
}

void CPlayer::Active_Claw()
{
}

void CPlayer::InActive_Claw()
{
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

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelID(), TEXT("Prototype_Model_Player"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_States()
{
	m_pStats = CPlayerStat::Create();

	m_States[(_uint)PlayerState::State_Idle] = CPlayerState_Idle::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Jog] = CPlayerState_Jog::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Sprint] = CPlayerState_Sprint::Create(m_pDevice, m_pContext, this);
	m_States[(_uint)PlayerState::State_Attack] = CPlayerState_Attack::Create(m_pDevice, m_pContext, this);

	return S_OK;
}

HRESULT CPlayer::Ready_Weapons()
{
	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.pParentTransform = m_pTransform;
	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_l");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Player_Dagger";
	XMStoreFloat4x4(&WeaponDesc.PivotMatrix, m_pModel->Get_PivotMatrix());

	m_pDagger = static_cast<CWeapon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Weapon", L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_pDagger)
		return E_FAIL;

	WeaponDesc.pSocketBone = m_pModel->Get_Bone("weapon_r");
	WeaponDesc.wstrModelTag = L"Prototype_Model_Player_Saber";
	m_pSaber = static_cast<CWeapon*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Weapon", L"Prototype_Weapon", &WeaponDesc));
	if (nullptr == m_pSaber)
		return E_FAIL;

	// Claw ¼ÕÅé»ý¼º

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

	for (_uint i = 0; i < (_uint)PlayerState::State_End; ++i)
		Safe_Release(m_States[i]);

	Safe_Release(m_pStats);

	Safe_Release(m_pModel);
	Safe_Release(m_pShader);
}
