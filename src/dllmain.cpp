#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <process.h>

namespace Cheat
{
    extern void Initialize(void* hDll);
}

static unsigned int WINAPI Start(void* handle)
{
    Cheat::Initialize(handle);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        ::DisableThreadLibraryCalls(hModule);
        
        HANDLE hThread = (HANDLE)_beginthreadex(nullptr, 0, Start, hModule, 0, nullptr);
        if (hThread == nullptr)
            return FALSE;

        ::CloseHandle(hThread);
    }

    return TRUE;
}