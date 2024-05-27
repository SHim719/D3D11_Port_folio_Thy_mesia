#include "Map_Tool.h"


CMap_Tool::CMap_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CToolState(pDevice, pContext)
{
}

HRESULT CMap_Tool::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CMap_Tool::Tick(_float fTimeDelta)
{
    Main_Window();
    __super::Camera_Window();
}

void CMap_Tool::Main_Window()
{
    ImGui::Begin("Main_Window", (bool*)0, ImGuiWindowFlags_MenuBar);
    Menu_Bar();
    TabBar();
    ImGui::End();
}

HRESULT CMap_Tool::Open_MeshesByFolder()
{
    wstring wstrFolderPath = Get_FolderPath();
    if (L"" == wstrFolderPath)
        return E_FAIL;

   fs::path folderPath(wstrFolderPath);
   
   for (const fs::directory_entry& entry : fs::directory_iterator(folderPath))
   {
       fs::path fileName = entry.path().filename();
       fs::path fileTitle = fileName.stem();
      
       CModel::LOADMODELDESC desc;
       ZeroMemory(&desc, sizeof(desc));
       desc.eType = CModel::TYPE_NONANIM;
       XMStoreFloat4x4(&desc.PivotMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
       desc.strModelFilePath = folderPath.generic_string() + "/";
       desc.strModelFileName = fileName.generic_string();

       wstring wstrPrototypeTag = L"Prototype_Model_";
       wstrPrototypeTag += fileTitle.c_str();
       if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, wstrPrototypeTag, CModel::Create(m_pDevice, m_pContext, &desc))))
           return E_FAIL;

       m_strPlacable_Objects.push_back(fileTitle.generic_string());
   }


    return S_OK;
}


wstring CMap_Tool::Get_FolderPath()
{
    ITEMIDLIST* pidlRoot = nullptr;
    _tchar szInitialPath[MAX_PATH] = L"D:\\JaeookDX11Tool\\Resources\\";
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
    wstring strModelPrototypeTag = L"Prototype_Model_" + Convert_StrToWStr(m_strPlacable_Objects[m_iSelModelObj]);

    CToolMapObj* pObj = static_cast<CToolMapObj*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"MapObject", L"Prototype_ToolMapObj", &strModelPrototypeTag));
    if (nullptr == pObj)
        return E_FAIL;

    m_MapObjects.emplace_back(pObj);

    return S_OK;
}



void CMap_Tool::Menu_Bar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open Meshes"))
            {
                Open_MeshesByFolder();
            }
            
            if (ImGui::MenuItem("Open Map"))
            {

            }

            if (ImGui::MenuItem("Save Map"))
            {

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
            Transform_View();

            Object_Child_Window();

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void CMap_Tool::Transform_View()
{
    ImGui::SetCursorPos(ImVec2(97, 78));
    ImGui::PushItemWidth(200);
    ImGui::InputFloat3("##Position", &m_vPosition.x);
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(26, 81));
    ImGui::Text("Position");

    ImGui::SetCursorPos(ImVec2(26, 114));
    ImGui::Text("Rotation");

    ImGui::SetCursorPos(ImVec2(97, 112));
    ImGui::PushItemWidth(200);
    ImGui::InputFloat3("##Rotation", &m_vRotation.x);
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(34, 145));
    ImGui::Text("Scale");

    ImGui::SetCursorPos(ImVec2(97, 143));
    ImGui::PushItemWidth(200);
    ImGui::InputFloat3("##Scale", &m_vScale.x);
    ImGui::PopItemWidth();
}

void CMap_Tool::Object_Child_Window()
{
    ImGui::SetCursorPos(ImVec2(12, 175));
    ImGui::BeginChild(24, ImVec2(370, 450), true);

    if (ImGui::BeginTabBar("ObjectTab"))
    {
        if (ImGui::BeginTabItem("Model List"))
        {
            ImGui::PushItemWidth(150);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.f);
            if (ImGui::BeginListBox("##Models_ListBox", { 200.f, 400.f }))
            {
                Models_ListBox();
                ImGui::EndListBox();
            }

            ImGui::PopItemWidth();


            ImGui::SetCursorPos({ 240.f, 45.f });
            if (ImGui::Button("Create Object"))
            {
                if (FAILED(Create_ObjectInLevel()))
                    MSG_BOX(L"Failed Create_ObjectInLevel().");
            }

            
            ImGui::EndTabItem();
        }
        

        if (ImGui::BeginTabItem("Placed"))
        {


            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::EndChild();
}

void CMap_Tool::Models_ListBox()
{
    for (_int i = 0; i < (_int)m_strPlacable_Objects.size(); ++i)
    {
        char buf[128];
        sprintf_s(buf, m_strPlacable_Objects[i].c_str());
        if (ImGui::Selectable(buf, m_iSelModelObj == (int)i))
        {
            m_iSelModelObj = i;
            break;
        }
    }
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
}
