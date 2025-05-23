#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CClientLevel :public CLevel
{
protected:
	CClientLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CClientLevel() = default;

public:
	HRESULT Load_Map(const wstring& wstrMapPath);
	HRESULT Load_MapObjects(vector<LOADOBJDESC>& LoadObjDescs, const _float& fCullingRadius, const wstring& wstrPrototypeTag);
	void Ready_ChangeLevel(LEVEL eNextLevel);

protected:
	wstring m_wstrBGMTag = L"";
};

END

