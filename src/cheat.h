#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <dxgi.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <shared_mutex>

#include "haxsdk/unity/hax_unity.h"
#include "safetyhook/safetyhook.hpp"

#include "game_classes.h"
#include "visuals.h"
#include "resource.h"

using present_t = HRESULT (__stdcall*)(IDXGISwapChain*,UINT,UINT);
using resize_buffers_t = HRESULT (__stdcall*)(IDXGISwapChain*,UINT,UINT,UINT,DXGI_FORMAT,UINT);

namespace Cheat
{
    void Initialize(void* hCheat);
    void Hook(void* ptr, void* detour, SafetyHookInline& out, const char* name);
    void HookModuleProc(HMODULE module, LPCSTR procName, void* procHook, SafetyHookInline& out);

    struct LevelBan
    {
        Hax::WStringView Name;
        int Index;
        bool Allowed = true;
    };

    enum class ChatPref
    {
        None = -1,
        Alive = 0,
        Dead = 1
    };

    struct Context
    {
        SafetyHookInline MonoRuntimeInvokeHook;
        SafetyHookInline LoadLibraryAHook;
        SafetyHookInline LoadLibraryWHook;
        //SafetyHookInline PresentHook;
       // SafetyHookInline ResizeBuffersHook;
        present_t PresentOrig;
        resize_buffers_t ResizeBuffersOrig;

        SafetyHookInline SetCursorHook;
        SafetyHookInline GetRawInputDataHook;
        SafetyHookInline GetCursorPosHook;
        SafetyHookInline ShowCursorHook;
        SafetyHookInline ClipCursorHook;
        SafetyHookInline SetCursorPosHook;
        SafetyHookInline GetAsyncKeyStateHook;
        SafetyHookInline GetKeyStateHook;
        SafetyHookInline GetKeyboardStateHook;
        SafetyHookInline PeekMessageAHook;
        SafetyHookInline PeekMessageWHook;
        SafetyHookInline GetMessageAHook;
        SafetyHookInline GetMessageWHook;
        SafetyHookInline TerminateProcessHook;

        SafetyHookInline Application_Quit_Hook;
        SafetyHookInline EventSystem_Update_Hook;
        SafetyHookInline PlayerHealth_Hurt_Hook;
        SafetyHookInline PlayerAvatar_PlayerDeath_Hook;
        SafetyHookInline PlayerController_FixedUpdate_Hook;
        SafetyHookInline PlayerController_Update_Hook;
        SafetyHookInline PlayerTumble_TumbleRequest_Hook;
        SafetyHookInline FlashlightController_Update_Hook;
        SafetyHookInline ItemUpgrade_Update_Hook;
        SafetyHookInline ItemHealthPack_Update_Hook;
        SafetyHookInline UpdateLoop_Hook;
        SafetyHookInline PostLateUpdateLoop_Hook;
        SafetyHookInline PresentAfterDrawLoop_Hook;
        SafetyHookInline PhysGrabObject_PhysicsGrabbingManipulation_Hook;
        SafetyHookInline PostProcessLayer_BuildCommandBuffers_Hook;
        SafetyHookInline PlayerAvatarVisuals_Update_Hook;
        SafetyHookInline PhysGrabber_PhysGrabLogic_Hook;
        SafetyHookInline PhysGrabber_RayCheck_Hook;
        SafetyHookInline Physics_Raycast_Hook;
        SafetyHookInline PhysGrabObjectImpactDetector_Break_Hook;
        SafetyHookInline PhysGrabObjectImpactDetector_DestroyObject_Hook;
        SafetyHookInline PhotonNetwork_IsMasterClient_Hook;
        SafetyHookInline ItemBattery_Update_Hook;
        SafetyHookInline Camera_FireOnPreRender_Hook;
        SafetyHookInline ItemGun_Update_Hook;
        SafetyHookInline RunManager_SetRunLevel_Hook;
        SafetyHookInline PhysGrabber_PhysGrabOverCharge_Hook;
        SafetyHookInline SpectateCamera_HeadEnergyLogic_Hook;
        SafetyHookInline EnemyRigidbody_FixedUpdate_Hook;

        ID3D11RenderTargetView* RenderTarget;
        ID3D11Device* Device;
        ID3D11DeviceContext* DeviceContext;
        HWND hWnd;

        Hax::LogFile LogFile;
        Hax::IniFile IniFile = L"haxsdk.ini";

        std::shared_mutex ShutdownMutex;
        std::shared_mutex RuntimeInvokeMutex;

        HANDLE UnityLoadedEvent;
        HMODULE hCheat;

        bool UseConsole;
        bool GraphicsHooked;
        int VkOpenClose = VK_OEM_3;

        Hax::Optional<POINT> LockedCursorPos;
        bool ForceCursorVisible;
        bool OrigCursorVisible;
        bool ForceCursorUnclip;
        bool OrigCursorUnclip;
        bool GameInputPrevented;
        bool ForceCursorTex;
        Hax::Optional<RECT> OrigClipArea;
        HCURSOR OrigMouseTexture;

        Hax::Gui::FontHandle NunitoSans_SemiBold, NunitoSans_Bold, NunitoSans_ExtraBold, Icons_Solid;
        size_t KeyListenerId;

        bool MenuVisible;

        Visuals::Language Lang;

        bool                                Godmode;
        bool                                HealToMax;
        bool                                InfStamina;
        struct 
        {
            int                             Walking = 1;
            int                             Sprinting = 1;
            int                             Crouching = 1;
        } Acceleration;
        bool                                InfJumps;
        bool                                NoTumble;
        struct
        {
            int                             Intensity = 10;
            int                             Angle = 60;
            bool                            AllowInCrouch;
        } Flashlight;
        struct
        {
            bool                            Initialized;
            bool                            Hooked;
            void*                           UpdatePtr;
            SafetyHookInline                UpdateHook;
            void*                           PostLateUpdatePtr;
            SafetyHookInline                PostLateUpdateHook;
            void*                           PresentAfterDrawPtr;
            SafetyHookInline                PresentAfterDrawHook;
        } LoopSystem;
        struct
        {
            Hax::TripleBuffer<ItemUpgrade>  Pool{24};
            ItemUpgrade                     ItemToToggle;
        } Upgrades;
        struct
        {
            Hax::TripleBuffer<ItemHealthPack> Pool{24};
            ItemHealthPack                   ItemToToggle;
        } Aids;
        bool                                EasyGrab;
        bool                                UnlimGrabRange;
        bool                                NoOverCharge;
        bool AutoUseUpgr;

        bool ImproveVision;
        int FOV = 60;
        bool ThirdPerson;

        bool IsClient;
        Hax::Map<Hax::WStringView, EnemySetup> EnemiesPool;
        size_t SelectedEnemySetup;

        EnemySetup EnemyToSpawn;
        bool Blind;
        bool NoGrabMaxTime;

        Hax::TripleBuffer<Visuals::EnemyEspData> EnemiesEspBuffer{20};
        bool EnemiesEsp;

        bool Unbreakable;
        bool InfBattery;

        bool ValuablesEsp;
        int ValuablesEspDistance = 50;
        Hax::TripleBuffer<Visuals::ValuableEspData> ValuablesEspBuffer{50};

        UnityEngine::AssetBundle Bundle;
        UnityEngine::CommandBuffer CommandBuffer;
        UnityEngine::Material ItemsChamsMat;
        bool ItemsChams;
        bool UseLaser;

        bool SetToZero;
        bool SetToMax;

        Hax::Map<Hax::WStringView, int> ItemsPool;
        Item ItemToSpawn;

        Hax::Vector<LevelBan> LevelsBan;
        bool ForceNextLevel;
        bool ActivateNextPoint;
        bool TeleportToTruck;

        Hax::TripleBuffer<Visuals::ExtrPointEspData> ExtrPointsEspBuffer{10};
        bool ExtrPointsEsp; 

        Hax::TripleBuffer<Visuals::TruckEspData> TruckEspBuffer{2};
        bool TruckEsp; 

        Hax::TripleBuffer<Visuals::PlayerEspData> PlayersEspBuffer{6};
        bool PlayersEsp;

        PlayerAvatar ToTumble;
        PlayerAvatar ToRevive;
        PlayerAvatar ToKill;

        bool PlayerChams;
        UnityEngine::Material PlayerChamsMat;

        float PixelHeight;
        float PixelWidth;
        float ScreenHeight;
        float ScreenWidth;

        UnityEngine::Material HeadChamsMat;

        ChatPref PreferedChat = ChatPref::None;

        bool HeadMaxBattery;
        bool IsInGame;
        bool DarkenBg;

        size_t TotalBans;

        bool UnlockAllCosmetic;
        bool ResetAllCosmetic;

        UVM::Thread* UvmThread;
    };

    inline Context* GCheat;
}
