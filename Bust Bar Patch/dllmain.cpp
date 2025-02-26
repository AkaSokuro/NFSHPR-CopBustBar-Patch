#include <Windows.h>
#include <Psapi.h>

wchar_t processName[MAX_PATH];

DWORD WINAPI MainThread(LPVOID lpParam) {
    Sleep(1000);

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(processName);
    DWORD processID = GetCurrentProcessId();
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

    if (hProcess != NULL) {
        uintptr_t addr1 = baseAddress + 0x181247B;
        unsigned char patch1[] = {
            0x4D, 0x85, 0xC9, 0x74, 0x28, 0x49, 0x8B, 0x59,
            0x28, 0x39, 0xAB, 0xA0, 0x00, 0x00, 0x00, 0x75,
            0x1C, 0x41, 0x39, 0x6C, 0x85, 0xBC, 0x75, 0x15,
            0x8B, 0xC1, 0x49, 0x8B, 0x1C, 0xC2, 0xFE, 0xC0,
            0x38, 0xD0, 0x77, 0x09, 0x39, 0x6B, 0x34, 0x75,
            0xF1, 0x41, 0x89, 0x5D, 0xF8, 0xC3
        };

        uintptr_t addr2 = baseAddress + 0x53B5AA;
        unsigned char patch2[] = {
            0xE8, 0xCC, 0x6E, 0x2D, 0x01, 0x90
        };

        bool patched = true;

        SIZE_T bytesWritten1;
        if (!WriteProcessMemory(hProcess, (LPVOID)addr1, patch1, sizeof(patch1), &bytesWritten1)) {
            patched = false;
        }

        SIZE_T bytesWritten2;
        if (!WriteProcessMemory(hProcess, (LPVOID)addr2, patch2, sizeof(patch2), &bytesWritten2)) {
            patched = false;
        }

        if (!patched) {
            MessageBoxA(NULL, "Failed to patch Cop Bust Bar, this shouldn't happened. Try restart the game.", "Patch Error", MB_OK);
        }

        //MessageBoxA(NULL, "Patches completed", "Patched", MB_OK);
        CloseHandle(hProcess);
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        GetModuleBaseNameW(GetCurrentProcess(), NULL, processName, MAX_PATH);

        CreateThread(NULL, 0, MainThread, hModule, 0, NULL);
    }
    return TRUE;
}