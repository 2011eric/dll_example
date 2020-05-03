#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

#define _T(X) L ## X
#define jmp 0xe9
#define CODE_LENGTH 5
BYTE oldCode[CODE_LENGTH];
BYTE newCode[CODE_LENGTH];

HANDLE hProcess;
HINSTANCE hInst;


typedef int(WINAPI *ptrMessageBoxW)(
	HWND    hWnd,
	LPCWSTR lpText,
	LPCWSTR lpCaption,
	UINT    uType
	);
ptrMessageBoxW originMsgBox;


void hookOff();
void hookOn();
void GetAdr();

int WINAPI hookedMessageBoxW(
	HWND    hWnd,
	LPCWSTR lpText,
	LPCWSTR lpCaption,
	UINT    uType
) {
	hookOff();
	int ret = MessageBoxW(hWnd, _T("Hooked"), lpCaption, uType);
	hookOn();
	return ret;
};
void debugPrivilege() {
	HANDLE hToken;
	bool bRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
	if (bRet) {
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
	}
}

void hookOn() {
	if (hProcess == NULL)return;
	DWORD dwTmp;
	DWORD dwOldProtect;
	SIZE_T writedByte;

	VirtualProtectEx(hProcess, originMsgBox, CODE_LENGTH, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, originMsgBox, newCode, CODE_LENGTH, &writedByte);
	if (writedByte == 0)return;

	VirtualProtectEx(hProcess, originMsgBox, CODE_LENGTH, dwOldProtect, &dwTmp);
}

void hookOff() {
	if (hProcess == NULL)return;
	DWORD dwTmp;
	DWORD dwOldProtect;
	SIZE_T writedByte;

	VirtualProtectEx(hProcess, originMsgBox, CODE_LENGTH, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, originMsgBox, oldCode, CODE_LENGTH, &writedByte);


	VirtualProtectEx(hProcess, originMsgBox, CODE_LENGTH, dwOldProtect, &dwTmp);
}

void GetAdr() {
	HMODULE hModule = LoadLibrary("user32.dll");
	if (hModule == NULL) {
		return;
	}
	originMsgBox = (ptrMessageBoxW)GetProcAddress(hModule, "MessageBoxW");
	if (originMsgBox == NULL) {
		return;
	}
	memcpy(oldCode, originMsgBox, 5);
	/*_asm {
	mov esi, originMsgBox
	lea edi, oldCode
	cld
	movsd
	movsb

	}*/
	newCode[0] = jmp;
	_asm
	{
		lea eax, hookedMessageBoxW
		mov ebx, originMsgBox
		sub eax, ebx
		sub eax, CODE_LENGTH
		mov dword ptr[newCode + 1], eax
	}/*
	 jmp dest
	 dest = myAddress - originAddress - 5
	 */

	hookOn();

}



BOOL APIENTRY DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		debugPrivilege();
		hProcess = GetCurrentProcess();
		GetAdr();
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