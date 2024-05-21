#pragma once

#include "ToolState.h"

class CAnim_Tool : public CToolState
{
protected:
	CAnim_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CAnim_Tool() = default;

public:
	HRESULT Initialize(void* pArg)	override;
	void Tick(_float fTimeDelta)	override;


private:
	void Main_Window();

private:
	void Menu_Bar();
	void PlayerAnim_Bar();
	void OtherAnim_Bar();
private:
	CGameObject* pAnimObject = nullptr;

public:
	static CAnim_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};


