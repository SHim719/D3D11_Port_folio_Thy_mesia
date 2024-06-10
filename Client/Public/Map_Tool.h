#pragma once

#include "ToolState.h"
#include "ToolMapObj.h"


BEGIN(Client)
class CMap_Tool : public CToolState
{
protected:
	CMap_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMap_Tool() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;

private:
	HRESULT Ready_Picking();

private:
	void Main_Window();
	void Camera_Window() override;

	HRESULT Open_MeshesByFolder();
	wstring Get_FolderPath();

	HRESULT Create_ObjectInLevel();
private:
	void Menu_Bar();
	void TabBar();

	void Destroy_MapObjects();
	void Transform_View();
	void Placable_Object();
	void Object_ListBox();
private:
	vector<string>								m_strPlacable_Objects;
	vector<CToolMapObj*>						m_MapObjects;
	vector<string>								m_strCreatedObjects;
	unordered_multimap<string, CToolMapObj*>	m_MapLayers; // 나중에 얘 데이터로 저장할거임.

	_int m_iSelPlacableObj = 0;
	_int m_iSelObj = -1;
	_bool m_bMouseUsed = false;

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

#pragma region Picking
private:

	ID3D11Texture2D*		m_pColorTexture = nullptr;
	ID3D11RenderTargetView* m_pColorRTV = nullptr;

	ID3D11Texture2D*		m_pPickingTexture = nullptr;

	void Picking();
	_int Picking_Object();
#pragma endregion


public:
	static CMap_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END

