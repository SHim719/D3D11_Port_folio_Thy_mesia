#include "OdurState_Cutscene.h"

#include "Main_Camera.h"

#include "FadeScreen.h"

#include "Cutscene_Manager.h"
#include "UI_Manager.h"


COdurState_Cutscene::COdurState_Cutscene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COdurState_Base(pDevice, pContext)
{
}

HRESULT COdurState_Cutscene::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModel->Bind_Func("FadeOut_CutScene", bind(&COdurState_Cutscene::FadeOut_Cutscene, this));

	return S_OK;
}

void COdurState_Cutscene::OnState_Start(void* pArg)
{
	m_pOdur->Set_LookTarget(false);
	m_pOdur->Set_Stanced(true);

	ATTACHDESC attachDesc;
	attachDesc.pAttachBone = m_pModel->Get_Bone("AnimTargetPoint");
	attachDesc.pParentTransform = m_pOwnerTransform;

	m_pCard = m_pGameInstance->Add_Clone(m_pGameInstance->Get_CurrentLevelID(), L"Object", L"Prototype_Odur_Card_Cutscene", &attachDesc);

	attachDesc.pAttachBone = m_pModel->Get_Bone("camera");

	static_cast<CMain_Camera*>(m_pGameInstance->Find_GameObject(GET_CURLEVEL, L"Camera", 0))->SetState_Cutscene(attachDesc);

	m_pModel->Change_Animation(MagicianLV1_Seq_BossFightStart, 0.f);

	m_pOdur->Get_Target()->Get_Transform()->Set_WorldMatrix(m_pOwnerTransform->Get_WorldMatrix());
}

void COdurState_Cutscene::OnGoing(_float fTimeDelta)
{
	if (m_pModel->Is_AnimComplete())
	{
		CUTSCENEMGR->OnEnd_Cutscene();
		m_pCard->Set_Destroy(true);
		_float fWalkTime = 6.f;
		m_pOdur->Change_State((_uint)OdurState::State_Walk, &fWalkTime);
	}
}

void COdurState_Cutscene::OnState_End()
{

}

void COdurState_Cutscene::FadeOut_Cutscene()
{
	CFadeScreen::FADEDESC FadeDesc;
	FadeDesc.eFadeColor = CFadeScreen::BLACK;
	FadeDesc.fFadeOutSpeed = 0.4f;
	FadeDesc.fFadeInSpeed = 1.5f;

	UIMGR->Active_UI("FadeScreen", &FadeDesc);
}

COdurState_Cutscene* COdurState_Cutscene::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	COdurState_Cutscene* pInstance = new COdurState_Cutscene(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COdurState_Cutscene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COdurState_Cutscene::Free()
{
	__super::Free();
}
