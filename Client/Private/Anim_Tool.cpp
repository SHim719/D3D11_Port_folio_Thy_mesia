#include "Anim_Tool.h"

#include "ToolAnimObj.h"
#include "Animation.h"
#include "Bone.h"
#include "ToolColliderObj.h"

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
	return S_OK;
}

void CAnim_Tool::Start_Tool()
{
    m_pGameInstance->Add_Clone(LEVEL_TOOL, L"TestGround", L"Prototype_TestGround");
}

void CAnim_Tool::Tick(_float fTimeDelta)
{
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
    Menu_Bar();
    Tab_Bar();

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

    ImGui::PushItemWidth(220);
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
    ImGui::PushItemWidth(210);
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
    ImGui::SetCursorPos(ImVec2(230.5, fCursorY));
    if (ImGui::Button("Play") && m_pModel)
        m_pModel->Set_AnimPlay();

    fCursorY = ImGui::GetCursorPosY() + 10.f;
        
    ImGui::SetCursorPos(ImVec2(230.5, fCursorY));
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
}
