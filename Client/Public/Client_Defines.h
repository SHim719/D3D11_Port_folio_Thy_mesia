#pragma once

#include "../Default/stdafx.h"
#include <process.h>

#include "Client_Enums.h"
#include "Client_Components.h"
#include "Client_Struct.h"
#include "Client_Macro.h"
#include "Client_Functions.h"


//#define AnimTool
#define testplay


/* 클라이언트에서 제작하는 모든 클래스들이 공통적으로 자주 사용하는 정의들을 모아둔다. */
namespace Client
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, 
		LEVEL_LOADING,
		LEVEL_GAMEPLAY,
		LEVEL_TOOL,
		LEVEL_END };

	enum TAG
	{
		TAG_PLAYER = 0,
		TAG_PLAYER_WEAPON,
		TAG_ENEMY,
		TAG_ENEMY_WEAPON,
		TAG_END,
	};
}

extern HWND				g_hWnd;

using namespace Client;