#pragma once

#include "Engine_Defines.h"

namespace Client
{
   wstring Convert_StrToWStr(const string& str);
   string Convert_WStrToStr(const wstring& wstr);
   wstring Get_Path(const wstring& wstrFileFullPath);

   POINT Get_ScreenCursorPos(HWND hWnd);
   void  Set_CursorToCenter(HWND hWnd, _uint iWinCX, _uint iWinCY);
};

namespace JoMath
{
	_matrix LookTo(_fvector vTargetLook);
	_matrix LookAt(_fvector vSrc, _fvector vDst);

	_vector ToEulerAngle(_fvector vQuat, _bool bToDegree = true);

	_vector Slerp_TargetLook(_fvector vNowLook, _fvector vTargetLook, _float fRotRate);
	_vector Calc_GroundLook(_fvector vSrc, _fvector vDst);

	_float Lerp(_float fA, _float fB, _float fRatio);
}

namespace JoRandom
{
	_int Random_Int(_int _iMin, _int _iMax);
	_float Random_Float(_float _fMin, _float _fMax);
}

