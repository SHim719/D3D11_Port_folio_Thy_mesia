#pragma once

#include "Engine_Defines.h"

namespace Client
{
   wstring Convert_StrToWStr(const string& str);
   string Convert_WStrToStr(const wstring& wstr);
   wstring Get_Path(const wstring& wstrFileFullPath);

   _matrix Get_LookTo(_fvector vLook);

	   
   
};

