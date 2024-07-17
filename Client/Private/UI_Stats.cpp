#include "UI_Stats.h"

#include "UI_Manager.h"

#include "Player.h"
#include "PlayerStats.h"

#include "FadeScreen.h"

CUI_Stats::CUI_Stats(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUI_Stats::CUI_Stats(const CUI_Stats& rhs)
	: CUI(rhs)
{
}


HRESULT CUI_Stats::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Init_UIMatrices();
	Init_FontPos();
	Init_HighlightedPos();
	Init_ArrowMatrices();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pPlayerStats = GET_PLAYER->Get_PlayerStats();
	Safe_AddRef(m_pPlayerStats);

	m_pTransform->Set_Scale({ 32.f, 32.f, 1.f });

	return S_OK;
}


void CUI_Stats::Tick(_float fTimeDelta)
{
	m_bCanLevelUp = m_iStatInfos[SOUL] >= m_iStatInfos[NEED_SOUL];
	m_bCanLevelDown = (m_iSelectInfo < 3) && (m_iStatInfos[STRENGTH + m_iSelectInfo] > m_iOriginStatInfos[STRENGTH + m_iSelectInfo]);
	

	if (KEY_DOWN(eKeyCode::Up))
	{
		m_iSelectInfo = (m_iSelectInfo + 4 - 1) % 4;
		if (m_iSelectInfo < 3)
			m_ArrowMatrices[0].m[3][1] = m_ArrowMatrices[1].m[3][1] = m_vNumberPos[STRENGTH + m_iSelectInfo][1].y;
	}

	else if (KEY_DOWN(eKeyCode::Down))
	{
		m_iSelectInfo = (m_iSelectInfo + 1) % 4;
		if (m_iSelectInfo < 3)
			m_ArrowMatrices[0].m[3][1] = m_ArrowMatrices[1].m[3][1] = m_vNumberPos[STRENGTH + m_iSelectInfo][1].y;
	}

	else if (KEY_DOWN(eKeyCode::Right))
	{
		if (3 == m_iSelectInfo)
			return;

		if (m_bCanLevelUp)
			Level_Up();

	}
	
	else if (KEY_DOWN(eKeyCode::Left))
	{
		if (3 == m_iSelectInfo)
			return;

		if (m_bCanLevelDown)
			Level_Down();
	}

	else if (KEY_DOWN(eKeyCode::Enter) && 3 == m_iSelectInfo)
	{
		Setting_EditInfo();
		Exit();
	}

}

void CUI_Stats::LateTick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Stats::Render()
{
	if (FAILED(Render_BG()))
		return E_FAIL;

	if (FAILED(Render_Highlighted()))
		return E_FAIL;

	if (FAILED(Render_StatArrow()))
		return E_FAIL;

	if (FAILED(Render_Fonts()))
		return E_FAIL;
	
	if (m_bCanLevelDown)
	{
		if (FAILED(Render_Arrow_Left()))
			return E_FAIL;
	}
	
	if (m_bCanLevelUp)
	{
		if (FAILED(Render_Arrow_Right()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Stats::OnEnter_Layer(void* pArg)
{
	UIMGR->Inactive_UI("UI_Menu");

	m_bReturnToPool = false;

	m_iOriginStatInfos[LEVEL] = m_iStatInfos[LEVEL] = m_pPlayerStats->Get_PlayerLevel();
	m_iOriginStatInfos[STRENGTH] = m_iStatInfos[STRENGTH] = m_pPlayerStats->Get_Strength();
	m_iOriginStatInfos[PLAGUE] = m_iStatInfos[PLAGUE] = m_pPlayerStats->Get_Plague();
	m_iOriginStatInfos[VITALITY] = m_iStatInfos[VITALITY] = m_pPlayerStats->Get_Vitality();
	m_iOriginStatInfos[SOUL] = m_iStatInfos[SOUL] = m_pPlayerStats->Get_SoulCount();
	m_iOriginStatInfos[DAMAGE] = m_iStatInfos[DAMAGE] = m_iOriginStatInfos[STRENGTH];
	m_iOriginStatInfos[CLAW_DAMAGE] = m_iStatInfos[CLAW_DAMAGE] = m_iOriginStatInfos[PLAGUE];
	m_iOriginStatInfos[HP] = m_iStatInfos[HP] = m_pPlayerStats->Get_MaxHp();

	m_iStatInfos[NEED_SOUL] = 50 * m_iOriginStatInfos[LEVEL];

	Update_StrFont();

	return S_OK;
}

HRESULT CUI_Stats::Render_BG()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_BGWorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pUITexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(0)))
		return E_FAIL;

	return m_pVIBuffer->Render();
}

HRESULT CUI_Stats::Render_Highlighted()
{
	_float fAlpha = 0.6f;

	_matrix HighlightedMatrix = XMLoadFloat4x4(&m_HighlightedMatrix);
	HighlightedMatrix.r[3] = XMLoadFloat4(&m_vHighlightedPos[m_iSelectInfo]);

	_float4x4 HighlightedMatrix_TP;
	XMStoreFloat4x4(&HighlightedMatrix_TP, XMMatrixTranspose(HighlightedMatrix));
	
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &HighlightedMatrix_TP, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pHighlightedTexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	return m_pVIBuffer->Render();
}

HRESULT CUI_Stats::Render_StatArrow()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_BGWorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pStatArrowsTexture->Set_SRV(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	return m_pVIBuffer->Render();
}

HRESULT CUI_Stats::Render_Fonts()
{
	for (_uint i = 0; i < INFO_END; ++i)
	{
		for (_uint j = 0; j < 2; ++j)
		{
			if (m_strNumbers[i][j].empty())
				continue;

			if (FAILED(Render_Number(m_strNumbers[i][j], XMLoadFloat4(&m_vNumberPos[i][j]))))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CUI_Stats::Render_Number(const string& strNumber, _fvector vRenderStartPos)
{
	_vector vRenderPos = vRenderStartPos;
	_float fScaleX = 11.f;

	for (_int i = (_int)strNumber.size() - 1; i >= 0; --i)
	{
		m_pTransform->Set_Position(vRenderPos);

		if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pNumberTexture->Set_SRV(m_pShader, "g_DiffuseTexture", strNumber[i] - '0')))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;

		vRenderPos = XMVectorSetX(vRenderPos, vRenderPos.m128_f32[0] - fScaleX);
	}

	return S_OK;
}

HRESULT CUI_Stats::Render_Arrow_Left()
{
	_matrix ArrowMatrix = XMLoadFloat4x4(&m_ArrowMatrices[0]);

	_float4x4 ArrowMatrix_TP;
	XMStoreFloat4x4(&ArrowMatrix_TP, XMMatrixTranspose(ArrowMatrix));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &ArrowMatrix_TP, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pArrowLeftTexture->Set_SRV(m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Stats::Render_Arrow_Right()
{
	_matrix ArrowMatrix = XMLoadFloat4x4(&m_ArrowMatrices[1]);

	_float4x4 ArrowMatrix_TP;
	XMStoreFloat4x4(&ArrowMatrix_TP, XMMatrixTranspose(ArrowMatrix));

	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &ArrowMatrix_TP, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pArrowRightTexture->Set_SRV(m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_Stats::Level_Up()
{
	m_iStatInfos[SOUL] -= m_iStatInfos[NEED_SOUL];
	m_iStatInfos[LEVEL] += 1;
	m_iStatInfos[m_iSelectInfo + STRENGTH] += 1;
	Calc_Info(StatInfo(m_iSelectInfo + DAMAGE));
	Calc_Info(NEED_SOUL);

	Update_StrFont();
}

void CUI_Stats::Level_Down()
{
	m_iStatInfos[LEVEL] -= 1;
	m_iStatInfos[m_iSelectInfo + STRENGTH] -= 1;

	Calc_Info(NEED_SOUL);

	m_iStatInfos[SOUL] += m_iStatInfos[NEED_SOUL];

	Calc_Info(StatInfo(m_iSelectInfo + DAMAGE));

	Update_StrFont();
}

void CUI_Stats::Calc_Info(StatInfo eInfo)
{
	switch (eInfo)
	{
	case NEED_SOUL:
		m_iStatInfos[NEED_SOUL] = m_iStatInfos[LEVEL] * 50;
		break;
	case DAMAGE:
		m_iStatInfos[DAMAGE] = m_iStatInfos[STRENGTH];
		break;
	case CLAW_DAMAGE:
		m_iStatInfos[HP] = 300 + (m_iStatInfos[LEVEL] - 1) * 4;
		break;
	case HP:
		m_iStatInfos[CLAW_DAMAGE] = m_iStatInfos[PLAGUE];
		break;
	}
}

void CUI_Stats::Setting_EditInfo()
{
	enum StatInfo { LEVEL, SOUL, NEED_SOUL, STRENGTH, VITALITY, PLAGUE, DAMAGE, CLAW_DAMAGE, HP, INFO_END };

	m_pPlayerStats->Set_PlayerLevel(m_iStatInfos[LEVEL]);
	m_pPlayerStats->Set_SoulCount(m_iStatInfos[SOUL]);
	m_pPlayerStats->Set_Strength(m_iStatInfos[STRENGTH]);
	m_pPlayerStats->Set_Vitality(m_iStatInfos[VITALITY]);
	m_pPlayerStats->Set_Plague(m_iStatInfos[PLAGUE]);
	m_pPlayerStats->Set_MaxHp(m_iStatInfos[HP]);
	m_pPlayerStats->SetHp_Full();
}

void CUI_Stats::Exit()
{
	CFadeScreen::FADEDESC FadeDesc{};
	FadeDesc.eFadeColor = CFadeScreen::BLACK;
	FadeDesc.fFadeOutSpeed = 3.f;
	FadeDesc.fFadeInSpeed = 10.f;
	FadeDesc.pCallback_FadeOutEnd = bind(&CUI_Manager::Active_UI, CUI_Manager::Get_Instance(), "UI_Menu", nullptr);
	FadeDesc.pCallback_FadeInStart = bind(&CGameObject::Set_ReturnToPool, this, true);

	UIMGR->Active_UI("FadeScreen", &FadeDesc);
}

HRESULT CUI_Stats::Ready_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"), TEXT("VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Window_Status"), TEXT("Status_Texture"), (CComponent**)&m_pUITexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Status_Number"), TEXT("Number_Texture"), (CComponent**)&m_pNumberTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Stat_Arrows"), TEXT("Arrows_Texture"), (CComponent**)&m_pStatArrowsTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Arrow_Right"), TEXT("Arrows_R_Texture"), (CComponent**)&m_pArrowRightTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Arrow_Left"), TEXT("Arrows_L_Texture"), (CComponent**)&m_pArrowLeftTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_Select_Highlighted"), TEXT("Highlighted_Texture"), (CComponent**)&m_pHighlightedTexture)))
		return E_FAIL;

	return S_OK;
}

void CUI_Stats::Init_UIMatrices()
{
	_matrix WorldMatrix = XMMatrixIdentity();
	_float3 vScale = { 1280.f, 720.f, 1.f };

	WorldMatrix.r[0] *= vScale.x;
	WorldMatrix.r[1] *= vScale.y;

	XMStoreFloat4x4(&m_BGWorldMatrix, XMMatrixTranspose(WorldMatrix));

	WorldMatrix = XMMatrixIdentity();
	vScale = { 429.f, 30.f, 1.f };

	WorldMatrix.r[0] *= vScale.x;
	WorldMatrix.r[1] *= vScale.y;

	XMStoreFloat4x4(&m_HighlightedMatrix, WorldMatrix);

}

void CUI_Stats::Init_HighlightedPos()
{
	XMStoreFloat4(&m_vHighlightedPos[0], Convert_ScreenToRenderPos(XMVectorSet(458.f, 377.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vHighlightedPos[1], Convert_ScreenToRenderPos(XMVectorSet(458.f, 448.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vHighlightedPos[2], Convert_ScreenToRenderPos(XMVectorSet(458.f, 522.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vHighlightedPos[3], Convert_ScreenToRenderPos(XMVectorSet(371.f, 615.f, 1.f, 1.f)));

}

void CUI_Stats::Init_FontPos()
{
	XMStoreFloat4(&m_vNumberPos[LEVEL][0], Convert_ScreenToRenderPos(XMVectorSet(338.f, 152.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vNumberPos[LEVEL][1], Convert_ScreenToRenderPos(XMVectorSet(595.f, 152.f, 1.f, 1.f)));

	XMStoreFloat4(&m_vNumberPos[SOUL][0], Convert_ScreenToRenderPos(XMVectorSet(338.f, 201.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vNumberPos[SOUL][1], Convert_ScreenToRenderPos(XMVectorSet(595.f, 201.f, 1.f, 1.f)));

	XMStoreFloat4(&m_vNumberPos[NEED_SOUL][1], Convert_ScreenToRenderPos(XMVectorSet(595.f, 258.f, 1.f, 1.f)));

	XMStoreFloat4(&m_vNumberPos[STRENGTH][0], Convert_ScreenToRenderPos(XMVectorSet(338.f, 379.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vNumberPos[STRENGTH][1], Convert_ScreenToRenderPos(XMVectorSet(595.f, 379.f, 1.f, 1.f)));

	XMStoreFloat4(&m_vNumberPos[VITALITY][0], Convert_ScreenToRenderPos(XMVectorSet(338.f, 451.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vNumberPos[VITALITY][1], Convert_ScreenToRenderPos(XMVectorSet(595.f, 451.f, 1.f, 1.f)));

	XMStoreFloat4(&m_vNumberPos[PLAGUE][0], Convert_ScreenToRenderPos(XMVectorSet(338.f, 524.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vNumberPos[PLAGUE][1], Convert_ScreenToRenderPos(XMVectorSet(595.f, 524.f, 1.f, 1.f)));

	XMStoreFloat4(&m_vNumberPos[DAMAGE][0], Convert_ScreenToRenderPos(XMVectorSet(976.f, 297.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vNumberPos[DAMAGE][1], Convert_ScreenToRenderPos(XMVectorSet(1184.f, 297.f, 1.f, 1.f)));

	XMStoreFloat4(&m_vNumberPos[CLAW_DAMAGE][0], Convert_ScreenToRenderPos(XMVectorSet(976.f, 386.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vNumberPos[CLAW_DAMAGE][1], Convert_ScreenToRenderPos(XMVectorSet(1184.f, 386.f, 1.f, 1.f)));

	XMStoreFloat4(&m_vNumberPos[HP][0], Convert_ScreenToRenderPos(XMVectorSet(976.f, 476.f, 1.f, 1.f)));
	XMStoreFloat4(&m_vNumberPos[HP][1], Convert_ScreenToRenderPos(XMVectorSet(1184.f, 476.f, 1.f, 1.f)));
}

void CUI_Stats::Init_ArrowMatrices()
{
	_matrix WorldMatrix = XMMatrixIdentity();
	WorldMatrix.r[0] *= 12.f;
	WorldMatrix.r[1] *= 24.f;

	WorldMatrix.r[3] = Convert_ScreenToRenderPos(XMVectorSet(519.f, m_vNumberPos[STRENGTH][1].y, 1.f, 1.f));
	XMStoreFloat4x4(&m_ArrowMatrices[0], WorldMatrix);

	WorldMatrix.r[3] = Convert_ScreenToRenderPos(XMVectorSet(666.f, m_vNumberPos[STRENGTH][1].y, 1.f, 1.f));
	XMStoreFloat4x4(&m_ArrowMatrices[1], WorldMatrix);

	m_ArrowMatrices[0].m[3][1] = m_ArrowMatrices[1].m[3][1] = m_vNumberPos[STRENGTH + m_iSelectInfo][1].y;
}

void CUI_Stats::Update_StrFont()
{
	for (_uint i = 0; i < INFO_END; ++i)
	{
		m_strNumbers[i][0] = to_string(m_iOriginStatInfos[i]);
		m_strNumbers[i][1] = to_string(m_iStatInfos[i]);
	}
		

	m_strNumbers[NEED_SOUL][0] = "";
}

CUI_Stats* CUI_Stats::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Stats* pInstance = new CUI_Stats(pDevice, pContext);

	if (FAILED(pInstance->Initialize(nullptr)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Stats"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CUI_Stats::Clone(void* pArg)
{
	CUI_Stats* pInstance = new CUI_Stats(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Stats"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Stats::Free()
{
	__super::Free();

	Safe_Release(m_pPlayerStats);
	Safe_Release(m_pNumberTexture);
	Safe_Release(m_pHighlightedTexture);
	Safe_Release(m_pStatArrowsTexture);

	Safe_Release(m_pArrowRightTexture);
	Safe_Release(m_pArrowLeftTexture);
}
