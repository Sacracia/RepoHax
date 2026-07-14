#include "pch.h"

#include "cheat.h"
//#include <third_party/haxsdk/unity/hax_unity_uvm.h>

namespace Cheat
{
    extern void HookMonoRuntimeInvoke(); // game_hooks.cpp
    extern void HookWinProcs(); // win_hooks.cpp
    extern void HookGraphics(); // graphics_hooks.cpp
    extern void HookGameFuncs(); // game_hooks.cpp
    extern void HookPlayerLoop(); // game_hooks.cpp

    Context* G;

    static void SetupConfig(Hax::IniFile& ini);
    static void WaitForUnityToLoad();

    void Initialize(HMODULE hCheatDll)
    {
        if (!Hax::Unity::IsUnityProcess())
            return;

        G = new Context();
        G->Handle = hCheatDll;

        SetupConfig(G->Config);
        Hax::InitLogFile(G->Logger, L"haxsdk_logs.txt", G->UseConsole);

        WaitForUnityToLoad();
        Hax::Unity::Initialize(&G->Logger);

        HookWinProcs();

        HMODULE hDirectX11 = 0;
        do 
        { 
            Sleep(200); 
            hDirectX11 = ::GetModuleHandleW(L"d3d11.dll"); 
        } while (hDirectX11 == 0);

        HookGraphics();
        HookGameFuncs();

        while (!G->PlayerLoop.ReadyToHook) 
            Sleep(200);
        HookPlayerLoop();
    }

    template <int Default, int Min, int Max>
    static void IniFileRead_IntClamped(void* data, const char* str)
    {
        int value = Default;
        if (sscanf_s(str, "%d", &value) != 1)
            value = Default;

        *(int*)data = Hax::Clamp(value, Min, Max);
    }

    static void SetupConfig(Hax::IniFile& ini)
    {
        Hax::IniAddEntry(ini, "Settings",   "UseConsole",          &G->UseConsole,          Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Settings",   "VkOpenClose",         &G->VkOpenClose,         Hax::IniFileWrite_Int,  IniFileRead_IntClamped<VK_OEM_3, 1, 255>);
        Hax::IniAddEntry(ini, "Settings",   "Language",            &G->Language,            Hax::IniFileWrite_Int,  IniFileRead_IntClamped<0, 0, Lang_Count - 1>);
        Hax::IniAddEntry(ini, "Settings",   "DarkenBackground",    &G->DarkenBackground,    Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(ini, "Stats",      "Godmode",             &G->Godmode,             Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "InfiniteStamina",     &G->InfStamina,          Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "WalkingSpeed",        &G->Accel.Walking,       Hax::IniFileWrite_Int,  IniFileRead_IntClamped<1, 1, 5>);
        Hax::IniAddEntry(ini, "Stats",      "SprintingSpeed",      &G->Accel.Sprinting,     Hax::IniFileWrite_Int,  IniFileRead_IntClamped<1, 1, 5>);
        Hax::IniAddEntry(ini, "Stats",      "CrouchingSpeed",      &G->Accel.Crouching,     Hax::IniFileWrite_Int,  IniFileRead_IntClamped<1, 1, 5>);
        Hax::IniAddEntry(ini, "Stats",      "InfiniteJumps",       &G->InfJumps,            Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "NoTumble",            &G->NoTumble,            Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "EasyGrab",            &G->EasyGrab,            Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "UnlimitedGrabRange",  &G->UnlimGrabRange,      Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "NoOverCharge",        &G->NoOvercharge,        Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Stats",      "AutoUseUpgrades",     &G->AutoUseUpgrs,        Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        static constexpr const char* s_Names[(int)(UpgradeType::N)] = 
        {
            "CrouchRest", "ExtraJump", "Range", "SprintSpeed", "Stamina", "Strength",
            "Launch", "Wings", "Health", "PlayersCount", "Climb", "HeadBattery"
        };

        for (int i = 0; i < (int)UpgradeType::N; ++i)
            Hax::IniAddEntry(ini, "UpgradeAutoUse", s_Names[i], &G->UpgradesData[i].AutoUse, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(ini, "Entities",   "NoGrabMaxTime",       &G->NoGrabMaxTime,       Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Entities",   "EnemiesEsp",          &G->EnemiesEsp,          Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Entities",   "PlayersEsp",          &G->PlayersEsp,          Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Entities",   "PlayersChams",        &G->PlayersChams,        Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(ini, "Vision",     "BetterVision",        &G->BetterVision,        Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Vision",     "ThirdPerson",         &G->ThirdPerson,         Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Vision",     "Fov",                 &G->Fov,                 Hax::IniFileWrite_Int,  IniFileRead_IntClamped<60, 60, 140>);
        Hax::IniAddEntry(ini, "Vision",     "FlashlightIntensity", &G->Flashlight.Intensity,Hax::IniFileWrite_Int,  IniFileRead_IntClamped<10, 10, 20>);
        Hax::IniAddEntry(ini, "Vision",     "FlashAngle",          &G->Flashlight.Angle,    Hax::IniFileWrite_Int,  IniFileRead_IntClamped<60, 60, 120>);
        Hax::IniAddEntry(ini, "Vision",     "FlashInCrouch",       &G->Flashlight.InCrouch, Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Vision",     "MaxHeadBattery",      &G->MaxHeadBattery,      Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(ini, "Valuables",  "ValuablesEsp",        &G->ValuablesEsp,        Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Valuables",  "ValuablesEspRange",   &G->ValuablesEspRange,   Hax::IniFileWrite_Int,  IniFileRead_IntClamped<50, 5, 500>);
        Hax::IniAddEntry(ini, "Valuables",  "ValuablesChams",      &G->ValuablesChams,      Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Valuables",  "Unbreakable",         &G->Unbreakable,         Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Valuables",  "ExtrPointsEsp",       &G->ExtrPointsEsp,       Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(ini, "Items",      "InfBattery",          &G->InfBattery,          Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);
        Hax::IniAddEntry(ini, "Items",      "UseLaser",            &G->UseLaser,            Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniAddEntry(ini, "Misc",       "TruckEsp",            &G->TruckEsp,            Hax::IniFileWrite_Bool, Hax::IniFileRead_Bool);

        Hax::IniLoad(G->Config);
        G->Loc = g_LocDict[G->Language];
    }

    static void WaitForUnityToLoad()
    {
        Hax::Log(G->Logger, L"Waiting for unity virtual machine...");
        while (!Hax::Unity::GetUvmHandle())
            ::Sleep(200);

        HANDLE hEvent = ::CreateEvent(0, TRUE, FALSE, nullptr);
        HAX_PANIC(hEvent != nullptr, &G->Logger, L"%zu", HAX_LINE);

        G->UnityLoadedEvent = hEvent;
        HookMonoRuntimeInvoke();

        Hax::Log(G->Logger, L"Waiting for unity...");
        ::WaitForSingleObject(hEvent, INFINITE);
        ::CloseHandle(hEvent);
    }
}