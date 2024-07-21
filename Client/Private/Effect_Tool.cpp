#include "Effect_Tool.h"

#include "ToolEffect_Particle.h"
#include "ToolEffect_Mesh.h"


CEffect_Tool::CEffect_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CToolState(pDevice, pContext)
{
}

HRESULT CEffect_Tool::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CEffect_Tool::Start_Tool()
{
    m_pGameInstance->Add_Clone(LEVEL_TOOL, L"TestGround", L"Prototype_TestGround");

    CCamera::CAMERADESC camDesc{};
    camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    camDesc.fNear = 0.1f;
    camDesc.fFar = 300.f;
    camDesc.fFovy = 65.f;
    camDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
    camDesc.vEye = { 0.f, 2.f, -2.f, 1.f };

    m_pCamera = static_cast<CFree_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_Free_Camera", &camDesc));

    m_pGameInstance->Change_MainCamera(m_pCamera);
}


void CEffect_Tool::Tick(_float fTimeDelta)
{
    Main_Window();

}

void CEffect_Tool::Main_Window()
{
    ImGui::Begin("Main Window", (bool*)0);

    if (ImGui::BeginTabBar("Effect_Tool"))
    {
        if (ImGui::BeginTabItem("Particle"))
        {
            Particle_Tool();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Effect Mesh"))
        {
            EffectMesh_Tool();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void CEffect_Tool::Key_Input_ParticleTool()
{
    if (m_iSelectIdx < 0)
        return;

    if (KEY_DOWN(eKeyCode::Space))
    {
        m_CreatedParticles[m_iSelectIdx].second->Restart_Particle();
    }

    if (KEY_DOWN(eKeyCode::RShift))
    {
        m_CreatedParticles[m_iSelectIdx].second->Remake_Particle();
    }
}

void CEffect_Tool::Particle_Tool()
{
    Key_Input_ParticleTool();
    Particle_Desc_Window();
    Particle_Created_Window();
}

void CEffect_Tool::EffectMesh_Tool()
{
    Key_Input_EffectMeshTool();
    EffectMesh_Desc_Window();
    EffectMesh_Created_Window();
}

void CEffect_Tool::Check_Destroyed()
{
}

void CEffect_Tool::Particle_Desc_Window()
{
    if (m_iSelectIdx < 0 || m_CreatedParticles.empty())
        return;

    CToolEffect_Particle* pParticle = m_CreatedParticles[m_iSelectIdx].second;

    ImGui::Checkbox("NoRender", &pParticle->m_bNoRender);
    ImGui::Checkbox("BillBoard?", &pParticle->m_bBillBoard);
    ImGui::Checkbox("OrientToVelocity?", &pParticle->m_bOrientToVelocity);

    if (ImGui::InputInt("NumInstances", &pParticle->m_iNumParticles))
        pParticle->Resize_Particles((_uint)pParticle->m_iNumParticles);

    ImGui::InputInt("MaskTexIdx", &pParticle->m_iMaskTexureIdx);
    ImGui::InputInt("PassIdx", &pParticle->m_iPassIdx);

    ImGui::DragFloat4("Color", (_float*)&pParticle->m_vColor, 0.1f, 0.f, 1.f);

    ImGui::DragFloat3("StartPosMin", (_float*)&pParticle->m_vStartPosMin, 0.1f, -99.f, 99.f);
    ImGui::DragFloat3("StartPosMax", (_float*)&pParticle->m_vStartPosMax, 0.1f, -99.f, 99.f);

    ImGui::DragFloat3("DirMin", (_float*)&pParticle->m_vDirMin, 0.05f, -1.f, 1.f);
    ImGui::DragFloat3("DirMax", (_float*)&pParticle->m_vDirMax, 0.05f, -1.f, 1.f);

    ImGui::DragFloat("VelocityScaleMin", &pParticle->m_fVelocityScaleMin, 0.1f, -999.f, 999.f);
    ImGui::DragFloat("VelocityScaleMax", &pParticle->m_fVelocityScaleMax, 0.1f, -999.f, 999.f);

    ImGui::InputFloat("LifeTime", &pParticle->m_fLifeTime);

    ImGui::DragFloat3("ForceDirMin", (_float*)&pParticle->m_vForceDirMin, 0.05f, -1.f, 1.f);
    ImGui::DragFloat3("ForceDirMax", (_float*)&pParticle->m_vForceDirMax, 0.05f, -1.f, 1.f);

    ImGui::DragFloat("ForceScaleMin", &pParticle->m_fForceScaleMin, 1.f, -999.f, 999.f);
    ImGui::DragFloat("ForceScaleMax", &pParticle->m_fForceScaleMax, 1.f, -999.f, 999.f);

    ImGui::DragFloat2("ScaleMin", (_float*)&pParticle->m_vScaleMin, 0.05f,  0.001f, 0.001f);
    ImGui::DragFloat2("ScaleMax", (_float*)&pParticle->m_vScaleMax, 0.05f, 100.f, 100.f);
    ImGui::DragFloat2("ScaleSpeed", (_float*)&pParticle->m_vScaleSpeed, 0.05f, 0.001f, 0.001f);

    ImGui::SliderFloat3("vRotationMin", (_float*)&pParticle->m_vRotationMin, -XM_PI, XM_PI);
    ImGui::SliderFloat3("vRotationMax", (_float*)&pParticle->m_vRotationMax, -XM_PI, XM_PI);
    ImGui::SliderFloat3("vRotationSpeed", (_float*)&pParticle->m_vRotationSpeed, -XM_PI, XM_PI);
}

void CEffect_Tool::Particle_Created_Window()
{
    ImGui::Begin("Particle List");

    ImGui::Text("Effect Name");
    ImGui::SameLine();
    ImGui::PushItemWidth(100.f);
    ImGui::InputText("##EffectName", m_szParticleName, MAX_PATH);
    ImGui::PopItemWidth();


   const _char** szParticleNames = new const _char* [m_CreatedParticles.size()];
   
   for (size_t i = 0; i < m_CreatedParticles.size(); ++i)
       szParticleNames[i] = m_CreatedParticles[i].first.c_str();
   
   ImGui::ListBox("##Particles", &m_iSelectIdx, szParticleNames, (_int)m_CreatedParticles.size(), 10);
   
   Safe_Delete_Array(szParticleNames);

   if (ImGui::Button("Create"))
   {
       CToolEffect_Particle* pParticle = static_cast<CToolEffect_Particle*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Effect", L"Prototype_ToolEffect_Particle"));
       m_CreatedParticles.emplace_back(make_pair(string(m_szParticleName), pParticle));
   }

   if (ImGui::Button("Save"))
   {
       Save_ParticleData();
      
   }
   ImGui::SameLine();
   if (ImGui::Button("Load"))
   {
       Load_ParticleData();
   }

   ImGui::End();
}

void CEffect_Tool::Save_ParticleData()
{
    _tchar szFullPath[200000] = {};
    OPENFILENAME ofn = {};

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFullPath;
    ofn.nMaxFile = sizeof(szFullPath);
    ofn.lpstrFilter = L"*.dat";
    ofn.lpstrInitialDir = L"D:\\JaeookDX11Tool\\Resources\\Effect\\EffectData\\Particle\\";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetSaveFileName(&ofn))
    {
        ofstream fout(szFullPath, ios::binary);

        if (!fout.is_open())
            return;

        CToolEffect_Particle* pParticle = m_CreatedParticles[m_iSelectIdx].second;
       
        fout.write((_char*)&pParticle->m_iNumParticles, sizeof(_int));
        fout.write((_char*)&pParticle->m_bBillBoard, sizeof(_bool));
        fout.write((_char*)&pParticle->m_iMaskTexureIdx, sizeof(_int));
        fout.write((_char*)&pParticle->m_iPassIdx, sizeof(_int));
        fout.write((_char*)&pParticle->m_bOrientToVelocity, sizeof(_bool));
        fout.write((_char*)&pParticle->m_bTargeting, sizeof(_bool));
        fout.write((_char*)&pParticle->m_fTargetPos, sizeof(_float3));
        fout.write((_char*)&pParticle->m_vColor, sizeof(_float4));
        fout.write((_char*)&pParticle->m_vScaleSpeed, sizeof(_float2));
        fout.write((_char*)&pParticle->m_vMaxScale, sizeof(_float2));
        fout.write((_char*)&pParticle->m_vRotationSpeed, sizeof(_float3));

        for (size_t i = 0; i < pParticle->m_InitParticleDatas.size(); ++i)
        {
            fout.write((_char*)&pParticle->m_InitParticleDatas[i], sizeof(VTXPARTICLE));
            fout.write((_char*)&pParticle->m_InitParticleDescs[i], sizeof(PARTICLE_DESC));
        }
    }
}

void CEffect_Tool::Load_ParticleData()
{
    _tchar szFullPath[200000] = {};
    OPENFILENAME ofn = {};

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFullPath;
    ofn.nMaxFile = sizeof(szFullPath);
    ofn.lpstrFilter = L"*.dat";
    ofn.lpstrInitialDir = L"D:\\JaeookDX11Tool\\Resources\\Effect\\EffectData\\Particle\\";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (GetOpenFileName(&ofn))
    {
        ifstream fin(szFullPath, ios::binary);

        if (!fin.is_open())
            return;

        CToolEffect_Particle* pParticle = static_cast<CToolEffect_Particle*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Effect", L"Prototype_ToolEffect_Particle"));

        fin.read((_char*)&pParticle->m_iNumParticles, sizeof(_int));
        fin.read((_char*)&pParticle->m_bBillBoard, sizeof(_bool));
        fin.read((_char*)&pParticle->m_iMaskTexureIdx, sizeof(_int));
        fin.read((_char*)&pParticle->m_iPassIdx, sizeof(_int));
        fin.read((_char*)&pParticle->m_bOrientToVelocity, sizeof(_bool));
        fin.read((_char*)&pParticle->m_bTargeting, sizeof(_bool));
        fin.read((_char*)&pParticle->m_fTargetPos, sizeof(_float3));
        fin.read((_char*)&pParticle->m_vColor, sizeof(_float4));
        fin.read((_char*)&pParticle->m_vScaleSpeed, sizeof(_float2));
        fin.read((_char*)&pParticle->m_vMaxScale, sizeof(_float2));
        fin.read((_char*)&pParticle->m_vRotationSpeed, sizeof(_float3));

        pParticle->Resize_Particles(pParticle->m_iNumParticles);

        for (size_t i = 0; i < pParticle->m_InitParticleDatas.size(); ++i)
        {
            fin.read((_char*)&pParticle->m_InitParticleDatas[i], sizeof(VTXPARTICLE));
            fin.read((_char*)&pParticle->m_InitParticleDescs[i], sizeof(PARTICLE_DESC));
        }


        fs::path FilePath(szFullPath);
        m_CreatedParticles.emplace_back(make_pair(FilePath.stem().generic_string(), pParticle));
    }

}


void CEffect_Tool::Key_Input_EffectMeshTool()
{
}

void CEffect_Tool::Models_Listbox()
{
    const _char** szModels = new const _char * [m_LoadedMeshes.size()];

    for (size_t i = 0; i < m_LoadedMeshes.size(); ++i)
        szModels[i] = m_LoadedMeshes[i].first.c_str();

    _int iModelIdx = 0;
    if (ImGui::ListBox("##LoadModels", &iModelIdx, szModels, (_int)m_LoadedMeshes.size(), 10))
    {
        if (m_pEffect_Mesh)
            m_pEffect_Mesh->m_pModel = m_LoadedMeshes[iModelIdx].second;
    }

    Safe_Delete_Array(szModels);

    if (ImGui::Button("Import Models"))
        Open_MeshesByFolder();
}

void CEffect_Tool::EffectMesh_Desc_Window()
{
    Models_Listbox();

    CToolEffect_Mesh* pEffectMesh = m_pEffect_Mesh;
    if (nullptr == m_pEffect_Mesh)
        return;

    ImGui::InputInt("MaskTexIdx", &pEffectMesh->m_iMaskTextureIdx);
    ImGui::InputInt("NoiseTexIdx", &pEffectMesh->m_iNoiseTextureIdx);
    ImGui::InputInt("DissloveTexIdx", &pEffectMesh->m_iDissolveTextureIdx);
    ImGui::InputInt("PassIdx", &pEffectMesh->m_iPassIdx);

    ImGui::DragFloat4("Color", (_float*)&pEffectMesh->m_vColor, 0.1f, 0.f, 1.f);
    ImGui::InputFloat("LifeTime", &pEffectMesh->m_fLifeTime);

    ImGui::DragFloat2("UVMaskSpeed", (_float*)&pEffectMesh->m_vUVMaskSpeed, 0.1f, -999.f, 999.f);
    ImGui::DragFloat2("UVNoiseSpeed", (_float*)&pEffectMesh->m_vUVNoiseSpeed, 0.1f, -999.f, 999.f);

    ImGui::DragFloat3("RotationSpeed", (_float*)&pEffectMesh->m_vRotationSpeed, 0.1f, -999.f, 999.f);
    ImGui::DragFloat3("ScaleSpeed", (_float*)&pEffectMesh->m_vScaleSpeed, 0.1f, -999.f, 999.f);

}

void CEffect_Tool::EffectMesh_Created_Window()
{
    ImGui::Begin("EffectMesh List");

    ImGui::Text("Effect Name");
    ImGui::SameLine();
    ImGui::PushItemWidth(100.f);
    ImGui::InputText("##EffectName", m_szParticleName, MAX_PATH);
    ImGui::PopItemWidth();

    if (ImGui::Button("Create"))
    {
        m_pEffect_Mesh = static_cast<CToolEffect_Mesh*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Effect", L"Prototype_ToolEffect_Mesh"));
        //m_CreatedParticles.emplace_back(make_pair(string(m_szParticleName), pParticle));
    }

    //const _char** szParticleNames = new const _char * [m_CreatedParticles.size()];
    //
    //for (size_t i = 0; i < m_CreatedParticles.size(); ++i)
    //    szParticleNames[i] = m_CreatedParticles[i].first.c_str();
    //
    //ImGui::ListBox("##Particles", &m_iSelectIdx, szParticleNames, (_int)m_CreatedParticles.size(), 10);
    //
    //Safe_Delete_Array(szParticleNames);

    ImGui::End();
}

void CEffect_Tool::EffectMesh_Gizmo()
{


}

void CEffect_Tool::Open_MeshesByFolder()
{
    wstring wstrFolderPath = L"D:\\JaeookDX11Tool\\Resources\\Effect\\EffectMesh\\";
 
    fs::path folderPath(wstrFolderPath);

    for (const fs::directory_entry& entry : fs::directory_iterator(folderPath))
    {
        if (entry.is_directory())
            continue;

        fs::path fileName = entry.path().filename();
        fs::path fileTitle = fileName.stem();

        wstring wstrPrototypeTag = L"Prototype_Model_";
        wstrPrototypeTag += fileTitle.c_str();

        CModel* pModel = CModel::Create(m_pDevice, m_pContext, entry.path().parent_path().generic_string() + "/", entry.path().filename().generic_string());
    
        m_LoadedMeshes.emplace_back(make_pair(fileTitle.generic_string(), pModel));
    }
}

CEffect_Tool* CEffect_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CEffect_Tool* pInstance = new CEffect_Tool(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CEffect_Tool"));
        Safe_Release(pInstance);
    }

    return pInstance;

}

void CEffect_Tool::Free()
{
    __super::Free();


}
