#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CBone;
END

BEGIN(Client)

class CToolWeaponObj final : public CGameObject
{
public:
	typedef struct tagWeaponDesc
	{
		CBone* pSocketBone = nullptr;
		CTransform* pParentTransform = nullptr;
		wstring wstrModelTag = L"";
		_float4x4 PivotMatrix;
	} WEAPONDESC;

private:
	CToolWeaponObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	ToolWeaponObj(const CToolWeaponObj& rhs);
	virtual ~CToolWeaponObj() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	CShader* m_pShader = nullptr;
	CModel* m_pModel = nullptr;
	//CCollider*	m_pCollider[COLLILDERTYPE_END] = { nullptr };

private:
	WEAPONDESC		m_WeaponDesc;

private:
	HRESULT Ready_Components(const wstring& wstrModelTag);

public:
	static CToolWeaponObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END