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
    __super::Camera_Window();
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

void CAnim_Tool::Menu_Bar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open FBX"))
            {
                _tchar szFullPath[MAX_PATH] = {};
                OPENFILENAME ofn = {};

                ofn.lStructSize = sizeof(OPENFILENAME);
                ofn.hwndOwner = g_hWnd;
                ofn.lpstrFile = szFullPath;
                ofn.nMaxFile = sizeof(szFullPath);
                ofn.lpstrFilter = L"*.fbx";
                ofn.lpstrInitialDir = L"../../Resources/Model/";
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetOpenFileName(&ofn))
                {
                    fs::path fullPath(szFullPath);

                    fs::path fileName = fullPath.filename();
                    fs::path fileTitle = fileName.stem();

                    CModel::LOADMODELDESC desc;
                    ZeroMemory(&desc, sizeof(desc));
                    desc.eType = CModel::TYPE_ANIM;
                    XMStoreFloat4x4(&desc.PivotMatrix, XMMatrixScaling(0.1f, 0.1f, 0.1f));
                    desc.strModelFilePath = fullPath.parent_path().generic_string() + "/";
                    desc.strModelFileName = fileName.generic_string();

                    wstring wstrPrototypeTag = L"Prototype_Model_";
                    wstrPrototypeTag += fileTitle.generic_wstring();
                    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, wstrPrototypeTag, CModel::Create(m_pDevice, m_pContext, &desc))))
                        assert(false);

                    Safe_Release(m_pAnimObj);
                    Safe_Release(m_pModel);

                    m_pAnimObj = static_cast<CToolAnimObj*>(m_pGameInstance->Add_Clone(LEVEL_TOOL, L"Model", L"Prototype_ToolAnimObj", &wstrPrototypeTag));
                    m_pAnimObj->Get_Transform()->Set_Position(XMVectorSet(0.f, 0.f, 1.f, 1.f));
                    m_pModel = m_pAnimObj->Get_Model();
                    m_strNowLoaded = fileName.generic_string();

                    const auto& Anims = m_pModel->Get_Animations();
                    m_strAnimations.reserve(Anims.size());
                    for (CAnimation* pAnim : Anims)
                        m_strAnimations.push_back(pAnim->Get_AnimName());

                    Safe_AddRef(m_pAnimObj);
                    Safe_AddRef(m_pModel);
                }
            }

            if (ImGui::MenuItem("Open AnimOnly FBX"))
            {
                _tchar szFullPath[MAX_PATH] = {};
                OPENFILENAME ofn = {};

                ofn.lStructSize = sizeof(OPENFILENAME);
                ofn.hwndOwner = g_hWnd;
                ofn.lpstrFile = szFullPath;
                ofn.nMaxFile = sizeof(szFullPath);
                ofn.lpstrFilter = L"*.fbx";
                ofn.lpstrInitialDir = L"../../Resources/Model/";
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetOpenFileName(&ofn))
                {
                    fs::path fullPath(szFullPath);
                    m_pAnimObj->Get_Model()->Add_Animations(fullPath.generic_string().c_str());

                    const auto& Anims = m_pAnimObj->Get_Model()->Get_Animations();
                    m_strAnimations.reserve(Anims.size());
                    for (CAnimation* pAnim : Anims)
                        m_strAnimations.push_back(pAnim->Get_AnimName());
                }
            }

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
        m_pModel->Set_Playing(true);
    }
        
    ImGui::SetCursorPos(ImVec2(220.5, 100));
    if (ImGui::Button("Stop"))
    {
        m_pModel->Set_Playing(false);
    }

    ImGui::SetCursorPos(ImVec2(220.5, 131));
    ImGui::Button("Replay");

    ImGui::SetCursorPos(ImVec2(220, 162));
    if (nullptr != m_pModel)
        m_bLoop = m_pModel->Is_Loop();
    if (nullptr != m_pModel && ImGui::Checkbox("Loop?", &m_bLoop))
        m_pModel->Set_Loop(!m_bLoop);
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
