#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CCutscene_Manager
{
	DECLARE_SINGLETON(CCutscene_Manager)

private:
	CCutscene_Manager();
	~CCutscene_Manager() = default;

public:
	void Add_Actor(class CCutscene_Actor* pActor) {
		m_CutsceneActors.push_back(pActor);
	}

	void OnEnter_Cutscene(CUTSCENE_NUMBER eCutscene);
	void OnStart_Cutscene();
	void OnEnd_Cutscene();

private:
	CGameInstance*					m_pGameInstance = { nullptr };

	list<class CCutscene_Actor*>	m_CutsceneActors;
	CUTSCENE_NUMBER					m_eNowPlaying = { CUTSCENE_END };
};

END


