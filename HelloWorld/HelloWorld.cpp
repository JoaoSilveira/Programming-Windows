#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR args, int cmdShow)
{
	MessageBox(nullptr, "Hello, World!", "Greeting Box", MB_OK);

	return 0;
}