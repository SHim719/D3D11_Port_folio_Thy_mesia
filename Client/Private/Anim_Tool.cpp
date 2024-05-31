#include "Anim_Tool.h"

#include "ToolAnimObj.h"
#include "Animation.h"

CAnim_Tool::CAnim_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolState(pDevice, pContext)
{
}

HRESULT CAnim_Tool::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	return S_OK;
}

void CAnim_Tool::Tick(_float fTimeDelta)
{
    Main_Window();
    Camera_Window();
}

void CAnim_Tool::Main_Window()
{
	ImGui::Begin("Animation", (bool*)0, ImGuiWindowFlags_MenuBar);
    Menu_Bar();
    ImGui::SetCursorPos(ImVec2(11.5, 45));
    ImGui::Text("Loaded Mesh: ");

    ImGui::SetCursorPos(ImVec2(100.5, 45));
    ImGui::Text(m_strNowLoaded.c_str());


    Anim_ListBox();
    Anim_Buttons();

	ImGui::End();
}

void CAnim_Tool::Camera_Window()
{
    __super::Camera_Window();
    ImGui::End();
}

void CAnim_Tool::Menu_Bar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Load KeyFrame"))
            {
            
            }
            
            if (ImGui::MenuItem("Save KeyFrame"))
            {
            
            }

            ImGui::EndMenu();
            
        }
        ImGui::EndMenuBar();
    }
}

void CAnim_Tool::Anim_ListBox()
{
    const char** szAnimations = new const char* [m_strAnimations.size()];

    for (_int i = 0; i < (_int)m_strAnimations.size(); ++i)
        szAnimations[i] = m_strAnimations[i].c_str();
    ImGui::PushItemWidth(180);
    if (ImGui::ListBox("##Anim_ListBox", &m_iCurSelAnimIdx, szAnimations, (_int)m_strAnimations.size()))
    {
        m_pModel->Change_Animation(m_iCurSelAnimIdx);
    }


    ImGui::PopItemWidth();
    Safe_Delete_Array(szAnimations);
}

void CAnim_Tool::Anim_Buttons()
{
    ImGui::SetCursorPos(ImVec2(220.5, 68.5));
    if (ImGui::Button("Play"))
    {
        m_pModel->Set_AnimPlay();
    }
        
    ImGui::SetCursorPos(ImVec2(220.5, 100));
    if (ImGui::Button("Stop"))
    {
        m_pModel->Set_AnimPause();
    }

    ImGui::SetCursorPos(ImVec2(220.5, 131));
    ImGui::Button("Replay");
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

    Safe_Release(m_pAnimObj);
    Safe_Release(m_pModel);
}
