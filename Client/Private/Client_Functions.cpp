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

	POINT Get_ScreenCursorPos(HWND hWnd)
	{
		POINT ptMouse = {};
		GetCursorPos(&ptMouse);
		ScreenToClient(hWnd, &ptMouse);

		return ptMouse;
	}

	void Set_CursorToCenter(HWND hWnd, _uint iWinCX, _uint iWinCY)
	{
		POINT ptCenter = { (_long)iWinCX >> 1, (_long)iWinCY >> 1 };
		ClientToScreen(hWnd, &ptCenter);
		SetCursorPos(ptCenter.x, ptCenter.y);
	}
}

namespace JoMath
{
	_matrix LookTo(_fvector vTargetLook)
	{
		_fvector		vLook = vTargetLook;
		_fvector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
		_fvector		vUp = XMVector3Cross(vLook, vRight);

		return _matrix(
			vRight,
			vUp,
			vLook,
			XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}

	_matrix LookAt(_fvector vSrc, _fvector vDst)
	{
		_fvector		vLook = XMVector3Normalize(vDst - vSrc);
		_fvector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
		_fvector		vUp = XMVector3Cross(vLook, vRight);

		return _matrix(
			vRight,
			vUp,
			vLook,
			XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}

	_vector ToEulerAngle(_fvector vQuat, _bool bToDegree)
	{
		_float w = vQuat.m128_f32[3];
		_float x = vQuat.m128_f32[0];
		_float y = vQuat.m128_f32[1];
		_float z = vQuat.m128_f32[2];

		_float sqw = w * w;
		_float sqx = x * x;
		_float sqy = y * y;
		_float sqz = z * z;

		// rotation about x-axis
		_float pitch = asinf(2.0f * (w * x - y * z));
		// rotation about y-axis
		_float yaw = atan2f(2.0f * (x * z + w * y), (-sqx - sqy + sqz + sqw));
		// rotation about z-axis
		_float roll = atan2f(2.0f * (x * y + w * z), (-sqx + sqy - sqz + sqw));

		if (bToDegree)
		{
			pitch = XMConvertToDegrees(pitch);
			yaw = XMConvertToDegrees(yaw);
			roll = XMConvertToDegrees(roll);
		}

		return XMVectorSet(pitch, yaw, roll, 1.f);
	}
	_vector Slerp_TargetLook(_fvector vNowLook, _fvector vTargetLook, _float fRotRate)
	{
		_vector StartQuat = XMQuaternionRotationMatrix(JoMath::LookTo(vNowLook));
		_vector EndQuat = XMQuaternionRotationMatrix(JoMath::LookTo(vTargetLook));

		return XMQuaternionSlerp(StartQuat, EndQuat, fRotRate);
	}
	_vector Calc_GroundLook(_fvector vDst, _fvector vSrc)
	{
		_vector vGroundLook = XMVectorSetY(vDst - vSrc, 0.f);
		vGroundLook = XMVector3Normalize(vDst - vSrc);

		return vGroundLook;
	}

	_float Lerp(_float fA, _float fB, _float fRatio)
	{
		return fA * (1.f - fRatio) + fB * fRatio;
	}
}

namespace JoRandom
{
	_int Random_Int(_int _iMin, _int _iMax)
	{
		std::random_device RD;
		std::mt19937 gen(RD());

		std::uniform_int_distribution<_int> dis(_iMin, _iMax);

		return dis(gen);
	}

	_float Random_Float(_float _fMin, _float _fMax)
	{
		std::random_device RD;
		std::mt19937 gen(RD());

		std::uniform_real_distribution<_float> dis(_fMin, _fMax);

		return dis(gen);
	}


}


