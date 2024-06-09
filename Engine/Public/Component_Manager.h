#pragma once

#include "Base.h"

#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"
#include "Transform.h"
#include "VIBuffer_Rect.h"
#include "Model.h"

#include "AABB.h"
#include "OBB.h"
#include "Sphere.h"


/* ��üȭ�� ������Ʈ(����)�� �������� �����Ѵ�. <-> �纻��ü�� �������� �ʴ´�. �纻��ü�� �� ��ü(�÷��̾�, ����, ���� ���)���� �����Ѵ�.  */
/* ���� ������ ������ü�� ã�Ƽ� ����(�纻�� �����Ѵ�)�Ͽ� �������ش�. */

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

private:
	class CComponent* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

public:
	static CComponent_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END