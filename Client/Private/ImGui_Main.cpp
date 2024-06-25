#include "ImGui_Main.h"
#include "GameInstance.h"

#include "Anim_Tool.h"
#include "Map_Tool.h"
#include "TestPlay_Tool.h"

CImGui_Main::CImGui_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
    , m_pGameInstance(CGameInstance::Get_Instance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CImGui_Main::Initialize()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    RECT rect = {};
    GetClientRect(g_hWnd, &rect);
    long Width = rect.right - rect.left;
    long Height = rect.bottom - rect.top;

    io.DisplaySize = ImVec2(float(Width), float(Height));

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    m_ToolStates[MAP] = CMap_Tool::Create(m_pDevice, m_pContext);
    m_ToolStates[ANIM] = CAnim_Tool::Create(m_pDevice, m_pContext);
    m_ToolStates[TESTPLAY] = CTestPlay_Tool::Create(m_pDevice, m_pContext);   

    ToolState eState;

#ifdef MapTool
    eState = MAP;
#endif

#ifdef AnimTool
    eState = ANIM;
#endif

#ifdef testplay
    eState = TESTPLAY;
#endif

    m_eToolState = eState;
    m_ToolStates[m_eToolState]->Start_Tool();
    return S_OK;
}

void CImGui_Main::Tick(_float fTimeDelta)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ComboBox_ToolSelect();
    if (m_ToolStates[m_eToolState])
        m_ToolStates[m_eToolState]->Tick(fTimeDelta);

}

void CImGui_Main::LateTick(_float fTimeDelta)
{
    if (m_ToolStates[m_eToolState])
        m_ToolStates[m_eToolState]->LateTick(fTimeDelta);
}

void CImGui_Main::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


void CImGui_Main::ComboBox_ToolSelect()
{
    ImGui::Begin("Tool");
    ImGui::SetCursorPos(ImVec2(11, 33.5));
    ImGui::PushItemWidth(100);
    const char* Tools[] = { "Map", "Animation", "Effect", "CutScene", "TestPlay"};
    if (ImGui::Combo("##ToolComboBox", (int*)(&m_eToolState), Tools, IM_ARRAYSIZE(Tools)))
    {
        m_pGameInstance->Clear_Level(LEVEL_TOOL);
    }
    ImGui::PopItemWidth();
    ImGui::End();
    
}

CImGui_Main* CImGui_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CImGui_Main* pInstance = new CImGui_Main(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Created : CImGui_Main"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CImGui_Main::Free()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);

    for (_uint i = 0; i < TOOL_END; ++i)
        Safe_Release(m_ToolStates[i]);
    
}
