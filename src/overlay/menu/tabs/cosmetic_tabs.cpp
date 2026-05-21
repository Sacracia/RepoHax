#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"

#define LINE_ID (HAX_LINE * 789)

namespace Cheat
{
    void DrawCosmeticTab()
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
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_GENERAL]);
            {
                ToggleEx(LINE_ID, G->CosmeticBoxesEsp, G->Loc[LocKey_DisplayThroughWalls]);

                HorizontalLine(1_px);

                Hax::Gui::BeginHorizontal(5_px);
                {
                    const float btnW = (Hax::Gui::GetContentRegionAvail().X - 5_px) / 2.f;
                    if (Button(LINE_ID, G->Loc[LocKey_UnlockAll], {}, {.MinW = btnW}))
                        G->UnlockAllCosmetic = true;
                    if (Button(LINE_ID, G->Loc[LocKey_ResetAll], {}, {.MinW = btnW}))
                        G->ResetAllCosmetic = true;
                }
                Hax::Gui::EndHorizontal();
                
            }
            EndPanel();
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
            BeginPanel(LINE_ID);
            PanelHeader(G->Loc[LocKey_SPAWN], G->Loc[LocKey_AvailableIfHost]);
            {
                const float px5 = 5_px;
                Hax::Gui::BeginVertical(px5);
                {
                    bool enabled = G->IsInGame && !G->IsClient;
                    const float btnW = (Hax::Gui::GetContentRegionAvail().X - px5) / 2.f;
                    Hax::Gui::BeginHorizontal(px5);
                    {
                        if (Button(LINE_ID, G->Loc[LocKey_Common], {}, {.Enabled = enabled, .MinW = btnW}))
                            G->RarityToSpawn = 1;
                        if (Button(LINE_ID, G->Loc[LocKey_Uncommon], {}, {.Enabled = enabled, .MinW = btnW}))
                            G->RarityToSpawn = 2;
                    }
                    Hax::Gui::EndHorizontal();

                    Hax::Gui::BeginHorizontal(px5);
                    {
                        if (Button(LINE_ID, G->Loc[LocKey_Rare], {}, {.Enabled = enabled, .MinW = btnW}))
                            G->RarityToSpawn = 3;
                        if (Button(LINE_ID, G->Loc[LocKey_UltraRare], {}, {.Enabled = enabled, .MinW = btnW}))
                            G->RarityToSpawn = 4;
                    }
                    Hax::Gui::EndHorizontal();
                }
                Hax::Gui::EndVertical();
            }
            EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }
}