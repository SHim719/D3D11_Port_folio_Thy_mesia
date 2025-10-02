#pragma once

#include "ToolState.h"
#include "ToolMapObj.h"


BEGIN(Client)
class CMap_Tool : public CToolState
{
private:
	CMap_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMap_Tool() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void Start_Tool()					override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;

private:
	HRESULT Ready_Picking();

private:
	void Main_Window();
	void Enemy_Window();
	void Trigger_Window();
	void Camera_Window() override;

	HRESULT Open_MeshesByFolder();
	wstring Get_FolderPath(const _tchar* szInitialPath = L"../../Resources/Maps/");

	HRESULT Create_ObjectInLevel();
	void Check_DestroyObjects();

private:
	void Menu_Bar();
	void TabBar();

	void Key_Input();

#pragma region MapTool
	void Map_Tool();
	void Key_Input_MapTool();
	void Destroy_MapObjects();
	void Transform_View();
	void Placable_ComboBox();
	void Placable_Object();
	void ForObject_Buttons(ImVec2 vNowCursorPos);
	void Object_ListBox();

	void Ready_InstanceObj();

	HRESULT Save_Map();
	HRESULT Load_Map();

private:
	vector<string>										m_strPlacable_Objects[OBJTYPE_END];
	unordered_map<string, _float>						m_CullingRadiuses;

	vector<CToolMapObj*>								m_MapObjects;						// 위치나 네비메쉬셀등의 값 등은 다 얘로 조절.
	vector<string>										m_strCreatedObjects;
	unordered_multimap<string, CToolMapObj*>			m_MapLayers;

	unordered_map<string, class CToolMapObjInstance*>	m_MapObjectInstances;				// 렌더링을 위한 인스턴스 객체.

	OBJTYPE												m_eNowObjMode = { MAPOBJECT };

	_int												m_iSelPlacableObj = { -1};
	set<_int>											m_SelectObjIndices;

#pragma region Transform_View
	_float3 m_vPosition = {};
	_float3 m_vRotation = {};
	_float3 m_vScale = {};

	void Reset_Transform(_fmatrix WorldMatrix);
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

#pragma region Enemy
	_int		m_iNowNaviIdx = -1;
#pragma endregion 

#pragma region Trigger
	_int		m_iTriggerIdx = -1;
	_float3		m_vColliderSize = { 0.f, 0.f, 0.f };
#pragma endregion

#pragma endregion MapTool


#pragma region NaviTool
private:
	void Navi_Tool();
	void Navi_KeyInput();
	void Navi_Transform();
	void Navi_Tool_Options();
	void Navi_ListBox();
	void Navi_Picking();

	void Picking_NaviMesh();
	void Picking_Point();

	HRESULT Save_NaviData();
	HRESULT Load_NaviData();
	bool	Load_UnrealNaviData();

	class CToolNaviCellPoint* Find_SamePoint(_fvector vPointPos);
	void Get_MouseRayInfo(OUT _float4& _vRayStartPos, OUT _float4& _vRayDir);
	void Destroy_Cells();
private:
	vector<class CToolNaviCell*>		m_CreatedCells;
	vector<class CToolNaviCellPoint*>	m_CreatedCellPoints;

	_int								m_iSelCellIdx = { -1 };
	vector<_int>						m_iSelPointIndices;
	_bool								m_bPickingMesh = { false };

#pragma endregion NaviTool


#pragma region Picking
private:

	ID3D11Texture2D*		m_pColorTexture = nullptr;
	ID3D11RenderTargetView* m_pColorRTV = nullptr;

	ID3D11Texture2D*		m_pPickingTexture = nullptr;

	_bool					m_bCanPick = false;
	void Object_Picking();
	_int ColorPicking_Object();

#pragma endregion


public:
	static CMap_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	void Free() override;
};

END

