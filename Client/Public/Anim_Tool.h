#pragma once

#include "ToolState.h"
#include "GameEffect.h"


BEGIN(Client)

class CAnim_Tool : public CToolState
{
private:
	void ComboBox_ToolSelect();
	_int m_iToolState = { 0 };

protected:
	CAnim_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CAnim_Tool() = default;

public:
	HRESULT Initialize(void* pArg)	override;
	void Start_Tool()				override;
	void Tick(_float fTimeDelta)	override;

private:
	HRESULT Load_KeyFrameNames(_int iSelModelIdx);
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

#pragma region Tool_Effect
private:
	void Main_Window_EffectTool();
	void Key_Input_EffectTool();
	void Bone_ListBox_EffectTool();

	void Effect_Tool();
	void Particle_Tool();
	void EffectMesh_Tool();
	void EffectTrail_Tool();

	void Check_Destroyed();

	void Effect_Created_Window();
	void Sync_Animation();

	CGameEffect::EFFECTSPAWNDESC Bake_EffectSpawnDesc();

private:
	void Key_Input_ParticleTool();
	void Particle_Desc_Window();
	
	HRESULT Save_EffectData();
	HRESULT Load_EffectData();
private:
	CGameEffect::EFFECTTYPE						m_eEffectMode;

	vector<pair<string, CModel*>>				m_LoadedMeshes;
	vector<pair<string, class CGameEffect*>>	m_CreatedEffects;
	_int										m_iSelectIdx = { -1 };
	_char										m_szEffectName[MAX_PATH] = "";

	_bool										m_bSyncAnimation = { false };
	_int										m_iSyncFrames[100] = {};
	_bool										m_bSyncd[100] = {};

	_bool										m_bGrouping[100] = {};

private:
	void Key_Input_EffectMeshTool();
	void EffectMesh_Desc_Window();
	void Models_Listbox();
	void Open_EffectMeshesByFolder();
	
	CModel* Get_Model(const string& strTag);

private:
	void Key_Input_EffectTrail();
	void EffectTrail_Desc_Window();
	void SyncEffectTrail();

private:
	_int	 m_iTrailStartFrame = { 0 };
	_int	 m_iTrailEndFrame = { 0 };

private:
#pragma region Transform_View
	_float3 m_vPosition = {};
	_float3 m_vRotation = {};
	_float3 m_vScale = {};

	class CGameObject* m_pGizmoObject = { nullptr };
	void Reset_Transform(_fmatrix WorldMatrix);
	void Transform_Window();
#pragma endregion

#pragma region Gizmo
private:
	typedef struct tagGizmoDesc
	{
		ImGuizmo::MODE CurrentGizmoMode = ImGuizmo::LOCAL;
		ImGuizmo::OPERATION CurrentGizmoOperation = ImGuizmo::TRANSLATE;
		bool bUseSnap = false;
		float snap[3] = { 0.5f, 0.5f, 0.5f };
		float bounds[6] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
		float boundsSnap[3] = { 0.1f, 0.1f, 0.1f };
		bool boundSizing = false;
		bool boundSizingSnap = false;
	}GIZMODESC;

	GIZMODESC m_tGizmoDesc;

	void Transform_Gizmo();
#pragma endregion
};

END

