#pragma once

#include "Manager_Headers.h"

/* 클라이언트와 엔진의 중계자의 역활을 수행한다. */
/* 클라이언트에서 엔진의 기능을 쓰고하자 한다라면 무조건 게임인스턴스를 떠올리면 되도록. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, OUT ID3D11Device** ppDevice, OUT ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT	Draw();
	void Clear(_uint iLevelIndex);
	void Clear_Level(_uint iLevelIndex);

#pragma region GRAPHIC_DEVICE
public:
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();
#pragma endregion

#pragma region LEVEL_MANAGER
public: /* For.Level_Manager */
	HRESULT Change_Level(class CLevel* pNewLevel);
	_uint Get_CurrentLevelID();
#pragma endregion

#pragma region TIMER_MANAGER
public:
	_float	Get_TimeDelta(const wstring& strTimerTag);
	HRESULT Add_Timer(const wstring& strTimerTag);
	void Compute_TimeDelta(const wstring& strTimerTag);
	void Set_TimeScale(_float fTimeScale);
	_float Get_TimeScale() { return m_fTimeScale; }
private:
	_float m_fTimeScale = 1.f;
#pragma endregion

#pragma region OBJECT_MANAGER
public:
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	class CGameObject* Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex = 0);
	class CGameObject* Find_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex = 0);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	void Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag);

	void Insert_GameObject(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pObj);

	CGameObject* Find_Target(_fvector vPlayerPos);
#pragma endregion

#pragma region COMPONENT_MANAGER
public:
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion


#pragma region RENDERER
public:
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
#pragma endregion

#pragma region KEY_MANAGER
	bool GetKeyDown(eKeyCode _keyCode) { return	 m_pKey_Manager->GetKeyDown(_keyCode); }
	bool GetKeyUp(eKeyCode _keyCode)	{ return m_pKey_Manager->GetKeyUp(_keyCode); }
	bool GetKey(eKeyCode _keyCode)	{ return m_pKey_Manager->GetKey(_keyCode); }
	bool GetKeyNone(eKeyCode _keyCode) { return m_pKey_Manager->GetKeyNone(_keyCode); }
#pragma endregion


#pragma region COLLISION_MANAGER
	void Add_ColliderToLayer(const string& strLayer, CCollider* pCollider);
#pragma endregion

#pragma region SOUND_MANAGER
	HRESULT Create_Sound(const string& strPath, const wstring& strSoundTag);
	HRESULT Play(const wstring& strSoundTag, _bool bLoop);
	HRESULT Stop(const wstring& strSoundTag);
	HRESULT SetVolume(const wstring& strSoundTag, const _float& fVolume);
	HRESULT SetPosition(const wstring& strSoundTag, _float fPosition);
	_bool Is_Playing(const wstring& strSoundTag);
#pragma endregion

#pragma region FRUSTUM
	_bool In_WorldFrustum(_float3 vWorldPos, _float fRadius);
#pragma endregion 

#pragma region PIPELINE
	void Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4 Get_CamPosition() const;
#pragma endregion



private:
	CGraphic_Device*		m_pGraphic_Device = { nullptr };
	CLevel_Manager*			m_pLevel_Manager = { nullptr };
	CTimer_Manager*			m_pTimer_Manager = { nullptr };
	CObject_Manager*		m_pObject_Manager = { nullptr };
	CComponent_Manager*		m_pComponent_Manager = { nullptr };
	CKey_Manager*			m_pKey_Manager = { nullptr };
	CCollision_Manager*		m_pCollision_Manager = { nullptr };
	CSound_Manager*			m_pSound_Manager = { nullptr };
	//CFrustum*				m_pFrustum = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };
	CPipeLine*				m_pPipeLine = { nullptr };

	
public:
	static void Release_Engine();
	virtual void Free() override;
	
};

END