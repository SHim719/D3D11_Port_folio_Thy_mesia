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
	void OnGoing(_float fTimeDelta)		override;
	void OnState_End()					override;

	void Init_AttackDesc()				override;
private:
	_int				m_iThrowCount = { 0 };
	_int				m_iMaxThrowCount = { 3 };
	_int				m_iThrowState = { 0 };

	vector<OdurAnim>	m_AnimPlaylists[2];
	vector<_int>		m_StartFrames[2];

	size_t				m_iCurAnimIdx = { 0 };

	vector<ATTACKDESC>	m_CardAttackDescs[2];
private:
	vector<class COdur_Card*>	m_Cards;
	size_t						m_iCurCardIdx = { 0 };
	CBone*						m_pLeftHandBone = { nullptr };

	_bool						m_bThrowMultipleCard = { false };
	_int						m_iNumCurrentThrowingCard = { 0 };
	_int						m_iNumThrowCard = { 5 };
	
	_float						m_fThrowingGap = { 0.08f };
	_float						m_fTimeAcc = { 0.f };

private:
	_vector Calc_LeftHandPos();

	void Throw_Card(_vector vLook);

#pragma region KeyFrameEvent
	void Throw_SingleCard();
	void Throw_MultipleCards();
	void Change_To_NextAnim();
#pragma endregion

public:
	static COdurState_ThrowCard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	void Free() override;
};

END
