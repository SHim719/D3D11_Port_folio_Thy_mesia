#include "Cutscene_Manager.h"

#include "FadeScreen.h"
#include "Cutscene_Actor.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCutscene_Manager);

CCutscene_Manager::CCutscene_Manager()
{
}


void CCutscene_Manager::OnEnter_Cutscene(CUTSCENE_NUMBER eCutscene)
{
	m_eNowPlaying = eCutscene;

	CFadeScreen::FADEDESC FadeDesc;
	FadeDesc.eFadeColor = CFadeScreen::BLACK;
	FadeDesc.eFadeType = CFadeScreen::FADEOUT;
	FadeDesc.fAlphaDeltaSpeed = 2.f;
	FadeDesc.fExtraTime = 0.f;
	FadeDesc.Callback_FadeEnd = bind(&CCutscene_Manager::OnStart_Cutscene, this);

	m_pGameInstance->Add_Clone(GET_CURLEVEL, L"UI", L"Prototype_FadeScreen", &FadeDesc);

	for (auto it = m_CutsceneActors.begin(); it != m_CutsceneActors.end(); ++it)
		(*it)->OnEnter_Cutscene();
}

void CCutscene_Manager::OnStart_Cutscene()
{
	for (auto it = m_CutsceneActors.begin(); it != m_CutsceneActors.end(); ++it)
		(*it)->OnStart_Cutscene(m_eNowPlaying);
	
}

void CCutscene_Manager::OnEnd_Cutscene()
{
}
