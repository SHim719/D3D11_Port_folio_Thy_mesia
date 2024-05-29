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
	HRESULT Initialize(void* pArg)	override;
	void Tick(_float fTimeDelta)	override;

private:
	void Main_Window();
	void Camera_Window() override;

	HRESULT Open_MeshesByFolder();
	wstring Get_FolderPath();

	HRESULT Create_ObjectInLevel();
private:
	void Menu_Bar();
	void TabBar();

	void Transform_View();
	void Object_Child_Window();
	void Models_ListBox();
private:
	vector<CToolMapObj*> m_MapObjects;
	vector<string>	m_strPlacable_Objects;

#pragma region Transform_View
	_float3 m_vPosition = {};
	_float3 m_vRotation = {};
	_float3 m_vScale = {};
#pragma endregion

#pragma region Scenes_View
	_int m_iSelModelObj = 0;

#pragma endregion

public:
	static CMap_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END

