#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CToolMapObj final : public CGameObject
{
public:
	typedef struct tagToolMapObjDesc
	{
		OBJTYPE eObjType = OBJTYPE_END;
		wstring wstrModelTag = L"";
	}TOOLMAPOBJDESC;

private:
	CToolMapObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolMapObj(const CToolMapObj& rhs);
	virtual ~CToolMapObj() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	HRESULT Initialize_Load(void* pArg);
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT Render_Picking(_int iSelectIdx);

	_bool Ray_Cast(_fvector vRayStartPos, _fvector vRayDir, OUT _float4& vPickedPos, OUT _float& fDist);

public:
	_int Get_NaviIdx() const { return m_iNaviIdx; }
	void Set_NaviIdx(_int iIdx) { m_iNaviIdx = iIdx; }

	_int Get_TriggerIdx() const { return m_iTriggerIdx; }
	void Set_TriggerIdx(_int iIdx) { m_iTriggerIdx = iIdx; }

	_float3 Get_ColliderSize() const;
	void Set_ColliderSize(const _float3& vSize);

	OBJTYPE Get_ObjType() const { return m_eObjType; }
private:
	OBJTYPE		m_eObjType = { MAPOBJECT };
	_int		m_iNaviIdx = { -1 };
	_int		m_iTriggerIdx = { 0 };

private:
	CShader*		m_pShader = nullptr;
	CModel*			m_pModel = nullptr;
	CCollider*		m_pTriggerCollider = { nullptr };

private:
	HRESULT Ready_Components(const wstring& wstrModelTag);

public:
	static CToolMapObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END

