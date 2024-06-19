#include "Map_Tool.h"
#include "ToolNaviCell.h"


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
    Key_Input();
    Main_Window();
    Camera_Window();
}

void CMap_Tool::LateTick(_float fTimeDelta)
{
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
       if (entry.is_directory())
           continue;

       fs::path fileName = entry.path().filename();
       fs::path fileTitle = fileName.stem();
      
       wstring wstrPrototypeTag = L"Prototype_Model_";
       wstrPrototypeTag += fileTitle.c_str();
       if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, wstrPrototypeTag, CModel::Create(m_pDevice, m_pContext
           ,entry.path().parent_path().generic_string() + "/", entry.path().filename().generic_string()))))
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
    wstring strModelPrototypeTag = L"Prototype_Model_" + Convert_StrToWStr(m_strPlacable_Objects[m_iSelPlacableObj]);

    CToolMapObj* pObj = static_cast<CToolMapObj*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"MapObject", L"Prototype_ToolMapObj", &strModelPrototypeTag));
    if (nullptr == pObj)
        return E_FAIL;

    m_MapObjects.emplace_back(pObj);
    m_strCreatedObjects.emplace_back(m_strPlacable_Objects[m_iSelPlacableObj]);
    m_MapLayers.emplace(m_strPlacable_Objects[m_iSelPlacableObj], pObj);

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
    Transform_Gizmo();
    Transform_View();
    Placable_Object();
    Object_ListBox();
    Destroy_MapObjects();
    Picking_Object();
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
        if (-1 == m_iSelObj)
            return;
        m_MapObjects[m_iSelObj]->Get_Transform()->Set_Position(XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
    }

    if (ImGui::InputFloat3("Rotation", &m_vRotation.x))
    {
        if (-1 == m_iSelObj)
            return;
        _vector vQuat = XMQuaternionRotationRollPitchYaw(m_vRotation.x, m_vRotation.y, m_vRotation.z);
        m_MapObjects[m_iSelObj]->Get_Transform()->Rotation_Quaternion(vQuat);
    }

    if (ImGui::InputFloat3("Scale", &m_vScale.x))
    {
        if (-1 == m_iSelObj)
            return;
        m_MapObjects[m_iSelObj]->Get_Transform()->Set_Scale(m_vScale);
    }
}

void CMap_Tool::Reset_Transform(_fmatrix WorldMatrix)
{
    _vector vScale, vPosition, vQuaternion;
    XMMatrixDecompose(&vScale, &vQuaternion, &vPosition, WorldMatrix);

    XMStoreFloat3(&m_vScale, vScale);
    XMStoreFloat3(&m_vPosition, vPosition);
    XMStoreFloat3(&m_vRotation, JoMath::ToEulerAngle(vQuaternion));
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

        if (m_pGameInstance->GetKeyDown(eKeyCode::Y))
            m_tGizmoDesc.bUseSnap = !m_tGizmoDesc.bUseSnap;
    }
#pragma endregion 
    
    _float4x4 WorldMatrix = m_MapObjects[m_iSelObj]->Get_Transform()->Get_WorldFloat4x4();
    if (ImGuizmo::Manipulate(*ViewMatrix.m, *ProjMatrix.m
        , m_tGizmoDesc.CurrentGizmoOperation
        , m_tGizmoDesc.CurrentGizmoMode
        , *WorldMatrix.m, NULL, m_tGizmoDesc.bUseSnap ? &m_tGizmoDesc.snap[0] : NULL, m_tGizmoDesc.boundSizing ? m_tGizmoDesc.bounds : NULL
        , m_tGizmoDesc.boundSizingSnap ? m_tGizmoDesc.boundsSnap : NULL))
    {   
        Reset_Transform(XMLoadFloat4x4(&WorldMatrix));
        m_MapObjects[m_iSelObj]->Get_Transform()->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));
    }
    
}

void CMap_Tool::Placable_Object()
{
    ImGui::SeparatorText("Models");

    ImVec2 vStartPos = ImGui::GetCursorPos();
    vStartPos.x += 240.f;

    const _char** szPlacables = new const _char * [m_strPlacable_Objects.size()];

    for (size_t i = 0; i < m_strPlacable_Objects.size(); ++i)
        szPlacables[i] = m_strPlacable_Objects[i].c_str();

    ImGui::ListBox("##Placble_ListBox", &m_iSelPlacableObj, szPlacables, (_int)m_strPlacable_Objects.size(), 10);

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
        m_tGizmoDesc.CurrentGizmoMode = bIsLocalMode ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
    
}

void CMap_Tool::Object_ListBox()
{
    ImGui::SeparatorText("Created Objects");
    const _char** szObjects = new const _char * [m_strCreatedObjects.size()];

    for (size_t i = 0; i < m_strCreatedObjects.size(); ++i)
        szObjects[i] = m_strCreatedObjects[i].c_str();

    if (ImGui::ListBox("##CreatedObj_ListBox", &m_iSelObj, szObjects, (_int)m_strCreatedObjects.size(), 10))
    {
        _float4x4 WorldMatrix = m_MapObjects[m_iSelObj]->Get_Transform()->Get_WorldFloat4x4();
        Reset_Transform(XMLoadFloat4x4(&WorldMatrix));
    }
       
    Safe_Delete_Array(szObjects);
}

void CMap_Tool::Navi_Tool()
{
    ImGui::SeparatorText("Navigation");

    Navi_Picking();
    Navi_Tool_Options();
    Navi_ListBox();
   
}

void CMap_Tool::Navi_Tool_Options()
{
    if (ImGui::Checkbox("Picking?", &m_bCanPick))
        m_bCanPick = !m_bCanPick;

    if (ImGui::Checkbox("Picking NaviMesh?", &m_bPickingNaviMesh))
        m_bPickingNaviMesh = !m_bPickingNaviMesh;
}

void CMap_Tool::Navi_ListBox()
{
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
        _int iPickingIdx = Picking_Object();
        if (-1 == iPickingIdx)
            return;

        POINT CurMousePos = Get_ScreenCursorPos(g_hWnd);

        _vector vMouseNDC_Near = XMVectorSet(CurMousePos.x * 2.0f / g_iWinSizeX - 1, -CurMousePos.y * 2.0f / g_iWinSizeY + 1, 0.f, 1.f);
        _vector vMouseNDC_Far = XMVectorSet(CurMousePos.x * 2.0f / g_iWinSizeX - 1, -CurMousePos.y * 2.0f / g_iWinSizeY + 1, 1.f, 1.f);

        _matrix InverseViewProj = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW) * m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
        InverseViewProj = XMMatrixInverse(nullptr, InverseViewProj);

        _vector vMouseWorld_Near = XMVector3TransformCoord(vMouseNDC_Near, InverseViewProj);
        _vector vMouseWorld_Far = XMVector3TransformCoord(vMouseNDC_Far, InverseViewProj);

        _vector vRayDir = XMVector3Normalize(vMouseWorld_Far - vMouseWorld_Near);

        _float4 vPickedPos = {};
        _float fDist = -1.f;

        if (m_MapObjects[iPickingIdx]->Ray_Cast(vMouseWorld_Near, vRayDir, vPickedPos, fDist))
        {
            _bool bPickCreatedPoint = false;
            for (auto pCell : m_CreatedCells)
            {
               // pCell->Check_Points(XMLoadFloat4(&vPickedPos));
            }

            if (nullptr == m_NowCreatingCell)
                m_NowCreatingCell = static_cast<CToolNaviCell*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Cell", L"Prototype_ToolNaviCell"));
            
            if (m_NowCreatingCell->Add_CellPoint(XMLoadFloat4(&vPickedPos)))
            {
                m_CreatedCells.push_back(m_NowCreatingCell);
                m_NowCreatingCell = nullptr;
            }
        }
            
    }


}


void CMap_Tool::Object_Picking()
{
    if (m_MapObjects.empty() || g_hWnd != GetFocus() || !m_bCanPick )
        return;

    if (KEY_DOWN(eKeyCode::LButton))
    {
        _int iPickingIdx = Picking_Object();
        if (-1 == iPickingIdx)
            return;
        else
            m_iSelObj = iPickingIdx;
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
    iResult += (_int)pickColor[0];

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

    Safe_Release(m_pColorRTV);
    Safe_Release(m_pColorTexture);
    
    Safe_Release(m_pPickingTexture);
}




