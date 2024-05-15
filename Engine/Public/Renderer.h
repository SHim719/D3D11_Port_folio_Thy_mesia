#pragma once

#include "Base.h"

/* ȭ�鿡 �׷������� ��ü���� �׸��� ������� ��Ƽ� �����Ѵ�. */
/* �����ϰ� �ִ� ��ü���� Render�Լ��� ȣ���Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CBase
{
public:
	/* �׸��� ������� ����ü�� �����ߴ�. */
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_BLEND, RENDER_UI, RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

public:
	HRESULT Initialize(void* pArg);
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);	
	HRESULT Draw();
	void Clear();

private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END