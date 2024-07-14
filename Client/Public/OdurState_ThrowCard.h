#pragma once

#include "OdurState_Base.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)
class COdurState_ThrowCard : public COdurState_Base
{
protected:
	COdurState_ThrowCard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COdurState_ThrowCard() = default;

public:
	HRESULT Initialize(void* pArg)		override;
	void OnState_Start(void* pArg)		override;
	void Update(_float fTimeDelta)		override;
	void Late_Update(_float fTimeDelta)	override;
	void OnState_End()					override;

	void Init_AttackDesc()				override;
private:
	_int						m_iThrowState = { 0 };

	_int						m_iThrowCount = { 0 };
	_int						m_iMaxThrowCount = { 3 };

	vector<OdurAnim>			m_AnimPlaylists[2];
	vector<_uint>				m_StartFrames[2];

	size_t						m_iCurAnimIdx = { 0 };

	vector<ATTACKDESC>			m_CardAttackDescs[2];
private:
	vector<class COdur_Card*>	m_Cards;
	size_t						m_iCurCardIdx = { 0 };
	CBone*						m_pLeftHandBone = { nullptr };
	
private:
	_vector Calc_LeftHandPos();

	void Throw_Card(_fvector vLook);
	void Throw_SingleCard();
	void Throw_MultipleCards();
	void Change_To_NextAnim();

public:
	static COdurState_ThrowCard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END
