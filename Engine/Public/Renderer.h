#pragma once

#include "Base.h"

/* 화면에 그려져야할 객체들을 그리는 순서대로 모아서 저장한다. */
/* 저장하고 있는 객체들의 Render함수를 호출한다. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CBase
{
public:
	/* 그리는 순서대로 열거체를 정의했다. */
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