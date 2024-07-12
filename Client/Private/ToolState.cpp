#include "ToolState.h"

CToolState::CToolState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
    , m_pGameInstance(CGameInstance::Get_Instance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CToolState::Initialize(void* pArg)
{
	return S_OK;
}

void CToolState::Start_Tool()
{
    m_pCamera->Get_Transform()->Set_Position(XMVectorSet(0.f, 2.f, -1.f, 1.f));
}

void CToolState::Tick(_float fTimeDelta)
{
}

void CToolState::LateTick(_float fTimeDelta)
{
}

void CToolState::Camera_Window()
{
    ImGui::Begin("Camera_State", (bool*)0);
    ImGui::SetCursorPos(ImVec2(23, 36));
    ImGui::Text("Camera Position");

    ImGui::SetCursorPos(ImVec2(16, 57));
    ImGui::PushItemWidth(200);

    _float4 vCamPos;
    XMStoreFloat4(&vCamPos, m_pCamera->Get_Transform()->Get_Position());
    if (ImGui::InputFloat3("##CamPos", &(vCamPos.x)))
        m_pCamera->Get_Transform()->Set_Position(XMLoadFloat4(&vCamPos));
    ImGui::PopItemWidth();


    ImGui::SetCursorPos(ImVec2(25, 90));
    ImGui::Text("FovY");

    ImGui::SetCursorPos(ImVec2(18, 105));
    ImGui::PushItemWidth(183);

    _float fFovY = m_pCamera->Get_FovY();
    if (ImGui::InputFloat("##Fov", &fFovY, 0.01f, 179.f, "%.3f"))
        m_pCamera->Set_FovY(fFovY);

    ImGui::PopItemWidth();

}

bool _stdcall CToolState::VectorOfStringGetter(void* data, _int n, const char** out_text)
{
    const vector<string>* v = (vector<string>*)data;
    *out_text = (*v)[n].c_str();
    return true;
}

void CToolState::Free()
{
	__super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
