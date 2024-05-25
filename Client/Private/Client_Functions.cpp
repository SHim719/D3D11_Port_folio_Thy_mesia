#include "Client_Functions.h"


namespace Client
{
	wstring Convert_StrToWStr(const string& str)
	{
		return wstring().assign(str.begin(), str.end());
	}

	string Convert_WStrToStr(const wstring& wstr)
	{
		return string().assign(wstr.begin(), wstr.end());
	}

}
