#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class COdur_Card_Cutscene :public CGameObject
{
private:
	COdur_Card_Cutscene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COdur_Card_Cutscene(const COdur_Card_Cutscene& rhs);
	virtual ~COdur_Card_Cutscene() = default;

	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	ATTACHDESC	m_AttachDesc;

private:
	CShader*	m_pShader = { nullptr };
	CModel*		m_pModel = { nullptr };

private:
	HRESULT Ready_Component();

public:
	static COdur_Card_Cutscene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	void Free()	override;
};
END

