#pragma once

#include "Client_Defines.h"
#include "Base.h"


BEGIN(Engine)
class CGameInstance;
END


class CLoader : public CBase
{
public:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	_bool isFinished() const {
		return m_bIsFinished;
	}

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	unsigned int Loading();

private:
	HRESULT Loading_Default();
	HRESULT Loading_Stage1();
	HRESULT Loading_StageOdur();
	HRESULT Loading_StageUrd();

private:
	HRESULT Ready_Camera();
	HRESULT Ready_Weapon();
	HRESULT Ready_Player();

	HRESULT Ready_Odur();
	HRESULT Ready_Villager_F();
	HRESULT Ready_Villager_M();
	HRESULT Ready_Joker();
	HRESULT Ready_HalberdKnight();
	HRESULT Ready_TwinBladeKnight();
	HRESULT Ready_Urd();

	HRESULT Ready_MapObjects(const wstring& wstrPath);

	HRESULT Ready_UIResource();
	HRESULT Ready_UI();

	HRESULT Ready_Effects();
	HRESULT Ready_EffectResources();
	HRESULT Ready_EffectGroup();
	HRESULT Ready_EffectTrail();

	HRESULT Ready_Etc();
	
	HRESULT Ready_Sound(const wstring& wstrPath);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private:
	HANDLE				m_hThread = 0;
	CRITICAL_SECTION	m_CriticalSection;
	LEVEL				m_eNextLevelID = LEVEL_END;

private:
	CGameInstance*		m_pGameInstance = { nullptr };

	_tchar				m_szLoadingText[MAX_PATH] = TEXT("");
	_bool				m_bIsFinished = false;

public:
	const _tchar* Get_LoadingText() const {
		return m_szLoadingText;
	}

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

