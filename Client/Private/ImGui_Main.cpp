#include "ImGui_Main.h"
#include "GameInstance.h"

#include "Anim_Tool.h"

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
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);


    m_ToolStates[ANIM] = CAnim_Tool::Create(m_pDevice, m_pContext);
    return S_OK;
}

void CImGui_Main::Tick(_float fTimeDelta)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ComboBox_ToolSelect();
    m_ToolStates[m_eToolState]->Tick(fTimeDelta);
    ImGui::EndFrame();
}

void CImGui_Main::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


void CImGui_Main::ComboBox_ToolSelect()
{
    if (ImGui::Begin("window_name", (bool*)0))
    {

        ImGui::SetCursorPos(ImVec2(11, 33.5));
        ImGui::PushItemWidth(100);
        const char* Tools[TOOL_END] = { "Map", "Animation", "Effect"};
        ImGui::Combo("##", (int*)(&m_eToolState), Tools, IM_ARRAYSIZE(Tools));
        ImGui::PopItemWidth();

    }
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
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
