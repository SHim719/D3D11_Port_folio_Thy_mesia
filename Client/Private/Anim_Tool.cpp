#include "Anim_Tool.h"

CAnim_Tool::CAnim_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolState(pDevice, pContext)
{
}

HRESULT CAnim_Tool::Initialize(void* pArg)
{
	return S_OK;
}

void CAnim_Tool::Tick(_float fTimeDelta)
{
    Main_Window();
}

void CAnim_Tool::Main_Window()
{
	ImGui::Begin("Animation", (bool*)0, ImGuiWindowFlags_MenuBar);
    PlayerAnim_Bar();
	ImGui::End();
}

void CAnim_Tool::Menu_Bar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open Animation FBX"))
            {
                

            }

            if (ImGui::MenuItem("Load Anim Data"))
            {

            }

            // �ִϸ��̼��� �и��ؼ� Ű������ �̺�Ʈ�� �Բ� ���� ����.
            // �޽��� �پ��ִ� �ִϸ��̼��� �Ⱦ�.
            if (ImGui::MenuItem("Save Animations"))
            {
               
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void CAnim_Tool::PlayerAnim_Bar()
{
    if (ImGui::BeginTabBar("Player"))
    {
        Menu_Bar();
        if (ImGui::BeginTabItem("Player"))
        {


            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void CAnim_Tool::OtherAnim_Bar()
{

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
}
