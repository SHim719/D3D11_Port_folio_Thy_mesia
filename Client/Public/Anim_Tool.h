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
	void Start_Tool()				override;
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
	void Tab_Bar();

	void Model_ListBox();
	void Model_Button();

	void Anim_ListBox();
	void Anim_Buttons();

	void KeyFrame_Window();
	void KeyFrameEvent_ComboBox();
	void KeyFrameEvent_ListBox();
	void KeyFrameEvent_Button();

	void Bone_ListBox();
	void Collider_ListBox();
	void ColliderType_CheckBox();
	void Collider_Buttons();

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

	const char**				m_szBoneNames = nullptr;
	_int						m_iNumBones = 0;
	_int						m_iSelBoneIdx = 0;
	
	vector<class CToolColliderObj*>	m_Colliders;
	vector<string>					m_strColliders;
	_int							m_iSelColliderIdx = 0;

	CCollider::COLLIDERDESC			m_ColliderDesc;
	_bool							m_bColliderTypes[CCollider::Collider_End] = {};
	_int							m_iTypeIdx = 0;

public:
	static CAnim_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END

