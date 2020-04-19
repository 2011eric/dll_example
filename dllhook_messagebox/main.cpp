#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

BOOL APIENTRY DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		MessageBoxA(NULL, "Hello From The Injected DLL", "Injected !", MB_OK | MB_ICONINFORMATION);
		break;

	case DLL_THREAD_ATTACH:
		MessageBoxA(NULL, "Hello From The Injected DLL", "Injected !", MB_OK | MB_ICONINFORMATION);
		break;

	case DLL_THREAD_DETACH:
		MessageBoxA(NULL, "Hello Again From The Injected DLL", "Injected !", MB_OK | MB_ICONINFORMATION);
		break;

	case DLL_PROCESS_DETACH:
		MessageBoxA(NULL, "Hello Again From The Injected DLL", "Injected !", MB_OK | MB_ICONINFORMATION);
		break;
	}

	return TRUE;
}