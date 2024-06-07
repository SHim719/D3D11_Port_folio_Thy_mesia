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
	HRESULT Load_KeyFrameNames();
	HRESULT Load_KeyFrames();
	HRESULT Load_KeyFrameData(const _tchar* pPath);
	
	HRESULT Save_KeyFrameData();

private:
	void Main_Window();
	void Camera_Window()			override;
	
	void Menu_Bar();
	void Model_ListBox();
	void Model_Button();

	void Anim_ListBox();
	void Anim_Buttons();

	void KeyFrame_Window();
	void KeyFrameEvent_ComboBox();
	void KeyFrameEvent_ListBox();
	void KeyFrameEvent_Button();

private:
	class CToolAnimObj* m_pAnimObj = nullptr;
	class CModel*		m_pModel = nullptr;

	vector<string>		m_strModelLists;
	_int				m_iSelModelIdx = 0;

	vector<string>		m_strAnimations;
	_int				m_iSelAnimIdx = 0;
	_float				m_fAnimListBoxStartCursorY;

	vector<string>		m_strEventNames;
	_int				m_iSelEventName = 0;
	_char				m_szEventName[MAX_PATH] = "";

	vector<vector<pair<_int, string>>>	m_KeyFrameEvents;
	_int								m_iSelKeyFrameIdx = 0;
	_float								m_fKeyFrameButtonStartCursorY;

public:
	static CAnim_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END

