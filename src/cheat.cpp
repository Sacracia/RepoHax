#include "cheat.h"

#include "win_hooks.h"
#include "game_hooks.h"

namespace Cheat
{
    static void* Hooked_MonoRuntimeInvoke(UVM::Method* a1, void* a2, void* a3, void* a4);

    static void IniFileRead_ValuablesEspDistance(void* data, const char* str)
    {
        int value = 50;
        if (sscanf_s(str, "%d", &value) != 1)
            value = 50;

        *(int*)data = Hax::Clamp(value, 5, 500);
    }

    static void RegisterConfig(Hax::IniFile& iniFile)
    {
        Hax::IniAddEntry(iniFile, "Cheat", "UseConsole",  &GCheat->UseConsole,  Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Cheat", "VkOpenClose", &GCheat->VkOpenClose, Hax::IniFileWrite_Int,  Hax::IniFileRead_Int);
        Hax::IniAddEntry(iniFile, "Cheat", "Language", &GCheat->Lang, Hax::IniFileWrite_Int,  Hax::IniFileRead_Int);
        Hax::IniAddEntry(iniFile, "Cheat", "DarkenBackground", &GCheat->DarkenBg, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(iniFile, "Stats", "Godmode", &GCheat->Godmode, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Stats", "InfiniteStamina", &GCheat->InfStamina, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Stats", "WalkingSpeed", &GCheat->Acceleration.Walking, Hax::IniFileWrite_Int, Hax::IniFileRead_Int);
        Hax::IniAddEntry(iniFile, "Stats", "SprintingSpeed", &GCheat->Acceleration.Sprinting, Hax::IniFileWrite_Int, Hax::IniFileRead_Int);
        Hax::IniAddEntry(iniFile, "Stats", "CrouchingSpeed", &GCheat->Acceleration.Crouching, Hax::IniFileWrite_Int, Hax::IniFileRead_Int);
        Hax::IniAddEntry(iniFile, "Stats", "InfiniteJumps", &GCheat->InfJumps, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Stats", "NoTumble", &GCheat->NoTumble, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Stats", "EasyGrab", &GCheat->EasyGrab, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Stats", "UnlimitedGrabRange", &GCheat->UnlimGrabRange, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Stats", "NoOverCharge", &GCheat->NoOverCharge, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Stats", "AutoUseUpgrades", &GCheat->AutoUseUpgr, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(iniFile, "Entities", "NoGrabMaxTime", &GCheat->NoGrabMaxTime, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Entities", "EnemiesEsp", &GCheat->EnemiesEsp, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Entities", "PlayersEsp", &GCheat->PlayersEsp, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Entities", "PlayerChams", &GCheat->PlayerChams, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(iniFile, "Vision", "ImproveVision", &GCheat->ImproveVision, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Vision", "ThirdPerson", &GCheat->ThirdPerson, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Vision", "FOV", &GCheat->FOV, Hax::IniFileWrite_Int, Hax::IniFileRead_Int);
        Hax::IniAddEntry(iniFile, "Vision", "FlashlightIntensity", &GCheat->Flashlight.Intensity, Hax::IniFileWrite_Int, Hax::IniFileRead_Int);
        Hax::IniAddEntry(iniFile, "Vision", "FlashlightAngle", &GCheat->Flashlight.Angle, Hax::IniFileWrite_Int, Hax::IniFileRead_Int);
        Hax::IniAddEntry(iniFile, "Vision", "FlashlightAllowInCrouch", &GCheat->Flashlight.AllowInCrouch, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Vision", "HeadMaxBattery", &GCheat->HeadMaxBattery, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(iniFile, "Valuables", "ValuablesEsp", &GCheat->ValuablesEsp, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Valuables", "ValuablesEspDistance", &GCheat->ValuablesEspDistance, Hax::IniFileWrite_Int, IniFileRead_ValuablesEspDistance);
        Hax::IniAddEntry(iniFile, "Valuables", "ItemsChams", &GCheat->ItemsChams, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Valuables", "Unbreakable", &GCheat->Unbreakable, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Valuables", "ExtractionPointsEsp", &GCheat->ExtrPointsEsp, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(iniFile, "Items", "InfBattery", &GCheat->InfBattery, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(iniFile, "Items", "UseLaser", &GCheat->UseLaser, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(iniFile, "Misc", "TruckEsp", &GCheat->TruckEsp, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Visuals::RegisterConfig(iniFile);
    }

    bool Initialize(void* hCheat)
    {
        if (!Hax::Unity::IsUnityProcess())
            return false;

        GCheat = Hax::New<Context>();
        GCheat->hCheat = (HMODULE)hCheat;

        Hax::IniFile& iniFile = GCheat->IniFile;
        RegisterConfig(iniFile);
        Hax::IniLoad(iniFile);

        Hax::LogFile& logFile = GCheat->LogFile;
        Hax::InitLogFile(logFile, L"haxsdk_logs.txt", GCheat->UseConsole);

        Hax::Log(logFile, L"Waiting for unity virtual machine...");
        while (!Hax::Unity::GetUvmHandle())
            Sleep(200);

        {
            HANDLE hEvent = ::CreateEvent(0, TRUE, FALSE, nullptr);
            HAX_ASSERT(hEvent != nullptr);

            // To verify the unity is fully loaded, I wait for the first Update method to be called
            GCheat->UnityLoadedEvent = hEvent;
            HookModuleProc((HMODULE)Hax::Unity::GetUvmHandle(), "mono_runtime_invoke", Hooked_MonoRuntimeInvoke, GCheat->MonoRuntimeInvokeHook);

            Hax::Log(logFile, L"Waiting for unity...");
            ::WaitForSingleObject(hEvent, INFINITE);
            ::CloseHandle(hEvent);

            GCheat->MonoRuntimeInvokeHook.reset();
            GCheat->UnityLoadedEvent = nullptr;
        }

        Hax::Unity::Initialize(&logFile);
        Cheat::WinHooks::Install();

        HMODULE hDirectX11 = 0;
        do { Sleep(200); hDirectX11 = ::GetModuleHandleW(L"d3d11.dll"); } while (hDirectX11 == 0);

        Cheat::Visuals::InitializeMenu((Hax::Handle)hDirectX11);
        Cheat::GameHooks::Install();

        return true;
    }

    void Hook(void* ptr, void* detour, SafetyHookInline& out, const char* name)
    {
        auto res = SafetyHookInline::create(ptr, detour);
        HAX_PANIC(res.has_value(), &GCheat->LogFile, L"Failed to hooks %hs. Error %d", name, (int)res.error().type);
        out = std::move(*res);
    }

    void HookModuleProc(HMODULE module, LPCSTR procName, void* procHook, SafetyHookInline& out)
    {
        void* procPtr = ::GetProcAddress(module, procName);
        HAX_PANIC(procPtr != nullptr, &GCheat->LogFile, L"Proc %s not found", procName);

        Hook(procPtr, procHook, out, procName);
    }

    static void* Hooked_MonoRuntimeInvoke(UVM::Method* a1, void* a2, void* a3, void* a4)
    {
        void* ret = GCheat->MonoRuntimeInvokeHook.unsafe_fastcall<void*, void*, void*, void*, void*>(a1, a2, a3, a4);

        Hax::StringView methodName = UVM::MethodGetName(*a1);
        if (methodName == "Update")
            ::SetEvent(GCheat->UnityLoadedEvent);

        return ret;
    }
}
