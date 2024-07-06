#include "PlagueWeapon.h"

CPlagueWeapon::CPlagueWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon(pDevice, pContext)
{
}

CPlagueWeapon::CPlagueWeapon(const CPlagueWeapon& rhs)
	: CWeapon(rhs)
{
}

HRESULT CPlagueWeapon::Render()
{
	if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pModel->SetUp_OnShader(m_pShader, 0, TextureType_DIFFUSE, "g_DiffuseTexture")))
		return E_FAIL;

	/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModel->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		return E_FAIL;*/

	if (FAILED(m_pModel->Bind_Buffers(0)))
		return E_FAIL;

	if (FAILED(m_pModel->Render(m_pShader, 0, 3)))
		return E_FAIL;

	return S_OK;
}


CPlagueWeapon* CPlagueWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlagueWeapon* pInstance = new CPlagueWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlagueWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlagueWeapon::Clone(void* pArg)
{
	CPlagueWeapon* pInstance = new CPlagueWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlagueWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlagueWeapon::Free()
{
	__super::Free();

}


