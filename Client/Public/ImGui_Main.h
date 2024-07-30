#pragma once

#include "Client_Defines.h"
#include "Base.h"


BEGIN(Engine)
class CGameInstance;
END


BEGIN(Client)
class CImGui_Main :  public CBase
{
private:
	enum ToolState { MAP, ANIM, TOOL_END};

private:
	CImGui_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CImGui_Main() = default;

	HRESULT Initialize();

public:
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Render();

private:
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*			m_pContext = nullptr;
	CGameInstance*					m_pGameInstance = nullptr;

	class CFree_Camera*				m_pFree_Camera = { nullptr };

private:
	ToolState						m_eToolState = TOOL_END;
	class CToolState*				m_ToolStates[TOOL_END] = {};


public:
	static CImGui_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;


};
END

