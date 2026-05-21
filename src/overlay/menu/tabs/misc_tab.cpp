#include <src/pch.h>

#include <src/cheat.h>

#include "../localization.h"
#include "../widgets.h"

#define LINE_ID (HAX_LINE * 456)

namespace Cheat
{
    void DrawMiscTab()
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
            PanelHeader(G->Loc[LocKey_TRUCK]);
            {
                {
                    ToggleEx(LINE_ID, G->TruckEsp, G->Loc[LocKey_DisplayThroughWalls]);
                }

                HorizontalLine(1_px);

                {
                    TruckSafetySpawnPoint truck = TruckSafetySpawnPoint::instance();
                    PlayerAvatar avatar = PlayerAvatar::instance();
                    bool disabled = !truck || !G->IsInGame || !avatar || avatar.deadSet();
                    if (Button(LINE_ID, G->Loc[LocKey_TeleportTruck], {}, {.Enabled = !disabled, .MinW = Hax::Gui::GetContentRegionAvail().X}))
                        G->TeleportToTruck = true;
                }
            }
            EndPanel();
        }
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }
}