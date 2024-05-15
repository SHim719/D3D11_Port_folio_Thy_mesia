#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)
#pragma warning (disable : 4828)
#pragma warning (disable : 4819)

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include <directxtk/DDSTextureLoader.h>
#include <directxtk/WICTextureLoader.h>

#include <DirectXMath.h>
using namespace DirectX;


#include <Assimp/scene.h>
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>


#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include <assert.h>
#include <iostream>
#include <ctime>
#include <random>
#include <unordered_map>
#include <functional>


#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif

using namespace Engine;
using namespace std;
