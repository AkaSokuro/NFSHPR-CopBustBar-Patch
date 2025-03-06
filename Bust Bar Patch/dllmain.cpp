#include <Windows.h>
#include <Psapi.h>
#include <process.h>
#include <cstdio>

DWORD WINAPI MainThread(LPVOID lpParam) {
    Sleep(1000);

    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);
    if (baseAddress == 0) {
        return 1;
    }

    uintptr_t addr1 = baseAddress + 0x1A644DD2;
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
        0xE8, 0x23, 0x98, 0x10, 0x1A, 0x90
    };

    bool patched = true;

    SIZE_T bytesWritten1;
    if (!WriteProcessMemory(GetCurrentProcess(), (LPVOID)addr1, patch1, sizeof(patch1), &bytesWritten1)) {
        patched = false;
        /* Debugging Only
        char errorMsg[256];
        sprintf_s(errorMsg, "Patch 1 failed! Error code: %lu", GetLastError());
        MessageBoxA(NULL, errorMsg, "Patch Error", MB_OK | MB_ICONERROR);
        */
    }

    SIZE_T bytesWritten2;
    if (!WriteProcessMemory(GetCurrentProcess(), (LPVOID)addr2, patch2, sizeof(patch2), &bytesWritten2)) {
        patched = false;
        /* Debugging Only
        char errorMsg[256];
        sprintf_s(errorMsg, "Patch 2 failed! Error code: %lu", GetLastError());
        MessageBoxA(NULL, errorMsg, "Patch Error", MB_OK | MB_ICONERROR);
        */
    }

    if (!patched) {
        MessageBoxA(NULL, "Failed to patch Cop Bust Bar. Try restarting the game.", "Patch Error", MB_OK);
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)MainThread, hModule, 0, NULL);
        if (hThread) CloseHandle(hThread);
    }
    return TRUE;
}
