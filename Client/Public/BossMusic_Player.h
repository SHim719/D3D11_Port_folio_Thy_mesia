#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CSound;
END

BEGIN(Client)
class CBossMusic_Player
{
	DECLARE_SINGLETON(CBossMusic_Player)

private:
	CBossMusic_Player();
	~CBossMusic_Player();

public:
	void Update();

public:
	void Play_BossBGM(vector<BOSSMUSICDESC> BGMInfos);
	void Play_NextBGM();

	void Finish_BGM();
				
private:
	CGameInstance*					m_pGameInstance = { nullptr };
	vector<BOSSMUSICDESC>			m_BGMInfos;
	size_t							m_iNowPlayingIdx = { 0 };
	CSound*							m_pNowPlayingBGM = { nullptr };
private:
	void Release();
};

END


