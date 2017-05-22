#include "common.h"
#include <Windows.h>



char Keys::kp[256] = { 0 };
int Keys::key(int iKey)
{
	return (GetAsyncKeyState(iKey) >> 15) & 1;
}

int Keys::onekey(int iKey)
{
	if (key(iKey) && !kp[iKey]) { kp[iKey] = 1; return 1; }
	if (!key(iKey))kp[iKey] = 0;
	return 0;
}
