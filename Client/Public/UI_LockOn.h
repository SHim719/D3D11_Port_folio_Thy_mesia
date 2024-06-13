#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_LockOn final : public CUI
{
public:
	typedef struct tagLockOnDesc
	{
		CTransform* pTargetTransform = { nullptr };
		class CBone* pTargetBone = { nullptr };
	} LOCKONDESC;
private:
	CUI_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LockOn(const CUI_LockOn& rhs);
	virtual ~CUI_LockOn() = default;

private:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	void On_UIActive(void* pArg)		override;

private:
	CTransform*		m_pTargetTransform = { nullptr };
	class CBone*	m_pTargetBone = { nullptr };

public:
	static CUI_LockOn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END

