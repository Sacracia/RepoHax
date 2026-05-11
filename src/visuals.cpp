#include "visuals.h"

#include "cheat.h"
#include "widgets.h"
#include "game_classes.h"
#include "game_hooks.h"

namespace Cheat
{
    extern void Hook(void* ptr, void* detour, SafetyHookInline& out, const char* name);
}

namespace Cheat::Visuals
{
    #define LOC_LIST(X) \
    X(LocKey_Stats, L"Stats", L"Статы") \
    X(LocKey_Entities, L"Entities", L"Существа") \
    X(LocKey_Vision, L"Vision", L"Зрение") \
    X(LocKey_Valuables, L"Valuables", L"Ценности") \
    X(LocKey_Items, L"Items", L"Предметы") \
    X(LocKey_Level, L"Level", L"Уровень") \
    X(LocKey_Misc, L"Miscellaneous", L"Разное")\
    X(LocKey_Settings, L"Settings", L"Настройки")\
    X(LocKey_HEALTH, L"HEALTH", L"ЗДОРОВЬЕ") \
    X(LocKey_Godmode, L"Godmode", L"Бессмертие") \
    X(LocKey_HealToMax, L"Heal to max", L"Исцелиться на максимум") \
    X(LocKey_MOVEMENT, L"MOVEMENT", L"ПЕРЕДВИЖЕНИЕ") \
    X(LocKey_InfiniteStamina, L"Infinite stamina", L"Стамина не кончается") \
    X(LocKey_Default, L"Default", L"Обычная") \
    X(LocKey_WalkingSpeed, L"Walking speed", L"Скорость ходьбы") \
    X(LocKey_RunningSpeed, L"Running speed", L"Скорость бега") \
    X(LocKey_CrouchingSpeed, L"Crouching speed", L"Скорость ползания") \
    X(LocKey_InfiniteJumps, L"Infinite jumps", L"Бесконечные прыжки") \
    X(LocKey_DontTumble, L"Dont tumble", L"Не падать") \
    X(LocKey_DisableTumbling, L"Disable tumbling from external sources", L"Отключает падение от внешних источников") \
    X(LocKey_GRAB, L"GRAB", L"ЗАХВАТ") \
    X(LocKey_HighStrength, L"High strength", L"Большая сила") \
    X(LocKey_AvailableIfHost, L"Available if you are host", L"Доступно если вы хост") \
    X(LocKey_UnlimitedRange, L"Unlimited range", L"Неограниченная дальность") \
    X(LocKey_NoOvercharge, L"No overcharge", L"Нет перегрузки") \
    X(LocKey_POWERUPS, L"POWERUPS", L"УЛУЧШЕНИЯ") \
    X(LocKey_AutoApplyUpgrades, L"Automatically apply upgrades", L"Автоматически применять улучшения") \
    X(LocKey_ManualApplication, L"Manual application", L"Ручное применение") \
    X(LocKey_HEALTHPACKS, L"HEALTH PACKS", L"АПТЕЧКИ") \
    X(LocKey_ApplyAidKit, L"Apply an available aid kit to yourself", L"Применить на себя доступную аптечку") \
    X(LocKey_VIEW, L"VIEW", L"ОБЗОР") \
    X(LocKey_BetterVision, L"Better vision", L"Улучшенное зрение") \
    X(LocKey_ThirdPerson, L"Third person", L"Третье лицо") \
    X(LocKey_DefaultView, L"Default", L"Обычное") \
    X(LocKey_Degrees, L"%d degrees", L"%d градусов") \
    X(LocKey_FieldOfView, L"Field of view", L"Поле зрения") \
    X(LocKey_FLASHLIGHT, L"FLASHLIGHT", L"ФОНАРИК") \
    X(LocKey_LightIntensity, L"Light intensity", L"Яркость света") \
    X(LocKey_IlluminationAngle, L"Illumination angle", L"Угол освещения") \
    X(LocKey_WorksWhileCrouching, L"Works while crouching", L"Работает в приседе") \
    X(LocKey_DEADHEAD, L"DEAD HEAD", L"МЕРТВАЯ ГОЛОВА") \
    X(LocKey_MaxBatteryHead, L"Max battery", L"Полный заряд") \
    X(LocKey_VISUALS, L"VISUALS", L"ВИЗУАЛ") \
    X(LocKey_DisplayThroughWalls, L"Display through walls", L"Отображать через стены") \
    X(LocKey_Within50m, L"Within 50 metres", L"В пределах 50 метров") \
    X(LocKey_XrayHighlight, L"X-ray hightlight", L"Рентген подсветка") \
    X(LocKey_NotAllHighlighted, L"Some objects are not highlighted", L"Некоторые предметы не подсвечиваются") \
    X(LocKey_VALUE, L"VALUE", L"ЦЕНА") \
    X(LocKey_NoDamageValuables, L"Valuables dont get damage", L"Ценности не получают повреждений") \
    X(LocKey_ChangeValue, L"Change value of all valuables:", L"Изменить цену всех ценностей:") \
    X(LocKey_ToZero, L"To zero", L"На ноль") \
    X(LocKey_ToMax, L"To max", L"На максимальную") \
    X(LocKey_EXTRACTION, L"EXTRACTION POINTS", L"ТОЧКИ СБОРА") \
    X(LocKey_ActivateNext, L"Activate next", L"Активировать следующую точку") \
    X(LocKey_ENEMIES, L"ENEMIES", L"ВРАГИ") \
    X(LocKey_Spawn, L"Spawn", L"Заспавить") \
    X(LocKey_SpawnAlt, L"Spawn", L"Заспавнить") \
    X(LocKey_CantSeePlayers, L"Cant see players", L"Не видят игроков") \
    X(LocKey_NoGrabLimit, L"No grab time limitation", L"Можно держать вечно") \
    X(LocKey_PLAYERS, L"PLAYERS", L"ИГРОКИ") \
    X(LocKey_Tumble, L"Tumble", L"Уронить") \
    X(LocKey_Kill, L"Kill", L"Убить") \
    X(LocKey_HostOnly, L"Host only", L"Только хост") \
    X(LocKey_Revive, L"Revive", L"Воскресить") \
    X(LocKey_VoiceChat, L"Voice chat", L"Голосовой чат") \
    X(LocKey_Alive, L"Alive", L"Живых") \
    X(LocKey_Dead, L"Dead", L"Мертвых") \
    X(LocKey_GENERAL, L"GENERAL", L"ОБЩЕЕ") \
    X(LocKey_LevelsCompleted, L"Number of levels completed", L"Количество пройденных уровней") \
    X(LocKey_NextLevel, L"Go next level", L"Перейти на следующий уровень") \
    X(LocKey_MAPFILTER, L"MAP FILTER", L"ФИЛЬТР КАРТ") \
    X(LocKey_MapFilterDesc, L"Select maps that will not be set. Available if you are host", L"Выберите карты которые не будут выпадать. Доступно если вы хост") \
    X(LocKey_GUNS, L"GUNS", L"ОРУЖИЕ") \
    X(LocKey_MaxBatteryGun, L"Max battery", L"Максимальная батарея") \
    X(LocKey_LaserCrosshair, L"Laser crosshair", L"Лазерный прицел") \
    X(LocKey_SPAWN, L"SPAWN", L"СПАВН") \
    X(LocKey_TRUCK, L"TRUCK", L"ФУРГОН") \
    X(LocKey_COSMETICS, L"COSMETICS", L"КОСМЕТИКА") \
    X(LocKey_TeleportTruck, L"Teleport to truck", L"Телепортироваться в фургон") \
    X(LocKey_SETTINGS, L"SETTINGS", L"НАСТРОЙКИ") \
    X(LocKey_Language, L"Language", L"Язык") \
    X(LocKey_Hotkey, L"Hotkey", L"Горячая клавиша") \
    X(LocKey_OpenCloseMenu, L"Open / close menu", L"Открыть / закрыть меню") \
    X(LocKey_DarkenBackground, L"Darken the screen behind the menu", L"Затемнять экрана позади меню")\
    X(LocKey_CrouchRest, L"Crouch rest", L"Восст. в приседе") \
    X(LocKey_ExtraJump, L"Extra jump", L"Доп. прыжок") \
    X(LocKey_Range, L"Range", L"Дальность") \
    X(LocKey_SprintSpeed, L"Sprint speed", L"Скорость бега") \
    X(LocKey_Stamina, L"Stamina", L"Выносливость") \
    X(LocKey_Strength, L"Strength", L"Сила") \
    X(LocKey_Launch, L"Launch", L"Запуск") \
    X(LocKey_Wings, L"Wings", L"Крылья") \
    X(LocKey_Health, L"Health", L"Здоровье") \
    X(LocKey_PlayersCount, L"Players count", L"Счетчик игроков") \
    X(LocKey_Climb, L"Climb", L"Карабканье") \
    X(LocKey_HeadBattery, L"Head battery", L"Заряд головы") \
    X(LocKey_Small, L"Small", L"Маленькая") \
    X(LocKey_Medium, L"Medium", L"Средняя") \
    X(LocKey_Large, L"Large", L"Большая") \
    X(LocKey_Unknown, L"Unknown", L"Неизвестное") \
    X(LocKey_UnlockAll, L"Unlock all", L"Открыть все") \
    X(LocKey_CosmeticBox, L"Cosmetic box", L"Коробка косметики") \
    X(LocKey_RarityCommon, L"Common", L"Обычная") \
    X(LocKey_RarityUncommon, L"Uncommon", L"Необычная") \
    X(LocKey_RarityRare, L"Rare", L"Редкая") \
    X(LocKey_RarityUltraRare, L"Ultra rare", L"Очень редкая") \
    X(LocKey_RemoveCosmeticLimit, L"Remove extraction limit", L"Снять лимит выноса") \
    X(LocKey_EnableBeforeLevel, L"Enable before entering a level", L"Включать до входа в уровень") \
    X(LocKey_TeleportFarthestBox, L"Teleport farthest box", L"Телепортировать дальнюю коробку") \
    X(LocKey_CosmeticBoxesEsp, L"Cosmetic boxes ESP", L"Подсветка коробок") \
    X(LocKey_StartingValue, L"Starting value", L"Начальная цена") \
    X(LocKey_ValueHint, L"e.g. 1000 (0 = random)", L"например 1000 (0 = случайно)")

    enum LocKey 
    {
        #define X(key, en, ru) key,
        LOC_LIST(X)
        #undef X
        LocKey_Count
    };

    static Hax::WStringView g_Dicts[][LocKey_Count] = 
    {
        {
            #define X(key, en, ru) en,
            LOC_LIST(X)
            #undef X
        },
        {
            #define X(key, en, ru) ru,
            LOC_LIST(X)
            #undef X
        }
    };

    static Hax::WStringView* g_Loc = g_Dicts[Lang_Eng];

    static size_t g_ActiveTab = 0;
    static size_t g_KeyListenerId = 0;
    static Hax::Gui::Texture2D g_LogoTex;

    struct UpgradeData
    {
        Hax::Gui::Texture2D Img;
        Hax::WStringView InternalNameEnd;
        LocKey Loc;
        bool AutoUse;
    };

    enum class UpgradeType : int
    {
        Rest,
        Jump,
        Range,
        Speed,
        Stamina,
        Strength,
        Tumble,
        Wings,
        Health,
        Map,
        Climb,
        Battery,
        N
    };

    enum class AidType
    {
        Small,
        Medium,
        Large,
        N
    };

    struct AidData
    {
        Hax::Gui::Texture2D Img;
        Hax::WStringView InternalNameEnd;
        LocKey Loc;
    };


    static AidData g_AidInfo[(int)AidType::N + 1] =
    {
        { .InternalNameEnd = L"Small", .Loc = LocKey_Small },
        { .InternalNameEnd = L"Medium", .Loc = LocKey_Medium },
        { .InternalNameEnd = L"Large", .Loc = LocKey_Large },
        { .InternalNameEnd = L"", .Loc = LocKey_Unknown }
    };

    static UpgradeData g_UpgradeInfo[(int)UpgradeType::N + 1] =
    {
        { .InternalNameEnd = L"Rest", .Loc = LocKey_CrouchRest },
        { .InternalNameEnd = L"Jump", .Loc = LocKey_ExtraJump },
        { .InternalNameEnd = L"Range", .Loc = LocKey_Range },
        { .InternalNameEnd = L"Speed", .Loc = LocKey_SprintSpeed },
        { .InternalNameEnd = L"Energy", .Loc = LocKey_Stamina },
        { .InternalNameEnd = L"Strength", .Loc = LocKey_Strength },
        { .InternalNameEnd = L"Launch", .Loc = LocKey_Launch },
        { .InternalNameEnd = L"Wings", .Loc = LocKey_Wings },
        { .InternalNameEnd = L"Health", .Loc = LocKey_Health },
        { .InternalNameEnd = L"Count", .Loc = LocKey_PlayersCount },
        { .InternalNameEnd = L"Climb", .Loc = LocKey_Climb },
        { .InternalNameEnd = L"Battery", .Loc = LocKey_HeadBattery },
        { .InternalNameEnd = L"", .Loc = LocKey_Unknown }
    };

    enum VerticalAlignment : int 
    {
        VerticalAlignment_Bottom,
        VerticalAlignment_Top,
        VerticalAlignment_Center
    };

    enum HorizontalAlignment : int
    {
        HorizontalAlignment_Right,
        HorizontalAlignment_Left,
        HorizontalAlignment_Center
    };

    static LRESULT WINAPI Hooked_Present(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);
    static HRESULT WINAPI Hooked_ResizeBuffers(IDXGISwapChain* swapChain, UINT a1, UINT a2, UINT a3, DXGI_FORMAT a4, UINT a5);
    static void CreateRenderTarget(IDXGISwapChain* swapChain);
    static void OnGuiContextCreated();
    static void ToggleMenuVisibility();
    static bool IsKeyJustDown(size_t id, Hax::uint8 vk);

    static void DrawCheatMenu();
    static void DrawStatsTab();
    static void DrawEntitiesTab();
    static void DrawVisionTab();
    static void DrawValuablesTab();
    static void DrawItemsTab();
    static void DrawLevelTab();
    static void DrawMiscTab();
    static void DrawSettingsTab();

    static void DrawBackground();
    static bool ShouldDrawEsp();
    static void DrawEnemyEsp(EnemyEspData& data);
    static void DrawValuableEsp(ValuableEspData& data);
    static void DrawExtrPointEsp(ExtrPointEspData& data);
    static void DrawTruckEsp(TruckEspData& data);
    static void DrawPlayerEsp(PlayerEspData& data);
    static void DrawCosmeticBoxEsp(CosmeticBoxEspData& data);
    //static void CornerBox(float x, float y, float width, float height, const Hax::Gui::Color& color, float thickness, float cornerProp);
    static void Box(const Hax::Rect& rect, const Hax::Gui::Color& col);
    static void VertHealthBar(float x, float y, float width, float height, float cur, float max);
    static void VertHealthBar(float x, float y, float width, float height, int cur, int max);
    static float EspFontSize();
    static float IconFontSize();

    static void Text(Hax::Gui::FontHandle hFont,
        Hax::WStringView text,
        const Hax::Vector2& pos,
        const Hax::Gui::Color& col,
        float fontSize,
        VerticalAlignment vAlign = VerticalAlignment_Bottom,
        HorizontalAlignment hAlign = HorizontalAlignment_Right);

    static bool IsPlayerAlive();
    static bool IsPlayerFullHp();

    struct Tab
    {
        void(*DrawContent)(void);
        LocKey Loc;
        Hax::char16 Icon;
    };

    static const Tab g_Tabs[] = 
    {
        {DrawStatsTab, LocKey_Stats, L'\ue473'},
        {DrawEntitiesTab, LocKey_Entities, L'\uf544'},
        {DrawVisionTab, LocKey_Vision, L'\uf06e'},
        {DrawValuablesTab, LocKey_Valuables, L'\uf81d'},
        {DrawItemsTab, LocKey_Items, L'\uf1e2'},
        {DrawLevelTab, LocKey_Level, L'\uf5fd'},
        {DrawMiscTab, LocKey_Misc, L'\uf86d'},
        {DrawSettingsTab, LocKey_Settings, L'\uf013'}
    };

    void InitializeMenu(Hax::Handle hDirectX11)
    {
        Hax::LogDebug(GCheat->LogFile, L"Hooking graphics...");

        HWND hwnd = 0;
        WNDCLASSEX dummyWindow{};
        dummyWindow.cbSize = 0;
        dummyWindow.cbSize = sizeof(WNDCLASSEX);
        dummyWindow.style = CS_HREDRAW | CS_VREDRAW;
        dummyWindow.lpfnWndProc = ::DefWindowProcW;
        dummyWindow.cbClsExtra = 0;
        dummyWindow.cbWndExtra = 0;
        dummyWindow.hInstance = ::GetModuleHandleW(NULL);
        dummyWindow.hIcon = NULL;
        dummyWindow.hCursor = NULL;
        dummyWindow.hbrBackground = NULL;
        dummyWindow.lpszMenuName = nullptr;
        dummyWindow.lpszClassName = L"Dummy window";
        dummyWindow.hIconSm = NULL;
        ::RegisterClassEx(&dummyWindow);
        hwnd = ::CreateWindow(dummyWindow.lpszClassName, L"Dummy window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, dummyWindow.hInstance, nullptr);

        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = hwnd;
        swapChainDesc.SampleDesc.Count = 1;

        const D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_0
        };

        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;
        IDXGISwapChain* swapChain = nullptr;

        HRESULT result = ::D3D11CreateDeviceAndSwapChain
        (
            NULL, D3D_DRIVER_TYPE_NULL, NULL, 0, featureLevels, 2,
            D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, nullptr, &context
        );
        HAX_PANIC(result == S_OK, &GCheat->LogFile, L"D3D11CreateDeviceAndSwapChain %d", result);

        void** vtable = *(void***)(swapChain);
        void* present = vtable[8];
        void* resizeBuffers = vtable[13];

        swapChain->Release();
        context->Release();
        device->Release();

        ::DestroyWindow(hwnd);
        ::UnregisterClass(dummyWindow.lpszClassName, dummyWindow.hInstance);

        Cheat::Hook(present, Hooked_Present, GCheat->PresentHook, "Present");
        Cheat::Hook(resizeBuffers, Hooked_ResizeBuffers, GCheat->ResizeBuffersHook, "ResizeBuffers");
    }

    bool IsUpgrAutouse(void* assetName)
    {
        System::String name = (UVM::String*)assetName;
        for (int i = 0; i < (int)UpgradeType::N; ++i)
        {
            if (name.ToView().EndsWith(g_UpgradeInfo[i].InternalNameEnd))
                return g_UpgradeInfo[i].AutoUse;
        }
        return false;
    }

    static LRESULT WINAPI Hooked_Present(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags)
    {
        if (GCheat->Device == nullptr)
        {
            swapChain->GetDevice(IID_PPV_ARGS(&GCheat->Device));
            GCheat->Device->GetImmediateContext(&GCheat->DeviceContext);

            DXGI_SWAP_CHAIN_DESC sd;
            swapChain->GetDesc(&sd);
            GCheat->hWnd = sd.OutputWindow;

            Hax::Gui::Initialize((Hax::Handle)sd.OutputWindow, GCheat->Device);
            OnGuiContextCreated();
        }

        if (GCheat->RenderTarget == nullptr)
            CreateRenderTarget(swapChain);

        GCheat->NoGrabMaxTime &= !GCheat->IsClient;
        GCheat->Unbreakable &= !GCheat->IsClient;
        //GCheat->Blind &= !GCheat->IsClient;
        GCheat->EasyGrab &= !GCheat->IsClient;
        GCheat->InfBattery &= !GCheat->IsClient;

        if (GCheat->LoopSystem.Initialized && !GCheat->LoopSystem.Hooked)
        {
            GCheat->LoopSystem.Hooked = true;
            GameHooks::LoopInstall();
        }

        try
        {
            Hax::Gui::BeginFrame();
            if (IsKeyJustDown(HAX_LINE, GCheat->VkOpenClose))
                ToggleMenuVisibility();

            Hax::Gui::SwitchLayer(L"Background");
            DrawBackground();
            if (GCheat->MenuVisible && GCheat->DarkenBg)
                Hax::Gui::DrawRect(Hax::Vector2(), Hax::Gui::GetViewportSize(), { .FillColor = 0x000000E6 });
            Hax::Gui::RestoreLayer();

            if (GCheat->MenuVisible)
            {
                Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Arrow);
                DrawCheatMenu();
            }

            GCheat->DeviceContext->OMSetRenderTargets(1, &GCheat->RenderTarget, nullptr);
            Hax::Gui::EndFrame();
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(GCheat->LogFile, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        HRESULT result = GCheat->PresentHook.call<HRESULT, IDXGISwapChain*, UINT, HRESULT>(swapChain, syncInterval, flags);
        return result;
    }

    static HRESULT WINAPI Hooked_ResizeBuffers(IDXGISwapChain* swapChain, UINT a1, UINT a2, UINT a3, DXGI_FORMAT a4, UINT a5)
    {
        if (GCheat->RenderTarget != nullptr)
        {
            GCheat->RenderTarget->Release();
            GCheat->RenderTarget = nullptr;
        }

        return GCheat->ResizeBuffersHook.stdcall<HRESULT, IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT>(swapChain, a1, a2, a3, a4, a5);
    }

    static void CreateRenderTarget(IDXGISwapChain* swapChain)
    {
        ID3D11Texture2D* backBuffer = NULL;
        swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        if (backBuffer && GCheat->Device)
        {
            D3D11_RENDER_TARGET_VIEW_DESC desc{};
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            GCheat->Device->CreateRenderTargetView(backBuffer, &desc, &GCheat->RenderTarget);
            backBuffer->Release();
        }
    }

    static void OnGuiContextCreated()
    {
        Hax::Gui::CreateLayer(L"Background", -1);
        Hax::Gui::CreateLayer(L"Popups", 1);

        g_Loc = g_Dicts[GCheat->Lang];

        Hax::Handle hCheat = (Hax::Handle)GCheat->hCheat;
        g_LogoTex =                     Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG1, L"PNG"));
        GCheat->NunitoSans_SemiBold =   Hax::Gui::LoadFont(Hax::Gui::GetResourceData(hCheat, IDR_WOFF21, L"WOFF2"));
        GCheat->NunitoSans_Bold =       Hax::Gui::LoadFont(Hax::Gui::GetResourceData(hCheat, IDR_WOFF22, L"WOFF2"));
        GCheat->NunitoSans_ExtraBold =  Hax::Gui::LoadFont(Hax::Gui::GetResourceData(hCheat, IDR_WOFF23, L"WOFF2"));
        GCheat->Icons_Solid =           Hax::Gui::LoadFont(Hax::Gui::GetResourceData(hCheat, IDR_WOFF24, L"WOFF2"));

        Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_Arrow, hCheat, IDC_CURSOR7);
        Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_TextInput, hCheat, IDC_CURSOR1);
        Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeAll, hCheat, IDC_CURSOR6);
        Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeNS, hCheat, IDC_CURSOR9);
        Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeEW, hCheat, IDC_CURSOR4);
        Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeNESW, hCheat, IDC_CURSOR3);
        Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeNWSE, hCheat, IDC_CURSOR2);
        Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_Hand, hCheat, IDC_CURSOR5);
        Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_NotAllowed, hCheat, IDC_CURSOR8);

        g_UpgradeInfo[(int)UpgradeType::Rest].Img =     Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG2, L"PNG"));
        g_UpgradeInfo[(int)UpgradeType::Jump].Img =     Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG3, L"PNG"));
        g_UpgradeInfo[(int)UpgradeType::Range].Img =    Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG8, L"PNG"));
        g_UpgradeInfo[(int)UpgradeType::Speed].Img =    Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG9, L"PNG"));
        g_UpgradeInfo[(int)UpgradeType::Stamina].Img =  Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG10, L"PNG"));
        g_UpgradeInfo[(int)UpgradeType::Strength].Img = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG11, L"PNG"));
        g_UpgradeInfo[(int)UpgradeType::Tumble].Img =   Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG12, L"PNG"));
        g_UpgradeInfo[(int)UpgradeType::Wings].Img =    Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG13, L"PNG"));
        g_UpgradeInfo[(int)UpgradeType::Health].Img =   Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG14, L"PNG"));
        g_UpgradeInfo[(int)UpgradeType::Map].Img =      Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG15, L"PNG"));

        Hax::Gui::Texture2D tex = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG7, L"PNG"));
        g_UpgradeInfo[(int)UpgradeType::Climb].Img    = tex;
        g_UpgradeInfo[(int)UpgradeType::Battery].Img  = tex;
        g_UpgradeInfo[(int)UpgradeType::N].Img        = tex;

        g_AidInfo[(int)AidType::Small].Img =    Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG6, L"PNG"));
        g_AidInfo[(int)AidType::Medium].Img =   Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG5, L"PNG"));
        g_AidInfo[(int)AidType::Large].Img =    Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG4, L"PNG"));

        UVM::ThreadAttach();

        ToggleMenuVisibility();
    }

    static void ToggleMenuVisibility()
    {
        ::PostMessageW(GCheat->hWnd, WM_USER, 0, 0);
    }

    static bool IsKeyJustDown(size_t id, Hax::uint8 vk)
    {
        return (GCheat->KeyListenerId == 0 || GCheat->KeyListenerId == id) && Hax::Gui::IsKeyJustDown(vk);
    }

    static void DrawCheatMenu()
    {
        Widgets::BeginWindow();
        {
            if (Widgets::BeginSidePanel())
            {
                const float sidePanelWidth = Hax::Gui::GetContentRegionAvail().X;

                Hax::Gui::Space(30_px);

                // Logo
                {
                    Hax::Gui::Texture2D shrinkedLogo = g_LogoTex;
                    shrinkedLogo.Width = (int)(Hax::Gui::Scale(g_LogoTex.Width / 2.f));
                    shrinkedLogo.Height = (int)(Hax::Gui::Scale(g_LogoTex.Height / 2.f));

                    Hax::Gui::BeginHorizontal();
                    {
                        Hax::Gui::Space((sidePanelWidth - shrinkedLogo.Width) / 2.f);
                        Widgets::Image(shrinkedLogo);
                    }
                    Hax::Gui::EndHorizontal();
                }

                Hax::Gui::Space(30_px);

                const float padding = 12_px;
                Hax::Gui::BeginHorizontal();
                {
                    Hax::Gui::Space(padding);
                    Hax::Gui::BeginContainer(0, { .W = sidePanelWidth - padding * 2.f });
                    {
                        Hax::Gui::BeginVertical(5_px);
                        {
                            for (size_t i = 0; i < 3; ++i)
                                if (Widgets::TabButton(HAX_LINE + i, g_Loc[g_Tabs[i].Loc], g_Tabs[i].Icon, g_ActiveTab == i))
                                    g_ActiveTab = i;

                            Hax::Gui::Space(7_px);
                            Hax::Gui::HorizontalLine(1_px, 0x2F3033FF);
                            Hax::Gui::Space(7_px);

                            for (size_t i = 3; i < 7; ++i)
                                if (Widgets::TabButton(HAX_LINE + i, g_Loc[g_Tabs[i].Loc], g_Tabs[i].Icon, g_ActiveTab == i))
                                    g_ActiveTab = i;

                            Hax::Gui::Space(7_px);
                            Hax::Gui::HorizontalLine(1_px, 0x2F3033FF);
                            Hax::Gui::Space(7_px);

                            if (Widgets::TabButton(HAX_LINE + 7, g_Loc[g_Tabs[7].Loc], g_Tabs[7].Icon, g_ActiveTab == 7))
                                g_ActiveTab = 7;
                        }
                        Hax::Gui::EndVertical();
                    }
                    Hax::Gui::EndContainer();
                }
                Hax::Gui::EndHorizontal();

                Widgets::EndSidePanel();
            }

            // Main area
            Hax::Gui::BeginContainer(Hax::Hash("Main area"), {.Clip = true, .ScrollY = true});
            {
                Hax::Gui::BeginHorizontal();
                g_Tabs[g_ActiveTab].DrawContent();
                Hax::Gui::EndHorizontal();
            }
            Hax::Gui::EndContainer();
        }
        Widgets::EndWindow();
    }

    static void DrawStatsTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = {(mainAreaSize.X - 3 * spacing) / 2, mainAreaSize.Y};

        PlayerAvatar avatar = PlayerAvatar::instance();
        PlayerHealth health = avatar ? avatar.playerHealth() : nullptr;

        // Column 1
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_HEALTH]);
            {
                Widgets::ToggleEx(HAX_LINE, GCheat->Godmode, g_Loc[LocKey_Godmode]);

                Widgets::HorizontalLine(1_px);

                bool disabled = !GCheat->IsInGame || !avatar || !health || health.health() >= health.maxHealth();
                if (Widgets::Button(HAX_LINE, g_Loc[LocKey_HealToMax], {}, {.Enabled = !disabled, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                    GCheat->HealToMax = true;
            }
            Widgets::EndPanel();

            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_MOVEMENT]);
            {
                Widgets::ToggleEx(HAX_LINE, GCheat->InfStamina, g_Loc[LocKey_InfiniteStamina]);

                Widgets::HorizontalLine(1_px);

                {
                    Hax::char16 buf[16]{};
                    swprintf_s(buf, _countof(buf), GCheat->Acceleration.Walking == 1 ? g_Loc[LocKey_Default].Data() : L"%dx", GCheat->Acceleration.Walking);

                    Widgets::SliderEx(HAX_LINE, g_Loc[LocKey_WalkingSpeed], buf, &GCheat->Acceleration.Walking, 1, 5, Widgets::SliderConvertInt);
                }

                Widgets::HorizontalLine(1_px);

                {
                    Hax::char16 buf[16]{};
                    swprintf_s(buf, _countof(buf), GCheat->Acceleration.Sprinting == 1 ? g_Loc[LocKey_Default].Data() : L"%dx", GCheat->Acceleration.Sprinting);

                    Widgets::SliderEx(HAX_LINE, g_Loc[LocKey_RunningSpeed], buf, &GCheat->Acceleration.Sprinting, 1, 5, Widgets::SliderConvertInt);
                }

                Widgets::HorizontalLine(1_px);

                {
                    Hax::char16 buf[16]{};
                    swprintf_s(buf, _countof(buf), GCheat->Acceleration.Crouching == 1 ? g_Loc[LocKey_Default].Data() : L"%dx", GCheat->Acceleration.Crouching);

                    Widgets::SliderEx(HAX_LINE, g_Loc[LocKey_CrouchingSpeed], buf, &GCheat->Acceleration.Crouching, 1, 5, Widgets::SliderConvertInt);
                }

                Widgets::HorizontalLine(1_px);

                Widgets::ToggleEx(HAX_LINE, GCheat->InfJumps, g_Loc[LocKey_InfiniteJumps]);

                Widgets::HorizontalLine(1_px);

                Widgets::ToggleEx(HAX_LINE, GCheat->NoTumble, g_Loc[LocKey_DontTumble], g_Loc[LocKey_DisableTumbling]);
            }
            Widgets::EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();

        // Column 2
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_GRAB]);
            {
                Widgets::ToggleEx(HAX_LINE, GCheat->EasyGrab, g_Loc[LocKey_HighStrength], g_Loc[LocKey_AvailableIfHost], {.Disabled = GCheat->IsClient});

                Widgets::HorizontalLine(1_px);

                Widgets::ToggleEx(HAX_LINE, GCheat->UnlimGrabRange, g_Loc[LocKey_UnlimitedRange]);

                Widgets::HorizontalLine(1_px);

                Widgets::ToggleEx(HAX_LINE, GCheat->NoOverCharge, g_Loc[LocKey_NoOvercharge]);
            }
            Widgets::EndPanel();

            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_POWERUPS]);
            {
                size_t id = HAX_LINE;
                constexpr size_t settingsPopupId = Hax::Hash("PowerupPopup");
                Widgets::ToggleEx(id, GCheat->AutoUseUpgr, g_Loc[LocKey_AutoApplyUpgrades], {}, {.SettingsId = settingsPopupId});

                if (Widgets::BeginModal(settingsPopupId))
                {
                    const float px10 = 10_px;
                    Hax::Gui::BeginHorizontal(15_px);
                    {
                        size_t half = ((size_t)UpgradeType::N + 1) / 2;

                        Hax::Gui::Dummy(Hax::Vector2(px10, 0.f));
                        Hax::Gui::BeginVertical(5_px);
                        {
                            {
                                Hax::Gui::Dummy(Hax::Vector2(0.f, px10));
                                for (size_t i = 0; i < half; ++i)
                                {
                                    auto& upgrInfo = g_UpgradeInfo[i];
                                    Widgets::CheckboxEx((size_t)&upgrInfo, upgrInfo.AutoUse, g_Loc[upgrInfo.Loc]);
                                }
                                Hax::Gui::Dummy(Hax::Vector2(0.f, px10));
                            }
                        }
                        Hax::Gui::EndVertical();

                        Hax::Gui::BeginVertical(5_px);
                        {
                            Hax::Gui::Dummy(Hax::Vector2(0.f, px10));
                            for (size_t i = half; i < (size_t)UpgradeType::N; ++i)
                            {
                                auto& upgrInfo = g_UpgradeInfo[i];
                                Widgets::CheckboxEx((size_t)&upgrInfo, upgrInfo.AutoUse, g_Loc[upgrInfo.Loc]);
                            }
                            Hax::Gui::Dummy(Hax::Vector2(0.f, px10));
                        }

                        Hax::Gui::EndVertical();
                        Hax::Gui::Dummy(Hax::Vector2(px10, 0.f));
                    }
                    Hax::Gui::EndHorizontal();

                    Widgets::EndModal();
                }

               Widgets::HorizontalLine(1_px);

                {
                    Widgets::MainLabel(g_Loc[LocKey_ManualApplication]);

                    Hax::Gui::BeginVertical(5_px);
                    Hax::Gui::BeginHorizontal(5_px);
                    {
                        const Hax::Vector<ItemUpgrade>& upgrades = GCheat->Upgrades.Pool.GetFront();
                        for (size_t i = 0; i < upgrades.Size(); ++i)
                        {
                            ItemUpgrade item = upgrades[i];
                            if (!item || !item.itemAttributes() || !item.itemToggle())
                                continue;

                            UpgradeData& data = g_UpgradeInfo[(int)UpgradeType::N];
                            Hax::WStringView name = item.itemAttributes().itemAssetName().ToView();
                            for (size_t j = 0; j < (size_t)UpgradeType::N; ++j)
                            {
                                if (name.EndsWith(g_UpgradeInfo[j].InternalNameEnd))
                                {
                                    data = g_UpgradeInfo[j];
                                    break;
                                }
                            }

                            bool disabled = item.itemToggle().toggleState() == true;
                            if (Widgets::AutoLayoutButton(HAX_LINE + i * 5542, g_Loc[data.Loc], {.Disabled = disabled, .Icon = data.Img}))
                                GCheat->Upgrades.ItemToToggle = item;
                        }
                    }
                    Hax::Gui::EndHorizontal();
                    Hax::Gui::EndVertical();
                }
            }
            Widgets::EndPanel();

            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_HEALTHPACKS], g_Loc[LocKey_ApplyAidKit]);
            {
                Hax::Gui::BeginVertical(5_px);
                Hax::Gui::BeginHorizontal(5_px);
                {
                    const Hax::Vector<ItemHealthPack>& aids = GCheat->Aids.Pool.GetFront();
                    for (size_t i = 0; i < aids.Size(); ++i)
                    {
                        ItemHealthPack aid = aids[i];
                        if (!aid || !aid.itemAttributes() || !aid.itemToggle())
                            continue;

                        AidData& data = g_AidInfo[(int)AidType::N];
                        Hax::WStringView name = aid.itemAttributes().itemAssetName().ToView();
                        for (size_t j = 0; j < (size_t)AidType::N; ++j)
                        {
                            if (name.EndsWith(g_AidInfo[j].InternalNameEnd))
                            {
                                data = g_AidInfo[j];
                                break;
                            }
                        }

                        bool disabled = aid.itemToggle().toggleState() == true || IsPlayerFullHp();
                        if (Widgets::AutoLayoutButton(HAX_LINE + i * 5542, g_Loc[data.Loc], {.Disabled = disabled, .Icon = data.Img}))
                            GCheat->Aids.ItemToToggle = aid;
                    }
                }
                Hax::Gui::EndHorizontal();
                Hax::Gui::EndVertical();
            }
            Widgets::EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }

    static void UpToDatePlayer(PlayerAvatar& player)
    {
        if (GameDirector dir = GameDirector::instance())
        {
            for (PlayerAvatar avatar : dir.PlayerList())
            {
                if (avatar == player)
                    return;
            }
        }

        player = nullptr;
    }

    static void DrawEntitiesTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = {(mainAreaSize.X - 3 * spacing) / 2, mainAreaSize.Y};

        // Column 1 
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_ENEMIES]);
            {
                const float spacing2 = 5_px;
                Hax::Vector2 sz = Widgets::CalcButtonSize(g_Loc[LocKey_Spawn]);
                Hax::Gui::BeginHorizontal(spacing2);
                {
                    constexpr size_t dropListId = Hax::Hash(L"EnemySelect");
                    const float dropListW = Hax::Gui::GetContentRegionAvail().X - spacing2 - sz.X;

                    Hax::Gui::BeginVertical(3_px);
                    {
                        Hax::WStringView view = L"-";
                        if (GCheat->SelectedEnemySetup < GCheat->EnemiesPool.Size())
                            view = GCheat->EnemiesPool.begin()[GCheat->SelectedEnemySetup].key;

                        if (Widgets::DropdownBtn(HAX_LINE, view, dropListW))
                            Widgets::OpenPopup(dropListId, Hax::Gui::GetCursorPos());
                    }
                    Hax::Gui::EndVertical();

                    if (Widgets::BeginDropList(dropListId, {dropListW, 150_px}))
                    {
                        float w = Hax::Gui::GetContentRegionAvail().X;
                        for (size_t i = 0; i < GCheat->EnemiesPool.Size(); ++i)
                        {
                            if (Widgets::Selectable(HAX_LINE + i * 10000, GCheat->EnemiesPool.begin()[i].key, 
                                GCheat->SelectedEnemySetup == i, {.MinW = w}))
                            {
                                GCheat->SelectedEnemySetup = i;
                                Widgets::ClosePopup(dropListId);
                            }
                        }

                        Widgets::EndDropList();
                    }

                    bool enabled = !GCheat->IsClient && GCheat->SelectedEnemySetup < GCheat->EnemiesPool.Size() && GCheat->IsInGame;
                    if (Widgets::Button(HAX_LINE, g_Loc[LocKey_Spawn], {}, {.Enabled = enabled}))
                        GCheat->EnemyToSpawn = (GCheat->EnemiesPool.begin() + GCheat->SelectedEnemySetup)->value;
                }
                Hax::Gui::EndHorizontal();

               /* Widgets::HorizontalLine(1_px);
                Widgets::ToggleEx(HAX_LINE, GCheat->Blind, g_Loc[LocKey_CantSeePlayers], g_Loc[LocKey_AvailableIfHost], {.Disabled = GCheat->IsClient});*/
                Widgets::HorizontalLine(1_px);
                Widgets::ToggleEx(HAX_LINE, GCheat->NoGrabMaxTime, g_Loc[LocKey_NoGrabLimit], g_Loc[LocKey_AvailableIfHost], {.Disabled = GCheat->IsClient});
                Widgets::HorizontalLine(1_px);
                Widgets::ToggleEx(HAX_LINE, GCheat->EnemiesEsp, g_Loc[LocKey_DisplayThroughWalls]);
            }
            Widgets::EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();

        // Column 2
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_PLAYERS]);
            {
                Widgets::ToggleEx(HAX_LINE, GCheat->PlayersEsp, g_Loc[LocKey_DisplayThroughWalls]);
                Widgets::HorizontalLine(1_px);
                Widgets::ToggleEx(HAX_LINE, GCheat->PlayerChams, g_Loc[LocKey_XrayHighlight]);
                Widgets::HorizontalLine(1_px);

                {
                    static PlayerAvatar s_SelectedPlayer;

                    GameDirector dir = GameDirector::instance();
                    System::List<PlayerAvatar> players{}; if (dir) players = dir.PlayerList();

                    if (s_SelectedPlayer)
                        UpToDatePlayer(s_SelectedPlayer);

                    const float w = Hax::Gui::GetContentRegionAvail().X;
                    constexpr size_t dropListId = Hax::Hash(L"PlayerSelect");
                    Hax::Gui::BeginVertical(3_px);
                    {
                        Hax::WStringView preview = s_SelectedPlayer ? SemiFunc::PlayerGetName(s_SelectedPlayer).ToView() : L"-";
                        if (Widgets::DropdownBtn(HAX_LINE, preview, w))
                            Widgets::OpenPopup(dropListId, Hax::Gui::GetCursorPos());
                    }
                    Hax::Gui::EndVertical();

                    Hax::Gui::BeginVertical(5_px);
                    {
                        if (Widgets::Button(HAX_LINE, g_Loc[LocKey_Tumble], g_Loc[LocKey_HostOnly], {.Enabled = GCheat->IsInGame && !GCheat->IsClient && s_SelectedPlayer && !s_SelectedPlayer.isDisabled(), .MinW = w}))
                            GCheat->ToTumble = s_SelectedPlayer;

                        Hax::Gui::BeginHorizontal(5_px);
                        {
                            const float btnW = (w - 5_px) / 2.f;

                            bool enabled = GCheat->IsInGame && !GCheat->IsClient && s_SelectedPlayer && !s_SelectedPlayer.deadSet();
                            if (Widgets::Button(HAX_LINE, g_Loc[LocKey_Kill], g_Loc[LocKey_HostOnly], {.Enabled = enabled, .MinW = btnW}))
                                GCheat->ToKill = s_SelectedPlayer;

                            enabled = GCheat->IsInGame && !GCheat->IsClient && s_SelectedPlayer && s_SelectedPlayer.deadSet();
                            if (Widgets::Button(HAX_LINE, g_Loc[LocKey_Revive], g_Loc[LocKey_HostOnly], {.Enabled = enabled, .MinW = btnW}))
                                GCheat->ToRevive = s_SelectedPlayer;
                        }
                        Hax::Gui::EndHorizontal();
                    }
                    Hax::Gui::EndVertical();

                    const size_t nPlayers = players ? players.GetCount() : 0;
                    const float selectableH = Widgets::CalcButtonHeight();
                    const float dropListH = selectableH * Hax::Max(1ULL, Hax::Min(nPlayers, 5ULL)) + 5_px * 2.f + 1.f;
                    if (Widgets::BeginDropList(dropListId, {w, dropListH}))
                    {
                        for (size_t j = 0; j < nPlayers; ++j)
                        {
                            PlayerAvatar player = players[j];
                            if (Widgets::Selectable(HAX_LINE + j * 10000, SemiFunc::PlayerGetName(player).ToView(), player == s_SelectedPlayer, {.MinW = w}))
                            {
                                s_SelectedPlayer = player;
                                Widgets::ClosePopup(dropListId);
                            }
                        }
                        Widgets::EndDropList();
                    }
                }

                Widgets::HorizontalLine(1_px);

                {
                    const float w = Hax::Gui::GetContentRegionAvail().X;
                    Hax::Gui::BeginVertical(5_px);
                    {
                        Widgets::MainLabel(g_Loc[LocKey_VoiceChat]);
                        Hax::Gui::BeginHorizontal(5_px);
                        {
                            PlayerAvatar avatar = PlayerAvatar::instance();
                            PlayerVoiceChat chat = avatar ? avatar.voiceChat() : nullptr;

                            bool disabled = !GCheat->IsInGame || !avatar.voiceChatFetched() || !chat;

                            const float btnW = (w - 5_px) / 2.f;
                            if (Widgets::Button(HAX_LINE, g_Loc[LocKey_Alive], {}, {.Enabled = !disabled && chat.inLobbyMixer(), .MinW = btnW}))
                                GCheat->PreferedChat = ChatPref::Alive;

                            if (Widgets::Button(HAX_LINE, g_Loc[LocKey_Dead], {}, {.Enabled = !disabled && !chat.inLobbyMixer(), .MinW = btnW}))
                                GCheat->PreferedChat = ChatPref::Dead;
                        }
                        Hax::Gui::EndHorizontal();
                    }
                    Hax::Gui::EndVertical();
                }
            }
            Widgets::EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }

    static void DrawVisionTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = {(mainAreaSize.X - 3 * spacing) / 2, mainAreaSize.Y};

        // Column 1
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_VIEW]);
            {
                {
                    Widgets::ToggleEx(HAX_LINE, GCheat->ImproveVision, g_Loc[LocKey_BetterVision]);
                }

               Widgets::HorizontalLine(1_px);

                {
                   Widgets::ToggleEx(HAX_LINE, GCheat->ThirdPerson, g_Loc[LocKey_ThirdPerson]);
                }

               Widgets::HorizontalLine(1_px);

                {
                    Hax::char16 buf[32] = {};
                    swprintf_s(buf, _countof(buf), GCheat->FOV < 61 ? g_Loc[LocKey_Default].Data() : g_Loc[LocKey_Degrees].Data(), GCheat->FOV);

                    Widgets::SliderEx(HAX_LINE, g_Loc[LocKey_FieldOfView], buf, &GCheat->FOV, 60, 140, Widgets::SliderConvertInt);
                }
            }
            Widgets::EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();

        // Column 2
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_FLASHLIGHT]);
            {
                {
                    int& intensity = GCheat->Flashlight.Intensity;
                    Hax::char16 buf[16] = {};
                    swprintf_s(buf, _countof(buf), intensity < 11 ? g_Loc[LocKey_Default].Data() : L"x%.1f", intensity / 10.f);

                    Widgets::SliderEx(HAX_LINE, g_Loc[LocKey_LightIntensity], buf, &intensity, 10, 20, Widgets::SliderConvertInt);
                }

               Widgets::HorizontalLine(1_px);

                {
                    int& angle = GCheat->Flashlight.Angle;
                    Hax::char16 buf[16] = {};
                    swprintf_s(buf, _countof(buf), g_Loc[LocKey_Degrees].Data(), angle);

                    Widgets::SliderEx(HAX_LINE, g_Loc[LocKey_IlluminationAngle], buf, &angle, 60, 120, Widgets::SliderConvertInt);
                }

               Widgets::HorizontalLine(1_px);

                {
                   Widgets::ToggleEx(HAX_LINE, GCheat->Flashlight.AllowInCrouch, g_Loc[LocKey_WorksWhileCrouching]);
                }
            }
            Widgets::EndPanel();

            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_DEADHEAD]);
            {
                Widgets::ToggleEx(HAX_LINE, GCheat->HeadMaxBattery, g_Loc[LocKey_MaxBatteryHead]);
            }
            Widgets::EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }

    static void DrawValuablesTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = {(mainAreaSize.X - 3 * spacing) / 2, mainAreaSize.Y};

        // Column 1
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_VISUALS]);
            {
                {
                    Widgets::ToggleEx(HAX_LINE, GCheat->ValuablesEsp, g_Loc[LocKey_DisplayThroughWalls], g_Loc[LocKey_Within50m]);
                }

                Widgets::HorizontalLine(1_px);

                {
                   Widgets::ToggleEx(HAX_LINE, GCheat->ItemsChams, g_Loc[LocKey_XrayHighlight], g_Loc[LocKey_NotAllHighlighted]);
                }
            }
            Widgets::EndPanel();

            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_VALUE]);
            {
                {
                    Widgets::ToggleEx(HAX_LINE, GCheat->Unbreakable, g_Loc[LocKey_NoDamageValuables], g_Loc[LocKey_AvailableIfHost], {.Disabled = GCheat->IsClient});
                }

               Widgets::HorizontalLine(1_px);

                {
                    Widgets::MainLabel(g_Loc[LocKey_ChangeValue]);
                    Hax::Gui::BeginHorizontal(5_px);
                    {
                        const float w = Widgets::CalcWidgetEqWidth(2);
                        if (Widgets::Button(HAX_LINE, g_Loc[LocKey_ToZero], g_Loc[LocKey_HostOnly], {.Enabled = GCheat->IsInGame && !GCheat->IsClient, .MinW = w}))
                            GCheat->SetToZero = true;
                        if (Widgets::Button(HAX_LINE, g_Loc[LocKey_ToMax], g_Loc[LocKey_HostOnly], {.Enabled = GCheat->IsInGame  && !GCheat->IsClient, .MinW = w}))
                            GCheat->SetToMax = true;
                    }
                    Hax::Gui::EndHorizontal();
                }
            }
            Widgets::EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();

        // Column 2
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_EXTRACTION]);
            {
                {
                    Widgets::ToggleEx(HAX_LINE, GCheat->ExtrPointsEsp, g_Loc[LocKey_DisplayThroughWalls]);
                }

               Widgets::HorizontalLine(1_px);

                RoundDirector dir = RoundDirector::instance();
                bool disabled = !GCheat->IsInGame || !dir || dir.extractionPointActive();
                if (Widgets::Button(HAX_LINE, g_Loc[LocKey_ActivateNext], {}, {.Enabled = !disabled, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                    GCheat->ActivateNextPoint = true;
            }
            Widgets::EndPanel();

            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_SPAWN], g_Loc[LocKey_AvailableIfHost]);
            {
                static size_t s_SelectedValuable;
                size_t nVal = GCheat->ValuablesPool.Size();

                const float spacing2 = 5_px;
                Hax::Vector2 sz = Widgets::CalcButtonSize(g_Loc[LocKey_Spawn]);

                Hax::Gui::BeginHorizontal(spacing2);
                {
                    constexpr size_t dropListId = Hax::Hash(L"ValuableSelect");
                    const float dropListW = Hax::Gui::GetContentRegionAvail().X - spacing2 - sz.X;

                    Hax::Gui::BeginVertical(3_px);
                    {
                        Hax::WStringView preview = L"-";
                        if (s_SelectedValuable < nVal)
                            preview = GCheat->ValuablesPool.begin()[s_SelectedValuable].key;

                        if (Widgets::DropdownBtn(HAX_LINE, preview, dropListW))
                            Widgets::OpenPopup(dropListId, Hax::Gui::GetCursorPos());
                    }
                    Hax::Gui::EndVertical();

                    const float selectableH = Widgets::CalcButtonHeight();
                    const float dropListH = selectableH * Hax::Max(1ULL, Hax::Min(nVal, 8ULL)) + 5_px * 2.f + 1.f;
                    if (Widgets::BeginDropList(dropListId, {dropListW, dropListH}))
                    {
                        for (size_t i = 0; i < nVal; ++i)
                        {
                            if (Widgets::Selectable(HAX_LINE + i * 10000, GCheat->ValuablesPool.begin()[i].key, s_SelectedValuable == i, {.MinW = dropListW}))
                            {
                                s_SelectedValuable = i;
                                Widgets::ClosePopup(dropListId);
                            }
                        }
                        Widgets::EndDropList();
                    }

                    bool enabled = !GCheat->IsClient && s_SelectedValuable < nVal && GCheat->IsInGame;
                    if (Widgets::Button(HAX_LINE, g_Loc[LocKey_Spawn], {}, {.Enabled = enabled}))
                    {
                        int val = _wtoi(GCheat->ValuableValueInputBuf);
                        if (val < 0)
                            val = 0;
                        GCheat->ValuableSpawnValue = val;
                        GCheat->ValuableToSpawn = (GCheat->ValuablesPool.begin() + s_SelectedValuable)->value;
                    }
                }
                Hax::Gui::EndHorizontal();

                Widgets::HorizontalLine(1_px);

                Widgets::MainLabel(g_Loc[LocKey_StartingValue]);
                Widgets::TextInput(HAX_LINE, GCheat->ValuableValueInputBuf, _countof(GCheat->ValuableValueInputBuf),
                    {.Hint = g_Loc[LocKey_ValueHint], .Filter = Widgets::TextInputFilter::Int, .MinW = Hax::Gui::GetContentRegionAvail().X / Hax::Gui::G.ScaleFactor});
            }
            Widgets::EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }

    static void DrawItemsTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = {(mainAreaSize.X - 3 * spacing) / 2, mainAreaSize.Y};

        // Column 1
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_GUNS]);
            {
                {
                    bool enabled = !GCheat->IsClient;
                    Widgets::ToggleEx(HAX_LINE, GCheat->InfBattery, g_Loc[LocKey_MaxBatteryGun], g_Loc[LocKey_HostOnly], {.Disabled = !enabled});
                }

                Widgets::HorizontalLine(1_px);

                {
                   Widgets::ToggleEx(HAX_LINE, GCheat->UseLaser, g_Loc[LocKey_LaserCrosshair]);
                }
            }
            Widgets::EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();

        // Column 2
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_SPAWN], g_Loc[LocKey_AvailableIfHost]);
            {
                static size_t s_SelectedItem;

                const float spacing2 = 5_px;
                Hax::Vector2 sz = Widgets::CalcButtonSize(g_Loc[LocKey_Spawn]);
                Hax::Gui::BeginHorizontal(spacing2);
                {
                    size_t nItems = GCheat->ItemsPool.Size();
                    constexpr size_t dropListId = Hax::Hash(L"ItemSelect");
                    const float dropListW = Hax::Gui::GetContentRegionAvail().X - spacing2 - sz.X;

                    Hax::Gui::BeginVertical(3_px);
                    {
                        Hax::WStringView preview = L"-";
                        if (s_SelectedItem < nItems)
                            preview = GCheat->ItemsPool.begin()[s_SelectedItem].key;

                        if (Widgets::DropdownBtn(HAX_LINE, preview, dropListW))
                            Widgets::OpenPopup(dropListId, Hax::Gui::GetCursorPos());
                    }
                    Hax::Gui::EndVertical();

                    const float selectableH = Widgets::CalcButtonHeight();
                    const float dropListH = selectableH * Hax::Max(1ULL, Hax::Min(nItems, 8ULL)) + 5_px * 2.f + 1.f;
                    if (Widgets::BeginDropList(dropListId, {dropListW, dropListH}))
                    {
                        for (size_t i = 0; i < nItems; ++i)
                        {
                            if (Widgets::Selectable(HAX_LINE + i * 10000, GCheat->ItemsPool.begin()[i].key, s_SelectedItem == i, {.MinW = dropListW}))
                            {
                                s_SelectedItem = i;
                                Widgets::ClosePopup(dropListId);
                            }
                        }
                        Widgets::EndDropList();
                    }

                    bool enabled = !GCheat->IsClient && s_SelectedItem < GCheat->ItemsPool.Size() && GCheat->IsInGame;
                    if (Widgets::Button(HAX_LINE, g_Loc[LocKey_Spawn], {}, {.Enabled = enabled}))
                        GCheat->ItemToSpawn = (StatsManager::instance().itemDictionary().begin() + s_SelectedItem)->Value;
                }
                Hax::Gui::EndHorizontal();
            }
            Widgets::EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }

    static void DrawLevelTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = {(mainAreaSize.X - 3 * spacing) / 2, mainAreaSize.Y};

        // Column 1
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_GENERAL]);
            {
                {
                    RunManager manager = RunManager::instance();
                    int levelsCompl = manager ? manager.levelsCompleted() : 0;

                    Hax::char16 buf[16] = {};
                    int nChars = swprintf_s(buf, _countof(buf), L"%d", levelsCompl);

                    if (nChars > 0)
                    {
                        Hax::WStringView valTxt = { buf, (size_t)nChars };
                        Hax::Vector2 valTxtSize = Widgets::CalcMainLabelSize(valTxt);

                        float spacing = 2_px;
                        Hax::Vector2 btnSize = Widgets::CalcRepeatBtnSize(L"+");

                        Hax::Gui::BeginHorizontal(spacing);
                        {
                            bool disabled = !manager || GCheat->IsClient || !GCheat->IsInGame;

                            Widgets::MainLabelAlignedByH(g_Loc[LocKey_LevelsCompleted], btnSize.Y);
                            Hax::Gui::Space(Hax::Gui::GetContentRegionAvail().X - valTxtSize.X - spacing * 2.f - btnSize.X * 2.f - 8_px);
                            Widgets::MainLabelAlignedByH(buf, btnSize.Y);
                            Hax::Gui::Space(8_px);
                            if (Widgets::RepeatBtn(HAX_LINE, L"-", !disabled && levelsCompl > 0)) manager.levelsCompleted()--;
                            if (Widgets::RepeatBtn(HAX_LINE, L"+", !disabled && levelsCompl < 100)) manager.levelsCompleted()++;
                        }
                        Hax::Gui::EndHorizontal();
                    }
                }

               Widgets::HorizontalLine(1_px);

                bool disabled = GCheat->IsClient || !GCheat->IsInGame;
                if (Widgets::Button(HAX_LINE, g_Loc[LocKey_NextLevel], {}, {.Enabled = !disabled, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                    GCheat->ForceNextLevel = true;
            }
            Widgets::EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();

        // Column 2
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_MAPFILTER], g_Loc[LocKey_AvailableIfHost]);
            {
                Hax::Gui::BeginVertical(5_px);
                {
                    Hax::Gui::BeginHorizontal(5_px);
                    {
                        bool lastMap = GCheat->TotalBans + 1 == GCheat->LevelsBan.Size();
                        for (size_t i = 0; i < GCheat->LevelsBan.Size(); ++i)
                        {
                            LevelBan& ban = GCheat->LevelsBan[i];
                            bool disabled = GCheat->IsClient || lastMap && ban.Allowed;
                            if (Widgets::Selectable(HAX_LINE + i * 10000, ban.Name, !GCheat->IsClient && !ban.Allowed, { .AutoLayout = true, .Disabled = disabled, .R = 5_px }))
                            {
                                ban.Allowed = !ban.Allowed;
                                GCheat->TotalBans += ban.Allowed ? -1 : 1;
                            }
                        }
                    }
                    Hax::Gui::EndHorizontal();
                }
                Hax::Gui::EndVertical();
            }
            Widgets::EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }

    static void DrawMiscTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = {(mainAreaSize.X - 3 * spacing) / 2, mainAreaSize.Y};

        // Column 1
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_TRUCK]);
            {
                {
                    Widgets::ToggleEx(HAX_LINE, GCheat->TruckEsp, g_Loc[LocKey_DisplayThroughWalls]);
                }

               Widgets::HorizontalLine(1_px);

                {
                    TruckSafetySpawnPoint truck = TruckSafetySpawnPoint::instance();
                    PlayerAvatar avatar = PlayerAvatar::instance();
                    bool disabled = !truck || !GCheat->IsInGame || !avatar || avatar.deadSet();
                    if (Widgets::Button(HAX_LINE, g_Loc[LocKey_TeleportTruck], {}, {.Enabled = !disabled, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                        GCheat->TeleportToTruck = true;
                }
            }
            Widgets::EndPanel();

            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_COSMETICS], g_Loc[LocKey_AvailableIfHost]);
            {
                if (Widgets::Button(HAX_LINE, g_Loc[LocKey_UnlockAll], {}, {.MinW = Hax::Gui::GetContentRegionAvail().X}))
                    GCheat->UnlockAllCosmetic = true;

                Widgets::HorizontalLine(1_px);

                Widgets::ToggleEx(HAX_LINE, GCheat->RemoveCosmeticLimit, g_Loc[LocKey_RemoveCosmeticLimit], g_Loc[LocKey_EnableBeforeLevel]);

                Widgets::HorizontalLine(1_px);

                Widgets::ToggleEx(HAX_LINE, GCheat->CosmeticBoxesEsp, g_Loc[LocKey_CosmeticBoxesEsp], g_Loc[LocKey_DisplayThroughWalls]);

                Widgets::HorizontalLine(1_px);

                {
                    bool enabled = !GCheat->IsClient && GCheat->IsInGame;
                    if (Widgets::Button(HAX_LINE, g_Loc[LocKey_TeleportFarthestBox], {}, {.Enabled = enabled, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                        GCheat->TeleportFarthestBox = true;
                }

                Widgets::HorizontalLine(1_px);

                {
                    static size_t s_SelectedRarity;
                    static const LocKey s_RarityKeys[] = {
                        LocKey_RarityCommon,
                        LocKey_RarityUncommon,
                        LocKey_RarityRare,
                        LocKey_RarityUltraRare,
                    };

                    Widgets::MainLabel(g_Loc[LocKey_CosmeticBox]);

                    const float spacing2 = 5_px;
                    Hax::Vector2 sz = Widgets::CalcButtonSize(g_Loc[LocKey_Spawn]);
                    Hax::Gui::BeginHorizontal(spacing2);
                    {
                        constexpr size_t dropListId = Hax::Hash(L"CosmeticBoxSelect");
                        const float dropListW = Hax::Gui::GetContentRegionAvail().X - spacing2 - sz.X;

                        Hax::Gui::BeginVertical(3_px);
                        {
                            if (Widgets::DropdownBtn(HAX_LINE, g_Loc[s_RarityKeys[s_SelectedRarity]], dropListW))
                                Widgets::OpenPopup(dropListId, Hax::Gui::GetCursorPos());
                        }
                        Hax::Gui::EndVertical();

                        const float selectableH = Widgets::CalcButtonHeight();
                        const float dropListH = selectableH * _countof(s_RarityKeys) + 5_px * 2.f + 1.f;
                        if (Widgets::BeginDropList(dropListId, {dropListW, dropListH}))
                        {
                            for (size_t i = 0; i < _countof(s_RarityKeys); ++i)
                            {
                                if (Widgets::Selectable(HAX_LINE + i * 10000, g_Loc[s_RarityKeys[i]], s_SelectedRarity == i, {.MinW = dropListW}))
                                {
                                    s_SelectedRarity = i;
                                    Widgets::ClosePopup(dropListId);
                                }
                            }
                            Widgets::EndDropList();
                        }

                        bool enabled = !GCheat->IsClient && GCheat->IsInGame;
                        if (Widgets::Button(HAX_LINE, g_Loc[LocKey_Spawn], {}, {.Enabled = enabled}))
                            GCheat->CosmeticBoxToSpawn = (int)s_SelectedRarity;
                    }
                    Hax::Gui::EndHorizontal();
                }
            }
            Widgets::EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }

    static void DrawSettingsTab()
    {
        const Hax::Vector2 mainAreaSize = Hax::Gui::GetContentRegionAvail();
        const float spacing = 20_px;
        const Hax::Vector2 columnSize = {(mainAreaSize.X - 3 * spacing) / 2, mainAreaSize.Y};

        // Column 1
        Hax::Gui::Space(spacing);
        Hax::Gui::BeginContainer(0, {.W = columnSize.X, .H = columnSize.Y});
        Hax::Gui::BeginVertical(spacing);
        Hax::Gui::Dummy({0.f, 0.f});
        {
            Widgets::BeginPanel(HAX_LINE);
            Widgets::PanelHeader(g_Loc[LocKey_SETTINGS]);
            {
                {
                    Widgets::MainLabel(g_Loc[LocKey_Language]);

                    constexpr size_t dropListId = Hax::Hash(L"LanguageSelect");
                    static Hax::WStringView s_Languages[] = { L"English", L"Русский" };
                    const float w = Hax::Gui::GetContentRegionAvail().X;
                    Hax::Gui::BeginVertical(3_px);
                    if (Widgets::DropdownBtn(HAX_LINE, s_Languages[GCheat->Lang], w))
                        Widgets::OpenPopup(dropListId, Hax::Gui::GetCursorPos());
                    Hax::Gui::EndVertical();

                    const float selectableH = Widgets::CalcButtonHeight();
                    const float dropListH = selectableH * Hax::Max(1ULL, Hax::Min(_countof(s_Languages), 5ULL)) + 5_px * 2.f + 1.f;
                    if (Widgets::BeginDropList(dropListId, { w, dropListH }))
                    {
                        if (Widgets::Selectable(HAX_LINE, s_Languages[0], GCheat->Lang == Lang_Eng, { .MinW = w })) { g_Loc = g_Dicts[Lang_Eng]; GCheat->Lang = Lang_Eng; }
                        if (Widgets::Selectable(HAX_LINE, s_Languages[1], GCheat->Lang == Lang_Ru, {.MinW = w})) { g_Loc = g_Dicts[Lang_Ru]; GCheat->Lang = Lang_Ru; }
                        Widgets::EndDropList();
                    }
                }

                Widgets::HorizontalLine(1_px);

                {
                   Widgets::HotkeyEx(HAX_LINE, GCheat->VkOpenClose, g_Loc[LocKey_Hotkey], g_Loc[LocKey_OpenCloseMenu]);
                }

                Widgets::HorizontalLine(1_px);

                {
                    Widgets::ToggleEx(HAX_LINE, GCheat->DarkenBg, g_Loc[LocKey_DarkenBackground]);
                }
            }
            Widgets::EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }

    static void DrawBackground()
    {
        bool shouldDrawEsp = GCheat->IsInGame && MenuManager::instance() && !MenuManager::instance().currentMenuPage();
        if (!shouldDrawEsp)
            return;

        Hax::Vector<EnemyEspData>& buf1 = GCheat->EnemiesEspBuffer.GetFront();

        if (GCheat->EnemiesEsp)
            std::sort(buf1.begin(), buf1.end(), [](const EnemyEspData& d1, const EnemyEspData& d2) { return d1.Distance > d2.Distance; });

        Hax::Vector<ValuableEspData>& buf2 = GCheat->ValuablesEspBuffer.GetFront();
        if (GCheat->ValuablesEsp)
            std::sort(buf2.begin(), buf2.end(), [](const ValuableEspData& d1, const ValuableEspData& d2) { return d1.Distance > d2.Distance; });

        Hax::Vector<ExtrPointEspData>& buf3 = GCheat->ExtrPointsEspBuffer.GetFront();
        if (GCheat->ExtrPointsEsp)
            std::sort(buf3.begin(), buf3.end(), [](const ExtrPointEspData& d1, const ExtrPointEspData& d2) { return d1.Distance > d2.Distance; });

        Hax::Vector<TruckEspData>& buf4 = GCheat->TruckEspBuffer.GetFront();
        Hax::Vector<PlayerEspData>& buf5 = GCheat->PlayersEspBuffer.GetFront();
        if (GCheat->PlayersEsp)
            std::sort(buf5.begin(), buf5.end(), [](const PlayerEspData& d1, const PlayerEspData& d2) { return d1.Distance > d2.Distance; });

        Hax::Vector<CosmeticBoxEspData>& buf6 = GCheat->CosmeticBoxesEspBuffer.GetFront();
        if (GCheat->CosmeticBoxesEsp)
            std::sort(buf6.begin(), buf6.end(), [](const CosmeticBoxEspData& d1, const CosmeticBoxEspData& d2) { return d1.Distance > d2.Distance; });

        size_t i1 = GCheat->EnemiesEsp ? 0 : 9999;
        size_t i2 = GCheat->ValuablesEsp ? 0 : 9999;
        size_t i3 = GCheat->ExtrPointsEsp ? 0 : 9999;
        size_t i4 = GCheat->TruckEsp ? 0 : 9999;
        size_t i5 = GCheat->PlayersEsp ? 0 : 9999;
        size_t i6 = GCheat->CosmeticBoxesEsp ? 0 : 9999;
        while (i1 < buf1.Size() || i2 < buf2.Size() || i3 < buf3.Size() || i4 < buf4.Size() || i5 < buf5.Size() || i6 < buf6.Size())
        {
            float d1 = i1 < buf1.Size() ? buf1[i1].Distance : FLT_MIN;
            float d2 = i2 < buf2.Size() ? buf2[i2].Distance : FLT_MIN;
            float d3 = i3 < buf3.Size() ? buf3[i3].Distance : FLT_MIN;
            float d4 = i4 < buf4.Size() ? buf4[i4].Distance : FLT_MIN;
            float d5 = i5 < buf5.Size() ? buf5[i5].Distance : FLT_MIN;
            float d6 = i6 < buf6.Size() ? buf6[i6].Distance : FLT_MIN;

            if (d1 >= d2 && d1 >= d3 && d1 >= d4 && d1 >= d5 && d1 >= d6)
                DrawEnemyEsp(buf1[i1++]);
            else if (d2 >= d1 && d2 >= d3 && d2 >= d4 && d2 >= d5 && d2 >= d6)
                DrawValuableEsp(buf2[i2++]);
            else if (d3 >= d1 && d3 >= d2 && d3 >= d4 && d3 >= d5 && d3 >= d6)
                DrawExtrPointEsp(buf3[i3++]);
            else if (d4 >= d1 && d4 >= d2 && d4 >= d3 && d4 >= d5 && d4 >= d6)
                DrawTruckEsp(buf4[i4++]);
            else if (d5 >= d1 && d5 >= d2 && d5 >= d3 && d5 >= d4 && d5 >= d6)
                DrawPlayerEsp(buf5[i5++]);
            else
                DrawCosmeticBoxEsp(buf6[i6++]);
        }
    }

    static void DrawEnemyEsp(EnemyEspData& data)
    {
        float fontH = 9_px;

        Box(data.Box, 0xFF0000FF);
        Text(GCheat->NunitoSans_Bold, data.Name, Hax::Vector2(data.Box.GetCenter().X, data.Box.Min.Y - 2.f), 0xFFFFFFFF, fontH, VerticalAlignment_Top, HorizontalAlignment_Center);
        VertHealthBar(data.Box.Min.X - 10.f, data.Box.Min.Y, 3.f, data.Box.GetSize().Y, data.CurHp, data.MaxHp);

        wchar_t buf[16] = {0};
        swprintf_s(buf, L"%d", data.CurHp);
        Hax::Gui::Color lerped = Hax::Lerp(Hax::Gui::Color(0xFF0000FF), Hax::Gui::Color(0x00FF00FF), (float)data.CurHp / data.MaxHp);
        Text(GCheat->NunitoSans_Bold, buf, Hax::Vector2(data.Box.Min.X - 15.f, data.Box.Min.Y), lerped, fontH, VerticalAlignment_Bottom, HorizontalAlignment_Left);
    }

    static void DrawValuableEsp(ValuableEspData& data)
    {
        Hax::WStringView s = data.Name;
        if (s.EndsWith(L"(Clone)"))
            s.RemoveSuffix(7);
        if (data.Name.StartsWith(L"Valuable "))
            s.RemovePrefix(9);

        wchar_t buf[64] = {0};
        if (swprintf_s(buf, L"%.*ls | %.0f$", (int)s.Length(), s.begin(), data.Value) > 0)
        {
            Text(GCheat->NunitoSans_Bold, buf, data.Pos, 0xFFFF00FF, EspFontSize(), VerticalAlignment_Bottom, HorizontalAlignment_Center);
            return;
        }

        Text(GCheat->NunitoSans_Bold, s, data.Pos, 0xFF00FFFF, EspFontSize(), VerticalAlignment_Bottom, HorizontalAlignment_Center);
    }

    static void DrawExtrPointEsp(ExtrPointEspData& data)
    {
        Hax::Gui::Color color = data.Active ? 0x00FF00FF : data.Completed ? 0x545454FF : 0xFF0000FF;
        Text(GCheat->Icons_Solid, L"\uF468", data.Pos, color, IconFontSize());
    }

    static void DrawTruckEsp(TruckEspData& data)
    {
        Hax::Gui::Color color = 0x00FF00FF;
        Text(GCheat->Icons_Solid, L"\uF0D1", data.Pos, color, IconFontSize());
    }

    static void DrawCosmeticBoxEsp(CosmeticBoxEspData& data)
    {
        static constexpr Hax::Gui::Color rarityColors[] = {
            0xBFBFBFFF,   // Common  - light gray
            0x4DE94CFF,   // Uncommon - green
            0x4D9CFFFF,   // Rare    - blue
            0xC44DFFFF,   // UltraRare - purple
        };
        static constexpr Hax::WStringView rarityLabels[] = {
            L"Common",
            L"Uncommon",
            L"Rare",
            L"Ultra rare",
        };

        int r = data.Rarity;
        if (r < 0 || r >= (int)_countof(rarityColors))
            r = 0;

        Hax::Gui::Color color = rarityColors[r];

        wchar_t buf[48] = {};
        if (swprintf_s(buf, L"%.*ls | %.0fm", (int)rarityLabels[r].Length(), rarityLabels[r].begin(), data.Distance) > 0)
        {
            Text(GCheat->NunitoSans_Bold, buf, data.Pos, color, EspFontSize(), VerticalAlignment_Bottom, HorizontalAlignment_Center);
            return;
        }

        Text(GCheat->NunitoSans_Bold, rarityLabels[r], data.Pos, color, EspFontSize(), VerticalAlignment_Bottom, HorizontalAlignment_Center);
    }

    static void DrawPlayerEsp(PlayerEspData& data)
    {
        Box(data.Box, data.Dead ? 0x545454FF : 0x00FF00FF);
        Text(GCheat->NunitoSans_Bold, data.Name, Hax::Vector2(data.Box.GetCenter().X, data.Box.Min.Y - 2.f), 0xFFFFFFFF, EspFontSize(), VerticalAlignment_Top, HorizontalAlignment_Center);
        if (!data.Dead)
        {
            VertHealthBar(data.Box.Min.X - 10_px, data.Box.Min.Y, 3_px, data.Box.GetSize().Y, data.CurHp, data.MaxHp);

            wchar_t buf[16] = {};
            swprintf_s(buf, L"%d", data.CurHp);
            Hax::Gui::Color lerped = Hax::Lerp(Hax::Gui::Color(0xFF0000FF), Hax::Gui::Color(0x00FF00FF), (float)data.CurHp / data.MaxHp);
            Text(GCheat->NunitoSans_Bold, buf, Hax::Vector2(data.Box.Min.X - 15_px, data.Box.Min.Y), lerped, EspFontSize(), VerticalAlignment_Bottom, HorizontalAlignment_Left);
        }
    }

    /*static void CornerBox(float x, float y, float width, float height, const Hax::Gui::Color& color, float thickness, float cornerProp)
    {
        float yOffset = height * cornerProp;
        float xOffset = width * cornerProp;
        float px2 = 2_px;

        Hax::Vector2 tl = {x, y};
        Hax::Vector2 tr = {x + width, y};
        Hax::Vector2 br = {x + width, y + height};
        Hax::Vector2 bl = {x, y + height};

        Hax::Gui::DrawLine(tl, tr, {.FillColor = 0x000000FF, .Th = 3_px});
        Hax::Gui::DrawLine(tr, br, {.FillColor = 0x000000FF, .Th = 3_px});
        Hax::Gui::DrawLine(bl, br, {.FillColor = 0x000000FF, .Th = 3_px});
        Hax::Gui::DrawLine(tl, bl, {.FillColor = 0x000000FF, .Th = 3_px});

        Hax::Gui::DrawLine(tl, tr, {.FillColor = Hax::Gui::Color::Red, .Th = 1_px});
        Hax::Gui::DrawLine(tr, br, {.FillColor = Hax::Gui::Color::Red, .Th = 1_px});
        Hax::Gui::DrawLine(bl, br, {.FillColor = Hax::Gui::Color::Red, .Th = 1_px});
        Hax::Gui::DrawLine(tl, bl, {.FillColor = Hax::Gui::Color::Red, .Th = 1_px});
    }*/

    static void Box(const Hax::Rect& rect, const Hax::Gui::Color& col)
    {
        float px2 = 2_px;

        Hax::Vector2 tl = rect.GetTL();
        Hax::Vector2 tr = rect.GetTR();
        Hax::Vector2 br = rect.GetBR();
        Hax::Vector2 bl = rect.GetBL();

        Hax::Gui::DrawLine(tl, tr, {.FillColor = 0x000000FF, .Th = 3_px});
        Hax::Gui::DrawLine(tr, br, {.FillColor = 0x000000FF, .Th = 3_px});
        Hax::Gui::DrawLine(bl, br, {.FillColor = 0x000000FF, .Th = 3_px});
        Hax::Gui::DrawLine(tl, bl, {.FillColor = 0x000000FF, .Th = 3_px});

        Hax::Gui::DrawLine(tl, tr, {.FillColor = Hax::Gui::Color::Red, .Th = 1_px});
        Hax::Gui::DrawLine(tr, br, {.FillColor = Hax::Gui::Color::Red, .Th = 1_px});
        Hax::Gui::DrawLine(bl, br, {.FillColor = Hax::Gui::Color::Red, .Th = 1_px});
        Hax::Gui::DrawLine(tl, bl, {.FillColor = Hax::Gui::Color::Red, .Th = 1_px});
    }

    static void Text(Hax::Gui::FontHandle hFont,
        Hax::WStringView text,
        const Hax::Vector2& pos,
        const Hax::Gui::Color& col,
        float fontSize,
        VerticalAlignment vAlign,
        HorizontalAlignment hAlign)
    {
        Hax::Vector2 textSize = Hax::Gui::CalcTextSize(hFont, text, fontSize);
        float xShift = hAlign == HorizontalAlignment_Right ? 0.f : textSize.X / (float)hAlign;
        float yShift = vAlign == VerticalAlignment_Bottom ? 0.f : textSize.Y / (float)vAlign;

        Hax::Gui::DrawString(hFont, text, Hax::Vector2(pos.X - xShift + 1.F, pos.Y - yShift - 1.F), fontSize, {.Color = 0x000000FF});
        Hax::Gui::DrawString(hFont, text, Hax::Vector2(pos.X - xShift + 1.F, pos.Y - yShift + 1.F), fontSize, {.Color = 0x000000FF});
        Hax::Gui::DrawString(hFont, text, Hax::Vector2(pos.X - xShift - 1.F, pos.Y - yShift + 1.F), fontSize, {.Color = 0x000000FF});
        Hax::Gui::DrawString(hFont, text, Hax::Vector2(pos.X - xShift - 1.F, pos.Y - yShift - 1.F), fontSize, {.Color = 0x000000FF});
        Hax::Gui::DrawString(hFont, text, Hax::Vector2(pos.X - xShift, pos.Y - yShift), fontSize, {.Color = col});
    }

    static void VertHealthBar(float x, float y, float width, float height, float cur, float max)
    {
        float coef = Hax::Clamp(cur / max, 0.f, 1.f);
        Hax::Gui::Color lerped = Hax::Lerp(Hax::Gui::Color(0xFF0000FF), Hax::Gui::Color(0x00FF00FF), coef);
        Hax::Gui::DrawLine({x, y}, {x, y + height}, {.FillColor = 0x000000FF, .Th = width + 2_px});
        Hax::Gui::DrawLine({x, y + height - height * coef}, {x, y + height}, {.FillColor = lerped, .Th = width});
    }

    static void VertHealthBar(float x, float y, float width, float height, int cur, int max)
    {
        VertHealthBar(x, y, width, height, (float)cur, (float)max);
    }

    static float EspFontSize() { return 9_px; }
    static float IconFontSize() { return 13_px; }

    static bool IsPlayerAlive()
    {
        PlayerAvatar avatar = PlayerAvatar::instance();
        if (!avatar)
            return false;

        return !avatar.deadSet() && !avatar.isDisabled();
    }

    static bool IsPlayerFullHp()
    {
        if (!IsPlayerAlive())
            return true;

        PlayerHealth health = PlayerAvatar::instance().playerHealth();
        return health.health() >= health.maxHealth();
    }
}