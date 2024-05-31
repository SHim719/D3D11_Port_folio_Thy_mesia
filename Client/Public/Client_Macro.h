#pragma once

#define KEY_DOWN(KEY) m_pGameInstance->GetKeyDown(KEY)
#define KEY_PUSHING(KEY) m_pGameInstance->GetKey(KEY)
#define KEY_UP(KEY) m_pGameInstance->GetKeyUp(KEY)
#define KEY_NONE(KEY) m_pGameInstance->GetKeyNone(KEY)


#define GET_CURLEVEL m_pGameInstance->Get_CurrentLevelID()



#define YAXIS XMVectorSet(0.f, 1.f, 0.f, 0.f)
 