#include "ClientLevel.h"
#include "MapObject.h"
#include "Instancing_Object.h"
#include "Level_Loading.h"
#include "Player.h"
#include "UI_Manager.h"
#include "GameInstance.h"
#include "MyCamera.h"


CClientLevel::CClientLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CClientLevel::Load_Map(const wstring& wstrMapPath)
{
    fs::path mapPath(wstrMapPath);

    for (const fs::directory_entry& entry : fs::directory_iterator(mapPath))
    {
        if (entry.is_directory())
            continue;

        ifstream fin(entry.path().c_str(), ios::binary);

        if (!fin.is_open())
            return E_FAIL;

        fs::path fileName = entry.path().filename();
        fs::path fileTitle = fileName.stem();

        LOADOBJDESC LoadDesc;

        _float fCullingRadius = 1.f;
        fin.read((_char*)&fCullingRadius, sizeof(_float));

        vector<LOADOBJDESC> LoadObjDescs;
        LoadObjDescs.reserve(30);

        while (true)
        {
            fin.read((_char*)&LoadDesc, sizeof(LOADOBJDESC));

            wsprintf(LoadDesc.szModelTag, L"Prototype_Model_%s", fileTitle.c_str());

            if (fin.eof())
                break;

            LoadObjDescs.emplace_back(LoadDesc);
        }
        fin.close();

        wstring wstrPrototypeTag = L"Prototype_" + fileTitle.generic_wstring();

        switch (LoadObjDescs[0].eObjType)
        {
        case MAPOBJECT:
        {
            if (FAILED(Load_MapObjects(LoadObjDescs, fCullingRadius, wstrPrototypeTag)))
                return E_FAIL;
        }
        break;
        case ENEMY:
            for (LOADOBJDESC& ObjDesc : LoadObjDescs)
            {
                CGameObject* pLoadObject = m_pGameInstance->Add_Clone(m_iLevelID, L"Enemy", wstrPrototypeTag, &ObjDesc);
                pLoadObject->Set_CullingRadius(fCullingRadius);
            }
            break;
        case TRIGGEROBJ:
            for (LOADOBJDESC& ObjDesc : LoadObjDescs)
            {
                CGameObject* pLoadObject = m_pGameInstance->Add_Clone(m_iLevelID, L"TriggerObject", wstrPrototypeTag, &ObjDesc);
                pLoadObject->Set_CullingRadius(fCullingRadius);
            }
                
            break;
        }
    }

    return S_OK;
}

HRESULT CClientLevel::Load_MapObjects(vector<LOADOBJDESC>& LoadObjDescs, const _float& fCullingRadius, const wstring& wstrPrototypeTag)
{
    // 같은 물체 20개 이상이면 인스턴싱하도록.
    if (LoadObjDescs.size() < 20)
    {
        for (LOADOBJDESC& ObjDesc : LoadObjDescs)
        {
            CGameObject* pLoadObject = m_pGameInstance->Add_Clone(m_iLevelID, L"MapObject", wstrPrototypeTag, &ObjDesc);
             if (nullptr == pLoadObject)
                 pLoadObject = m_pGameInstance->Add_Clone(m_iLevelID, L"MapObject", L"Prototype_MapObject", &ObjDesc);
             
            if (nullptr == pLoadObject)
                return E_FAIL;

            pLoadObject->Set_CullingRadius(fCullingRadius);
        }
    }
    else
    {
        wstring wstrModelTag = LoadObjDescs[0].szModelTag;
        CInstancing_Object* pInstancingObj = static_cast<CInstancing_Object*>(m_pGameInstance->Add_Clone(m_iLevelID, L"MapObject", L"Prototype_InstancingObj", &wstrModelTag));
        if (nullptr == pInstancingObj)
            return E_FAIL;

        pInstancingObj->Set_CullingRadius(fCullingRadius);

        _bool bIsOriginObject = m_pGameInstance->Find_Prototype(wstrPrototypeTag);

        for (LOADOBJDESC& ObjDesc : LoadObjDescs)
        {
            pInstancingObj->Add_WorldMatrix(ObjDesc.WorldMatrix);

            if (bIsOriginObject)
            {
                CGameObject* pLoadObject = m_pGameInstance->Add_Clone(m_iLevelID, L"MapObject", wstrPrototypeTag, &ObjDesc);
                pLoadObject->Set_CullingRadius(fCullingRadius);
                pLoadObject->Set_Instancing(true);
            }
        }
    }

    return S_OK;
}

void CClientLevel::Ready_ChangeLevel(LEVEL eNextLevel)
{
    m_pGameInstance->Stop(m_wstrBGMTag);

    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Player"));
    pPlayer->Set_Active(false);
    UIMGR->Inactive_UI("UI_PlayerDefault");

    m_pGameInstance->Get_MainCamera()->Set_Active(false);

    m_pGameInstance->Change_Level(CLevel_Loading::Create(eNextLevel, m_pDevice, m_pContext));
}
