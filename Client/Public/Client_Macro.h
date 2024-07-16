#pragma once

#define KEY_DOWN(KEY) m_pGameInstance->GetKeyDown(KEY)
#define KEY_PUSHING(KEY) m_pGameInstance->GetKey(KEY)
#define KEY_UP(KEY) m_pGameInstance->GetKeyUp(KEY)
#define KEY_NONE(KEY) m_pGameInstance->GetKeyNone(KEY)


#define GET_CURLEVEL m_pGameInstance->Get_CurrentLevelID()
#define SET_CURLEVEL(LEVEL) m_pGameInstance->Set_CurrentLevelID(LEVEL)
#define ADD_EVENT(FUNC) m_pGameInstance->Add_Event(FUNC)
#define GET_PLAYER static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Player"))

#define Get_Inst(TYPE) TYPE::Get_Instance()

#define UIMGR Get_Inst(CUI_Manager)
#define CUTSCENEMGR Get_Inst(CCutscene_Manager)


#define YAXIS XMVectorSet(0.f, 1.f, 0.f, 0.f)
 