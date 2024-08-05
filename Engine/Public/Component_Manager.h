#pragma once

#include "Base.h"

#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"
#include "Transform.h"

#include "VIBuffer_Rect.h"
#include "VIBuffer_Point.h"
#include "VIBuffer_Cell.h"
#include "VIBuffer_Sphere.h"
#include "VIBuffer_Particle.h"
#include "VIBuffer_Screen.h"
#include "VIBuffer_Rect_NorTex.h"
#include "VIBuffer_Trail.h"

#include "Model.h"
#include "Model_Instance.h"

#include "Navigation.h"

#include "AABB.h"
#include "OBB.h"
#include "Sphere.h"


/* 객체화된 컴포넌트(원형)를 레벨별로 저장한다. <-> 사본객체를 보관하지 않는다. 사본객체는 각 객체(플레이어, 몬스터, 지형 등등)들이 보관한다.  */
/* 내가 지정한 원형객체를 찾아서 복제(사본을 생성한다)하여 리턴해준다. */

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg);
	void Clear(_uint iLevelIndex);

private:
	_uint											m_iNumLevels = { 0 };
	map<const wstring, class CComponent*>*			m_pPrototypes = { nullptr };
	typedef map<const wstring, class CComponent*>	PROTOTYPES;

public:
	class CComponent* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

public:
	static CComponent_Manager* Create(_uint iNumLevels);
	void Free() override;
};

END