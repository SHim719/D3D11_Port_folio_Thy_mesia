#include "BossMusic_Player.h"
#include "GameInstance.h"
#include "Base.h"
#include "Sound.h"


IMPLEMENT_SINGLETON(CBossMusic_Player);

CBossMusic_Player::CBossMusic_Player()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
}

CBossMusic_Player::~CBossMusic_Player()
{
	Release();
}

void CBossMusic_Player::Update()
{
	if (nullptr == m_pNowPlayingBGM)
		return;

	if (false == m_pNowPlayingBGM->Is_Playing())
	{
		m_pNowPlayingBGM->Play(false, 1.f);
		m_pNowPlayingBGM->SetPosition(m_BGMInfos[m_iNowPlayingIdx].fLoopStartPos);
	}

}

void CBossMusic_Player::Play_BossBGM(vector<BOSSMUSICDESC> BGMInfos)
{
	m_BGMInfos = BGMInfos;
	m_iNowPlayingIdx = 0;

	m_pNowPlayingBGM = m_pGameInstance->Get_Sound(m_BGMInfos[m_iNowPlayingIdx].wstrMusicTag);
	Safe_AddRef(m_pNowPlayingBGM);

	PLAY_SOUND(m_BGMInfos[m_iNowPlayingIdx].wstrMusicTag, false, 1.f);
}

void CBossMusic_Player::Play_NextBGM()
{
	Finish_BGM();
	m_iNowPlayingIdx += 1;

	Safe_Release(m_pNowPlayingBGM);

	m_pNowPlayingBGM = m_pGameInstance->Get_Sound(m_BGMInfos[m_iNowPlayingIdx].wstrMusicTag);
	Safe_AddRef(m_pNowPlayingBGM);

	PLAY_SOUND(m_BGMInfos[m_iNowPlayingIdx].wstrMusicTag, false, 1.f);
}

void CBossMusic_Player::Finish_BGM()
{
	m_pGameInstance->Set_Sound_FadeOut(m_BGMInfos[m_iNowPlayingIdx].wstrMusicTag, 1.f);
}


void CBossMusic_Player::Release()
{
	Safe_Release(m_pNowPlayingBGM);
}




