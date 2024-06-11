#include "Client_Functions.h"

#include <codecvt>
#include <locale> 

namespace Client
{
	wstring Convert_StrToWStr(const string& str)
	{
		wstring_convert<codecvt_utf8<_tchar>> converter;
		return converter.from_bytes(str);
	}

	string Convert_WStrToStr(const wstring& wstr)
	{
		wstring_convert<codecvt_utf8<_tchar>> converter;
		return converter.to_bytes(wstr);
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

	_vector To_EulerAngle(_fvector vQuat)
	{
		_vector vEulerAngle;

		_float sinr_cosp = 2.f * (vQuat.m128_f32[3] * vQuat.m128_f32[0] + vQuat.m128_f32[1] * vQuat.m128_f32[2]);
		_float cosr_cosp = 1.f - 2.f * (vQuat.m128_f32[0] * vQuat.m128_f32[0] + vQuat.m128_f32[1] * vQuat.m128_f32[1]);
		vEulerAngle.m128_f32[0] = atan2(sinr_cosp, cosr_cosp);

		_float sinp = std::sqrt(1.f + 2.f * (vQuat.m128_f32[3] * vQuat.m128_f32[1] - vQuat.m128_f32[0] * vQuat.m128_f32[2]));
		_float cosp = std::sqrt(1.f - 2.f * (vQuat.m128_f32[3] * vQuat.m128_f32[1] - vQuat.m128_f32[0] * vQuat.m128_f32[2]));
		vEulerAngle.m128_f32[1] = 2.f * atan2(sinp, cosp) - 3.14159265f / 2.f;

		_float siny_cosp = 2.f * (vQuat.m128_f32[3] * vQuat.m128_f32[2] + vQuat.m128_f32[0] * vQuat.m128_f32[1]);
		_float cosy_cosp = 1.f - 2.f * (vQuat.m128_f32[1] * vQuat.m128_f32[1] + vQuat.m128_f32[2] * vQuat.m128_f32[2]);
		vEulerAngle.m128_f32[2] = atan2(siny_cosp, cosy_cosp);

		return vEulerAngle;
	}


}
