#pragma once

#include "../Default/stdafx.h"
#include <process.h>
#include <filesystem>

#include "Client_Components.h"
#include "Client_Struct.h"
#include "Client_Functions.h"
#include "Client_Macro.h"

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
}

extern HWND				g_hWnd;

namespace fs = std::filesystem;

using namespace Client;