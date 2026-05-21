#include <src/pch.h>

#include "esp.h"

#include <src/cheat.h>

namespace Cheat
{
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

    static void DrawEnemyEsp(EnemyEspData& data);
    static void DrawValuableEsp(ValuableEspData& data);
    static void DrawExtrPointEsp(ExtrPointEspData& data);
    static void DrawTruckEsp(TruckEspData& data);
    static void DrawPlayerEsp(PlayerEspData& data);
    static void DrawCosmeticBoxEsp(CosmeticBoxEspData& data);
    static void Box(const Hax::Rect& rect, const Hax::Gui::Color& col);
    static void Text(Hax::Gui::FontHandle hFont,
                     Hax::WStringView text,
                     Hax::Vector2 pos,
                     Hax::Gui::Color col,
                     float fontSize,
                     VerticalAlignment vAlign = VerticalAlignment_Bottom,
                     HorizontalAlignment hAlign = HorizontalAlignment_Right);
    static float EspFontSize() { return 9_px; }
    static float IconFontSize() { return 14_px; }
    static void VertHealthBar(float x, float y, float width, float height, float cur, float max);
    static void VertHealthBar(float x, float y, float width, float height, int cur, int max);
    static UnityEngine::Rect CalcBoundsInScreenSpace(const UnityEngine::Bounds& bigBounds, UnityEngine::Camera cam);

    void RenderEsp()
    {
        bool shouldDrawEsp = G->IsInGame && MenuManager::instance() && !MenuManager::instance().currentMenuPage();
        if (!shouldDrawEsp)
            return;

        size_t i1 = 9999;
        size_t i2 = 9999;
        size_t i3 = 9999;
        size_t i4 = 9999;
        size_t i5 = 9999;
        size_t i6 = 9999;

        Hax::Vector<EnemyEspData>& buf1 = G->EnemiesEspBuffer.GetFront();
        Hax::Vector<ValuableEspData>& buf2 = G->ValuablesEspBuffer.GetFront();
        Hax::Vector<ExtrPointEspData>& buf3 = G->ExtrPointsEspBuffer.GetFront();
        Hax::Vector<TruckEspData>& buf4 = G->TruckEspBuffer.GetFront();
        Hax::Vector<PlayerEspData>& buf5 = G->PlayersEspBuffer.GetFront();
        Hax::Vector<CosmeticBoxEspData>& buf6 = G->CosmeticBoxesEspBuffer.GetFront();

        if (G->EnemiesEsp)
        {
            i1 = 0;
            std::sort(buf1.begin(), buf1.end(), [](const EnemyEspData& d1, const EnemyEspData& d2) { return d1.Distance > d2.Distance; });
        }
        if (G->ValuablesEsp)
        {
            i2 = 0;
            std::sort(buf2.begin(), buf2.end(), [](const ValuableEspData& d1, const ValuableEspData& d2) { return d1.Distance > d2.Distance; });
        }
        if (G->ExtrPointsEsp)
        {
            i3 = 0;
            std::sort(buf3.begin(), buf3.end(), [](const ExtrPointEspData& d1, const ExtrPointEspData& d2) { return d1.Distance > d2.Distance; });
        }
        if (G->PlayersEsp)
        {
            i5 = 0;
            std::sort(buf5.begin(), buf5.end(), [](const PlayerEspData& d1, const PlayerEspData& d2) { return d1.Distance > d2.Distance; });
        }
        if (G->TruckEsp)
        {
            i4 = 0;
        }
        if (G->CosmeticBoxesEsp)
        {
            i6 = 0;
            std::sort(buf6.begin(), buf6.end(), [](const CosmeticBoxEspData& d1, const CosmeticBoxEspData& d2) { return d1.Distance > d2.Distance; });
        }

        while (i1 < buf1.Size() || i2 < buf2.Size() || i3 < buf3.Size() || i4 < buf4.Size() || i5 < buf5.Size() || i6 < buf6.Size())
        {
            float d1 = i1 < buf1.Size() ? buf1[i1].Distance : FLT_MIN;
            float d2 = i2 < buf2.Size() ? buf2[i2].Distance : FLT_MIN;
            float d3 = i3 < buf3.Size() ? buf3[i3].Distance : FLT_MIN;
            float d4 = i4 < buf4.Size() ? buf4[i4].Distance : FLT_MIN;
            float d5 = i5 < buf5.Size() ? buf5[i5].Distance : FLT_MIN;
            float d6 = i6 < buf6.Size() ? buf6[i6].Distance : FLT_MIN;

            if (d1 >= d2 && d1 >= d3 && d1 >= d4 && d1 >= d5 && d1 >= d6)       { DrawEnemyEsp(buf1[i1++]);     }
            else if (d2 >= d1 && d2 >= d3 && d2 >= d4 && d2 >= d5 && d2 >= d6)  { DrawValuableEsp(buf2[i2++]);  }
            else if (d3 >= d1 && d3 >= d2 && d3 >= d4 && d3 >= d5 && d3 >= d6)  { DrawExtrPointEsp(buf3[i3++]); }
            else if (d4 >= d1 && d4 >= d2 && d4 >= d3 && d4 >= d5 && d4 >= d6)  { DrawTruckEsp(buf4[i4++]);     }
            else if (d5 >= d1 && d5 >= d2 && d5 >= d3 && d5 >= d4 && d5 >= d6)  { DrawPlayerEsp(buf5[i5++]);    }
            else                                                                { DrawCosmeticBoxEsp(buf6[i6++]); }
        }
    }

    Hax::Optional<EnemyEspData> ParseEnemyEspData(Enemy enemy)
    {
        UnityEngine::Camera mainCam = GameDirector::instance().MainCamera();
        if (!mainCam || !enemy)
            return {};

        float dist = mainCam.GetTransform().GetPosition().Distance(enemy.GetTransform().GetPosition());
        if (dist < 2.f)
            return {};

        EnemyParent parent = enemy.enemyParent();
        if (parent.enemyName() == L"Spewer")
        {
            EnemySlowMouth comp = enemy.GetComponent<EnemySlowMouth>();
            if (comp && comp.attachedTimer() > 0.f)
                return {};
        }

        UnityEngine::Bounds enemyBounds{enemy.CenterTransform().GetPosition(), UnityEngine::Vector3()};
        if (EnemyRigidbody rb = enemy.Rigidbody())
        {
            PhysGrabObject grabObj = rb.physGrabObject();
            enemyBounds = UnityEngine::Bounds(grabObj.centerPoint(), grabObj.boundingBox());
        }
        else
        {
            enemyBounds = enemy.GetTransform().GetChild(1).GetChild(0).GetComponent<UnityEngine::Collider>().GetBounds();
        }

        UnityEngine::Rect bounds = CalcBoundsInScreenSpace(enemyBounds, mainCam);
        UnityEngine::Rect screenRect{0.f, 0.f, G->ScreenWidth, G->ScreenHeight};

        if (!bounds.Overlaps(screenRect))
            return {};

        EnemyHealth health = enemy.Health();

        return EnemyEspData
        {
            .Box = bounds.ToHax(),
            .Name = enemy.enemyParent().enemyName().ToView(),
            .Distance = dist,
            .CurHp = health.healthCurrent(),
            .MaxHp = health.health(),
        };
    }

    Hax::Optional<ValuableEspData> ParseValuableEspData(ValuableObject obj)
    {
        if (!obj || !obj.GetEnabled())
            return {};

        UnityEngine::Camera cam = GameDirector::instance().MainCamera();
        float dist = cam.GetTransform().GetPosition().Distance(obj.GetTransform().GetPosition());
        if (dist < 2.f || dist > (float)G->ValuablesEspRange)
            return {};

        PhysGrabObject physObject = obj.physGrabObject();
        if (!physObject)
            return {};

        UnityEngine::Vector3 worldPos = physObject.midPoint();
        UnityEngine::Vector3 screenPos = cam.WorldToScreenPoint(worldPos);
        UnityEngine::Rect screenRect{0.f, 0.f, G->ScreenWidth, G->ScreenHeight};
        if (screenPos.z <= 0 || !screenRect.Contains(screenPos))
            return {};

        float scaleX = G->ScreenWidth / G->PixelWidth;
        float scaleY = G->ScreenHeight / G->PixelHeight;

        screenPos.x *= scaleX;
        screenPos.y = G->ScreenHeight - screenPos.y * scaleY;

        return ValuableEspData
        {
            .Name = obj.GetName().ToView(),
            .Pos = screenPos.ToVector2().ToHax(),
            .Distance = dist,
            .Value = obj.dollarValueCurrent()
        };
    }

    Hax::Optional<ExtrPointEspData> ParseExtrPointEspData(UnityEngine::GameObject obj)
    {
        UnityEngine::Camera cam = GameDirector::instance().MainCamera();

        if (!obj || !cam)
            return {};

        ExtractionPoint point = obj.GetComponent<ExtractionPoint>();
        if (!point)
            return {};

        float dist = cam.GetTransform().GetPosition().Distance(obj.GetTransform().GetPosition());
        if (dist < 1.f)
            return {};

        UnityEngine::Vector3 screenPos = cam.WorldToScreenPoint(obj.GetTransform().GetPosition());
        UnityEngine::Rect screenRect{0.f, 0.f, G->ScreenWidth, G->ScreenHeight};
        if (screenPos.z <= 0 || !screenRect.Contains(screenPos))
            return {};

        float scaleX = G->ScreenWidth / G->PixelWidth;
        float scaleY = G->ScreenHeight / G->PixelHeight;

        screenPos.x *= scaleX;
        screenPos.y = G->ScreenHeight - screenPos.y * scaleY;

        return ExtrPointEspData 
        {
            .Pos = screenPos.ToVector2().ToHax(),
            .Distance = dist,
            .Completed = (point.currentState() == ExtractionPoint_State::Complete()),
            .Active = (point == RoundDirector::instance().extractionPointCurrent())
        };
    }

    Hax::Optional<TruckEspData> ParseTruckEspData(TruckSafetySpawnPoint truck)
    {
        UnityEngine::Camera cam = GameDirector::instance().MainCamera();

        if (!truck || !cam)
            return {};

        UnityEngine::Vector3 worldPos = truck.GetTransform().GetPosition();

        float dist = cam.GetTransform().GetPosition().Distance(worldPos);
        if (dist < 1.f)
            return {};

        UnityEngine::Vector3 screenPos = cam.WorldToScreenPoint(worldPos);
        UnityEngine::Rect screenRect{0.f, 0.f, G->ScreenWidth, G->ScreenHeight};
        if (screenPos.z <= 0 || !screenRect.Contains(screenPos))
            return {};

        float scaleX = G->ScreenWidth / G->PixelWidth;
        float scaleY = G->ScreenHeight / G->PixelHeight;

        screenPos.x *= scaleX;
        screenPos.y = G->ScreenHeight - screenPos.y * scaleY;

        return TruckEspData
        {
            .Pos = screenPos.ToVector2().ToHax(),
            .Distance = dist
        };
    }

    Hax::Optional<PlayerEspData> ParsePlayerEspData(PlayerAvatar avatar)
    {
        UnityEngine::Camera mainCam = GameDirector::instance().MainCamera();
        if (!mainCam || !avatar || avatar.isLocal())
            return {};

        float dist = mainCam.GetTransform().GetPosition().Distance(avatar.GetTransform().GetPosition());
        if (dist < 1.f)
            return {};

        if (avatar.deadSet())
        {
            PlayerDeathHead head = avatar.playerDeathHead();
            if (head && head.GetEnabled())
            {
                System::Array<UnityEngine::Collider> colliders = head.colliders();
                UnityEngine::Bounds bounds = colliders[0].GetBounds();
                for (auto col : colliders)
                    bounds.Encapsulate(col.GetBounds());

                if (bounds.m_Extents == UnityEngine::Vector3::zero())
                    return {};

                UnityEngine::Rect rect = CalcBoundsInScreenSpace(bounds, mainCam);
                UnityEngine::Rect screenRect{0.f, 0.f, G->ScreenWidth, G->ScreenHeight};

                if (!rect.Overlaps(screenRect))
                    return {};

                PlayerEspData espData{};
                espData.Box = rect.ToHax();
                espData.Distance = dist;
                espData.Name = SemiFunc::PlayerGetName(avatar).ToView();
                espData.CurHp = 0;
                espData.MaxHp = 0;
                espData.Dead = true;

                return espData;
            }

            return {};
        }

        if (avatar.isDisabled())
            return {};

        PlayerAvatarVisuals visuals = avatar.playerAvatarVisuals();
        UnityEngine::Bounds bounds2 = visuals.headSideTransform().GetComponent<UnityEngine::Collider>().GetBounds();
        UnityEngine::Vector3 ext = bounds2.m_Extents;
        for (UnityEngine::Collider col2 : visuals.legTwistTransform().GetComponentsInChildren<UnityEngine::Collider>())
        {
            if (col2)
                bounds2.Encapsulate(col2.GetBounds());
        }
        if (bounds2.m_Extents == UnityEngine::Vector3::zero())
        {
            return {};
        }

        ext.y = bounds2.m_Extents.y * 1.2f;
        ext.z *= 1.5f;
        ext.x *= 1.5f;
        bounds2.m_Extents = ext;
        UnityEngine::Rect rect = CalcBoundsInScreenSpace(bounds2, mainCam);

        UnityEngine::Rect screenRect{0.f, 0.f, G->ScreenWidth, G->ScreenHeight};
        if (!rect.Overlaps(screenRect))
            return {};

        PlayerHealth health = avatar.playerHealth();

        return PlayerEspData
        {
            .Box = rect.ToHax(),
            .Name = SemiFunc::PlayerGetName(avatar).ToView(),
            .Distance = dist,
            .CurHp = health.health(),
            .MaxHp = health.maxHealth(),
            .Dead = false,
        };
    }

    Hax::Optional<CosmeticBoxEspData> ParseCosmeticBoxEspData(CosmeticWorldObject box)
    {
        UnityEngine::Camera cam = GameDirector::instance().MainCamera();

        if (!box || !cam)
            return {};

        UnityEngine::Vector3 worldPos = box.GetTransform().GetPosition();

        float dist = cam.GetTransform().GetPosition().Distance(worldPos);
        if (dist < 1.f)
            return {};

        UnityEngine::Vector3 screenPos = cam.WorldToScreenPoint(worldPos);
        UnityEngine::Rect screenRect{0.f, 0.f, G->ScreenWidth, G->ScreenHeight};
        if (screenPos.z <= 0 || !screenRect.Contains(screenPos))
            return {};

        float scaleX = G->ScreenWidth / G->PixelWidth;
        float scaleY = G->ScreenHeight / G->PixelHeight;

        screenPos.x *= scaleX;
        screenPos.y = G->ScreenHeight - screenPos.y * scaleY;

        return CosmeticBoxEspData
        {
            .Pos = screenPos.ToVector2().ToHax(),
            .Distance = dist,
            .Rarity = box.rarity()
        };
    }

    static void DrawEnemyEsp(EnemyEspData& data)
    {
        float fontH = 9_px;

        Box(data.Box, 0xFF0000FF);
        Text(G->NunitoSans_Bold, data.Name, Hax::Vector2(data.Box.GetCenter().X, data.Box.Min.Y - 2.f), 0xFFFFFFFF, fontH, VerticalAlignment_Top, HorizontalAlignment_Center);
        VertHealthBar(data.Box.Min.X - 10.f, data.Box.Min.Y, 3.f, data.Box.GetSize().Y, data.CurHp, data.MaxHp);

        Hax::Gui::Color lerped = Hax::Lerp(0xFF0000FF, Hax::Gui::Color(0x00FF00FF), (float)data.CurHp / data.MaxHp);
        Hax::WStringBuilder<16> sb;
        sb.AppendF(L"%d", data.CurHp);
        Text(G->NunitoSans_Bold, sb.View(), Hax::Vector2(data.Box.Min.X - 15.f, data.Box.Min.Y), lerped, fontH, VerticalAlignment_Bottom, HorizontalAlignment_Left);
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
            Text(G->NunitoSans_Bold, buf, data.Pos, 0xFFFF00FF, EspFontSize(), VerticalAlignment_Bottom, HorizontalAlignment_Center);
            return;
        }

        Text(G->NunitoSans_Bold, s, data.Pos, 0xFF00FFFF, EspFontSize(), VerticalAlignment_Bottom, HorizontalAlignment_Center);
    }

    static void DrawExtrPointEsp(ExtrPointEspData& data)
    {
        Hax::Gui::Color color = data.Active ? 0x00FF00FF : data.Completed ? 0x545454FF : 0xFF0000FF;
        Text(G->Icons_Solid, L"\uF468", data.Pos, color, IconFontSize());
    }

    static void DrawTruckEsp(TruckEspData& data)
    {
        Hax::Gui::Color color = 0x00FF00FF;
        Text(G->Icons_Solid, L"\uF0D1", data.Pos, color, IconFontSize());
    }

    static void DrawPlayerEsp(PlayerEspData& data)
    {
        Box(data.Box, data.Dead ? 0x545454FF : 0x00FF00FF);
        Text(G->NunitoSans_Bold, data.Name, Hax::Vector2(data.Box.GetCenter().X, data.Box.Min.Y - 2.f), 0xFFFFFFFF, EspFontSize(), VerticalAlignment_Top, HorizontalAlignment_Center);
        if (!data.Dead)
        {
            VertHealthBar(data.Box.Min.X - 10_px, data.Box.Min.Y, 3_px, data.Box.GetSize().Y, data.CurHp, data.MaxHp);

            Hax::Gui::Color lerped = Hax::Lerp(Hax::Gui::Color(0xFF0000FF), Hax::Gui::Color(0x00FF00FF), (float)data.CurHp / data.MaxHp);
            Hax::WStringBuilder<16> sb;
            sb.AppendF(L"%d", data.CurHp);
            Text(G->NunitoSans_Bold, sb.View(), Hax::Vector2(data.Box.Min.X - 15_px, data.Box.Min.Y), lerped, EspFontSize(), VerticalAlignment_Bottom, HorizontalAlignment_Left);
        }
    }

    static void DrawCosmeticBoxEsp(CosmeticBoxEspData& data)
    {
        static Hax::Array<Hax::Gui::Color, 4> s_RarityToColor = {0x7FFF00FF, 0x57A0D3FF, 0xF81894FF, 0xE6CC00FF};

        Text(G->Icons_Solid, L"\uF553", data.Pos, s_RarityToColor[data.Rarity], IconFontSize());
    }

    static void Box(const Hax::Rect& rect, const Hax::Gui::Color& col)
    {
        float px1 = 1_px;
        float px3 = 3_px;

        Hax::Vector2 tl = rect.GetTL();
        Hax::Vector2 tr = rect.GetTR();
        Hax::Vector2 br = rect.GetBR();
        Hax::Vector2 bl = rect.GetBL();

        Hax::Gui::DrawLine(tl, tr, {.FillColor = 0x000000FF, .Th = px3});
        Hax::Gui::DrawLine(tr, br, {.FillColor = 0x000000FF, .Th = px3});
        Hax::Gui::DrawLine(bl, br, {.FillColor = 0x000000FF, .Th = px3});
        Hax::Gui::DrawLine(tl, bl, {.FillColor = 0x000000FF, .Th = px3});

        Hax::Gui::DrawLine(tl, tr, {.FillColor = col, .Th = px1});
        Hax::Gui::DrawLine(tr, br, {.FillColor = col, .Th = px1});
        Hax::Gui::DrawLine(bl, br, {.FillColor = col, .Th = px1});
        Hax::Gui::DrawLine(tl, bl, {.FillColor = col, .Th = px1});
    }

    static void Text(Hax::Gui::FontHandle hFont,
                     Hax::WStringView text,
                     Hax::Vector2 pos,
                     Hax::Gui::Color col,
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

    static UnityEngine::Rect CalcBoundsInScreenSpace(const UnityEngine::Bounds& bigBounds, UnityEngine::Camera cam)
    {
        float scaleX = G->ScreenWidth / G->PixelWidth;
        float scaleY = G->ScreenHeight / G->PixelHeight;

        UnityEngine::Vector3 screenSpaceCorners[8] =
        {
            {bigBounds.m_Center.x + bigBounds.m_Extents.x, bigBounds.m_Center.y + bigBounds.m_Extents.y, bigBounds.m_Center.z + bigBounds.m_Extents.z},
            {bigBounds.m_Center.x + bigBounds.m_Extents.x, bigBounds.m_Center.y + bigBounds.m_Extents.y, bigBounds.m_Center.z - bigBounds.m_Extents.z},
            {bigBounds.m_Center.x + bigBounds.m_Extents.x, bigBounds.m_Center.y - bigBounds.m_Extents.y, bigBounds.m_Center.z + bigBounds.m_Extents.z},
            {bigBounds.m_Center.x + bigBounds.m_Extents.x, bigBounds.m_Center.y - bigBounds.m_Extents.y, bigBounds.m_Center.z - bigBounds.m_Extents.z},
            {bigBounds.m_Center.x - bigBounds.m_Extents.x, bigBounds.m_Center.y + bigBounds.m_Extents.y, bigBounds.m_Center.z + bigBounds.m_Extents.z},
            {bigBounds.m_Center.x - bigBounds.m_Extents.x, bigBounds.m_Center.y + bigBounds.m_Extents.y, bigBounds.m_Center.z - bigBounds.m_Extents.z},
            {bigBounds.m_Center.x - bigBounds.m_Extents.x, bigBounds.m_Center.y - bigBounds.m_Extents.y, bigBounds.m_Center.z + bigBounds.m_Extents.z},
            {bigBounds.m_Center.x - bigBounds.m_Extents.x, bigBounds.m_Center.y - bigBounds.m_Extents.y, bigBounds.m_Center.z - bigBounds.m_Extents.z}
        };

        for (int i = 0; i < 8; ++i)
        {
            screenSpaceCorners[i] = cam.WorldToScreenPoint(screenSpaceCorners[i]);
            screenSpaceCorners[i].x *= scaleX;
            screenSpaceCorners[i].y = G->ScreenHeight - screenSpaceCorners[i].y * scaleY;
        }

        float x = FLT_MAX;
        float y = FLT_MAX;
        float x2 = FLT_MIN;
        float y2 = FLT_MIN;

        for (const UnityEngine::Vector3& v : screenSpaceCorners)
        {
            if (v.z <= 0.f)
                continue;

            x = Hax::Min(v.x, x);
            y = Hax::Min(v.y, y);
            x2 = Hax::Max(v.x, x2);
            y2 = Hax::Max(v.y, y2);
        }

        if (y2 - y < 15.f)
            y2 = y + 15.f;

        return UnityEngine::Rect::MinMaxRect(x, y, x2, y2);
    }
}