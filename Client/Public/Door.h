#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CDoor final : public CGameObject
{
private:
	CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDoor(const CDoor& rhs);
	virtual ~CDoor() = default;

	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	_bool m_bOpening = { false};

	_float m_fTargetAngle = { -120.f };
	_float m_fNowAngle = { 0.f };

private:
	void OnCollisionStay(CGameObject* pOther)	override;

private:
	CCollider*	m_pCollider = { nullptr };
	CShader*	m_pShader = { nullptr };
	CModel*		m_pModel = { nullptr };

private:
	HRESULT Ready_Components(void* pArg);

public:
	static CDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END



