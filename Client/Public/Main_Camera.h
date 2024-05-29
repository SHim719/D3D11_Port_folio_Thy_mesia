#pragma once

#include "Client_Defines.h"
#include "MyCamera.h"


BEGIN(Client)

class CMain_Camera final : public CCamera
{
public:
	enum CameraState
	{
		DEFAULT,
		LOCKON,
		CUTSCENE,
		UI,
		TARGET_END
	};

protected:
	CMain_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_Camera(const CMain_Camera& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CMain_Camera() = default;

public:
	HRESULT Initialize(void* pArg)			override;
	void OnActive()							override;
	void OnInActive()						override;
	void PriorityTick(_float fTimeDelta)	override;
	void Tick(_float fTimeDelta)			override;
	void LateTick(_float fTimeDelta)		override;

private:
	CameraState m_eState = DEFAULT;

	class CGameObject* m_pTarget = nullptr;
	class CBone* m_pCameraBone = nullptr;

	_float4 m_vPlayerNeckOffset = { 0.f, 2.f, 0.f, 0.f };
	POINT m_CurrentMousePos = { -1, -1 };
public:
	void Set_Target(CGameObject* pTarget);

private:
	void Default_State(_float fTimeDelta);

	void Rotation_ByMouse(_float fTimeDelta);

public:
	static CMain_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	void Free()	override;
};

END

