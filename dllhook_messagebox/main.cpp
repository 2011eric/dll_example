#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

#define _T(X) L ## X
#define jmp 0xe9
#define CODE_LENGTH 5
BYTE oldCode[CODE_LENGTH];
BYTE newCode[CODE_LENGTH];

HANDLE hProcess;
HINSTANCE hInst;



typedef BOOL(WINAPI *ptrReadFile)(
	HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfByteToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
	);
ptrReadFile originReadFile;
void hookOff();
void hookOn();
void GetAdr();

BOOL WINAPI hookedReadFile(
	HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfByteToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
) {
	MessageBoxA(NULL, "HOOKED", "TEST", MB_OK);
	hookOff();
	char *line = new char(nNumberOfByteToRead);
	BOOL ret = ReadFile(hFile, line, nNumberOfByteToRead, lpNumberOfBytesRead, lpOverlapped);
	memcpy(lpBuffer, line, *lpNumberOfBytesRead);
	MessageBoxA(NULL, line, "TEST", MB_OK);
	return ret;

}

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

	VirtualProtectEx(hProcess, originReadFile, CODE_LENGTH, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, originReadFile, newCode, CODE_LENGTH, &writedByte);
	if (writedByte == 0)return;

	VirtualProtectEx(hProcess, originReadFile, CODE_LENGTH, dwOldProtect, &dwTmp);
}

void hookOff() {
	if (hProcess == NULL)return;
	DWORD dwTmp;
	DWORD dwOldProtect;
	SIZE_T writedByte;

	VirtualProtectEx(hProcess, originReadFile, CODE_LENGTH, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, originReadFile, oldCode, CODE_LENGTH, &writedByte);


	VirtualProtectEx(hProcess, originReadFile, CODE_LENGTH, dwOldProtect, &dwTmp);
}

void GetAdr() {
	HMODULE hModule = LoadLibrary("user32.dll");
	if (hModule == NULL) {
		return;
	}
	originReadFile = (ptrReadFile)GetProcAddress(hModule, "ReadFile");
	if (originReadFile == NULL) {
		return;
	}
	memcpy(oldCode, originReadFile, 5);
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
		lea eax, hookedReadFile
		mov ebx, originReadFile
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
		//MessageBoxW(NULL, L"Hello From The Injected DLL", L"THREAD_ATTACH", MB_OK | MB_ICONINFORMATION);
		break;

	case DLL_THREAD_DETACH:
		//MessageBoxW(NULL, L"Hello Again From The Injected DLL", L"THREAD_DETACH", MB_OK | MB_ICONINFORMATION);
		break;

	case DLL_PROCESS_DETACH:
		MessageBoxW(NULL, L"Hello Again From The Injected DLL", L"PROCESS_DETACH", MB_OK | MB_ICONINFORMATION);
		break;
	}

	return TRUE;
}