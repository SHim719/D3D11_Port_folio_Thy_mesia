#include "Anim_Tool.h"

#include "ToolAnimObj.h"
#include "Animation.h"
#include "Bone.h"
#include "ToolColliderObj.h"

#include "ToolEffect_Mesh.h"
#include "ToolEffect_Particle.h"
//#include "Effect_Trail.h"

CAnim_Tool::CAnim_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolState(pDevice, pContext)
{
}

HRESULT CAnim_Tool::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_strModelLists.reserve(10);
    m_strModelLists.emplace_back("Prototype_Model_Player");
    m_strModelLists.emplace_back("Prototype_Model_Odur");
    m_strModelLists.emplace_back("Prototype_Model_Villager_F");
    m_strModelLists.emplace_back("Prototype_Model_Villager_M");
    m_strModelLists.emplace_back("Prototype_Model_Joker");
    m_strModelLists.emplace_back("Prototype_Model_HalberdKnight");
    m_strModelLists.emplace_back("Prototype_Model_TwinBladeKnight");
	return S_OK;
}

void CAnim_Tool::Start_Tool()
{
    m_pGameInstance->Add_Clone(LEVEL_TOOL, L"TestGround", L"Prototype_TestGround");

    CCamera::CAMERADESC camDesc{};
    camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    camDesc.fNear = 0.1f;
    camDesc.fFar = 300.f;
    camDesc.fFovy = 70.f;
    camDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
    camDesc.vEye = { 0.f, 2.f, -2.f, 1.f };

    m_pCamera = static_cast<CFree_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_Free_Camera", &camDesc));

    m_pGameInstance->Change_MainCamera(m_pCamera);

    Open_EffectMeshesByFolder();
}

void CAnim_Tool::Tick(_float fTimeDelta)
{
    ComboBox_ToolSelect();
    Main_Window();
    Camera_Window();
    KeyFrame_Window();
}

HRESULT CAnim_Tool::Load_KeyFrameNames(_int iSelModelIdx)
{
    string strFullPath; 

    switch (iSelModelIdx)
    {
    case 0:
        strFullPath = "../../Resources/KeyFrame/KeyFrames_Player.txt";
        break;
    case 1:
        strFullPath = "../../Resources/KeyFrame/KeyFrames_Odur.txt";
        break;
    case 2:
        strFullPath = "../../Resources/KeyFrame/KeyFrames_Villager_F.txt";
        break;
    case 3:
        strFullPath = "../../Resources/KeyFrame/KeyFrames_Villager_M.txt";
        break;
    case 4:
        strFullPath = "../../Resources/KeyFrame/KeyFrames_Joker.txt";
        break;
    case 5:
        strFullPath = "../../Resources/KeyFrame/KeyFrames_HalberdKnight.txt";
        break;
    case 6:
        strFullPath = "../../Resources/KeyFrame/KeyFrames_TwinBladeKnight.txt";
        break;
    }

    ifstream fin;
    fin.open(strFullPath);

    string strKeyFrame = "";
    while (getline(fin, strKeyFrame))
    {
        m_strEventNames.emplace_back(strKeyFrame);
    }
    fin.close();

    return S_OK;
}

HRESULT CAnim_Tool::Load_KeyFrames()
{
    if (nullptr == m_pModel)
        return E_FAIL;

    _tchar szFullPath[200000] = {};
    OPENFILENAME ofn = {};

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFullPath;
    ofn.nMaxFile = sizeof(szFullPath);
    ofn.lpstrFilter = L"*.dat";
    ofn.lpstrInitialDir = L"../../Resources/Model/";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (GetOpenFileName(&ofn))
    {
        if (szFullPath[ofn.nFileOffset - 1] == '\0') //복수 선택
        {
            _tchar* p = ofn.lpstrFile;
            vector<wstring> files;

            wstring wstrDirectory(p);
            p += wstrDirectory.length() + 1;

            while (*p)
            {
                wstring wstrFilePath = p;
                p += wstrFilePath.length() + 1;

                wstring wstrPath(wstrDirectory + L"\\" + wstrFilePath);

                if (FAILED(Load_KeyFrameData(wstrPath.c_str())))
                    return E_FAIL;
            }
            
        }
        else // 단일 선택
        {
            if (FAILED(Load_KeyFrameData(szFullPath)))
                return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CAnim_Tool::Load_KeyFrameData(const _tchar* pPath)
{
    ifstream fin(pPath, ios::binary);

    if (!fin.is_open())
        return E_FAIL;

    _int iAnimNameLength = 0;
    _char szAnimName[MAX_PATH] = "";
    _int iKeyFrame = 0;
    _int iEventLength = 0;
    _char szEvent[MAX_PATH] = "";

    fin.read((_char*)&iAnimNameLength, sizeof(_int));
    fin.read(szAnimName, iAnimNameLength);

    while (true)
    {   
        memset(szEvent, 0, MAX_PATH);
        fin.read((_char*)&iKeyFrame, sizeof(_int));
        fin.read((_char*)&iEventLength, sizeof(_int));
        fin.read(szEvent, iEventLength);
        if (fin.eof())
            break;

        size_t iAnimIndex = 0;
        for (; iAnimIndex < m_strAnimations.size(); ++iAnimIndex) // 애니메이션 이름에 해당하는 인덱스 찾아가서 기록.
        {
            if (m_strAnimations[iAnimIndex] == szAnimName)
                break;
        }

        if (m_strAnimations.size() == iAnimIndex)
            return E_FAIL;
        

        m_KeyFrameEvents[iAnimIndex].emplace_back(iKeyFrame, string(szEvent));
    }

    return S_OK;
}

HRESULT CAnim_Tool::Save_KeyFrameData()
{
    _tchar szFullPath[200000] = {};
    OPENFILENAME ofn = {};

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFullPath;
    ofn.nMaxFile = sizeof(szFullPath);
    ofn.lpstrFilter = L"*.dat";
    ofn.lpstrInitialDir = L"../../Resources/Model/";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetSaveFileName(&ofn))
    {
        ofstream fout(szFullPath, ios::binary);

        if (!fout.is_open())
            return E_FAIL;

        _int iAnimNameLength = (_int)m_strAnimations[m_iSelAnimIdx].length();
        _int iEventLength = 0;

        fout.write((_char*)&iAnimNameLength, sizeof(_int));
        fout.write(m_strAnimations[m_iSelAnimIdx].c_str(), iAnimNameLength);

        size_t iIndex = 0;
        for (auto& Pair : m_KeyFrameEvents[m_iSelAnimIdx])
        {
            iEventLength = (_int)Pair.second.length();
            fout.write((_char*)&Pair.first, sizeof(_int));      // 키프레임 저장
            fout.write((_char*)&iEventLength, sizeof(_int));    // 키프레임 이벤트 문자열 길이 저장
            fout.write(Pair.second.c_str(), iEventLength);      // 키프레임 문자열 자체를 저장.
            iIndex++;
        }
    }
    return S_OK;
}


void CAnim_Tool::Main_Window()
{
	ImGui::Begin("Main Window", (bool*)0, ImGuiWindowFlags_MenuBar);
    if (0 == m_iToolState)
    {
        Menu_Bar();
        Tab_Bar();
    }

	ImGui::End();
}

void CAnim_Tool::Camera_Window()
{
    __super::Camera_Window();
    ImGui::End();
}

void CAnim_Tool::KeyFrame_Window()
{
    if (nullptr == m_pModel)
        return;

    auto& Animations = m_pModel->Get_Animations();
    _int iNowKeyFrame = (_int)Animations[m_iSelAnimIdx]->Get_NowKeyFrame();
    _int iNumKeyFrames = (_int)Animations[m_iSelAnimIdx]->Get_NumKeyFrames();

    ImGui::Begin("KeyFrame", (bool*)0);

    ImGui::Text("Now Animaton: %s", m_strAnimations[m_iSelAnimIdx].c_str());
    ImGui::Text("Now KeyFrame: %d", iNowKeyFrame);

    if (ImGui::SliderInt("##KeyFrameController", &iNowKeyFrame, 0, iNumKeyFrames))
        Animations[m_iSelAnimIdx]->Set_CurrentKeyFrames((_uint)iNowKeyFrame);

    if (ImGui::Button("U"))

        Animations[m_iSelAnimIdx]->Set_CurrentKeyFrames((_uint)(++iNowKeyFrame));
    ImGui::SameLine();

    if (ImGui::Button("D"))
        Animations[m_iSelAnimIdx]->Set_CurrentKeyFrames((_uint)(iNowKeyFrame = iNowKeyFrame == 0 ? 0 : --iNowKeyFrame));
    
        

    ImGui::SeparatorText("Event");
    KeyFrameEvent_ComboBox();
    KeyFrameEvent_ListBox();
    KeyFrameEvent_Button();

    ImGui::End();
}

void CAnim_Tool::KeyFrameEvent_ComboBox()
{   
    const char** szEventNames = new const char* [m_strEventNames.size()];

    for (_int i = 0; i < (_int)m_strEventNames.size(); ++i)
        szEventNames[i] = m_strEventNames[i].c_str();

   ImGui::PushItemWidth(180);
   ImGui::Combo("##EventLists", &m_iSelEventName, szEventNames, (_int)m_strEventNames.size());
   ImGui::PopItemWidth();

    ImGui::InputText("Event Name", m_szEventName, MAX_PATH);
    if (ImGui::Button("Add Event Name"))
        m_strEventNames.emplace_back(m_szEventName);

    Safe_Delete_Array(szEventNames);
}

void CAnim_Tool::KeyFrameEvent_ListBox()
{
    ImGui::SeparatorText("Now KeyFrames");

    string* strKeyFrames = new string[m_KeyFrameEvents[m_iSelAnimIdx].size()];
    for (size_t i = 0; i < m_KeyFrameEvents[m_iSelAnimIdx].size(); ++i)
        strKeyFrames[i] = "<" + to_string(m_KeyFrameEvents[m_iSelAnimIdx][i].first) + " , " + m_KeyFrameEvents[m_iSelAnimIdx][i].second + ">";

    const _char** szKeyFrames = new const _char* [m_KeyFrameEvents[m_iSelAnimIdx].size()];
    for (size_t i = 0; i < m_KeyFrameEvents[m_iSelAnimIdx].size(); ++i)
        szKeyFrames[i] = strKeyFrames[i].c_str();

    ImGui::PushItemWidth(300);
    ImGui::ListBox("##KeyFrame_ListBox", &m_iSelKeyFrameIdx, szKeyFrames, (_int)m_KeyFrameEvents[m_iSelAnimIdx].size(), 10);
    ImGui::PopItemWidth();

    Safe_Delete_Array(strKeyFrames);
    Safe_Delete_Array(szKeyFrames);
}

void CAnim_Tool::KeyFrameEvent_Button()
{
    if (ImGui::Button("Add"))
    {
        auto Anims = m_pModel->Get_Animations();
        _int iNowKeyFrame = (_int)Anims[m_iSelAnimIdx]->Get_NowKeyFrame();
       m_KeyFrameEvents[m_iSelAnimIdx].emplace_back(iNowKeyFrame, m_strEventNames[m_iSelEventName]);
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete"))
    {
        m_KeyFrameEvents[m_iSelAnimIdx].erase(m_KeyFrameEvents[m_iSelAnimIdx].begin() + m_iSelKeyFrameIdx);
        m_iSelKeyFrameIdx = 0;
    }
}

void CAnim_Tool::Bone_ListBox()
{
    if (nullptr == m_pAnimObj)
        return;

    ImGui::SeparatorText("Bones");
    ImGui::PushItemWidth(150);
    if (ImGui::ListBox("##Bone_ListBox", &m_iSelBoneIdx, m_szBoneNames, m_iNumBones, 20))
    {
        if (m_Colliders.empty())
            return;
        m_Colliders[m_iSelColliderIdx]->Set_AttachBone(m_szBoneNames[m_iSelBoneIdx]);
    }

}

void CAnim_Tool::Collider_ListBox()
{
    ImGui::SeparatorText("Collider");

    const _char** szColliders = new const _char * [m_Colliders.size()];
    
    for (size_t i = 0; i < m_strColliders.size(); ++i)
        szColliders[i] = m_strColliders[i].c_str();

    ImGui::PushItemWidth(150);
    ImGui::ListBox("##Collider_ListBox", &m_iSelColliderIdx, szColliders, (_int)m_Colliders.size(), 5);

    Safe_Delete_Array(szColliders);
}

void CAnim_Tool::ColliderType_CheckBox()
{
    if (ImGui::Checkbox("AABB", &m_bColliderTypes[0]))
    {
        m_bColliderTypes[1] = false;
        m_bColliderTypes[2] = false;
        m_iTypeIdx = 0;
    }

    if (ImGui::Checkbox("OBB", &m_bColliderTypes[1]))
    {
        m_bColliderTypes[0] = false;
        m_bColliderTypes[2] = false;
        m_iTypeIdx = 1;
    }

    if (ImGui::Checkbox("Sphere", &m_bColliderTypes[2]))
    {
        m_bColliderTypes[0] = false;
        m_bColliderTypes[1] = false;
        m_iTypeIdx = 2;
    }
}

void CAnim_Tool::Collider_Buttons()
{
    ImGui::InputFloat3("Center", &m_ColliderDesc.vCenter.x);
    ImGui::InputFloat3("Size", &m_ColliderDesc.vSize.x);
    ImGui::InputFloat3("Rotation", &m_ColliderDesc.vRotation.x);

    if (ImGui::Button("Add Collider"))
    {
        CCollider::COLLIDERDESC desc;
        desc.vCenter = m_ColliderDesc.vCenter;
        desc.vSize = m_ColliderDesc.vSize;
        desc.vRotation.x = To_Radian(m_ColliderDesc.vRotation.x);
        desc.vRotation.y = To_Radian(m_ColliderDesc.vRotation.y);
        desc.vRotation.z = To_Radian(m_ColliderDesc.vRotation.z);
        desc.pOwner = m_pAnimObj;
        desc.eType = (CCollider::ColliderType)m_iTypeIdx;
        desc.bActive = true;

        CToolColliderObj* pObj = static_cast<CToolColliderObj*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"ToolObject", L"Prototype_ToolColliderObj", &desc));
        m_Colliders.push_back(pObj);

        m_strColliders.push_back("Collider");
    }
    
    if (ImGui::Button("Edit Collider"))
    {
        if (m_Colliders.empty())
            return;

        CCollider::COLLIDERDESC desc;
        desc.vCenter = m_ColliderDesc.vCenter;
        desc.vSize = m_ColliderDesc.vSize;
        desc.vRotation.x = To_Radian(m_ColliderDesc.vRotation.x);
        desc.vRotation.y = To_Radian(m_ColliderDesc.vRotation.y);
        desc.vRotation.z = To_Radian(m_ColliderDesc.vRotation.z);

        m_Colliders[m_iSelColliderIdx]->Remake_Collider(&desc);
    }
}


void CAnim_Tool::Menu_Bar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("KeyFrame"))
        {
            if (ImGui::MenuItem("Load KeyFrame"))
            {
                if (FAILED(Load_KeyFrames()))
                    assert(false);
            }
            
            if (ImGui::MenuItem("Save KeyFrame"))
            {
                if (FAILED(Save_KeyFrameData()))
                    assert(false);
            }

            ImGui::EndMenu();
            
        }

        ImGui::EndMenuBar();
    }
}

void CAnim_Tool::Tab_Bar()
{
    if (ImGui::BeginTabBar("Models"))
    {
        if (ImGui::BeginTabItem("Anim"))
        {
            Model_ListBox();
            Model_Button();

            Anim_ListBox();
            Anim_Buttons();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Collider"))
        {
            Bone_ListBox();
            Collider_ListBox();
            ColliderType_CheckBox();
            Collider_Buttons();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Effect"))
        {
            ImGui::BeginChild(1);

            Effect_Tool();

            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();

    }

}

void CAnim_Tool::Model_ListBox()
{
    ImGui::SeparatorText("Model");
    const char** szModels = new const char* [m_strModelLists.size()];

    for (_int i = 0; i < (_int)m_strModelLists.size(); ++i)
        szModels[i] = m_strModelLists[i].c_str();

    ImGui::PushItemWidth(210);
    ImGui::ListBox("##Model_ListBox", &m_iSelModelIdx, szModels, (_int)m_strModelLists.size(), 5);
    ImGui::PopItemWidth();

    Safe_Delete_Array(szModels);
}

void CAnim_Tool::Model_Button()
{
    if (ImGui::Button("Load Model"))
    {
        if (m_pAnimObj)
        {
            m_pAnimObj->Set_Destroy(true);
            m_pGameInstance->Clear_Level(LEVEL_TOOL);
        }

        m_pAnimObj = static_cast<CToolAnimObj*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"ToolObject", L"Prototype_ToolAnimObj", &m_iSelModelIdx));
        m_pModel = m_pAnimObj->Get_Model();

        Load_KeyFrameNames(m_iSelModelIdx);

        m_strAnimations.clear();
        m_strAnimations.shrink_to_fit();
        auto Anims = m_pModel->Get_Animations();
        m_strAnimations.reserve(Anims.size());
        for (auto pAnim : Anims)
            m_strAnimations.emplace_back(pAnim->Get_AnimName());

        m_KeyFrameEvents.clear();
        m_KeyFrameEvents.shrink_to_fit();
        m_KeyFrameEvents.resize(Anims.size());

        auto& Bones = m_pModel->Get_Bones();

        Safe_Delete_Array(m_szBoneNames);

        m_szBoneNames = new const _char * [Bones.size()];
        for (size_t i = 0; i < Bones.size(); ++i)
            m_szBoneNames[i] = Bones[i]->Get_Name();

        m_iNumBones = (_int)Bones.size();
    }
}

void CAnim_Tool::Anim_ListBox()
{
    ImGui::SeparatorText("Animation");

    m_fAnimListBoxStartCursorY = ImGui::GetCursorPosY();

    const char** szAnimations = new const char* [m_strAnimations.size()];

    for (_int i = 0; i < (_int)m_strAnimations.size(); ++i)
        szAnimations[i] = m_strAnimations[i].c_str();
    ImGui::PushItemWidth(270);
    if (ImGui::ListBox("##Anim_ListBox", &m_iSelAnimIdx, szAnimations, (_int)m_strAnimations.size(), 20))
    {
        m_pModel->Change_Animation(m_iSelAnimIdx);
        m_iSelKeyFrameIdx = 0;
    }

    ImGui::PopItemWidth();
    Safe_Delete_Array(szAnimations);
}

void CAnim_Tool::Anim_Buttons()
{
    _float fCursorY = m_fAnimListBoxStartCursorY;
    ImGui::SetCursorPos(ImVec2(280.f, fCursorY));
    if (ImGui::Button("Play") && m_pModel)
        m_pModel->Set_AnimPlay();

    fCursorY = ImGui::GetCursorPosY() + 10.f;
        
    ImGui::SetCursorPos(ImVec2(280.f, fCursorY));
    if (ImGui::Button("Stop") && m_pModel)
        m_pModel->Set_AnimPause();
    

}


CAnim_Tool* CAnim_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CAnim_Tool* pInstance = new CAnim_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnim_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CAnim_Tool::Free()
{
	__super::Free();

    Safe_Delete_Array(m_szBoneNames);

    for (auto& Pair : m_LoadedMeshes)
        Safe_Release(Pair.second);
}

void CAnim_Tool::ComboBox_ToolSelect()
{
    ImGui::Begin("Tool");
    ImGui::PushItemWidth(100);

    const char* Tools[] = { "Anim", "Effect" };
    ImGui::Combo("##ToolComboBox", (int*)(&m_iToolState), Tools, IM_ARRAYSIZE(Tools));
    ImGui::PopItemWidth();
    ImGui::End();
}



void CAnim_Tool::Effect_Tool()
{
    Key_Input_EffectTool();
    Main_Window_EffectTool();
    Effect_Created_Window();
    Bone_ListBox_EffectTool();
    Transform_Gizmo();
   
    Sync_Animation();
    Check_Destroyed();
}

void CAnim_Tool::Main_Window_EffectTool()
{
    if (ImGui::BeginTabBar("Effect_Tool"))
    {
        if (ImGui::BeginTabItem("Particle"))
        {
            m_eEffectMode = CGameEffect::PARTICLE;
            Particle_Tool();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Effect Mesh"))
        {
            m_eEffectMode = CGameEffect::MESH;
            EffectMesh_Tool();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    
}

void CAnim_Tool::Key_Input_EffectTool()
{
    if (KEY_DOWN(eKeyCode::Space))
    {
        for (size_t i = 0; i < m_CreatedEffects.size(); ++i)
            m_CreatedEffects[i].second->Restart_Effect(&Bake_EffectSpawnDesc());

    }
    if (KEY_DOWN(eKeyCode::Delete) && m_iSelectIdx >= 0)
    {
        m_CreatedEffects[m_iSelectIdx].second->Set_Destroy(true);
    }

}

void CAnim_Tool::Bone_ListBox_EffectTool()
{
    if (nullptr == m_pAnimObj)
        return;

    ImGui::Begin("Bone_ListBoxForTool");

    _int iSelBoneIdx = 0;
    ImGui::SeparatorText("Bones");
    ImGui::PushItemWidth(150);
    if (ImGui::ListBox("##Bone_ListBox", &iSelBoneIdx, m_szBoneNames, m_iNumBones, 20) && m_iSelectIdx >= 0)
    {
        CGameEffect* pEffect = m_CreatedEffects[m_iSelectIdx].second;
        strcpy_s(pEffect->m_szBoneName, m_szBoneNames[iSelBoneIdx]);
        pEffect->Restart_Effect(&Bake_EffectSpawnDesc());
    }

    ImGui::End();
}

void CAnim_Tool::Key_Input_ParticleTool()
{
    if (m_iSelectIdx < 0 || m_CreatedEffects[m_iSelectIdx].second->m_eEffectType != CGameEffect::PARTICLE)
        return;


    if (KEY_DOWN(eKeyCode::RShift))
    {
        CToolEffect_Particle* pEffect = static_cast<CToolEffect_Particle*>(m_CreatedEffects[m_iSelectIdx].second);
        pEffect->Resize_Particles(pEffect->m_tParticleInfo.iNumParticles);
    }


}

void CAnim_Tool::Particle_Tool()
{
    Key_Input_ParticleTool();
    Particle_Desc_Window();
}

void CAnim_Tool::EffectMesh_Tool()
{
    Key_Input_EffectMeshTool();
    EffectMesh_Desc_Window();
    Models_Listbox();
}

void CAnim_Tool::Check_Destroyed()
{
    for (auto it = m_CreatedEffects.begin(); it != m_CreatedEffects.end();)
    {
        if (it->second->Is_Destroyed())
        {
            it = m_CreatedEffects.erase(it);
            m_iSelectIdx = -1;
        }
        else
        {
            ++it;
        }
    }

}

void CAnim_Tool::Particle_Desc_Window()
{
    if (m_iSelectIdx < 0 || m_CreatedEffects[m_iSelectIdx].second->m_eEffectType != CGameEffect::PARTICLE)
        return;
    
    CToolEffect_Particle* pParticle = static_cast<CToolEffect_Particle*>(m_CreatedEffects[m_iSelectIdx].second);
    
    ImGui::Checkbox("NoRender", &pParticle->m_bNoRender);
    ImGui::Checkbox("Loop?", &pParticle->m_bLoop);
    ImGui::DragFloat("PlayTime", &pParticle->m_fPlayTime, 0.01f, 0.f, 99.f);

    const _char* szTFButtons[CGameEffect::TF_END] = { "Spawn Only", "Follow Parent" };
    ImGui::Text("SpawnType");
    for (_int i = 0; i < CGameEffect::TF_END; ++i)
    {
        if (ImGui::RadioButton(szTFButtons[i], (_int)pParticle->m_eTransformType == i))
            pParticle->m_eTransformType = (CGameEffect::TRANSFORMTYPE)i;
    }

    ImGui::Text("Attach Bone: "); ImGui::SameLine();
    ImGui::Text(pParticle->m_szBoneName);
    if (ImGui::Button("Clear"))
        memset(pParticle->m_szBoneName, 0, MAX_PATH);
    
    ImGui::DragFloat3("World Offset", (_float*)&pParticle->m_vWorldOffset, 0.01f, -99.f, 99.f);
    ImGui::Checkbox("Only Position", &pParticle->m_bFollowOnlyPosition);

    ImGui::Text("Particle_Option");
    const _char* szParticleMode[CToolEffect_Particle::PARTICLE_TYPE::PARTICLE_END] = { "BillBoard", "Aligned_Velocity", "SpawnAtBone"};

    for (size_t i = 0; i < CToolEffect_Particle::PARTICLE_TYPE::PARTICLE_END; ++i)
    {
        _bool bActivated = pParticle->m_tParticleInfo.iParticleMode & (1 << i);
        if (ImGui::Checkbox(szParticleMode[i], &bActivated))
            pParticle->m_tParticleInfo.iParticleMode ^= (1 << i);
    }

    ImGui::Checkbox("Particle Loop?", &pParticle->m_tParticleInfo.bParticleLoop);
    
    if (ImGui::InputInt("NumInstances", &pParticle->m_tParticleInfo.iNumParticles))
        pParticle->Resize_Particles((_uint)pParticle->m_tParticleInfo.iNumParticles);
    
    ImGui::InputInt("BaseTexIdx", &pParticle->m_iBaseTextureIdx);
    if (ImGui::InputInt("MaskTexIdx", &pParticle->m_iMaskTextureIdx))
        pParticle->Update_TextureFlag();
    if (ImGui::InputInt("NoiseTexIdx", &pParticle->m_iNoiseTextureIdx))
        pParticle->Update_TextureFlag();
    if (ImGui::InputInt("EmissiveTexIdx", &pParticle->m_iEmissiveTextureIdx))
        pParticle->Update_TextureFlag();
    ImGui::InputInt("PassIdx", &pParticle->m_iPassIdx);
    
    ImGui::InputFloat4("Clip Color", (_float*)&pParticle->m_vClipColor);

    ImGui::DragFloat3("StartPosMin", (_float*)&pParticle->m_tParticleInfo.vStartPosMin, 0.01f, -99.f, 99.f);
    ImGui::DragFloat3("StartPosMax", (_float*)&pParticle->m_tParticleInfo.vStartPosMax, 0.01f, -99.f, 99.f);
    
    ImGui::DragFloat("SpawnTimeMin", (_float*)&pParticle->m_tParticleInfo.fSpawnTimeMin, 0.01f, -99.f, 99.f);
    ImGui::DragFloat("SpawnTimeMax", (_float*)&pParticle->m_tParticleInfo.fSpawnTimeMax, 0.01f, -99.f, 99.f);
    
    ImGui::DragFloat2("StartSizeMin", (_float*)&pParticle->m_tParticleInfo.vStartSizeMin, 0.01f, -99.f, 99.f);
    ImGui::DragFloat2("StartSizeMax", (_float*)&pParticle->m_tParticleInfo.vStartSizeMax, 0.01f, -99.f, 99.f);
    
    ImGui::Checkbox("Lerp Size?", &pParticle->m_tParticleInfo.bSizeLerp);
    if (pParticle->m_tParticleInfo.bSizeLerp)
        ImGui::DragFloat2("End Size", (_float*)&pParticle->m_tParticleInfo.vSizeEnd, 0.01f, 0.f, 99.f);

    ImGui::DragFloat3("StartRotationMin", (_float*)&pParticle->m_tParticleInfo.vStartRotationMin, 0.01f, -XM_PI, XM_PI);
    ImGui::DragFloat3("StartRotationMax", (_float*)&pParticle->m_tParticleInfo.vStartRotationMax, 0.01f, -XM_PI, XM_PI);

    ImGui::DragFloat3("vRotationSpeedMin", (_float*)&pParticle->m_tParticleInfo.vRotationSpeedMin, 0.01f, -XM_PI, XM_PI);
    ImGui::DragFloat3("vRotationSpeedMax", (_float*)&pParticle->m_tParticleInfo.vRotationSpeedMax, 0.01f, -XM_PI, XM_PI);
    
    ImGui::DragFloat3("SpeedDirMin", (_float*)&pParticle->m_tParticleInfo.vSpeedDirMin, 0.01f, -99.f, 99.f);
    ImGui::DragFloat3("SpeedDirMax", (_float*)&pParticle->m_tParticleInfo.vSpeedDirMax, 0.01f, -99.f, 99.f);
    
    ImGui::DragFloat("StartSpeedMin", (_float*)&pParticle->m_tParticleInfo.fStartSpeedMin, 0.01f, -99.f, 99.f);
    ImGui::DragFloat("StartSpeedMax", (_float*)&pParticle->m_tParticleInfo.fStartSpeedMax, 0.01f, -99.f, 99.f);
    
    ImGui::Checkbox("Lerp Speed?", &pParticle->m_tParticleInfo.bSpeedLerp);
    if (pParticle->m_tParticleInfo.bSpeedLerp)
        ImGui::DragFloat("End Speed", (_float*)&pParticle->m_tParticleInfo.fSpeedEnd, 0.01f, -99.f, 99.f);

    ImGui::DragFloat3("ForceDirMin", (_float*)&pParticle->m_tParticleInfo.vForceDirMin, 0.01f, 0.f, 99.f);
    ImGui::DragFloat3("ForceDirMax", (_float*)&pParticle->m_tParticleInfo.vForceDirMax, 0.01f, 0.f, 99.f);
    ImGui::DragFloat("ForceScaleMin", (_float*)&pParticle->m_tParticleInfo.fForceScaleMin, 0.01f, 0.f, 99.f);
    ImGui::DragFloat("ForceScaleMax", (_float*)&pParticle->m_tParticleInfo.fForceScaleMax, 0.01f, 0.f, 99.f);

    ImGui::DragFloat4("StartColorMin", (_float*)&pParticle->m_tParticleInfo.vStartColorMin, 0.1f,  0.f, 999.f);
    ImGui::DragFloat4("StartColorMax", (_float*)&pParticle->m_tParticleInfo.vStartColorMax, 0.1f,  0.f, 999.f);
    
    ImGui::Checkbox("Lerp Color?", &pParticle->m_tParticleInfo.bColorLerp);
    if (pParticle->m_tParticleInfo.bColorLerp)
        ImGui::DragFloat4("End Color", (_float*)&pParticle->m_tParticleInfo.vColorEnd, 0.01f, -99.f, 99.f);
    
    ImGui::DragFloat("LifeTimeMin", (_float*)&pParticle->m_tParticleInfo.fLifeTimeMin, 0.01f, 0.f, 99.f);
    ImGui::DragFloat("LifeTimeMax", (_float*)&pParticle->m_tParticleInfo.fLifeTimeMax, 0.01f, 0.f, 99.f);
    
    ImGui::Checkbox("Sprite?", &pParticle->m_tParticleInfo.bSprite);
    
    if (pParticle->m_tParticleInfo.bSprite)
    {
        ImGui::DragInt("Col", &pParticle->m_tParticleInfo.iCol, 1, 0, 99);
        ImGui::SameLine();
        ImGui::DragInt("Row", &pParticle->m_tParticleInfo.iRow, 1, 0, 99);
    }

    ImGui::Checkbox("Glow?", &pParticle->m_bGlow);
    if (pParticle->m_bGlow)
    {
        ImGui::DragFloat4("Glow Color", (_float*)&pParticle->m_vGlowColor, 0.1f, 0.f, 1.f);
        ImGui::DragFloat("Glow Intensity", (_float*)&pParticle->m_fGlowIntensity, 0.1f, 0.f, 99.f);
    }

    ImGui::Checkbox("Bloom?", &pParticle->m_bBloom);
}

HRESULT CAnim_Tool::Save_EffectData()
{
    _tchar szFullPath[200000] = {};
    OPENFILENAME ofn = {};

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFullPath;
    ofn.nMaxFile = sizeof(szFullPath);
    ofn.lpstrFilter = L"*.dat";
    ofn.lpstrInitialDir = L"D:\\JaeookDX11Tool\\Resources\\Effect\\EffectData\\";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetSaveFileName(&ofn))
    {
        ofstream fout(szFullPath, ios::binary);

        if (!fout.is_open())
            return E_FAIL;

        vector<size_t> vecGroupings;
        for (size_t i = 0; i < m_CreatedEffects.size(); ++i)
        {
            if (m_bGrouping[i])
                vecGroupings.push_back(i);
        }
        if (vecGroupings.empty())
        {
            MSG_BOX(L"그룹핑해주세요");
            return S_OK;
        }
            
        _uint iNumEffects = (_uint)vecGroupings.size();
        fout.write((_char*)&iNumEffects, sizeof(_uint));

        for (size_t i = 0; i < vecGroupings.size(); ++i)
        {
            size_t iIdx = vecGroupings[i];
            m_CreatedEffects[iIdx].second->Save_EffectData(fout);
        }
    }
    return S_OK;
}

HRESULT CAnim_Tool::Load_EffectData()
{
    _tchar szFullPath[200000] = {};
    OPENFILENAME ofn = {};

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFullPath;
    ofn.nMaxFile = sizeof(szFullPath);
    ofn.lpstrFilter = L"*.dat";

    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (GetOpenFileName(&ofn))
    {
        ifstream fin(szFullPath, ios::binary);

        if (!fin.is_open())
            return E_FAIL;

        _uint iNumEffects = 0;
        fin.read((_char*)&iNumEffects, sizeof(_uint));
        
        for (_uint i = 0; i < iNumEffects; ++i)
        {
            CGameEffect::EFFECTTYPE eEffectType;
            fin.read((_char*)&eEffectType, sizeof(CGameEffect::EFFECTTYPE));
            
            CGameEffect* pEffect = nullptr;
            switch (eEffectType)
            {
            case CGameEffect::PARTICLE:
                pEffect = static_cast<CGameEffect*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Effect", L"Prototype_ToolEffect_Particle"));
                break;
            case CGameEffect::MESH:
                pEffect = static_cast<CGameEffect*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Effect", L"Prototype_ToolEffect_Mesh"));
                break;

            }

            if (FAILED(pEffect->Load_EffectData(fin)))
            {
                Safe_Release(pEffect);
                return E_FAIL;
            }

            if (CGameEffect::MESH == pEffect->m_eEffectType)
            {
                CToolEffect_Mesh* pEffectMesh = static_cast<CToolEffect_Mesh*>(pEffect);
                wstring wstrModelTag = pEffectMesh->m_tMeshEffectInfo.szModelTag;
                pEffectMesh->m_pModel = Get_Model(Convert_WStrToStr(wstrModelTag));
            }

            //pEffect->Restart_Effect(&Bake_EffectSpawnDesc());

            m_CreatedEffects.emplace_back(make_pair(pEffect->m_szEffectName, pEffect));
        }
    }
    return S_OK;
}

void CAnim_Tool::Effect_Created_Window()
{
    ImGui::Begin("Effect List");

    ImGui::Text("Effect Name");
    ImGui::SameLine();
    ImGui::PushItemWidth(100.f);
    ImGui::InputText("##EffectName", m_szEffectName, MAX_PATH);
    ImGui::PopItemWidth();
    string strNowGroupingNumbers = "";

    const _char** szParticleNames = new const _char * [m_CreatedEffects.size()];

    for (size_t i = 0; i < m_CreatedEffects.size(); ++i)
        szParticleNames[i] = m_CreatedEffects[i].first.c_str();

    ImGui::ListBox("##Particles", &m_iSelectIdx, szParticleNames, (_int)m_CreatedEffects.size(), 10);

    Safe_Delete_Array(szParticleNames);

    if (ImGui::Button("Create"))
    {
        CGameEffect* pEffect = nullptr;

        switch (m_eEffectMode)
        {
        case CGameEffect::PARTICLE:
            pEffect = static_cast<CGameEffect*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Effect", L"Prototype_ToolEffect_Particle"));
            break;

        case CGameEffect::MESH:
            pEffect = static_cast<CGameEffect*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Effect", L"Prototype_ToolEffect_Mesh"));
            break;
        }
        m_CreatedEffects.emplace_back(make_pair(string(m_szEffectName), pEffect));
        strcpy_s(pEffect->m_szEffectName, m_szEffectName);
    }
    ImGui::SameLine();
    if (ImGui::Button("Edit"))
    {
        if (m_iSelectIdx < 0)
            goto end;

        m_CreatedEffects[m_iSelectIdx].first = m_szEffectName;
        strcpy_s(m_CreatedEffects[m_iSelectIdx].second->m_szEffectName, m_szEffectName);
    }

    if (ImGui::Button("Save"))
    {
        if (m_iSelectIdx < 0)
            goto end;

        if (FAILED(Save_EffectData()))
            MSG_BOX(L"세이브 실패..");
    }
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {
        if (FAILED(Load_EffectData()))
        {
            MSG_BOX(L"로드 실패..");
            m_iSelectIdx = -1;
        }

    }

    ImGui::Checkbox("Sync Animation", &m_bSyncAnimation);
    if (m_bSyncAnimation)
    {
        if (m_iSelectIdx < 0)
            goto end;
           
        ImGui::PushItemWidth(30);
        ImGui::DragInt("Sync Frame", &m_iSyncFrames[m_iSelectIdx], 1.f, 0, 200);
        ImGui::PopItemWidth();
    }

    for (size_t i = 0; i < m_CreatedEffects.size(); ++i)
        if (m_bGrouping[i])
            strNowGroupingNumbers += to_string(i) + ", ";

    ImGui::Text("Now Group: %s", strNowGroupingNumbers.c_str());
    ImGui::Checkbox("Grouping?", &m_bGrouping[m_iSelectIdx]);
    if (ImGui::Button("All Check Group"))
        memset(m_bGrouping, 1, sizeof(m_bGrouping));
    if (ImGui::Button("All Uncheck Group"))
        memset(m_bGrouping, 0, sizeof(m_bGrouping));

 end:
    ImGui::End();
}

void CAnim_Tool::Sync_Animation()
{
    if (false == m_bSyncAnimation || m_iSelectIdx < 0 || nullptr == m_pAnimObj || !m_pAnimObj->Get_Model()->Is_Playing())
        return;

    _uint iCurAnimIdx = m_pAnimObj->Get_Model()->Get_CurrentAnimIndex();

    auto anims = m_pAnimObj->Get_Model()->Get_Animations();

    for (size_t i = 0; i < m_CreatedEffects.size(); ++i)
    {
        if (!m_bSyncd[i] && m_iSyncFrames[i] == anims[iCurAnimIdx]->Get_NowKeyFrame())
        {
            m_bSyncd[i] = true;
            m_CreatedEffects[i].second->Restart_Effect(&Bake_EffectSpawnDesc());
        }
        else if (m_iSyncFrames[i] != anims[iCurAnimIdx]->Get_NowKeyFrame())
            m_bSyncd[i] = false;  
    } 
}

CGameEffect::EFFECTSPAWNDESC CAnim_Tool::Bake_EffectSpawnDesc()
{
    CGameEffect::EFFECTSPAWNDESC Desc;

    if (m_pAnimObj)
    {
        Desc.pParentTransform = m_pAnimObj->Get_Transform();
        Desc.pParentModel = m_pAnimObj->Get_Model();
    }
    return Desc;
}

void CAnim_Tool::Key_Input_EffectMeshTool()
{

}

void CAnim_Tool::EffectMesh_Desc_Window()
{
    if (m_iSelectIdx < 0 || m_CreatedEffects[m_iSelectIdx].second->m_eEffectType != CGameEffect::MESH)
        return;

    CToolEffect_Mesh* pEffect = static_cast<CToolEffect_Mesh*>(m_CreatedEffects[m_iSelectIdx].second);

    ImGui::Checkbox("NoRender", &pEffect->m_bNoRender);

    ImGui::Checkbox("Loop?", &pEffect->m_bLoop);
    ImGui::DragFloat("PlayTime", &pEffect->m_fPlayTime, 0.01f, 0.f, 99.f);
    ImGui::DragFloat("SpawnTime", &pEffect->m_tMeshEffectInfo.fSpawnTime, 0.1f, 0.f, 100.f);

    const _char* szButtons[CGameEffect::TF_END] = { "Spawn Only", "Follow Parent" };
    ImGui::Text("SpawnType");
    for (_int i = 0; i < CGameEffect::TF_END; ++i)
    {
        if (ImGui::RadioButton(szButtons[i], (_int)pEffect->m_eTransformType == i))
            pEffect->m_eTransformType = (CGameEffect::TRANSFORMTYPE)i;
    }

    ImGui::Text("Attach Bone: "); ImGui::SameLine();
    ImGui::Text(pEffect->m_szBoneName);
    if (ImGui::Button("Clear"))
        memset(pEffect->m_szBoneName, 0, MAX_PATH);

    ImGui::DragFloat3("World Offset", (_float*)&pEffect->m_vWorldOffset, 0.01f, -99.f, 99.f);
    ImGui::Checkbox("Only Position", &pEffect->m_bFollowOnlyPosition);

    ImGui::InputInt("BaseTexIdx", &pEffect->m_iBaseTextureIdx);
    if (ImGui::InputInt("MaskTexIdx", &pEffect->m_iMaskTextureIdx))
        pEffect->Update_TextureFlag();
    if (ImGui::InputInt("NoiseTexIdx", &pEffect->m_iNoiseTextureIdx))
        pEffect->Update_TextureFlag();
    if (ImGui::InputInt("EmissiveTexIdx", &pEffect->m_iEmissiveTextureIdx))
        pEffect->Update_TextureFlag();
    ImGui::InputInt("PassIdx", &pEffect->m_iPassIdx);

    ImGui::InputFloat4("Clip Color", (_float*)&pEffect->m_vClipColor);

    ImGui::DragFloat3("Start Position", (_float*)&pEffect->m_tMeshEffectInfo.vStartPosition, 0.01f, -99.f, 99.f);

    ImGui::Checkbox("Local?", &pEffect->m_tMeshEffectInfo.bLocal);
    if (ImGui::DragFloat("TurnSpeed", &pEffect->m_tMeshEffectInfo.fTurnSpeed, 0.1f, -180.f, 180.f))
        pEffect->Get_Transform()->Set_RotationSpeed(pEffect->m_tMeshEffectInfo.fTurnSpeed);

    ImGui::DragFloat3("RotationAxis", (_float*)&pEffect->m_tMeshEffectInfo.vRotationAxis, 0.1f, -99.f, 99.f);

    ImGui::DragFloat3("Start Rotation", (_float*)&pEffect->m_tMeshEffectInfo.vStartRotation, 0.1f, -180.f, 180.f);
    ImGui::Checkbox("Lerp Rotation?", &pEffect->m_tMeshEffectInfo.bRotationLerp);
    if (pEffect->m_tMeshEffectInfo.bRotationLerp)
        ImGui::DragFloat3("End Rotation", (_float*)&pEffect->m_tMeshEffectInfo.vRotationEnd, 0.1f, -180.f, 180.f);

    ImGui::DragFloat3("Start Scale", (_float*)&pEffect->m_tMeshEffectInfo.vStartScale, 0.01f, -99.f, 99.f);
    ImGui::Checkbox("Lerp Scale?", &pEffect->m_tMeshEffectInfo.bScaleLerp);
    if (pEffect->m_tMeshEffectInfo.bScaleLerp)
        ImGui::DragFloat3("End Scale", (_float*)&pEffect->m_tMeshEffectInfo.vScaleEnd, 0.01f, 0.f, 99.f);

    ImGui::DragFloat4("Start Color", (_float*)&pEffect->m_tMeshEffectInfo.vStartColor, 0.1f, 0.f, 1.f);

    ImGui::Checkbox("Lerp Color?", &pEffect->m_tMeshEffectInfo.bColorLerp);
    if (pEffect->m_tMeshEffectInfo.bColorLerp)
        ImGui::DragFloat4("End Color", (_float*)&pEffect->m_tMeshEffectInfo.vColorEnd, 0.1f, 0.f, 1.f);

    ImGui::DragFloat2("StartMaskUVOffset", (_float*)&pEffect->m_tMeshEffectInfo.vStartMaskUVOffset, 0.01f, 0.f, 99.f);
    ImGui::DragFloat2("MaskUVSpeed", (_float*)&pEffect->m_tMeshEffectInfo.vMaskUVSpeed, 0.01f, 0.f, 99.f);

    ImGui::DragFloat2("StartNoiseUVOffset", (_float*)&pEffect->m_tMeshEffectInfo.vStartNoiseUVOffset, 0.01f, 0.f, 99.f);
    ImGui::DragFloat2("NoiseUVSpeed", (_float*)&pEffect->m_tMeshEffectInfo.vNoiseUVSpeed, 0.01f, 0.f, 99.f);


    ImGui::Checkbox("Glow?", &pEffect->m_bGlow);
    if (pEffect->m_bGlow)
    {
        ImGui::DragFloat4("Glow Color", (_float*)&pEffect->m_vGlowColor, 0.1f, 0.f, 1.f);
        ImGui::DragFloat("Glow Intensity", (_float*)&pEffect->m_fGlowIntensity, 0.1f, 0.f, 99.f);
    }

    ImGui::Checkbox("Bloom?", &pEffect->m_bBloom);

}

void CAnim_Tool::Models_Listbox()
{
    const _char** szModels = new const _char * [m_LoadedMeshes.size()];

    for (size_t i = 0; i < m_LoadedMeshes.size(); ++i)
        szModels[i] = m_LoadedMeshes[i].first.c_str();

    _int iModelIdx = 0;
    if (ImGui::ListBox("##LoadModels", &iModelIdx, szModels, (_int)m_LoadedMeshes.size(), 10))
    {
        if (m_iSelectIdx < 0 || m_CreatedEffects[m_iSelectIdx].second->m_eEffectType != CGameEffect::MESH)
            return;

        CToolEffect_Mesh* pEffect = static_cast<CToolEffect_Mesh*>(m_CreatedEffects[m_iSelectIdx].second);

        if (pEffect)
        {
            pEffect->m_pModel = m_LoadedMeshes[iModelIdx].second;
            wcscpy_s(pEffect->m_tMeshEffectInfo.szModelTag, Convert_StrToWStr(szModels[iModelIdx]).c_str());
        }

    }

    Safe_Delete_Array(szModels);
}



void CAnim_Tool::Open_EffectMeshesByFolder()
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
        if (nullptr == pModel)
            assert(false);

        m_pGameInstance->Add_Prototype(LEVEL_STATIC, wstrPrototypeTag, pModel);

        m_LoadedMeshes.emplace_back(make_pair(fileTitle.generic_string(), pModel));
    }
}

CModel* CAnim_Tool::Get_Model(const string& strTag)
{
    for (auto& Pair : m_LoadedMeshes)
    {
        if (Pair.first == strTag)
            return Pair.second;
    }

    return nullptr;
}

void CAnim_Tool::Reset_Transform(_fmatrix WorldMatrix)
{
    _vector vScale, vPosition, vQuaternion;
    XMMatrixDecompose(&vScale, &vQuaternion, &vPosition, WorldMatrix);

    _vector vEulerAngle = JoMath::ToEulerAngle(vQuaternion);

    XMStoreFloat3(&m_vScale, vScale);
    XMStoreFloat3(&m_vPosition, vPosition);
    XMStoreFloat3(&m_vRotation, vEulerAngle);
}

void CAnim_Tool::Transform_Window()
{
    if (nullptr == m_pGizmoObject)
        return;

    if (ImGui::Begin("Transform"))
    {
        ImGui::SeparatorText("Transform");
        if (ImGui::InputFloat3("Position", &m_vPosition.x))
        {
            m_pGizmoObject->Get_Transform()->Set_Position(XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));

        }

        if (ImGui::InputFloat3("Rotation", &m_vRotation.x))
        {
            _vector vQuat = XMQuaternionRotationRollPitchYaw(To_Radian(m_vRotation.x), To_Radian(m_vRotation.y), To_Radian(m_vRotation.z));

            m_pGizmoObject->Get_Transform()->Rotation_Quaternion(vQuat);
        }

        if (ImGui::InputFloat3("Scale", &m_vScale.x))
        {
            m_pGizmoObject->Get_Transform()->Set_Scale(m_vScale);
        }

        ImGui::End();
    }

}

void CAnim_Tool::Transform_Gizmo()
{
    m_pGizmoObject = nullptr;

    if (m_iSelectIdx == -1)
        return;

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGuizmo::BeginFrame();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    _float4x4 ViewMatrix = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
    _float4x4 ProjMatrix = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

#pragma region InputKey
    if (true == m_pGameInstance->GetKey(eKeyCode::RButton))
    {
        if (m_pGameInstance->GetKeyDown(eKeyCode::W))
        {
            m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::TRANSLATE;
            m_tGizmoDesc.snap[0] = 0.5f;
        }
        if (m_pGameInstance->GetKeyDown(eKeyCode::E))
        {
            m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::ROTATE;
            m_tGizmoDesc.snap[1] = To_Radian(15.f);
        }
        if (m_pGameInstance->GetKeyDown(eKeyCode::R))
        {
            m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::SCALE;
            m_tGizmoDesc.snap[2] = 0.5f;
        }

        if (m_pGameInstance->GetKeyDown(eKeyCode::T))
            m_tGizmoDesc.CurrentGizmoMode = m_tGizmoDesc.CurrentGizmoMode == ImGuizmo::LOCAL ? ImGuizmo::WORLD : ImGuizmo::LOCAL;

        if (m_pGameInstance->GetKeyDown(eKeyCode::Y))
            m_tGizmoDesc.bUseSnap = !m_tGizmoDesc.bUseSnap;
    }
#pragma endregion 

    m_pGizmoObject = m_CreatedEffects[m_iSelectIdx].second;

    if (nullptr == m_pGizmoObject)
        return;

    _float4x4 WorldMatrix = m_pGizmoObject->Get_Transform()->Get_WorldFloat4x4();
    if (ImGuizmo::Manipulate(*ViewMatrix.m, *ProjMatrix.m
        , m_tGizmoDesc.CurrentGizmoOperation
        , m_tGizmoDesc.CurrentGizmoMode
        , *WorldMatrix.m, NULL, m_tGizmoDesc.bUseSnap ? &m_tGizmoDesc.snap[0] : NULL, m_tGizmoDesc.boundSizing ? m_tGizmoDesc.bounds : NULL
        , m_tGizmoDesc.boundSizingSnap ? m_tGizmoDesc.boundsSnap : NULL))
    {
        Reset_Transform(XMLoadFloat4x4(&WorldMatrix));
        m_pGizmoObject->Get_Transform()->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));
    }

}



#pragma endregion
