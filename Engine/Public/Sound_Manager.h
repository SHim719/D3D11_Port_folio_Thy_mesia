#pragma once

#include "Base.h"


BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);

public:
	HRESULT Create_Sound(const string& strPath, const wstring& strSoundTag);
	HRESULT Play(const wstring& strSoundTag, _bool bLoop, _float fVolume = 1.f);
	HRESULT Play_WithPosition(const wstring& strSoundTag, _bool bLoop, _float fPositionSec, _float fVolume = 1.f);
	HRESULT Stop(const wstring& strSoundTag);
	HRESULT SetVolume(const wstring& strSoundTag, const _float& fVolume);
	HRESULT SetPosition(const wstring& strSoundTag, _float fPosition);
	HRESULT Set_Sound_FadeOut(const wstring& strSoundTag, _float fTime);

	_bool Is_Playing(const wstring& strSoundTag);

	class CSound* Get_Sound(const wstring& strSoundTag);
private:
	class CFMOD_Core* m_pFmodCore = { nullptr };

	map<const wstring, class CSound*>	m_Sounds;
	list<class CSound*>					m_FadeOut_Sounds;

public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END