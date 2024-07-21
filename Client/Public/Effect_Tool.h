#pragma once

#include "ToolState.h"

BEGIN(Client)

class CEffect_Tool : public CToolState
{
protected:
	CEffect_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEffect_Tool() = default;

public:
	HRESULT Initialize(void* pArg)	override;
	void Start_Tool()				override;
	void Tick(_float fTimeDelta)	override;

private:
	void Main_Window();

	void Particle_Tool();
	void EffectMesh_Tool();
	
	void Check_Destroyed();

private:
	void Key_Input_ParticleTool();
	void Particle_Desc_Window();
	void Particle_Created_Window();
	//void Particle_Gizmo();

	void Save_ParticleData();
	void Load_ParticleData();

	vector<pair<string, class CToolEffect_Particle*>>	m_CreatedParticles;
	_int												m_iSelectIdx = { -1 };
	_char												m_szParticleName[MAX_PATH] = "";

private:
	vector<pair<string, CModel*>>					m_LoadedMeshes;
	class CToolEffect_Mesh*							m_pEffect_Mesh = { nullptr };
	//_int											m_iSelectIdx = { -1 };
	_char											m_szEffectMeshName[MAX_PATH] = "";

	void Key_Input_EffectMeshTool();
	void Models_Listbox();
	void EffectMesh_Desc_Window();
	void EffectMesh_Created_Window();
	void EffectMesh_Gizmo();

	void Open_MeshesByFolder();
public:
	static CEffect_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;


};

END

