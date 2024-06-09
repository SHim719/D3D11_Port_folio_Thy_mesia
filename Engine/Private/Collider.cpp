#include "Collider.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameInstance.h"

_uint CCollider::iCollisionID = 0;

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
	, m_pGameInstance{ rhs.m_pGameInstance }
	, m_iCollisionID{ iCollisionID++ }
	, m_eColliderType{rhs.m_eColliderType}
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{
	Safe_AddRef(m_pInputLayout);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollider::Initialize_Prototype()
{
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCodes = nullptr;
	size_t		iLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, iLength, &m_pInputLayout)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	COLLIDERDESC* pDesc = (COLLIDERDESC*)pArg;
	m_pOwner = pDesc->pOwner;

	return S_OK;
}

void CCollider::Update(_fmatrix WorldMatrix)
{
}

void CCollider::Render()
{
	m_pEffect->SetWorld(XMMatrixIdentity());

	m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);
}

_matrix CCollider::Remove_Rotation(_fmatrix Matrix)
{
	_matrix	ResultMatrix = Matrix;

	ResultMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(Matrix.r[0]);
	ResultMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(Matrix.r[1]);
	ResultMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(Matrix.r[2]);

	return ResultMatrix;
}

void CCollider::Free()
{
	__super::Free();

	m_pOwner = nullptr;

	Safe_Release(m_pGameInstance);

#ifdef _DEBUG
	if (false == m_bIsCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);
#endif
}
