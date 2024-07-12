#pragma once

#include "Base.h"

/* ȭ�鿡 �׷������� ��ü���� �׸��� ������� ��Ƽ� �����Ѵ�. */
/* �����ϰ� �ִ� ��ü���� Render�Լ��� ȣ���Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CBase
{
public:
	/* �׸��� ������� ����ü�� �����ߴ�. */
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	class CGameInstance*	m_pGameInstance = { nullptr };

	class CVIBuffer_Rect*	m_pVIBuffer = { nullptr };

	/* ����Ÿ�� ������ �н� (0), �������� ���� �н� (1) */
	class CShader*			m_pShader = { nullptr };

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

	HRESULT Render_NonBlend();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_UI();
	HRESULT Render_LightAcc();
	HRESULT Render_Deferred();
	HRESULT Render_Components();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif

private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	list<class CComponent*>				m_RenderComponents;
	list<class CShader*>				m_UsingShaders;

	class CVIBuffer*					 m_pVIBuffer_UI = { nullptr };

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END