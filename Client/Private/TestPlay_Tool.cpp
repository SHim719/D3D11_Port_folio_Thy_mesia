#include "TestPlay_Tool.h"

#include "Main_Camera.h"
#include "Player.h"

#include "ToolMapObj.h"

CTestPlay_Tool::CTestPlay_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CToolState(pDevice, pContext)
{
}

HRESULT CTestPlay_Tool::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CTestPlay_Tool::Start_Tool()
{
    CCamera::CAMERADESC camDesc{};
    camDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    camDesc.fNear = 0.1f;
    camDesc.fFar = 300.f;
    camDesc.fFovy = 70.f;
    camDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
    camDesc.vEye = { 0.f, 10.f, 0.f, 1.f };

    m_pMain_Camera = static_cast<CMain_Camera*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Camera", L"Prototype_Main_Camera", &camDesc));
    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Player", L"Prototype_Player", nullptr));

    m_pMain_Camera->Set_Player(m_pPlayer);
    m_pMain_Camera->Set_Active(false);

    Safe_AddRef(m_pPlayer);
    Safe_AddRef(m_pMain_Camera);

    if (FAILED(Load_TestMap()))
        return;
}

void CTestPlay_Tool::Tick(_float fTimeDelta)
{
    Main_Window();
    Camera_Window();
}

void CTestPlay_Tool::Main_Window()
{
    //ImGui::Begin("Animation", (bool*)0, ImGuiWindowFlags_MenuBar);
    ////ImGui::SetCursorPos(ImVec2(11.5, 45));
    ////ImGui::Text("Loaded Mesh: ");
    ////
    ////ImGui::SetCursorPos(ImVec2(100.5, 45));
    //
    //ImGui::End();
}   //

void CTestPlay_Tool::Camera_Window()
{
    __super::Camera_Window();
    ImGui::SetCursorPos(ImVec2(20, 140));
    ImGui::Text("Free Camera?");


    _bool bFreeCam = m_pCamera->Is_Active();
    if (KEY_DOWN(eKeyCode::NUMPAD0))
    {
        bFreeCam = !bFreeCam;
        m_pCamera->Set_Active(bFreeCam);
        m_pMain_Camera->Set_Active(!bFreeCam);
    }

    ImGui::Checkbox("##CheckCamera", &bFreeCam);


    ImGui::End();
}

HRESULT CTestPlay_Tool::Load_TestMap()
{
    string strMapDataPath = "../../Resources/Maps/SaveTest/";

    fs::path MapDataPath(strMapDataPath);

    for (const fs::directory_entry& entry : fs::directory_iterator(MapDataPath))
    {
        if (entry.is_directory())
            continue;

        ifstream fin(entry.path().c_str(), ios::binary);

        if (!fin.is_open())
            return E_FAIL;

        fs::path fileName = entry.path().filename();
        fs::path fileTitle = fileName.stem();

        LOADOBJDESC LoadDesc;

        wstring wstrPrototypeTag = L"Prototype_";
        wstrPrototypeTag += fileTitle.c_str();

        while (true)
        {
            fin.read((_char*)&LoadDesc, sizeof(LOADOBJDESC));

            if (fin.eof())
                break;

            wsprintf(LoadDesc.szModelTag, L"Prototype_Model_%s", fileTitle.c_str());

            CGameObject* pObj = m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Object", wstrPrototypeTag, &LoadDesc);
            if (nullptr == pObj)
                m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Object", L"Prototype_MapObject", &LoadDesc);
        }
    }


    return S_OK;
}


CTestPlay_Tool* CTestPlay_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CTestPlay_Tool* pInstance = new CTestPlay_Tool(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CTestPlay_Tool"));
        Safe_Release(pInstance);
    }

    return pInstance;

}


void CTestPlay_Tool::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
    Safe_Release(m_pMain_Camera);
}
