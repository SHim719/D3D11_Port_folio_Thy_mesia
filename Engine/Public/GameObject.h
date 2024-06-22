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

	virtual void OnActive();
	virtual void OnInActive();

	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

	virtual HRESULT Render();
public:
	HRESULT Add_PoolingObject_To_Layer(const wstring& wstrLayer, void* pArg);
	virtual HRESULT OnEnter_Layer(void* pArg);

	
protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	class CGameInstance*	m_pGameInstance = { nullptr };

protected:
	class CTransform*	m_pTransform = nullptr;

	map<const wstring, class CComponent*>	m_Components;

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
	_bool	m_bDestroyed = { false };
	_bool	m_bActive = { true };
	_bool	m_bCreatedThisFrame = { true };

	_bool	m_bReturnToPool = { false }; // For Object Pool
	_bool	m_bNoRender = { false };

	_uint m_iTag = 0;
public:
	void Set_Destroy(_bool b) { m_bDestroyed = b; }
	_bool Is_Destroyed() { 
		return m_bDestroyed; }

	void Set_Active(_bool b);
	_bool Is_Active() { 
		return m_bActive; }

	_bool Is_CreatedThisFrame() const { 
		return m_bCreatedThisFrame; }

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

	void Enable_Render() {
		m_bNoRender = false;
	}
	void Disable_Render() {
		m_bNoRender = true;
	}

	_bool No_Render() const {
		return m_bNoRender;
	}

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

/* Mouse Picking!!!!*/
//void CToolCamera::Mouse_Ray()
//{
//	_float3 fMouseNDC_Near = _float3(m_tCurrentMousePos.x * 2.0f / g_iWinSizeX - 1, -m_tCurrentMousePos.y * 2.0f / g_iWinSizeY + 1, 0.f);
//	_float3 fMouseNDC_Far = _float3(m_tCurrentMousePos.x * 2.0f / g_iWinSizeX - 1, -m_tCurrentMousePos.y * 2.0f / g_iWinSizeY + 1, 1.f);
//
//	_float4x4 inverseProjView;
//	D3DXMatrixInverse(&inverseProjView, nullptr, &(m_ViewMatrix * m_ProjMatrix));
//
//	_float3 fMouseWorld_Near = *D3DXVec3TransformCoord(&fMouseWorld_Near, &fMouseNDC_Near, &inverseProjView);
//	_float3 fMouseWorld_Far = *D3DXVec3TransformCoord(&fMouseWorld_Far, &fMouseNDC_Far, &inverseProjView);
//
//	_float3 vRayDir = *D3DXVec3Normalize(&vRayDir, &(fMouseWorld_Far - fMouseWorld_Near));
//
//	RAY_DESC rayDesc{};
//	rayDesc.iLevel = LEVEL_TOOL;
//	rayDesc.strDstLayer = L"Wall";
//	rayDesc.vRayDir = vRayDir;
//	rayDesc.vRayWorldPos = fMouseWorld_Near;
//
//	m_pGameInstance->Add_RayDesc(rayDesc);
//}