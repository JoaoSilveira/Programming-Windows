#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

int CDECL MessageBoxF(TCHAR* caption, TCHAR* message, ...)
{
	TCHAR buffer[1024];
	va_list argList;

	va_start(argList, message);

	_vsntprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR), message, argList);

	va_end(argList);

	return MessageBox(nullptr, buffer, caption, MB_OK);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR args, int cmdShow)
{
	auto width = GetSystemMetrics(SM_CXSCREEN);
	auto height = GetSystemMetrics(SM_CYSCREEN);

	MessageBoxF(TEXT("Size Box"), TEXT("The size of the screen is %i pixels wide %i pixels high"), width, height);
	return 0;
}