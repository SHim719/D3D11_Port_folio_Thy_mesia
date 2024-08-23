#pragma once

#include "Base.h"
#include "GameInstance.h"

/*클라이언트개발자가 실질적으로 생성해낸 객체타입(Player, Monster)의 부모가 되는 클래스이다. */


BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT OnEnter_Layer(void* pArg);

	virtual void OnActive();
	virtual void OnInActive();

	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

	virtual HRESULT Render();

	virtual	_vector Get_Center() const;
protected:
	virtual HRESULT Bind_ShaderResources();
	virtual void Decide_PassIdx();

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	class CGameInstance*	m_pGameInstance = { nullptr };

protected:
	class CTransform*	m_pTransform = nullptr;

	map<const wstring, class CComponent*>	m_Components;

protected:
	CTexture*						m_pDissolveTexture = { nullptr };
	_bool							m_bDissolve = { false };
	_float							m_fDissolveAmount = { 0.f };
	_float							m_fDissolveSpeed = { 0.4f };

public:
	void Active_Dissolve();
	void Update_Dissolve(_float fTimeDelta);

	_bool Is_Dissolving() const {
		return m_bDissolve;
	}
public:
	CTransform* Get_Transform() { return m_pTransform; }

public:
	HRESULT Add_Component(_uint iPrototoypeLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);
	class CComponent* Find_Component(const wstring& strComponentTag);

public:
	virtual _bool On_Ray_Intersect(const _float3& fHitWorldPos, const _float& fDist, void* pArg = nullptr) { return false; }

public:
	virtual void OnCollisionEnter(CGameObject* pOther) {}
	virtual void OnCollisionStay(CGameObject* pOther) {}
	virtual void OnCollisionExit(CGameObject* pOther) {}

protected:
	_bool		m_bDestroyed = { false };
	_bool		m_bActive = { true };
	_bool		m_bInstancing = { false }; // 인스턴싱되는 오브젝트면 렌더링을 하지않아 물체자체의 로직만 돌아가게만듬.

	_bool		m_bReturnToPool = { false }; // For Object Pool
	_bool		m_bNoRender = { false };

	_uint		m_iTag = { 0 };

	_float		m_fCullingRadius = { 1.f };
	_float4		m_vCullingOffset = {};

	_uint		m_iPassIdx = { 0 };

	_uint		m_iActorID = { 999999 };
public:
	void Set_Destroy(_bool b) { m_bDestroyed = b; }
	_bool Is_Destroyed() { 
		return m_bDestroyed; }

	void Set_Active(_bool b);
	_bool Is_Active() { 
		return m_bActive; }

	_uint Get_Tag() const { return m_iTag; }

	_bool Is_OutOfLayer() const {
		return m_bDestroyed || m_bReturnToPool;
	}

	void Set_ReturnToPool(_bool b) {
		m_bReturnToPool = b;
	}

	_bool Is_ReturnToPool() const {
		return m_bReturnToPool;
	}

	void Set_NoRender(_bool bNoRender) {
		m_bNoRender = bNoRender;
	}

	_bool No_Render() const {
		return m_bNoRender;
	}

	void Set_CullingRadius(_float fRadius) {
		m_fCullingRadius = fRadius;
	}

	void Set_Instancing(_bool bInstancing) {
		m_bInstancing = bInstancing;
	}

	_float Get_CamDistance() const {
		return XMVector3Length(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()) - m_pTransform->Get_Position()).m128_f32[0];
	}

	_uint Get_ActorID() const {
		return m_iActorID;
	}

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	void Free() override;
};

END