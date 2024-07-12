#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CLadder_Down final : public CGameObject
{
private:
	CLadder_Down(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLadder_Down(const CLadder_Down& rhs);
	virtual ~CLadder_Down() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	LADDERDESC		m_LadderDesc;

	CCollider*		m_pCollider = { nullptr };
	CShader*		m_pShader = { nullptr };
	CModel*			m_pModel = { nullptr };

public:
	const LADDERDESC& Get_LadderDesc() const {
		return m_LadderDesc;
	}

private:
	void OnCollisionStay(CGameObject* pOther)	override;

	HRESULT Ready_Components(void* pArg);

public:
	static CLadder_Down* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END

