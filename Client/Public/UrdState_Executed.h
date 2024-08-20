#pragma once

#include "UrdState_Base.h"

BEGIN(Client)
class CUrdState_Executed final : public CUrdState_Base
{
private:
	CUrdState_Executed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUrdState_Executed() = default;

	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

private:
	ATTACHDESC	m_AttachDesc = {};

	_bool		m_bOnlyPosition = { false };

private:
	void Die();

	void Set_OnlyPosition() {
		m_bOnlyPosition = true;
	}

	void Spawn_Effect_Execution_Blood_Phase2()
	{
		if (true == m_pUrd->Is_Phase2())
			EFFECTMGR->Active_Effect("Effect_Urd_Execution_Blood_Phase2", &m_pUrd->Get_EffectSpawnDesc());
	}

	
	
public:
	static CUrdState_Executed* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END


