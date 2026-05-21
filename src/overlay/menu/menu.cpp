#include <src/pch.h>

#include "menu.h"

#include <src/cheat.h>
#include <src/resource.h>

#include "localization.h"
#include "widgets.h"
#include "tabs/tabs.h"

namespace Cheat
{
    struct Tab
    {
        void(*DrawFunc)(void);
        LocKey Loc;
        Hax::char16 Icon;
    };

    static const Tab g_Tabs[] = 
    {
        {DrawStatsTab, LocKey_Stats, L'\ue473'},
        {DrawEntitiesTab, LocKey_Entities, L'\uf8f5'},
        {DrawVisionTab, LocKey_Vision, L'\uf06e'},
        {DrawValuablesTab, LocKey_Valuables, L'\uf81d'},
        {DrawItemsTab, LocKey_Items, L'\uf1e2'},
        {DrawLevelTab, LocKey_Level, L'\uf5fd'},
        {DrawCosmeticTab, LocKey_Cosmetic, L'\ue136'},
        {DrawMiscTab, LocKey_Misc, L'\uf86d'},
        {DrawSettingsTab, LocKey_Settings, L'\uf013'}
    };
    static size_t g_ActiveTab = 0;

    void ToggleMenuVisibility()
    {
        ::PostMessageW(G->GameWndHandle, WM_USER, 0, 0);
    }

    void PrepareMenu()
    {
        Hax::Gui::CreateLayer(L"Background", -1);
        Hax::Gui::CreateLayer(L"Popups", 1);

        Hax::Handle hCheat = (Hax::Handle)G->Handle;

        // Images
        {
            G->Logo                                          = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG1, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Rest].Icon     = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG2, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Jump].Icon     = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG3, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Range].Icon    = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG8, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Speed].Icon    = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG9, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Stamina].Icon  = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG10, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Strength].Icon = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG11, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Tumble].Icon   = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG12, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Wings].Icon    = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG13, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Health].Icon   = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG14, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Map].Icon      = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG15, L"PNG"));
            Hax::Gui::TextureHandle unknownIcon              = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG7, L"PNG"));
            G->UpgradesData[(int)UpgradeType::Climb].Icon    = unknownIcon;
            G->UpgradesData[(int)UpgradeType::Battery].Icon  = unknownIcon;
            G->UpgradesData[(int)UpgradeType::N].Icon        = unknownIcon;
            G->AidsData[(int)AidType::Small].Icon            = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG6, L"PNG"));
            G->AidsData[(int)AidType::Medium].Icon           = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG5, L"PNG"));
            G->AidsData[(int)AidType::Large].Icon            = Hax::Gui::LoadImageFromMemory(Hax::Gui::GetResourceData(hCheat, IDB_PNG4, L"PNG"));
        }

        // Fonts
        {
            G->NunitoSans_SemiBold  = Hax::Gui::LoadFont(Hax::Gui::GetResourceData(hCheat, IDR_WOFF21, L"WOFF2"));
            G->NunitoSans_Bold      = Hax::Gui::LoadFont(Hax::Gui::GetResourceData(hCheat, IDR_WOFF22, L"WOFF2"));
            G->NunitoSans_ExtraBold = Hax::Gui::LoadFont(Hax::Gui::GetResourceData(hCheat, IDR_WOFF23, L"WOFF2"));
            G->Icons_Solid          = Hax::Gui::LoadFont(Hax::Gui::GetResourceData(hCheat, IDR_WOFF24, L"WOFF2"));
        }

        // Mouse textures
        {
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_Arrow, hCheat, IDC_CURSOR7);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_TextInput, hCheat, IDC_CURSOR1);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeAll, hCheat, IDC_CURSOR6);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeNS, hCheat, IDC_CURSOR9);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeEW, hCheat, IDC_CURSOR4);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeNESW, hCheat, IDC_CURSOR3);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_ResizeNWSE, hCheat, IDC_CURSOR2);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_Hand, hCheat, IDC_CURSOR5);
            Hax::Gui::SetMouseTextureFromRes(Hax::Gui::MouseIcon_NotAllowed, hCheat, IDC_CURSOR8);
        }
    }

    static bool IsKeyJustDown(size_t id, Hax::uint8 vk)
    {
        return (G->KeyListenerId == 0 || G->KeyListenerId == id) && Hax::Gui::IsKeyJustDown(vk);
    }

    void RenderMenu()
    {
        BeginWindow();
        {
            if (BeginSidePanel())
            {
                const float sidePanelWidth = Hax::Gui::GetContentRegionAvail().X;

                Hax::Gui::Space(30_px);

                // Logo
                {
                    Hax::Gui::BeginHorizontal();
                    {
                        Hax::Vector2 customSize = Hax::Gui::GetImageSize(G->Logo) / 2.f;
                        Hax::Gui::Space((sidePanelWidth - customSize.X) / 2.f);
                        Image(G->Logo, customSize);
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
                                if (TabButton(HAX_LINE + i, G->Loc[g_Tabs[i].Loc], g_Tabs[i].Icon, g_ActiveTab == i))
                                    g_ActiveTab = i;

                            Hax::Gui::Space(7_px);
                            Hax::Gui::HorizontalLine(1_px, 0x2F3033FF);
                            Hax::Gui::Space(7_px);

                            for (size_t i = 3; i < 8; ++i)
                                if (TabButton(HAX_LINE + i, G->Loc[g_Tabs[i].Loc], g_Tabs[i].Icon, g_ActiveTab == i))
                                    g_ActiveTab = i;

                            Hax::Gui::Space(7_px);
                            Hax::Gui::HorizontalLine(1_px, 0x2F3033FF);
                            Hax::Gui::Space(7_px);

                            if (TabButton(HAX_LINE + 8, G->Loc[g_Tabs[8].Loc], g_Tabs[8].Icon, g_ActiveTab == 8))
                                g_ActiveTab = 8;
                        }
                        Hax::Gui::EndVertical();
                    }
                    Hax::Gui::EndContainer();
                }
                Hax::Gui::EndHorizontal();

                EndSidePanel();
            }

            // Main area
            Hax::Gui::BeginContainer(Hax::Hash("Main area"), {.Clip = true, .ScrollY = true});
            {
                Hax::Gui::BeginHorizontal();
                g_Tabs[g_ActiveTab].DrawFunc();
                Hax::Gui::EndHorizontal();
            }
            Hax::Gui::EndContainer();
        }
        EndWindow();
    }
}