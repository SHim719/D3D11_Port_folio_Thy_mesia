#include "GameEffect.h"


CGameEffect::CGameEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
	//XMStoreFloat4x4(&m_ParentWorldMatrix, XMMatrixIdentity());
}

CGameEffect::CGameEffect(const CGameEffect& rhs)
	: CGameObject(rhs)
	, m_eEffectType(rhs.m_eEffectType)
	//, m_ParentWorldMatrix(rhs.m_ParentWorldMatrix)
	, m_eTransformType(rhs.m_eTransformType)
	, m_fPlayTime(rhs.m_fPlayTime)
	, m_bComplete(rhs.m_bComplete)
	, m_iBaseTextureIdx(rhs.m_iBaseTextureIdx)
	, m_iMaskTextureIdx(rhs.m_iMaskTextureIdx)
	, m_iNoiseTextureIdx(rhs.m_iNoiseTextureIdx)
	, m_iEmissiveTextureIdx(rhs.m_iEmissiveTextureIdx)
	, m_iPassIdx(rhs.m_iPassIdx)
	, m_iRenderGroup(rhs.m_iRenderGroup)
	, m_vTextureFlag(rhs.m_vTextureFlag)
	, m_bLoop(rhs.m_bLoop)
	, m_vClipColor(rhs.m_vClipColor)
	, m_bGlow(rhs.m_bGlow)
	, m_vGlowColor(rhs.m_vGlowColor)
	, m_fGlowIntensity(rhs.m_fGlowIntensity)
	, m_bBloom(rhs.m_bBloom)
	, m_vWorldOffset(rhs.m_vWorldOffset)
	, m_bFollowOnlyPosition(rhs.m_bFollowOnlyPosition)
{
	strcpy_s(m_szBoneName, rhs.m_szBoneName);
	strcpy_s(m_szEffectName, rhs.m_szEffectName);
}


HRESULT CGameEffect::Bind_GlobalVariables()
{
	return S_OK;
}

HRESULT CGameEffect::Bind_ShaderResources()
{
	return S_OK;
}


HRESULT CGameEffect::Save_EffectData(ofstream& fout)
{
	fout.write((_char*)&m_eEffectType, sizeof(m_eEffectType));
	fout.write((_char*)&m_eTransformType, sizeof(TRANSFORMTYPE));
	fout.write(m_szEffectName, MAX_PATH);
	fout.write((_char*)&m_fPlayTime, sizeof(_float));
	fout.write((_char*)&m_bComplete, sizeof(_bool));
	fout.write((_char*)&m_bLoop, sizeof(_bool));
	fout.write(m_szBoneName, MAX_PATH);
	fout.write((_char*)&m_vWorldOffset, sizeof(_float3));
	fout.write((_char*)&m_bFollowOnlyPosition, sizeof(_bool));

	fout.write((_char*)&m_iBaseTextureIdx, sizeof(_int));
	fout.write((_char*)&m_iMaskTextureIdx, sizeof(_int));
	fout.write((_char*)&m_iNoiseTextureIdx, sizeof(_int));
	fout.write((_char*)&m_iEmissiveTextureIdx, sizeof(_int));
	fout.write((_char*)&m_iPassIdx, sizeof(_int));
	fout.write((_char*)&m_iRenderGroup, sizeof(_int));

	fout.write((_char*)&m_vTextureFlag, sizeof(_float4));
	fout.write((_char*)&m_vClipColor, sizeof(_float4));
	
	fout.write((_char*)&m_bGlow, sizeof(_bool));
	fout.write((_char*)&m_vGlowColor, sizeof(_float4));
	fout.write((_char*)&m_fGlowIntensity, sizeof(_float));

	fout.write((_char*)&m_bBloom, sizeof(_bool));

	return S_OK;
}

HRESULT CGameEffect::Load_EffectData(ifstream& fin)
{
	fin.read((_char*)&m_eTransformType, sizeof(TRANSFORMTYPE));
	fin.read(m_szEffectName, MAX_PATH);
	fin.read((_char*)&m_fPlayTime, sizeof(_float));
	fin.read((_char*)&m_bComplete, sizeof(_bool));
	fin.read((_char*)&m_bLoop, sizeof(_bool));
	fin.read(m_szBoneName, MAX_PATH);
	fin.read((_char*)&m_vWorldOffset, sizeof(_float3));
	fin.read((_char*)&m_bFollowOnlyPosition, sizeof(_bool));

	fin.read((_char*)&m_iBaseTextureIdx, sizeof(_int));
	fin.read((_char*)&m_iMaskTextureIdx, sizeof(_int));
	fin.read((_char*)&m_iNoiseTextureIdx, sizeof(_int));
	fin.read((_char*)&m_iEmissiveTextureIdx, sizeof(_int));
	fin.read((_char*)&m_iPassIdx, sizeof(_int));
	fin.read((_char*)&m_iRenderGroup, sizeof(_int));
	
	fin.read((_char*)&m_vTextureFlag, sizeof(_float4));
	fin.read((_char*)&m_vClipColor, sizeof(_float4));
	 
	fin.read((_char*)&m_bGlow, sizeof(_bool));
	fin.read((_char*)&m_vGlowColor, sizeof(_float4));
	fin.read((_char*)&m_fGlowIntensity, sizeof(_float));
	 
	fin.read((_char*)&m_bBloom, sizeof(_bool));

	return S_OK;

}

void CGameEffect::Restart_Effect(EFFECTSPAWNDESC* pDesc)
{
	if (nullptr == pDesc || nullptr == pDesc->pParentTransform)
		return;

	Safe_Release(m_pParentTransform);
	Safe_Release(m_pParentBone);

	m_pParentTransform = pDesc->pParentTransform;
	Safe_AddRef(m_pParentTransform);

	if (m_szBoneName[0] != '\0')
	{
		m_pParentBone = pDesc->pParentModel->Get_Bone(m_szBoneName);
		Safe_AddRef(m_pParentBone);
	}

	m_fTimeAcc = 0.f;
}

void CGameEffect::OnEnd_Effect()
{
	Safe_Release(m_pParentTransform);
	Safe_Release(m_pParentBone);
	//XMStoreFloat4x4(&m_ParentWorldMatrix, XMMatrixIdentity());
}

void CGameEffect::Update_World()
{
	if (SPAWN_ONLY == m_eTransformType)
		return;

	_matrix WorldMatrix = XMMatrixIdentity();

	if (nullptr != m_pParentBone)
	{
		if (m_bFollowOnlyPosition)
		{
			CALC_TF->Set_WorldMatrix(m_pParentTransform->Get_WorldMatrix());
			CALC_TF->Attach_To_Bone(m_pParentBone, m_pParentTransform, XMMatrixIdentity(), true);
		}
		else
			CALC_TF->Attach_To_Bone(m_pParentBone, m_pParentTransform);

		WorldMatrix = CALC_TF->Get_WorldMatrix();
	}
	else
	{
		if (m_pParentTransform)
			WorldMatrix = m_pParentTransform->Get_WorldMatrix();
	}

	m_pTransform->Set_WorldMatrix(WorldMatrix);
	m_pTransform->Add_Position(XMLoadFloat3(&m_vWorldOffset), true);
}

void CGameEffect::Update_TextureFlag()
{
	m_vTextureFlag.x = _float(m_iNoiseTextureIdx != -1);
	m_vTextureFlag.y = _float(m_iMaskTextureIdx != -1);
	m_vTextureFlag.z = _float(m_iEmissiveTextureIdx != -1);
}

void CGameEffect::Spawn_Effect()
{
	if (nullptr == m_pParentTransform)
		return;

	if ('\0' != m_szBoneName[0])
	{
		if (m_bFollowOnlyPosition)
		{
			CALC_TF->Set_WorldMatrix(m_pParentTransform->Get_WorldMatrix());
			CALC_TF->Attach_To_Bone(m_pParentBone, m_pParentTransform, XMMatrixIdentity(), true);
		}
		else
			CALC_TF->Attach_To_Bone(m_pParentBone, m_pParentTransform);
	}
	else
	{
		CALC_TF->Set_WorldMatrix(m_pParentTransform->Get_WorldMatrix());
	}

	CALC_TF->Add_Position(XMLoadFloat3(&m_vWorldOffset), true);

	m_pTransform->Set_WorldMatrix(CALC_TF->Get_WorldMatrix());
}

void CGameEffect::Free()
{
	__super::Free();

	Safe_Release(m_pParentTransform);
	Safe_Release(m_pParentBone);
	Safe_Release(m_pBaseTexture);
	Safe_Release(m_pMaskTexture);
	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pShader);
}
