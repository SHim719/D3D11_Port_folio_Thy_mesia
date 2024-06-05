#pragma once

#include "ToolState.h"


BEGIN(Client)

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
	void Camera_Window() override;

	void Menu_Bar();
	void Anim_ListBox();
	void Anim_Buttons();

private:
	void Load_BinaryModel();
	void Load_Weapon();

private:
	class CToolAnimObj* m_pAnimObj = nullptr;
	class CModel*		m_pModel = nullptr;
	string				m_strNowLoaded = "";

	vector<string>		m_strAnimations;
	_int				m_iCurSelAnimIdx = 0;
public:
	static CAnim_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END

