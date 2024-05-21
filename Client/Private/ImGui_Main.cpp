#include "ImGui_Main.h"
#include "GameInstance.h"

#include "Anim_Tool.h"
#include "Map_Tool.h"

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
    //io.DisplaySize = ImVec2(float(g_iWinSizeX), float(g_iWinSizeY));

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    m_ToolStates[MAP] = CMap_Tool::Create(m_pDevice, m_pContext);
    m_ToolStates[ANIM] = CAnim_Tool::Create(m_pDevice, m_pContext);
   
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
    const char* Tools[] = { "Map", "Animation", "Effect"};
    if (ImGui::Combo("##ToolComboBox", (int*)(&m_eToolState), Tools, IM_ARRAYSIZE(Tools)))
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
}
