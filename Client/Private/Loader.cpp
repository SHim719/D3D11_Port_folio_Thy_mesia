#include "Loader.h"

#include "GameInstance.h"

#include "Main_Camera.h"
#include "Player.h"

#include "Odur.h"
#include "Odur_Card.h"
#include "Odur_Card_Cutscene.h"

#include "Villager_F.h"
#include "Villager_M.h"
#include "Joker.h"
#include "HalberdKnight.h"
#include "TwinBladeKnight.h"

#include "Urd.h"
#include "Urd_Weapon.h"
#include "Urd_MagicCircle.h"
#include "Urd_InversionSphere.h"

#include "PerceptionBounding.h"

#include "MapObject.h"
#include "EventTrigger.h"
#include "Loader.h"

#include "UI_Headers.h"
#include "UI_Manager.h"

#include "Object_Headers.h"

#include "Effect_Manager.h"

#include "Effect_Group.h"
#include "Effect_Particle.h"
#include "Effect_Mesh.h"
#include "Effect_Trail.h"
#include "Effect_Hit.h"

#include "LockOnCurve.h"
#include "LightObject.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

unsigned int APIENTRY Loading_Main(void* pArg)
{
	CLoader* pLoader = (CLoader*)pArg;

	return pLoader->Loading();
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

unsigned int CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	CoInitializeEx(nullptr, 0);

	HRESULT hr = S_OK;

	switch (m_eNextLevelID)
	{
	case LEVEL_STAGE1:
		hr = Loading_Stage1();
		break;
	case LEVEL_ODUR:
		hr = Loading_StageOdur();
		break;
	case LEVEL_URD:
		hr = Loading_StageUrd();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return 1;

	return 0;

}

HRESULT CLoader::Loading_Default()
{
	if (FAILED(Ready_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Weapon()))
		return E_FAIL;

	if (FAILED(Ready_Player()))
		return E_FAIL;

	if (FAILED(Ready_UIResource()))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;
	
	if (FAILED(Ready_Effects()))
		return E_FAIL;

	if (FAILED(Ready_Etc()))
		return E_FAIL;

	if (FAILED(Ready_Sound(L"../../Resources/Sounds/BGM/")))
		return E_FAIL;

	if (FAILED(Ready_Sound(L"../../Resources/Sounds/SoundEffect/Corvus/")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_Stage1()
{
	lstrcpy(m_szLoadingText, TEXT("Loading..."));

	if (FAILED(Loading_Default()))
		return E_FAIL;

	if (FAILED(Ready_Villager_F()))
		return E_FAIL;

	if (FAILED(Ready_Villager_M()))
		return E_FAIL;

	if (FAILED(Ready_Joker()))
		return E_FAIL;
	
	if (FAILED(Ready_HalberdKnight()))
		return E_FAIL;

	if (FAILED(Ready_TwinBladeKnight()))
		return E_FAIL;

	if (FAILED(Ready_MapObjects(L"../../Resources/MapObjects/Stage1/")))
		return E_FAIL;
		
	m_pGameInstance->Add_Prototype(LEVEL_STAGE1, L"Prototype_Navigation", CNavigation::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/NaviData/Stage1_Navi.dat")));

	if (FAILED(Ready_Sound(L"../../Resources/Sounds/SoundEffect/Enemy/")))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Load Complete."));

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_StageOdur()
{
	lstrcpy(m_szLoadingText, TEXT("Loading..."));

	if (FAILED(Ready_Odur()))
		return E_FAIL;

	if (FAILED(Ready_MapObjects(L"../../Resources/MapObjects/StageOdur/")))
		return E_FAIL;

	m_pGameInstance->Add_Prototype(LEVEL_ODUR, L"Prototype_Navigation", CNavigation::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/NaviData/StageOdur_Navi.dat")));

	lstrcpy(m_szLoadingText, TEXT("Load Complete."));

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_StageUrd()
{
	lstrcpy(m_szLoadingText, TEXT("Loading..."));

	if (FAILED(Ready_Urd()))
		return E_FAIL;

	if (FAILED(Ready_MapObjects(L"../../Resources/MapObjects/StageUrd/")))
		return E_FAIL;

	m_pGameInstance->Add_Prototype(LEVEL_URD, L"Prototype_Navigation", CNavigation::Create(m_pDevice, m_pContext,
		TEXT("../../Resources/NaviData/StageUrd_Navi.dat")));

	lstrcpy(m_szLoadingText, TEXT("Load Complete."));

	m_bIsFinished = true;
	
	return S_OK;
}


CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

HRESULT CLoader::Ready_Camera()
{
	m_pGameInstance->Add_Prototype(L"Prototype_Main_Camera", CMain_Camera::Create(m_pDevice, m_pContext));

	return S_OK;
}

HRESULT CLoader::Ready_Weapon()
{
	m_pGameInstance->Add_Prototype(L"Prototype_Weapon", CWeapon::Create(m_pDevice, m_pContext));

	return S_OK;
}

HRESULT CLoader::Ready_Player()
{
	m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Model_Player", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Corvus/", "Corvus_No2.dat", "../../Resources/KeyFrame/Player/"));

	m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Model_Player_Dagger", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Corvus/", "Corvus_Dagger.dat"));

	m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Model_Player_Saber", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Corvus/", "Corvus_Saber.dat"));

	m_pGameInstance->Add_Prototype(L"Prototype_Player", CPlayer::Create(m_pDevice, m_pContext));

	return S_OK;
}

HRESULT CLoader::Ready_Odur()
{
	m_pGameInstance->Add_Prototype(LEVEL_ODUR, L"Prototype_Model_Odur", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Odur/", "Odur.dat", "../../Resources/KeyFrame/Odur/"));

	m_pGameInstance->Add_Prototype(LEVEL_ODUR, L"Prototype_Model_Odur_Cane", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Odur/", "Odur_Cane.dat"));

	m_pGameInstance->Add_Prototype(LEVEL_ODUR, L"Prototype_Model_Odur_Sword", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Odur/", "Odur_Sword.dat"));

	m_pGameInstance->Add_Prototype(LEVEL_ODUR, L"Prototype_Model_Odur_Card", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Odur/", "Odur_Card.dat"));

	m_pGameInstance->Add_Prototype(L"Prototype_Odur_Card", COdur_Card::Create(m_pDevice, m_pContext));

	m_pGameInstance->Add_Prototype(L"Prototype_Odur_Card_Cutscene", COdur_Card_Cutscene::Create(m_pDevice, m_pContext));

	m_pGameInstance->Add_Prototype(L"Prototype_Odur", COdur::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Sound(L"../../Resources/Sounds/SoundEffect/Odur/")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Villager_F()
{
	m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Model_Villager_F", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Villager_F/", "Villager_F.dat", "../../Resources/KeyFrame/Villager_F/"));

	m_pGameInstance->Add_Prototype(L"Prototype_Villager_F", CVillager_F::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Sound(L"../../Resources/Sounds/SoundEffect/Villager_F/")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Villager_M()
{
	m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Model_Villager_M", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Villager_M/", "Villager_M.dat", "../../Resources/KeyFrame/Villager_M/"));

	m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Model_Villager_M_Axe", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Villager_M/", "Villager_Axe.dat"));

	m_pGameInstance->Add_Prototype(L"Prototype_Villager_M", CVillager_M::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Sound(L"../../Resources/Sounds/SoundEffect/Villager_M/")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Joker()
{
	m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Model_Joker", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Joker/", "Joker.dat", "../../Resources/KeyFrame/Joker/"));

	m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Model_Joker_Hammer", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Joker/", "Joker_Hammer.dat"));

	m_pGameInstance->Add_Prototype(L"Prototype_Joker", CJoker::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Sound(L"../../Resources/Sounds/SoundEffect/Joker/")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_HalberdKnight()
{
	m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Model_HalberdKnight", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/HalberdKnight/", "HalberdKnight.dat", "../../Resources/KeyFrame/HalberdKnight/"));

	m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Model_Spear", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/HalberdKnight/", "Spear.dat"));

	m_pGameInstance->Add_Prototype(L"Prototype_HalberdKnight", CHalberdKnight::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Sound(L"../../Resources/Sounds/SoundEffect/Halberd_Knight/")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_TwinBladeKnight()
{
	m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Model_TwinBladeKnight", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/TwinBladeKnight/", "TwinBladeKnight.dat", "../../Resources/KeyFrame/TwinBladeKnight/"));

	m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Model_Sword", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/TwinBladeKnight/", "Sword.dat"));

	m_pGameInstance->Add_Prototype(L"Prototype_TwinBladeKnight", CTwinBladeKnight::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Sound(L"../../Resources/Sounds/SoundEffect/TwinBladeKnight/")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Urd()
{
	m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Model_Urd", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Urd/", "Urd.dat", "../../Resources/KeyFrame/Urd/"));

	m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Model_Urd_Sword", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Urd/", "SM_Urd_Sword.dat"));

	m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Model_Urd_Weapon_VFX", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Urd/", "UrdSword_VFX.dat"));

	m_pGameInstance->Add_Prototype(L"Prototype_Urd_Weapon", CUrd_Weapon::Create(m_pDevice, m_pContext));

	m_pGameInstance->Add_Prototype(L"Prototype_Urd_MagicCircle", CUrd_MagicCircle::Create(m_pDevice, m_pContext));

	m_pGameInstance->Add_Prototype(L"Prototype_Urd_InversionSphere", CUrd_InversionSphere::Create(m_pDevice, m_pContext));

	m_pGameInstance->Add_Prototype(L"Prototype_Urd", CUrd::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Sound(L"../../Resources/Sounds/SoundEffect/Urd/")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_MapObjects(const wstring& wstrPath)
{
	fs::path MapObjectPath(wstrPath);

	for (const fs::directory_entry& entry : fs::directory_iterator(MapObjectPath))
	{
		if (entry.is_directory())
			continue;

		fs::path fileName = entry.path().filename();
		fs::path fileTitle = fileName.stem();

		wstring wstrPrototypeTag = L"Prototype_Model_";
		wstrPrototypeTag += fileTitle.c_str();

		if (m_pGameInstance->Find_Prototype(LEVEL_STATIC, wstrPrototypeTag))
			continue;

		CModel* pModel = CModel::Create(m_pDevice, m_pContext, entry.path().parent_path().generic_string() + "/", entry.path().filename().generic_string());

		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, wstrPrototypeTag, pModel)))
			return E_FAIL;

		wstrPrototypeTag += L"_Instance";
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, wstrPrototypeTag, CModel_Instance::Create(m_pDevice, m_pContext, pModel))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Ready_UIResource()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_FadeScreen", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/FadeScreen/FadeScreen.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_LockOn", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Target.png"))))
		return E_FAIL;

#pragma region PlayerHpBar
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_PlayerHpBar_Right", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/PlayerBar/Player_HPBar_Right.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_PlayerHpBar_Left", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/PlayerBar/Player_HPBar_Left.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_PlayerHpBar_BG", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/PlayerBar/Player_HPBar_BG.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_PlayerHpBar_MainBar", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/PlayerBar/Player_HPBar_MainBar.dds"))))
		return E_FAIL;

#pragma endregion

#pragma region PlayerMpBar
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_PlayerMpBar_Right", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/PlayerBar/Player_MPBar_Right.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_PlayerMpBar_Left", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/PlayerBar/Player_MPBar_Left.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_PlayerMpBar_BG", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/PlayerBar/Player_MPBar_BG.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_PlayerMpBar_MainBar", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/PlayerBar/Player_MPBar_MainBar.dds"))))
		return E_FAIL;
#pragma endregion

#pragma region EnemyBar
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_EnemyBar_Right", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/EnemyBar/Enemy_HpBarHead_R.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_EnemyBar_Left", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/EnemyBar/Enemy_HpBarHead_L.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_EnemyBar_BG", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/EnemyBar/Enemy_HpBorder.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_EnemyBar_MainHpBar", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/EnemyBar/Enemy_HpMain.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_EnemyBar_StunnedSign", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/EnemyBar/Enemy_StunnedShine1.png"))))
		return E_FAIL;

#pragma endregion

#pragma region DamageFont

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_DamageFont", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/DamageFont/%d.png", 10))))
		return E_FAIL;
#pragma endregion 

#pragma region SkillSlot

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_PlunderSlot", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/SkillSlot/UI_PlunderSlot.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_SkillSlot", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/SkillSlot/UI_SkillSlot.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Slot_Selected", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/SkillSlot/TexUI_PW_ActiveSelected.dds"))))
		return E_FAIL;

#pragma endregion

#pragma region SkillIcon

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_SkillIcon_Axe", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/SkillIcon/TexUI_SkillIcon_Axe.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_SkillIcon_Hammer", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/SkillIcon/TexUI_SkillIcon_Hammer.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_SkillIcon_TwinBlade", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/SkillIcon/TexUI_SkillIcon_TwinSword.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_SkillIcon_Spear", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/SkillIcon/TexUI_SkillIcon_Halberd.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_SkillIcon_Claw_Long", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/SkillIcon/TexUI_TalentIcon_Claw_Basic.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_SkillIcon_Claw_Short", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/SkillIcon/TexUI_TalentIcon_Claw_Fast.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Slot_Inactive", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/SkillSlot/TexUI_PW_Inactive.png"))))
		return E_FAIL;

#pragma endregion


#pragma region LandingMessage

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_BeaconFound", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/LandingMessages/TexUI_LandingMessage_BeconFound.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_LandingScreen_BG", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/LandingMessages/TexUI_LandingScreenBackground.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_NeedKey", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/LandingMessages/Message_NeedKey.dds"))))
		return E_FAIL;

#pragma endregion

#pragma region MainMenu

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_MainMenu_Left", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Menu/TexUI_EvolveLeftBackground.png"))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_MainMenu_Highlighted", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Menu/TexUI_HighlightBar.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Font_Thymesia", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Menu/Font/Thymesia.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Font_LevelUp", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Menu/Font/LevelUp.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Font_PW", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Menu/Font/PlagueWeapon.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Font_Resume", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Menu/Font/Resume.png"))))
		return E_FAIL;

#pragma endregion


#pragma region SoulBar

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_MemoryBG", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/SoulBar/TexUI_BlackGradientBar.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_MemoryIcon", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/SoulBar/TexUI_MemoryShardsIcon.png"))))
		return E_FAIL;

#pragma endregion

#pragma region Status

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Window_Status", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Menu_Status/Window_Status.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Arrow_Left", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Menu_Status/Arrow_Left.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Arrow_Right", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Menu_Status/Arrow_Right.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Status_Number", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Menu_Status/Number/%d.png", 10))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Stat_Arrows", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Menu_Status/Stat_Arrows.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Select_Highlighted", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Menu_Status/HighLight.png"))))
		return E_FAIL;

#pragma endregion

#pragma region PW_Menu

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Window_PW", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Menu_PW/Window_PW.png"))))
		return E_FAIL;

#pragma endregion 

#pragma region ObtainUI

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Popup_Axe", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Popup/Popup_Axe.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Popup_Hammer", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Popup/Popup_Hammer.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Popup_Spear", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Popup/Popup_Spear.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Popup_TwinBlade", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Popup/Popup_TwinBlade.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Popup_Key", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Popup/Popup_Key.png"))))
		return E_FAIL;

#pragma endregion

#pragma region PotionSlot

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_PotionSlot", CTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Potion/UI_PotionSlot.dds"))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLoader::Ready_UI()
{
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_FadeScreen", CFadeScreen::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_ProgressBar", CProgressBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_SkillIcon", CUI_SkillIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_EnemyBar", CUI_EnemyBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_DamageFont", CUI_DamageFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Message_NeedKey", CUI_NeedKey::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Effects()
{
	if (FAILED(Ready_EffectResources()))
		return E_FAIL;

	if (FAILED(Ready_EffectGroup()))
		return E_FAIL;

	if (FAILED(Ready_EffectTrail()))
		return E_FAIL;

	if (FAILED(EFFECTMGR->Initialize()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Effect_Hit", CEffect_Hit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;

}

HRESULT CLoader::Ready_EffectResources()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_BaseColor", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/Effect/Diffuse/%d.png", 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Masking", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/Effect/Mask/Masking/%d.dds", 41))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Noise", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/Effect/Noise/%d.dds", 17))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Dissolve", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/Effect/DissolvePattern.png", 1))))
		return E_FAIL;


	fs::path EffectMeshesPath(L"../../Resources/Effect/EffectMesh/");

	for (const fs::directory_entry& entry : fs::directory_iterator(EffectMeshesPath))
	{
		if (entry.is_directory())
			continue;

		fs::path fileName = entry.path().filename();
		fs::path fileTitle = fileName.stem();

		if (fileName.extension().generic_string() == ".ini")
			continue;

		wstring wstrPrototypeTag = L"Prototype_Model_";
		wstrPrototypeTag += fileTitle.c_str();

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, wstrPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().parent_path().generic_string() + "/"
			, entry.path().filename().generic_string()))))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLoader::Ready_EffectGroup()
{
	fs::path EffectDataPath(L"../../Resources/Effect/EffectData/");

	for (const fs::directory_entry& entry : fs::directory_iterator(EffectDataPath))
	{
		if (entry.is_directory())
			continue;

		fs::path fileName = entry.path().filename();
		fs::path fileTitle = fileName.stem();

		if (fileName.extension().generic_string() == ".ini")
			continue;

		ifstream fin(entry.path());

		wstring wstrEffectGroupTag = L"Prototype_Effect_" + fileTitle.generic_wstring();
		if (FAILED(m_pGameInstance->Add_Prototype(wstrEffectGroupTag, CEffect_Group::Create(m_pDevice, m_pContext, fin))))
			assert(false);
	}
	
	return S_OK;
}

HRESULT CLoader::Ready_EffectTrail()
{
	fs::path EffectTrailPath(L"../../Resources/Effect/EffectTrails/");

	for (const fs::directory_entry& entry : fs::directory_iterator(EffectTrailPath))
	{
		if (entry.is_directory())
			continue;

		fs::path fileName = entry.path().filename();
		fs::path fileTitle = fileName.stem();

		if (fileName.extension().generic_string() == ".ini")
			continue;

		ifstream fin(entry.path());

		_uint iNumEffects = 0;
		fin.read((_char*)&iNumEffects, sizeof(_uint));
		if (iNumEffects > 1)
			assert(false);

		CGameEffect::EFFECTTYPE eEffectType;
		fin.read((_char*)&eEffectType, sizeof(CGameEffect::EFFECTTYPE));
		if (eEffectType != CGameEffect::TRAIL)
			assert(false);
		
		wstring wstrTrailTag = L"Prototype_Trail_" + fileTitle.generic_wstring();
		if (FAILED(m_pGameInstance->Add_Prototype(wstrTrailTag, CEffect_Trail::Create(m_pDevice, m_pContext, fin))))
			assert(false);
	}

	return S_OK;
}

HRESULT CLoader::Ready_Etc()
{
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_PerceptionBounding", CPerceptionBounding::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_EventTrigger", CEventTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Ladder_Down", CLadder_Down::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Ladder_Up", CLadder_Up::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Jail_Door", CDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Archive_Chair", CArchive_Chair::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Aisemy", CAisemy::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_LockOnCurve", CLockOnCurve::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_LightObject", CLightObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(Ready_Sound(L"../../Resources/Sounds/SoundEffect/Etc/")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Sound(const wstring& wstrPath)
{
	fs::path SoundPath(wstrPath);

	for (const fs::directory_entry& entry : fs::directory_iterator(SoundPath))
	{
		fs::path fileName = entry.path().filename();
		fs::path fileTitle = fileName.stem();

		if (FAILED(m_pGameInstance->Create_Sound(entry.path().generic_string().c_str(), fileTitle.generic_wstring())))
			return E_FAIL;

	}

	return S_OK;
}


