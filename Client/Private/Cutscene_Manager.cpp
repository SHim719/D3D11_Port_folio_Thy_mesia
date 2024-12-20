#include "Cutscene_Manager.h"

#include "UI_Manager.h"
#include "Cutscene_Actor.h"
#include "FadeScreen.h"


#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCutscene_Manager);

CCutscene_Manager::CCutscene_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
}


void CCutscene_Manager::OnEnter_Cutscene(CUTSCENE_NUMBER eCutscene)
{
	m_eNowPlaying = eCutscene;

	switch (m_eNowPlaying)
	{
		case ENCOUNTER_ODUR:
		{
			CFadeScreen::FADEDESC FadeDesc;
			FadeDesc.eFadeColor = CFadeScreen::BLACK;
			FadeDesc.fFadeOutSpeed = 0.5f;
			FadeDesc.fFadeInSpeed = 2.f;
			FadeDesc.pCallback_FadeOutEnd = move(bind(&CGameInstance::Play, m_pGameInstance, L"Odur_Cutscene", false, 1.f));
			FadeDesc.fExtraTime = 1.5f;
			FadeDesc.pCallback_FadeInStart = move(bind(&CCutscene_Manager::OnStart_Cutscene, this));
			UIMGR->Active_UI("FadeScreen", &FadeDesc);
			break;
		}
		case ENCOUNTER_URD:
		{
			CFadeScreen::FADEDESC FadeDesc;
			FadeDesc.eFadeColor = CFadeScreen::BLACK;
			FadeDesc.fFadeOutSpeed = 0.5f;
			FadeDesc.fFadeInSpeed = 5.f;
			FadeDesc.pCallback_FadeOutEnd = move(bind(&CCutscene_Manager::OnStart_Cutscene, this));

			UIMGR->Active_UI("FadeScreen", &FadeDesc);
			break;
		}
	}

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
	for (auto it = m_CutsceneActors.begin(); it != m_CutsceneActors.end(); ++it)
		(*it)->OnEnd_Cutscene();
}


