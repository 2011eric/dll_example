
#include <Windows.h>






BOOL APIENTRY DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		MessageBoxW(NULL, L"Hello From The Injected DLL", L"PROCESS_ATTACH", MB_OK | MB_ICONINFORMATION);
		break;

	case DLL_THREAD_ATTACH:
		MessageBoxW(NULL, L"Hello From The Injected DLL", L"THREAD_ATTACH", MB_OK | MB_ICONINFORMATION);
		break;

	case DLL_THREAD_DETACH:
		MessageBoxW(NULL, L"Hello Again From The Injected DLL", L"THREAD_DETACH", MB_OK | MB_ICONINFORMATION);
		break;

	case DLL_PROCESS_DETACH:
		MessageBoxW(NULL, L"Hello Again From The Injected DLL", L"PROCESS_DETACH", MB_OK | MB_ICONINFORMATION);
		break;
	}

	return TRUE;
}