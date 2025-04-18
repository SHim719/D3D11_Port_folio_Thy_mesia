#include "Map_Tool.h"
#include "ToolNaviCell.h"
#include "ToolNaviCellPoint.h"

#include "ToolMapObjInstance.h"

#include "ToolBoundingSphere.h"


CMap_Tool::CMap_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CToolState(pDevice, pContext)
{
}

HRESULT CMap_Tool::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Picking()))
        return E_FAIL;

    m_iSelPointIndices.reserve(3);

    m_strPlacable_Objects[TRIGGEROBJ].emplace_back("EventTrigger");

    return S_OK;
}

void CMap_Tool::Start_Tool()
{
    CCamera::CAMERADESC camDesc{};
    camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    camDesc.fNear = 0.1f;
    camDesc.fFar = 300.f;
    camDesc.fFovy = 70.f;
    camDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
    camDesc.vEye = { 0.f, 2.f, -2.f, 1.f };

    m_pCamera = static_cast<CFree_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_Free_Camera", &camDesc));

    m_pGameInstance->Change_MainCamera(m_pCamera);
}

void CMap_Tool::Tick(_float fTimeDelta)
{
    Key_Input();
    Main_Window();
    Camera_Window();
}

void CMap_Tool::LateTick(_float fTimeDelta)
{
    Destroy_MapObjects();
    Destroy_Cells();

    Ready_InstanceObj();

    for (auto& Pair : m_MapObjectInstances)
        Pair.second->Ready_RenderInstance();
}

HRESULT CMap_Tool::Ready_Picking()
{
    D3D11_TEXTURE2D_DESC TextureDesc = {};

    TextureDesc.Width = g_iWinSizeX;
    TextureDesc.Height = g_iWinSizeY;
    TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pColorTexture)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateRenderTargetView(m_pColorTexture, nullptr, &m_pColorRTV)))
        return E_FAIL; 

    TextureDesc.Width = 1;
    TextureDesc.Height = 1;
    TextureDesc.Usage = D3D11_USAGE_STAGING;
    TextureDesc.BindFlags = 0;
    TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pPickingTexture)))
        return E_FAIL;

    return S_OK;
}

void CMap_Tool::Key_Input()
{
    if (KEY_DOWN(eKeyCode::P))
        m_bCanPick = !m_bCanPick;
}

void CMap_Tool::Main_Window()
{
    ImGui::Begin("Main_Window", (bool*)0, ImGuiWindowFlags_MenuBar);
    Menu_Bar();
    TabBar();
    ImGui::End();
}

void CMap_Tool::Enemy_Window()
{
    if (ImGui::InputInt("NaviIdx", &m_iNowNaviIdx))
    {
        if (m_SelectObjIndices.empty())
            return;

        m_MapObjects[*m_SelectObjIndices.begin()]->Set_NaviIdx(m_iNowNaviIdx);
    }
}

void CMap_Tool::Trigger_Window()
{
    if (ImGui::InputInt("TriggerIdx", &m_iTriggerIdx))
    {
        if (m_SelectObjIndices.empty()) 
            return;
        m_MapObjects[*m_SelectObjIndices.begin()]->Set_TriggerIdx(m_iTriggerIdx);
    }

    if (ImGui::InputFloat3("Size", &m_vColliderSize.x))
    {
        if (m_SelectObjIndices.empty())
            return;
        m_MapObjects[*m_SelectObjIndices.begin()]->Set_ColliderSize(m_vColliderSize);
    } 
}

void CMap_Tool::Camera_Window()
{
    __super::Camera_Window();
    ImGui::End();

}

HRESULT CMap_Tool::Open_MeshesByFolder()
{
    wstring wstrFolderPath = Get_FolderPath(L"D:\\JaeookDX11Tool\\Resources\\MapObjects\\");
    if (L"" == wstrFolderPath)
        return E_FAIL;

    if (m_strPlacable_Objects[MAPOBJECT].empty())
    {
        m_MapObjects.emplace_back(static_cast<CToolMapObj*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"MapObject", L"Prototype_ToolBoundingSphere")));
        m_strCreatedObjects.emplace_back("BoundingSphere");
    }

   fs::path folderPath(wstrFolderPath);
   
   for (const fs::directory_entry& entry : fs::directory_iterator(folderPath))
   {
       if (entry.is_directory())
           continue;

       fs::path fileName = entry.path().filename();
       fs::path fileTitle = fileName.stem();
      
       wstring wstrPrototypeTag = L"Prototype_Model_";
       wstrPrototypeTag += fileTitle.c_str();

       if (nullptr != m_pGameInstance->Find_Prototype(LEVEL_STATIC, wstrPrototypeTag))
           continue;

       CModel* pModel = CModel::Create(m_pDevice, m_pContext, entry.path().parent_path().generic_string() + "/", entry.path().filename().generic_string());

       if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, wstrPrototypeTag, pModel)))
           return E_FAIL;

       wstrPrototypeTag += L"_Instance";
       if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, wstrPrototypeTag, CModel_Instance::Create(m_pDevice, m_pContext, pModel))))
           return E_FAIL;

       m_strPlacable_Objects[m_eNowObjMode].emplace_back(fileTitle.generic_string());

       m_CullingRadiuses.emplace(fileTitle.generic_string(), 1.f);
   }


    return S_OK;
}


wstring CMap_Tool::Get_FolderPath(const _tchar* szInitialPath)
{
    ITEMIDLIST* pidlRoot = nullptr;
    if (!SUCCEEDED(SHParseDisplayName(szInitialPath, NULL, &pidlRoot, 0, NULL))) {
        pidlRoot = nullptr;
    }

    BROWSEINFO browseInfo;
    ZeroMemory(&browseInfo, sizeof(BROWSEINFO));
    browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    browseInfo.hwndOwner = g_hWnd;
    browseInfo.pidlRoot = pidlRoot;

    LPITEMIDLIST pItemIdList = SHBrowseForFolder(&browseInfo);
    if (0 != pItemIdList)
    {
        _tchar szPath[MAX_PATH];
        if (SHGetPathFromIDList(pItemIdList, szPath))
        {
            CoTaskMemFree(pItemIdList);
            return wstring(szPath);
        }
        CoTaskMemFree(pItemIdList);
    }

    return L"";
}


HRESULT CMap_Tool::Create_ObjectInLevel()
{
    if (m_iSelPlacableObj < 0)
        return S_OK;

    CToolMapObj::TOOLMAPOBJDESC Desc;
    Desc.wstrModelTag = L"Prototype_Model_" + Convert_StrToWStr(m_strPlacable_Objects[m_eNowObjMode][m_iSelPlacableObj]);
    Desc.eObjType = m_eNowObjMode;

    CToolMapObj* pObj = static_cast<CToolMapObj*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"MapObject", L"Prototype_ToolMapObj", &Desc));
    if (nullptr == pObj)
        return E_FAIL;

    string strObj = m_strPlacable_Objects[m_eNowObjMode][m_iSelPlacableObj];

    m_MapObjects.emplace_back(pObj);
    m_strCreatedObjects.emplace_back(strObj);
    m_MapLayers.emplace(strObj, pObj);

    if (m_eNowObjMode < TRIGGEROBJ)
    {
        auto it = m_MapObjectInstances.find(strObj);
        if (m_MapObjectInstances.end() == it)
        {
            CToolMapObjInstance* pInstanceObj = static_cast<CToolMapObjInstance*>(m_pGameInstance->Clone_GameObject(L"Prototype_ToolMapObj_Instance", &Desc.wstrModelTag));
            m_MapObjectInstances.emplace(strObj, pInstanceObj);
        }
    }

    return S_OK;
}

void CMap_Tool::Check_DestroyObjects()
{
}

void CMap_Tool::Menu_Bar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Map"))
        {
            if (ImGui::MenuItem("Open Meshes"))
            {
                Open_MeshesByFolder();
            }
            
            if (ImGui::MenuItem("Save Map"))
            {
                if (SUCCEEDED(Save_Map()))
                    MSG_BOX(L"맵 저장 승공");
                
            }

            if (ImGui::MenuItem("Open Map"))
            {
                if (SUCCEEDED(Load_Map()))
                    MSG_BOX(L"맵 로드 성공");
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Navi"))
        {
            if (ImGui::MenuItem("Save_NaviData"))
            {
                if (S_OK == Save_NaviData())
                    MSG_BOX(L"네비게이션 저장 성공");
            }

            if (ImGui::MenuItem("Open_NaviData"))
            {
                if (S_OK == Load_NaviData())
                    MSG_BOX(L"네비게이션 불러오기 성공");
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();

       
    }
}

void CMap_Tool::TabBar()
{
    if (ImGui::BeginTabBar("Map_Tool"))
    {
        if (ImGui::BeginTabItem("Object"))
        {
            Map_Tool();
            ImGui::EndTabItem();
        }
        

        if (ImGui::BeginTabItem("Navi"))
        {
            Navi_Tool();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}



void CMap_Tool::Map_Tool()
{
    switch (m_eNowObjMode)
    {
    case ENEMY:
        Enemy_Window();
        break;
    case TRIGGEROBJ:
        Trigger_Window();
        break;
    }

    Key_Input_MapTool();
    Transform_Gizmo();
    Transform_View();
    Placable_Object();
    Object_ListBox();
    Object_Picking();
}

void CMap_Tool::Key_Input_MapTool()
{
    if (KEY_DOWN(eKeyCode::Delete))
    {
        if (m_SelectObjIndices.empty())
            return;

        for (auto iSelObjIdx : m_SelectObjIndices)
            m_MapObjects[iSelObjIdx]->Set_Destroy(true);
        
        m_SelectObjIndices.clear();
    }

    if (KEY_PUSHING(eKeyCode::LCtrl))
    {
        if (KEY_DOWN(eKeyCode::D))
        {
            if (m_SelectObjIndices.empty())
                return;

           for (_int iSelObjIdx : m_SelectObjIndices)
           {
               CToolMapObj* pClonedObj = static_cast<CToolMapObj*>(m_MapObjects[iSelObjIdx]->Clone(nullptr));

               m_pGameInstance->Insert_GameObject(LEVEL_TOOL, L"MapObject", pClonedObj);

               m_MapObjects.emplace_back(pClonedObj);
               m_strCreatedObjects.emplace_back(m_strCreatedObjects[iSelObjIdx]);
               m_MapLayers.emplace(m_strCreatedObjects[iSelObjIdx], pClonedObj);
           }
        }
    }

}

void CMap_Tool::Destroy_MapObjects()
{
    vector<string>::iterator strIter = m_strCreatedObjects.begin();

    for (auto it = m_MapLayers.begin(); it != m_MapLayers.end();)
    {
        if (it->second->Is_Destroyed())
            it = m_MapLayers.erase(it);
        else
            ++it; 
    }

    for (auto it = m_MapObjects.begin(); it != m_MapObjects.end();)
    {
        if ((*it)->Is_Destroyed())
        {
            it = m_MapObjects.erase(it);
            strIter = m_strCreatedObjects.erase(strIter);
        }
        else
        {
            ++it;
            ++strIter;
        }
    }

}
void CMap_Tool::Transform_View()
{
    ImGui::SeparatorText("Transform");
    if (ImGui::InputFloat3("Position", &m_vPosition.x))
    {
        if (m_SelectObjIndices.empty())
            return;

        for (_int iSelObjIdx : m_SelectObjIndices)
            m_MapObjects[iSelObjIdx]->Get_Transform()->Set_Position(XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
        
    }

    if (ImGui::InputFloat3("Rotation", &m_vRotation.x))
    {
        if (m_SelectObjIndices.empty())
            return;

        _vector vQuat = XMQuaternionRotationRollPitchYaw(To_Radian(m_vRotation.x), To_Radian(m_vRotation.y), To_Radian(m_vRotation.z));
        for (_int iSelObjIdx : m_SelectObjIndices)
             m_MapObjects[iSelObjIdx]->Get_Transform()->Rotation_Quaternion(vQuat);
    }

    if (ImGui::InputFloat3("Scale", &m_vScale.x))
    {
        if (m_SelectObjIndices.empty() || 0.f == m_vScale.x || 0.f == m_vScale.y || 0.f == m_vScale.z)
            return;

        for (_int iSelObjIdx : m_SelectObjIndices)
            m_MapObjects[iSelObjIdx]->Get_Transform()->Set_Scale(m_vScale);
    }
}

void CMap_Tool::Placable_ComboBox()
{
    const char* combo_value[] = { "MapObject" , "Enemy", "Trigger" };
    if (ImGui::Combo("##Object", (_int*)&m_eNowObjMode, combo_value, IM_ARRAYSIZE(combo_value)))
    {
        m_iSelPlacableObj = -1;
        m_SelectObjIndices.clear();
    }
       
}


void CMap_Tool::Reset_Transform(_fmatrix WorldMatrix)
{
    _vector vScale, vPosition, vQuaternion;
    XMMatrixDecompose(&vScale, &vQuaternion, &vPosition, WorldMatrix);

    _vector vEulerAngle = JoMath::ToEulerAngle(vQuaternion);

    XMStoreFloat3(&m_vScale, vScale);
    XMStoreFloat3(&m_vPosition, vPosition);
    XMStoreFloat3(&m_vRotation, vEulerAngle);
}

void CMap_Tool::Transform_Gizmo()
{
    if (m_SelectObjIndices.empty() || m_SelectObjIndices.size() > 1)
        return;

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGuizmo::BeginFrame();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    _float4x4 ViewMatrix = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
    _float4x4 ProjMatrix = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

#pragma region InputKey
    if (false == m_pGameInstance->GetKey(eKeyCode::RButton))
    {
        if (m_pGameInstance->GetKeyDown(eKeyCode::W))
        {
            m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::TRANSLATE;
            m_tGizmoDesc.snap[0] = 0.5f;
        }
        if (m_pGameInstance->GetKeyDown(eKeyCode::E))
        {
            m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::ROTATE;
            m_tGizmoDesc.snap[0] = To_Radian(15.f);
        }
        if (m_pGameInstance->GetKeyDown(eKeyCode::R))
        {
            m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::SCALE;
            m_tGizmoDesc.snap[0] = 0.5f;
        }

        if (m_pGameInstance->GetKeyDown(eKeyCode::T))
            m_tGizmoDesc.CurrentGizmoMode = m_tGizmoDesc.CurrentGizmoMode == ImGuizmo::LOCAL ? ImGuizmo::WORLD : ImGuizmo::LOCAL;

        if (m_pGameInstance->GetKeyDown(eKeyCode::Y))
            m_tGizmoDesc.bUseSnap = !m_tGizmoDesc.bUseSnap;
    }
#pragma endregion 
    
    _float4x4 WorldMatrix = m_MapObjects[*(m_SelectObjIndices.begin())]->Get_Transform()->Get_WorldFloat4x4();
    if (ImGuizmo::Manipulate(*ViewMatrix.m, *ProjMatrix.m
        , m_tGizmoDesc.CurrentGizmoOperation
        , m_tGizmoDesc.CurrentGizmoMode
        , *WorldMatrix.m, NULL, m_tGizmoDesc.bUseSnap ? &m_tGizmoDesc.snap[0] : NULL, m_tGizmoDesc.boundSizing ? m_tGizmoDesc.bounds : NULL
        , m_tGizmoDesc.boundSizingSnap ? m_tGizmoDesc.boundsSnap : NULL))
    {   
        Reset_Transform(XMLoadFloat4x4(&WorldMatrix));
        m_MapObjects[*m_SelectObjIndices.begin()]->Get_Transform()->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));
    }
    
}

void CMap_Tool::Placable_Object()
{
    ImGui::SeparatorText("Model");
   
    Placable_ComboBox();

    ImVec2 vStartPos = ImGui::GetCursorPos();
    vStartPos.x += 240.f;

    const _char** szPlacables = new const _char * [m_strPlacable_Objects[m_eNowObjMode].size()];

    for (size_t i = 0; i < m_strPlacable_Objects[m_eNowObjMode].size(); ++i)
        szPlacables[i] = m_strPlacable_Objects[m_eNowObjMode][i].c_str();

    ImGui::ListBox("##Placble_ListBox", &m_iSelPlacableObj, szPlacables, (_int)m_strPlacable_Objects[m_eNowObjMode].size(), 10);

    ImVec2 vNowCursorPos = ImGui::GetCursorPos();

    ForObject_Buttons(vStartPos);

    ImGui::SetCursorPos(vNowCursorPos);
  
    Safe_Delete_Array(szPlacables);
}

void CMap_Tool::ForObject_Buttons(ImVec2 vNowCursorPos)
{
    _float yOffset = 30.f;
    ImGui::SetCursorPos(vNowCursorPos);

    if (ImGui::Button("Create"))
        Create_ObjectInLevel();

    vNowCursorPos.y += yOffset;
    ImGui::SetCursorPos(vNowCursorPos);
    
    ImGui::Checkbox("Picking?", &m_bCanPick);

    vNowCursorPos.y += yOffset;
    ImGui::SetCursorPos(vNowCursorPos);

    ImGui::Checkbox("Snap?", &m_tGizmoDesc.bUseSnap);

    vNowCursorPos.y += yOffset;
    ImGui::SetCursorPos(vNowCursorPos);

    _bool bIsLocalMode = m_tGizmoDesc.CurrentGizmoMode == ImGuizmo::LOCAL;
    if (ImGui::Checkbox("Local?", &bIsLocalMode))
        m_tGizmoDesc.CurrentGizmoMode = !bIsLocalMode ? ImGuizmo::WORLD : ImGuizmo::LOCAL;

    if (m_iSelPlacableObj >= 0)
    {
        vNowCursorPos.y += yOffset;
        ImGui::SetCursorPos(vNowCursorPos);

        ImGui::Text("CullingRadius");

        vNowCursorPos.y += yOffset - 10.f;
        ImGui::SetCursorPos(vNowCursorPos);

        string strSelectPlacableObject = m_strPlacable_Objects[m_eNowObjMode][m_iSelPlacableObj];
        ImGui::PushItemWidth(50.f);
        ImGui::InputFloat("##CullingRadius", &m_CullingRadiuses[strSelectPlacableObject]);
        ImGui::PopItemWidth();
    }

}


void CMap_Tool::Object_ListBox()
{
    ImGui::SeparatorText("Created Objects");

    for (size_t i = 0; i < m_strCreatedObjects.size(); ++i)
    {
        _bool bSelected = static_cast<_bool>(m_SelectObjIndices.count(static_cast<const int>(i)));

        string strObject = m_strCreatedObjects[i] + to_string(i);

        if (ImGui::Selectable(strObject.c_str(), bSelected))
        {
            if (false == KEY_PUSHING(eKeyCode::LCtrl))
                m_SelectObjIndices.clear();

            if (bSelected)
            {
                auto it = m_SelectObjIndices.find((_int)i);
                if (m_SelectObjIndices.end() != it)
                    m_SelectObjIndices.erase(it);
            }
            else
            {
                m_SelectObjIndices.insert((_int)i);

                _matrix WorldMatrix = m_MapObjects[i]->Get_Transform()->Get_WorldMatrix();

                Reset_Transform(WorldMatrix);

                m_iNowNaviIdx = m_MapObjects[i]->Get_NaviIdx();
                m_iTriggerIdx = m_MapObjects[i]->Get_TriggerIdx();
                m_vColliderSize = m_MapObjects[i]->Get_ColliderSize();
            }
        }
    }
 
}

void CMap_Tool::Ready_InstanceObj()
{
    for (size_t i = 0; i < m_MapObjects.size(); ++i)
    {
        auto it = m_MapObjectInstances.find(m_strCreatedObjects[i]);
        if (m_MapObjectInstances.end() == it)
            continue;

        it->second->Add_WorldMatrix(m_MapObjects[i]->Get_Transform()->Get_WorldFloat4x4());
    }
}

HRESULT CMap_Tool::Save_Map()
{
    wstring wstrFolderPath = Get_FolderPath(L"D:\\JaeookDX11Tool\\Resources\\Maps\\");
    if (L"" == wstrFolderPath)
        return E_FAIL;

    for (_int i = 0; i < OBJTYPE_END; ++i)
    {
        for (const string& strObj : m_strPlacable_Objects[i])
        {
            auto Pair = m_MapLayers.equal_range(strObj);

            wstring wstrFullPath = wstrFolderPath + L"\\" + Convert_StrToWStr(strObj) + L".dat";
            if (m_MapLayers.end() != Pair.first)
            {
                ofstream fout(wstrFullPath, ios::binary);
                if (!fout.is_open())
                    return E_FAIL;

                _float fCullingRadius = m_CullingRadiuses[strObj];
     
                fout.write((_char*)&fCullingRadius, sizeof(_float));

                for (auto it = Pair.first; it != Pair.second; ++it)
                {
                    LOADOBJDESC LoadDesc;
                    LoadDesc.eObjType = it->second->Get_ObjType();
                    LoadDesc.WorldMatrix = it->second->Get_Transform()->Get_WorldFloat4x4();
                    LoadDesc.iNaviIdx = it->second->Get_NaviIdx();
                    LoadDesc.iTriggerIdx = it->second->Get_TriggerIdx();
                    LoadDesc.vColliderSize = it->second->Get_ColliderSize();

                    fout.write((_char*)&LoadDesc, sizeof(LOADOBJDESC));
                }
            }
        }
    }
    return S_OK;
}

HRESULT CMap_Tool::Load_Map()
{
    wstring wstrFolderPath = Get_FolderPath(L"D:\\JaeookDX11Tool\\Resources\\Maps\\");
    if (L"" == wstrFolderPath)
        return E_FAIL;

    fs::path folderPath(wstrFolderPath);

    for (const fs::directory_entry& entry : fs::directory_iterator(folderPath))
    {
        if (entry.is_directory())
            continue;

        ifstream fin(entry.path().c_str(), ios::binary);
        
        if (!fin.is_open())
            return E_FAIL;

        fs::path fileName = entry.path().filename();
        fs::path fileTitle = fileName.stem();

        LOADOBJDESC LoadDesc;

        _float fCullingRadius = 1.f;
        fin.read((_char*)&fCullingRadius, sizeof(_float));
        
        m_CullingRadiuses[fileTitle.generic_string()] = fCullingRadius;

        while (true)
        {
            fin.read((_char*)&LoadDesc, sizeof(LOADOBJDESC));
           
            wsprintf(LoadDesc.szModelTag, L"Prototype_Model_%s", fileTitle.c_str());

           if (fin.eof())
               break;

            CToolMapObj* pObj = static_cast<CToolMapObj*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"MapObject", L"Prototype_ToolMapObj", nullptr));
            if (nullptr == pObj)
                return E_FAIL;

            pObj->Initialize_Load(&LoadDesc);

            m_MapObjects.emplace_back(pObj);
            m_MapLayers.emplace(fileTitle.generic_string(), pObj);
            m_strCreatedObjects.emplace_back(fileTitle.generic_string());

            if (LoadDesc.eObjType < TRIGGEROBJ)
            {
                CToolMapObjInstance* pInstanceObj = nullptr;

                auto it = m_MapObjectInstances.find(fileTitle.generic_string());
                if (m_MapObjectInstances.end() == it)
                {
                    pInstanceObj = static_cast<CToolMapObjInstance*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"MapObject", L"Prototype_ToolMapObj_Instance", &wstring(LoadDesc.szModelTag)));
                    m_MapObjectInstances.emplace(fileTitle.generic_string(), pInstanceObj);
                } 
            }
                
        }
        fin.close();
    }

    return S_OK;
}


void CMap_Tool::Navi_Tool()
{
    Navi_KeyInput();
    Navi_Picking();
    Navi_Transform();
    Navi_Tool_Options();
    Navi_ListBox();
}

void CMap_Tool::Navi_KeyInput()
{
    if (KEY_DOWN(eKeyCode::Delete))
    {
        if (-1 < m_iSelCellIdx && m_iSelCellIdx < (_int)m_CreatedCells.size())
        {
            m_CreatedCells[m_iSelCellIdx]->Set_Destroy(true);
            m_iSelCellIdx = -1;
            m_iSelPointIndices.clear();
        }
    }
}

void CMap_Tool::Navi_Transform()
{
    if (-1 == m_iSelCellIdx || m_iSelCellIdx >= (_int)m_CreatedCells.size())
        return;

    _float3* pCellPoints = m_CreatedCells[m_iSelCellIdx]->Get_CellPoints();

    if (ImGui::InputFloat3("Navi Pos0", &pCellPoints[0].x))
        m_CreatedCells[m_iSelCellIdx]->Set_CellPoints(pCellPoints);

    if (ImGui::InputFloat3("Navi Pos1", &pCellPoints[1].x))
        m_CreatedCells[m_iSelCellIdx]->Set_CellPoints(pCellPoints);

    if (ImGui::InputFloat3("Navi Pos2", &pCellPoints[2].x))
        m_CreatedCells[m_iSelCellIdx]->Set_CellPoints(pCellPoints);

    delete pCellPoints;
}

void CMap_Tool::Navi_Tool_Options()
{
    ImGui::Checkbox("Picking?", &m_bCanPick);
    if (ImGui::Checkbox("Mesh Picking?", &m_bPickingMesh) && m_bPickingMesh)
        m_iSelPointIndices.clear();

    if (ImGui::Button("Sort Clockwise"))
    {


    }
}

void CMap_Tool::Navi_ListBox()
{
    ImGui::SeparatorText("Created Cell");

    vector<string> strNaviMeshes;
    strNaviMeshes.resize(m_CreatedCells.size());
    for (size_t i = 0; i < m_CreatedCells.size(); ++i)
        strNaviMeshes[i] = to_string(i);

    const _char** szNaviMeshes = new const _char * [m_CreatedCells.size()];

    for (size_t i = 0; i < m_CreatedCells.size(); ++i)
        szNaviMeshes[i] = strNaviMeshes[i].c_str();

   ImGui::ListBox("##NaviMeshListBox", &m_iSelCellIdx, szNaviMeshes, (_int)m_CreatedCells.size(), 10);

   Safe_Delete_Array(szNaviMeshes);
}

void CMap_Tool::Navi_Picking()
{
    if (!m_bCanPick || 0 == m_MapObjects.size())
        return;

    if (KEY_DOWN(eKeyCode::LButton))
    {
        if (m_bPickingMesh)
            Picking_NaviMesh();
        else
            Picking_Point();
    }
}

void CMap_Tool::Picking_NaviMesh()
{
    _float4 f4RayStartPos, f4RayDir;

    Get_MouseRayInfo(f4RayStartPos, f4RayDir);

    _vector vRayStartPos = XMLoadFloat4(&f4RayStartPos);
    _vector vRayDir = XMLoadFloat4(&f4RayDir);

    for (size_t i = 0; i < m_CreatedCells.size(); ++i)
    {
        if (m_CreatedCells[i]->Intersect_Ray(vRayStartPos, vRayDir))
        {
            if (-1 != m_iSelCellIdx && m_iSelCellIdx < (_int)m_CreatedCells.size())
                m_CreatedCells[m_iSelCellIdx]->Uncheck_Picking();

            m_iSelCellIdx = (_int)i;
            return;
        }
    }

    m_iSelCellIdx = -1;
}

void CMap_Tool::Picking_Point()
{
    _float4 f4RayStartPos, f4RayDir;

    Get_MouseRayInfo(f4RayStartPos, f4RayDir);

    _vector vRayStartPos = XMLoadFloat4(&f4RayStartPos);
    _vector vRayDir = XMVector3Normalize(XMLoadFloat4(&f4RayDir));

    _int iPickingIdx = ColorPicking_Object();
    if (-1 == iPickingIdx || iPickingIdx >= (_int)m_MapObjects.size())
        return;

    _float4 vPickedPos = {};
    _float fDist = -1.f;

    if (m_MapObjects[iPickingIdx]->Ray_Cast(vRayStartPos, vRayDir, vPickedPos, fDist))
    {
        _bool bPickCreatedPoint = false;
        _int iPointIdx = 0;
        vPickedPos.y += 0.01f;

        for (; iPointIdx < (_int)m_CreatedCellPoints.size(); ++iPointIdx)
        {
            if (bPickCreatedPoint = m_CreatedCellPoints[iPointIdx]->Check_Picked(XMLoadFloat4(&vPickedPos)))
                break;
        }

        if (false == bPickCreatedPoint)
        {
            CToolNaviCellPoint* pPoint = static_cast<CToolNaviCellPoint*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Cell", L"Prototype_ToolNaviCellPoint", &vPickedPos));
            m_CreatedCellPoints.push_back(pPoint);
            iPointIdx = (_int)m_CreatedCellPoints.size() - 1;
        }

        m_iSelPointIndices.push_back(iPointIdx);
        if (3 == m_iSelPointIndices.size())
        {
            vector<CToolNaviCellPoint*> CellPoints;
            CellPoints.resize(3);

            for (size_t i = 0; i < 3; ++i)
            {
                CellPoints[i] = m_CreatedCellPoints[m_iSelPointIndices[i]];
                CellPoints[i]->Uncheck_Picked();
            }

            CToolNaviCell* pCell = static_cast<CToolNaviCell*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Cell", L"Prototype_ToolNaviCell", &CellPoints));
            m_CreatedCells.push_back(pCell);
            m_iSelPointIndices.clear();
        }
    }
}

HRESULT CMap_Tool::Save_NaviData()
{
    _tchar szFullPath[200000] = {};
    OPENFILENAME ofn = {};

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFullPath;
    ofn.nMaxFile = sizeof(szFullPath);
    ofn.lpstrFilter = L"*.dat";
    ofn.lpstrInitialDir = L"D:\\JaeookDX11Tool\\Resources\\NaviData\\";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetSaveFileName(&ofn))
    {
        ofstream fout(szFullPath, ios::binary);

        if (!fout.is_open())
            return E_FAIL;

        _int iReserveSize = (_int)m_CreatedCells.size();

        fout.write((_char*)&iReserveSize, sizeof(_int));

        for (auto pCell : m_CreatedCells)
        {
            _float3* CellPoints = pCell->Get_CellPoints();

            fout.write((_char*)CellPoints, sizeof(_float3) * 3);

            Safe_Delete(CellPoints);
        }
    }

    return S_OK;
}

HRESULT CMap_Tool::Load_NaviData()
{
    _tchar szFullPath[200000] = {};
    OPENFILENAME ofn = {};

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFullPath;
    ofn.nMaxFile = sizeof(szFullPath);
    ofn.lpstrFilter = L"*.dat";
    ofn.lpstrInitialDir = L"D:\\JaeookDX11Tool\\Resources\\NaviData\\";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn))
    {
        ifstream fin(szFullPath, ios::binary);

        if (!fin.is_open())
            return E_FAIL;

        for (auto pPoint : m_CreatedCellPoints)
            pPoint->Set_Destroy(true);
        for (auto pCell : m_CreatedCells)
            pCell->Set_Destroy(true);

        m_CreatedCellPoints.clear();
        m_CreatedCells.clear();
        m_iSelPointIndices.clear();
        m_iSelCellIdx = 0;

        _int iReserveSize = 0;

        fin.read((_char*)&iReserveSize, sizeof(_int));

        m_CreatedCellPoints.reserve(iReserveSize * 3);
        m_CreatedCells.reserve(iReserveSize);

        _float3 CellPoints[3] = {};
        while (true)
        {
            fin.read((_char*)CellPoints, sizeof(_float3) * 3);
            if (fin.eof())
                break;

            vector<CToolNaviCellPoint*> vecCellPoints;
            vecCellPoints.reserve(3);

            for (_int i = 0; i < 3; ++i)
            {
                _vector vCellPoint = XMVectorSetW(XMLoadFloat3(&CellPoints[i]), 1.f);

                CToolNaviCellPoint* pPoint = Find_SamePoint(vCellPoint);
                if (nullptr == pPoint)
                {
                    _float4 f4CellPoint;
                    XMStoreFloat4(&f4CellPoint, vCellPoint);
                    pPoint = static_cast<CToolNaviCellPoint*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Cell", L"Prototype_ToolNaviCellPoint", &f4CellPoint));
                    pPoint->Uncheck_Picked();

                    m_CreatedCellPoints.push_back(pPoint);
                }
                vecCellPoints.push_back(pPoint);
            }

            CToolNaviCell* pCell = static_cast<CToolNaviCell*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Cell", L"Prototype_ToolNaviCell", &vecCellPoints));
            m_CreatedCells.push_back(pCell);
        }
    }


    return S_OK;
}

void CMap_Tool::Destroy_Cells()
{
    for (auto it = m_CreatedCells.begin(); it != m_CreatedCells.end();)
    {
        if ((*it)->Is_Destroyed())
            it = m_CreatedCells.erase(it);
        else
            ++it;
    }

    for (auto it = m_CreatedCellPoints.begin(); it != m_CreatedCellPoints.end();)
    {
        if (m_bPickingMesh && static_cast<CToolNaviCellPoint*>(*it)->Get_RefCnt() == 1)
        {
            (*it)->Set_Destroy(true);
            it = m_CreatedCellPoints.erase(it);
        }
        else
            ++it;
    }
}


CToolNaviCellPoint* CMap_Tool::Find_SamePoint(_fvector vPointPos)
{
    for (CToolNaviCellPoint* pPoint : m_CreatedCellPoints)
        if (pPoint->Compare_PointPos(vPointPos))
            return pPoint;

    return nullptr;
}

void CMap_Tool::Get_MouseRayInfo(OUT _float4& _vRayStartPos, OUT _float4& _vRayDir)
{
    POINT CurMousePos = Get_ScreenCursorPos(g_hWnd);

    _vector vMouseNDC_Near = XMVectorSet(CurMousePos.x * 2.0f / g_iWinSizeX - 1, -CurMousePos.y * 2.0f / g_iWinSizeY + 1, 0.f, 1.f);
    _vector vMouseNDC_Far = XMVectorSet(CurMousePos.x * 2.0f / g_iWinSizeX - 1, -CurMousePos.y * 2.0f / g_iWinSizeY + 1, 1.f, 1.f);

    _matrix InverseViewProj = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW) * m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
    InverseViewProj = XMMatrixInverse(nullptr, InverseViewProj);

    _vector vMouseWorld_Near = XMVector3TransformCoord(vMouseNDC_Near, InverseViewProj);
    _vector vMouseWorld_Far = XMVector3TransformCoord(vMouseNDC_Far, InverseViewProj);

    _vector vRayDir = vMouseWorld_Far - vMouseWorld_Near;

    XMStoreFloat4(&_vRayStartPos, vMouseWorld_Near);
    XMStoreFloat4(&_vRayDir, vRayDir);
}

void CMap_Tool::Object_Picking()
{
    if (m_MapObjects.empty() || g_hWnd != GetFocus() || !m_bCanPick )
        return;

    if (KEY_DOWN(eKeyCode::LButton))
    {
        _int iPickingIdx = ColorPicking_Object();
        if (-1 == iPickingIdx || iPickingIdx >= (_int)m_MapObjects.size())
            return;
        else
        {
            _matrix WorldMatrix = m_MapObjects[iPickingIdx]->Get_Transform()->Get_WorldMatrix();
            Reset_Transform(WorldMatrix);

            m_iNowNaviIdx = m_MapObjects[iPickingIdx]->Get_NaviIdx();
            m_iTriggerIdx = m_MapObjects[iPickingIdx]->Get_TriggerIdx();
            m_vColliderSize = m_MapObjects[iPickingIdx]->Get_ColliderSize();

            m_SelectObjIndices.clear();
            m_SelectObjIndices.insert(iPickingIdx);
        }
            
    }
}

_int CMap_Tool::ColorPicking_Object()
{
    _int iResult = -1;

    POINT ptMouse = {};
    GetCursorPos(&ptMouse);
    ScreenToClient(g_hWnd, &ptMouse);

    RECT rect = {};
    GetClientRect(g_hWnd, &rect);

    if (!PtInRect(&rect, ptMouse))
        return -1;

    ID3D11RenderTargetView* pBackRTV = nullptr;
    ID3D11DepthStencilView* pBackDSV = nullptr;
    m_pContext->OMGetRenderTargets(1, &pBackRTV, &pBackDSV);
    m_pContext->OMSetRenderTargets(1, &m_pColorRTV, pBackDSV);

    _float vClearColor[4] = { 1.f, 1.f, 1.f, 1.f };

    m_pContext->ClearRenderTargetView(m_pColorRTV, vClearColor);
    m_pContext->ClearDepthStencilView(pBackDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    _int iIndex = 0;
    for (CToolMapObj* pObj : m_MapObjects)
        pObj->Render_Picking(iIndex++);

    D3D11_BOX PickingBox;
    PickingBox.left = ptMouse.x;
    PickingBox.right = ptMouse.x + 1;
    PickingBox.top = ptMouse.y;
    PickingBox.bottom = ptMouse.y + 1;
    PickingBox.front = 0;
    PickingBox.back = 1;

    m_pContext->CopySubresourceRegion(m_pPickingTexture, 0, 0, 0, 0, m_pColorTexture, 0, &PickingBox);

    _ubyte pickColor[4] = {};

    D3D11_MAPPED_SUBRESOURCE ms;
    m_pContext->Map(m_pPickingTexture, 0, D3D11_MAP_READ, 0, &ms);

    memcpy(pickColor, ms.pData, sizeof(_ubyte) * 4);

    m_pContext->Unmap(m_pPickingTexture, 0);

    iResult = 0;
    iResult += (_int)pickColor[0] + (_int)pickColor[1] + (_int)pickColor[2] + (_int)pickColor[3];

    m_pContext->OMSetRenderTargets(1, &pBackRTV, pBackDSV);

    Safe_Release(pBackRTV);
    Safe_Release(pBackDSV);

    return iResult;
}

CMap_Tool* CMap_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CMap_Tool* pInstance = new CMap_Tool(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CMap_Tool"));
        Safe_Release(pInstance);
    }

    return pInstance;

}

void CMap_Tool::Free()
{
    __super::Free();

    for (auto& Pair : m_MapObjectInstances)
        Safe_Release(Pair.second);

    m_MapObjectInstances.clear();

    Safe_Release(m_pColorRTV);
    Safe_Release(m_pColorTexture);
    
    Safe_Release(m_pPickingTexture);
}

