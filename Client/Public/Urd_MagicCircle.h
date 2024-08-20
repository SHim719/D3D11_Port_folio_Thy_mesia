#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Client)

class CUrd_MagicCircle : public CGameObject
{
public:
	typedef struct tagUrdMagicCircleDesc {
		_float4		vPosition = { 0.f, 0.f, 0.f, 1.f };
		_bool		bUltimate = false;
	} URD_MAGICCIRCLE_DESC;

private:
	CUrd_MagicCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUrd_MagicCircle(const CUrd_MagicCircle& rhs);
	virtual ~CUrd_MagicCircle() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;

	HRESULT OnEnter_Layer(void* pArg)	override;

public:
	void Explosion(_bool bDisappear = false);

	void Set_ForUltimate() {
		m_bForUltimate = true;
	}

	
private:
	void Change_Effect(class CEffect_Group* pEffectGroup);

private:
	_bool	m_bUltimate = { false };
	_bool	m_bExplosion = { false };
	_bool	m_bForUltimate = { false };
	
	_float  m_fAppearingTime = { 1.5f };

private:
	CGameEffect::EFFECTSPAWNDESC	m_tEffectSpawnDesc;
	class CEffect_Group*			m_pEffect_MagicAppear = { nullptr };
	class CEffect_Group*			m_pEffect_MagicImpact = { nullptr };

	CEffect_Group*					m_pNowEffect = { nullptr };
	CAttackable*					m_pAttackable = { nullptr };
	CCollider*						m_pCollider = { nullptr };

private:
	HRESULT Ready_Components(void* pArg);

public:
	static CUrd_MagicCircle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END

