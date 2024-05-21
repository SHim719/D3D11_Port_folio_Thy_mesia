#pragma once
#include "ToolState.h"


class CMap_Tool : public CToolState
{

protected:
	CMap_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMap_Tool() = default;

public:
	HRESULT Initialize(void* pArg)	override;
	void Tick(_float fTimeDelta)	override;

private:
	list<CGameObject*> m_MapObjects;

	
public:
	static CMap_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

