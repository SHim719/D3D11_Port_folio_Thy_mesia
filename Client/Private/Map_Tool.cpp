#include "Map_Tool.h"


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

    return S_OK;
}

void CMap_Tool::Tick(_float fTimeDelta)
{
    m_bMouseUsed = false;
    Main_Window();
    Camera_Window();
}

void CMap_Tool::LateTick(_float fTimeDelta)
{
    Destroy_MapObjects();
    Picking();
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

    TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
    TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pColorTexture)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateRenderTargetView(m_pColorTexture, nullptr, &m_pColorRTV)))
        return E_FAIL;

    TextureDesc.Width = 1;
    TextureDesc.Height = 1;
    TextureDesc.Usage = D3D11_USAGE_STAGING;
    TextureDesc.BindFlags = 0;
    TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pPickingTexture)))
        return E_FAIL;

    return S_OK;
}

void CMap_Tool::Main_Window()
{
    ImGui::Begin("Main_Window", (bool*)0, ImGuiWindowFlags_MenuBar);
    Picking();
    Menu_Bar();
    TabBar();
    ImGui::End();
}

void CMap_Tool::Camera_Window()
{
    __super::Camera_Window();
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
      
       wstring wstrPrototypeTag = L"Prototype_Model_";
       wstrPrototypeTag += fileTitle.c_str();
       if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, wstrPrototypeTag, CModel::Create(m_pDevice, m_pContext
           ,entry.path().parent_path().generic_string(), entry.path().filename().generic_string()))))
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
    wstring strModelPrototypeTag = L"Prototype_Model_" + Convert_StrToWStr(m_strPlacable_Objects[m_iSelObj]);

    CToolMapObj* pObj = static_cast<CToolMapObj*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"MapObject", L"Prototype_ToolMapObj", &strModelPrototypeTag));
    if (nullptr == pObj)
        return E_FAIL;

    m_MapObjects.emplace_back(pObj);
    m_MapLayers.insert({ m_strPlacable_Objects[m_iSelObj], pObj });

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
            Placable_Object();
            

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}



void CMap_Tool::Destroy_MapObjects()
{
    vector<string>::iterator strIter = m_strPlacable_Objects.begin();


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
            strIter = m_strPlacable_Objects.erase(strIter);
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
    ImGui::InputFloat3("Position", &m_vPosition.x);
    ImGui::InputFloat3("Rotation", &m_vRotation.x);
    ImGui::InputFloat3("Scale", &m_vScale.x);
}

void CMap_Tool::Transform_Gizmo()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGuizmo::BeginFrame();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    _float4x4 ViewMatrix = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
    _float4x4 ProjMatrix = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

    if (-1 == m_iSelObj)
        return;

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
    }
#pragma endregion 

    _float4x4 WorldMatrix = m_MapObjects[m_iSelObj]->Get_Transform()->Get_WorldFloat4x4();

    if (ImGuizmo::Manipulate(*ViewMatrix.m, *ProjMatrix.m
        , m_tGizmoDesc.CurrentGizmoOperation
        , m_tGizmoDesc.CurrentGizmoMode
        , *WorldMatrix.m, NULL, m_tGizmoDesc.bUseSnap ? &m_tGizmoDesc.snap[0] : NULL, m_tGizmoDesc.boundSizing ? m_tGizmoDesc.bounds : NULL
        , m_tGizmoDesc.boundSizingSnap ? m_tGizmoDesc.boundsSnap : NULL))
    {
        m_bMouseUsed = true;
        
        _vector vScale, vPosition, vQuaternion;
        XMMatrixDecompose(&vScale, &vQuaternion, &vPosition, XMLoadFloat4x4(&WorldMatrix));

        XMStoreFloat3(&m_vScale, vScale);
        XMStoreFloat3(&m_vPosition, vPosition);
        XMStoreFloat3(&m_vRotation, To_EulerAngle(vQuaternion));
    }

    m_MapObjects[m_iSelObj]->Get_Transform()->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));
}


void CMap_Tool::Placable_Object()
{
    ImGui::SeparatorText("Models");
    const _char** szPlacables = new const _char * [m_strPlacable_Objects.size()];

    ImGui::ListBox("##Placble_ListBox", &m_iSelObj, szPlacables, (_int)m_strPlacable_Objects.size(), 10);

    if (ImGui::Button("Create"))
    {
        wstring wstrModelName = Convert_StrToWStr(szPlacables[m_iSelObj]);
        CToolMapObj* pObj = static_cast<CToolMapObj*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, wstrModelName,
            L"Prototype_Model_" + wstrModelName, nullptr));

        string strObject = szPlacables[m_iSelObj];

        m_MapObjects.emplace_back(pObj);
        m_strPlacable_Objects.emplace_back(strObject);

        m_MapLayers.emplace(strObject, pObj);
    }

    Safe_Delete_Array(szPlacables);
}

void CMap_Tool::Picking()
{
    if (m_MapObjects.empty() || m_bMouseUsed)
        return;
    if (KEY_DOWN(eKeyCode::LButton))
    {
        _int iPickingIdx = Picking_Object();
        if (-1 == iPickingIdx)
            return;
    }
}

_int CMap_Tool::Picking_Object()
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
    m_pContext->OMGetRenderTargets(1, &pBackRTV, nullptr);
    m_pContext->OMSetRenderTargets(1, &m_pColorRTV, nullptr);

    _float vClearColor[4] = { 1.f, 1.f, 1.f, 1.f };

    m_pContext->ClearRenderTargetView(m_pColorRTV, vClearColor);

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
    iResult += (_int)pickColor[0];

    Safe_Release(pBackRTV);

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

    Safe_Release(m_pColorRTV);
    Safe_Release(m_pColorTexture);
    
    Safe_Release(m_pPickingTexture);
}


