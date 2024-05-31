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


	wstring Get_Path(const wstring& wstrFileFullPath)
	{
		size_t iPathLength = wstrFileFullPath.rfind(L'\\') + 1;
		return wstrFileFullPath.substr(0, iPathLength);
	}

	_matrix Get_LookTo(_fvector vNewLook)
	{
		_fvector		vLook = vNewLook;
		_fvector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
		_fvector		vUp = XMVector3Cross(vLook, vRight);

		return _matrix(
			vRight,
			vUp,
			vLook,
			XMVectorSet(0.f, 0.f, 0.f, 1.f)
		);
	}


}
