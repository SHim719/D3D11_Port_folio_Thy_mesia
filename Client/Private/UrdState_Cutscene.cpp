#include "UrdState_Cutscene.h"

#include "Main_Camera.h"

#include "FadeScreen.h"

#include "Cutscene_Manager.h"
#include "UI_Manager.h"


CUrdState_Cutscene::CUrdState_Cutscene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUrdState_Base(pDevice, pContext)
{
}

HRESULT CUrdState_Cutscene::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("FadeOut_CutScene", bind(&CUrdState_Cutscene::FadeOut_Cutscene, this));

	return S_OK;
}

void CUrdState_Cutscene::OnState_Start(void* pArg)
{
	m_pUrd->Set_LookTarget(false);
	m_pUrd->Set_Stanced(true);
	m_pUrd->Set_Adjust_NaviY(false);
	m_pUrd->Set_NoRender(false);

	ATTACHDESC attachDesc;
	//attachDesc.pAttachBone = m_pModel->Get_Bone("AnimTargetPoint");
	attachDesc.pParentTransform = m_pOwnerTransform;

	attachDesc.pAttachBone = m_pModel->Get_Bone("camera");

	static_cast<CMain_Camera*>(m_pGameInstance->Get_MainCamera())->SetState_Cutscene(attachDesc);

	m_pModel->Change_Animation(Urd_Seq_BossFightStart_1, 0.f);

	//m_pUrd->Get_Target()->Get_Transform()->Set_WorldMatrix(m_pOwnerTransform->Get_WorldMatrix());
}

void CUrdState_Cutscene::Update(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		CUTSCENEMGR->OnEnd_Cutscene();
		_float fWalkTime = 3.f;
		m_pUrd->Change_State((_uint)UrdState::State_Walk, &fWalkTime);
	}
}

void CUrdState_Cutscene::OnState_End()
{
	m_pUrd->Set_Adjust_NaviY(true);
}

void CUrdState_Cutscene::FadeOut_Cutscene()
{
	CFadeScreen::FADEDESC FadeDesc;
	FadeDesc.eFadeColor = CFadeScreen::BLACK;
	FadeDesc.fFadeOutSpeed = 0.62f;
	FadeDesc.fFadeInSpeed = 1.5f;
	
	UIMGR->Active_UI("FadeScreen", &FadeDesc);
}

CUrdState_Cutscene* CUrdState_Cutscene::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUrdState_Cutscene* pInstance = new CUrdState_Cutscene(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUrdState_Cutscene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrdState_Cutscene::Free()
{
	__super::Free();
}
