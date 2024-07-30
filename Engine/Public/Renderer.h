#pragma once

#include "Base.h"

/* ȭ�鿡 �׷������� ��ü���� �׸��� ������� ��Ƽ� �����Ѵ�. */
/* �����ϰ� �ִ� ��ü���� Render�Լ��� ȣ���Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND
		, RENDER_EFFECT_NONBLEND, RENDER_EFFECT_BLEND, RENDER_EFFECT_GLOW, RENDER_UI, RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	class CGameInstance*	m_pGameInstance = { nullptr };

	class CVIBuffer_Rect*	m_pVIBuffer_Rect = { nullptr };
	class CVIBuffer_Screen*	m_pVIBuffer = { nullptr };

	/* ����Ÿ�� ������ �н� (0), �������� ���� �н� (1) */
	class CShader*			m_pDeferredShader = { nullptr };
	class CShader*			m_pGlowShader = { nullptr };
	class CShader*			m_pBloomShader = { nullptr };
	class CShader*			m_pPostProcessShader = { nullptr };

private:
	/* m_WorldMatrix : ȭ���� �� ä��� ���������� ����� Ȱ��Ǳ⶧���� �����صѲ��� .*/
	_float4x4					m_WorldMatrix{};

public:
	HRESULT Initialize();
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);	
	HRESULT Add_RenderComponent(class CComponent* pRenderComponent);
	void	Add_UsingShader(class CShader* pShader);
	HRESULT Draw();
	void	Clear();

private:
	void Draw_Objects(_uint iGroup);
	void Bind_ViewProj();

	HRESULT Copy_RenderTarget(const wstring& wstrTargetTag);

	HRESULT Render_NonBlend();
	HRESULT Render_NonLight();
	HRESULT Render_LightAcc();
	HRESULT Render_Deferred();

	HRESULT Render_Effect();

	HRESULT Render_Glow();

	HRESULT Render_SampleForBloom();
	HRESULT Render_BrightPass();
	HRESULT Render_DownSample();
	HRESULT Render_UpSample();

	HRESULT Render_PostProcess();
	HRESULT PostProcess_Bloom();

	HRESULT Render_Final();

	HRESULT Render_Blend();
	HRESULT Render_UI();
	HRESULT Render_Components();

private:
	HRESULT Ready_DefaultTargets(_uint iWidth, _uint iHeight);
	HRESULT Ready_LightTargets(_uint iWidth, _uint iHeight);
	HRESULT Ready_DeferredTarget(_uint iWidth, _uint iHeight);
	HRESULT Ready_EffectTargets(_uint iWidth, _uint iHeight);
	HRESULT Ready_GlowTargets(_uint iWidth, _uint iHeight);
	HRESULT Ready_BloomTargets(_uint iWidth, _uint iHeight);

#ifdef _DEBUG
private:
	HRESULT Ready_Debug();
	HRESULT Render_Debug();
	_bool m_bRenderRTV = { true };
#endif


private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	list<class CComponent*>				m_RenderComponents;
	list<class CShader*>				m_UsingShaders;

	class CVIBuffer*					m_pVIBuffer_UI = { nullptr };

	D3D11_VIEWPORT						m_OriginViewPort = {};

private:
	_uint m_iBloomLevel = { 3 };

private:
	HRESULT Ready_Buffers();
	HRESULT Ready_Shaders();
	HRESULT Bind_Matrices();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END