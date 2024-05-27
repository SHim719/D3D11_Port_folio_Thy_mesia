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
	
	wstring Get_FileNameStem(const wstring& wstrFileFullPath)
	{
		size_t iOffset = wstrFileFullPath.rfind(L'\\') + 1;
		size_t iPathLength = wstrFileFullPath.length();
		return wstrFileFullPath.substr(iOffset, iPathLength - iOffset - 4);
	}
	

	wstring Get_Path(const wstring& wstrFileFullPath)
	{
		size_t iPathLength = wstrFileFullPath.rfind(L'\\') + 1;
		return wstrFileFullPath.substr(0, iPathLength);
	}

}
