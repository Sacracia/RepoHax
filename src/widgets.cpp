#include "widgets.h"
#include "cheat.h"

namespace Cheat::Widgets
{
    namespace Theme
    {
        constexpr Hax::Gui::Color WindowBg = 0x141415FF;
        constexpr Hax::Gui::Color SidePanelBg = 0x1D1E20FF;
        constexpr float WindowR = 10.f;
        constexpr float MainFontSize = 13.f;
        constexpr float DescFontSize = 12.f;
        constexpr float TitleFontSize = 12.f;
        constexpr Hax::Gui::LinearColor BtnBg = 0x292A2CFF;
        constexpr Hax::Gui::Color BtnBgActive = 0x353639FF;
        constexpr Hax::Gui::Color DescColor = 0x9299A8FF;
        constexpr Hax::Gui::Color DisabledMaskCol = 0x18181AB0;
        constexpr Hax::Gui::Color SeparatorCol = 0x202022FF;
        constexpr Hax::Gui::Color MainCol = 0xC2C2C2FF;
        constexpr Hax::Gui::Color PopupBg = 0x1D1E20FF;
        constexpr Hax::Gui::Color FrameColor = 0x282828FF;
        constexpr Hax::Gui::Color ActiveColor = 0x2B69FFFF;
        constexpr Hax::Gui::Color HeaderColor = Hax::Gui::Color::White;

        #define DESC_FONT GCheat->NunitoSans_SemiBold
        #define MAIN_FONT GCheat->NunitoSans_Bold
        #define HEADER_FONT GCheat->NunitoSans_ExtraBold
        #define ICONS_FONT GCheat->Icons_Solid

        constexpr Hax::Vector2 ToggleSize = {35.f, 20.f};
        constexpr float SettingBtnSize = 16.f;
        constexpr float CheckboxSize = 16.f;
        constexpr Hax::Vector2 WindowSize = {1200.f, 600.f};
    }

    void Label(Hax::Gui::FontHandle hFont, Hax::WStringView text, float fontH, Hax::Gui::Color col)
    {
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(hFont, text, fontH);

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + textSize;

        Hax::Gui::PlaceItem(textSize);
        if (Hax::Gui::IsItemVisible(bounds))
            Hax::Gui::DrawString(hFont, text, bounds.Min, fontH, {.Color = col});
    }

    void MainLabel(Hax::WStringView text)
    {
        Label(MAIN_FONT, text, Theme::MainFontSize * Hax::Gui::G.ScaleFactor, Theme::MainCol);
    }

    void MainLabel(Hax::WStringView text, const Hax::Gui::Color& col)
    {
        Label(MAIN_FONT, text, Theme::MainFontSize * Hax::Gui::G.ScaleFactor, col);
    }

    Hax::Vector2 CalcMainLabelSize(Hax::WStringView text)
    {
        return Hax::Gui::CalcTextSize(MAIN_FONT, text, Hax::Gui::Scale(Theme::MainFontSize));
    }

    void DescLabel(Hax::WStringView text)
    {
        Label(DESC_FONT, text, Theme::DescFontSize * Hax::Gui::G.ScaleFactor, Theme::DescColor);
    }

    void LabelAlignedByH(Hax::Gui::FontHandle hFont, Hax::WStringView label, float fontH, const Hax::Gui::Color& col, float h)
    {
        const Hax::Gui::FontMetrics metrics = Hax::Gui::GetFontMetrics(hFont, fontH);

        Hax::Gui::BeginVertical();
        {
            //Hax::Gui::Space((h - metrics.CapHeight) / 2.f - (metrics.Ascent - metrics.CapHeight));
            Hax::Gui::Space((h - metrics.LineHeight) / 2.f);
            Label(hFont, label, fontH, col);
        }
        Hax::Gui::EndVertical();
    }

    void MainLabelAlignedByH(Hax::WStringView label, float h)
    {
        LabelAlignedByH(MAIN_FONT, label, Hax::Gui::Scale(Theme::MainFontSize), Theme::MainCol, h);
    }

    void BeginWindow()
    {
        constexpr size_t WINDOW_ID = Hax::Hash("MainWindow");
        const Hax::Vector2 windowSize = Hax::Gui::Scale(Theme::WindowSize);

        const Hax::Vector2 viewportCenter = Hax::Gui::GetViewportCenter();
        Hax::Vector2& posOffset = Hax::Gui::GetState<Hax::Vector2>(WINDOW_ID);

        Hax::Rect windowBounds;
        windowBounds.Min = viewportCenter - windowSize / 2.f + posOffset;
        windowBounds.Max = windowBounds.Min + windowSize;

        Hax::Gui::Interact(WINDOW_ID, windowBounds);
        if (Hax::Gui::IsItemActive(WINDOW_ID))
            posOffset += Hax::Gui::GetMouseDeltaPos();

        Hax::Gui::SetCursorPos(windowBounds.Min);
        Hax::Gui::BeginContainer(WINDOW_ID, {.W = windowSize.X, .H = windowSize.Y, .Clip = true});
        Hax::Gui::BeginHorizontal();

        Hax::Gui::DrawRect(windowBounds.Min, windowBounds.Max, {.FillColor = Theme::WindowBg, .Rounding = Hax::Gui::Scale(Theme::WindowR)});
    }

    void EndWindow()
    {
        Hax::Gui::EndHorizontal();
        Hax::Gui::EndContainer();
    }

    bool BeginSidePanel()
    {
        const Hax::Vector2 size = {215_px, Hax::Gui::GetContentRegionAvail().Y};

        constexpr size_t id = Hax::Hash("SidePanel");
        Hax::Gui::BeginContainer(id, {.W = size.X, .H = size.Y});

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + size;

        if (!Hax::Gui::IsItemVisible(bounds))
        {
            Hax::Gui::EndContainer();
            return false;
        }

        const float r = Hax::Gui::Scale(Theme::WindowR);
        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::SidePanelBg, .Rounding = Hax::Vector4(r, 0.f, 0.f, r)});

        Hax::Gui::BeginVertical();
        return true;
    }

    void EndSidePanel()
    {
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
    }

    void PanelHeader(Hax::WStringView text, Hax::WStringView desc)
    {
        Hax::Gui::BeginVertical();
        {
            Label(HEADER_FONT, text, Hax::Gui::Scale(Theme::TitleFontSize), Theme::HeaderColor);
            if (!desc.Empty())
                DescLabel(desc);
        }
        Hax::Gui::EndVertical();
    }

    bool SettingsBtn(size_t id, bool disabled)
    {
        const float h = Theme::SettingBtnSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 size = Hax::Gui::CalcTextSize(ICONS_FONT, L"\uF013", h);
        const Hax::Rect bounds = Hax::Rect::FromPosSize(Hax::Gui::GetCursorPos(), size);

        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        if (!disabled)
            Hax::Gui::Interact(id, bounds);
        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);

        const float deltaTime = Hax::Gui::GetDeltaTime();
        if (Hax::Gui::IsItemHovered(id))
        {
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
            state.Elapse(deltaTime, 0.1f);
        }
        else
            state.Elapse(-deltaTime, 0.1f);

        Hax::uint8 c = Hax::Gui::IsItemActive(id) ? 0xB5 : Hax::Lerp(0xC2, 0xFF, state.Progress);
        Hax::Gui::DrawString(ICONS_FONT, L"\uF013", bounds.Min, h, {.Color = Hax::Gui::Color(c, c, c, 0xFF)});

        return Hax::Gui::IsItemClicked(id);
    }

    struct ToggleState
    {
        Hax::Gui::LinearAnim HoverAnim;
        Hax::Gui::LinearAnim ToggleAnim;
    };

    bool Toggle(size_t id, bool& value, bool enabled)
    {
        const Hax::Vector2 toggleSize = Hax::Gui::Scale(Theme::ToggleSize);

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + toggleSize;

        Hax::Gui::PlaceItem(toggleSize);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        ToggleState& state = Hax::Gui::GetState<ToggleState>(id);
        const float deltaTime = Hax::Gui::GetDeltaTime();
        state.ToggleAnim.Elapse(value ? deltaTime : -deltaTime, 0.1f);

        if (enabled)
            Hax::Gui::Interact(id, bounds);

        if (Hax::Gui::IsItemHovered(id))
        {
            state.HoverAnim.Elapse(deltaTime, 0.1f);
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
        }
        else
            state.HoverAnim.Elapse(-deltaTime, 0.1f);

        if (Hax::Gui::IsItemClicked(id))
            value = !value;

        const float outlineOff = Hax::Round(Hax::Lerp(0.f, 2_px, state.HoverAnim.Progress));
        if (outlineOff > 0.f)
        {
            const Hax::Vector2 outlineOff2 = { outlineOff, outlineOff };
            Hax::Gui::DrawRect(bounds.Min - outlineOff2, bounds.Max + outlineOff2, { .FillColor = 0x424242FF, .Rounding = 12_px});
        }
        constexpr Hax::Gui::LinearColor from = 0x282829FF;
        constexpr Hax::Gui::LinearColor to = 0x2B69FFFF;
        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Hax::Lerp(from, to, state.ToggleAnim.Progress).ToColor(), .Rounding = 10_px});

        // Circle
        {
            const float r = 7_px;
            const float px2 = 3_px;
            const float circlePosX = Hax::Lerp(bounds.Min.X + px2 + r, bounds.Max.X - px2 - r, state.ToggleAnim.Progress);
            const Hax::Vector2 circlePos = {circlePosX, bounds.GetCenter().Y};
            Hax::uint8 colorRGB = Hax::Lerp(0xB5, 0xFF, state.ToggleAnim.Progress);
            const Hax::Gui::Color cicleCol = {colorRGB, colorRGB, colorRGB};
            Hax::Gui::DrawCircle(circlePos, 7_px, {.FillColor = cicleCol});
        }

        return Hax::Gui::IsItemClicked(id);
    }

    void OpenPopup(size_t id, const Hax::Vector2& pos);
    
    bool ToggleEx(size_t id, bool& value, Hax::WStringView text, Hax::WStringView desc, ToggleExParams params)
    {
        bool changed = false;
        const float px8 = 8_px;
        const Hax::Vector2 toggleSize = Hax::Gui::Scale(Theme::ToggleSize);

        Hax::Gui::BeginVertical();
        {
            Hax::Gui::BeginHorizontal(px8);
            {
                LabelAlignedByH(MAIN_FONT, text, Hax::Gui::Scale(Theme::MainFontSize), Theme::MainCol, toggleSize.Y);
                float space = Hax::Gui::GetContentRegionAvail().X - toggleSize.X;

                if (params.SettingsId != 0)
                {
                    const float settingSize = Hax::Gui::Scale(Theme::SettingBtnSize);
                    Hax::Gui::Space(space - px8 - settingSize);
                    Hax::Gui::BeginVertical(3_px);
                    {
                        Hax::Gui::Space((toggleSize.Y - settingSize) / 2.f);
                        if (SettingsBtn(id + 1, params.Disabled))
                            OpenPopup(params.SettingsId, Hax::Gui::GetCursorPos());
                    }
                    Hax::Gui::EndVertical();
                    space = 0.f;
                }

                Hax::Gui::Space(space);
                changed = Toggle(id, value, !params.Disabled);

                if (params.Disabled)
                {
                    Hax::Rect bounds = Hax::Gui::GetLayoutBounds();
                    Hax::Gui::DrawRect(bounds.Min, bounds.Max, { .FillColor = Theme::DisabledMaskCol });
                }
            }
            Hax::Gui::EndHorizontal();

            if (!desc.Empty())
                DescLabel(desc);
        }
        Hax::Gui::EndVertical();

        return changed;
    }

    Hax::Vector2 CalcButtonSize(Hax::WStringView label)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {12_px, 5_px};

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        return textSize + padding * 2.f;
    }

    float CalcButtonHeight()
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        return Hax::Gui::GetFontLineHeight(MAIN_FONT, fontH) + 5_px * 2.f;
    }

    bool Button(size_t id, Hax::WStringView label, Hax::WStringView desc, const ButtonParams& params)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {12_px, 5_px};

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        const Hax::Vector2 btnSize = {Hax::Max(textSize.X + padding.X * 2.f, params.MinW), textSize.Y + padding.Y * 2.f};

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + btnSize;

        Hax::Vector2 descSize{};
        if (!desc.Empty())
        {
            descSize = Hax::Gui::CalcTextSize(DESC_FONT, desc, fontH);
            bounds.Max.Y += descSize.Y;
        }

        Hax::Gui::PlaceItem(bounds.GetSize());
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        if (params.Enabled)
            Hax::Gui::Interact(id, bounds);

        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);

        Hax::Gui::Color bg = 0x2B2B2BFF;
        Hax::Gui::Color fg = 0xC8C8C8FF;
        if (!Hax::Gui::IsItemActive(id))
        {
            constexpr Hax::Gui::LinearColor fromBg = 0x282828FF;
            constexpr Hax::Gui::LinearColor toBg = 0x343434FF;
            bg = Hax::Lerp(fromBg, toBg, state.Progress).ToColor();

            constexpr Hax::Gui::LinearColor fromFg = 0xC8C8C8FF;
            constexpr Hax::Gui::LinearColor toFg = 0xFFFFFFFF;
            fg = Hax::Lerp(fromFg, toFg, state.Progress).ToColor();
        }

        const float deltaTime = Hax::Gui::GetDeltaTime();
        if (Hax::Gui::IsItemHovered(id))
        {
            state.Elapse(deltaTime, 0.1f);
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
        }
        else
            state.Elapse(-deltaTime, 0.1f);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = bg, .Rounding = 5_px});
        Hax::Gui::DrawString(MAIN_FONT, label, bounds.Min + (btnSize - textSize) / 2.f, fontH, {.Color = fg});
        if (!desc.Empty())
        {
            Hax::Vector2 descPos;
            descPos.X = bounds.Min.X + (btnSize.X - descSize.X) / 2.f;
            descPos.Y = bounds.Min.Y + btnSize.Y - padding.Y;
            Hax::Gui::DrawString(DESC_FONT, desc, descPos, fontH, {.Color = Theme::DescColor});
        }

        if (!params.Enabled)
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMaskCol});

        return Hax::Gui::IsItemClicked(id);
    }

    void Image(Hax::Gui::TextureHandle tex, Hax::Vector2 customSize)
    {
        const Hax::Vector2 size = (bool)customSize ? customSize : Hax::Gui::GetImageSize(tex);

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + size;

        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return;

        Hax::Gui::DrawImage(tex, bounds.Min, bounds.Max);
    }

    bool TabButton(size_t id, Hax::WStringView text, Hax::char16 icon16, bool active)
    {
        const float fontSize = Hax::Gui::Scale(Theme::MainFontSize);
        const float iconHeight = Hax::Gui::GetFontLineHeight(MAIN_FONT, fontSize);
        const Hax::Vector2 padding = {12_px, 7_px};

        Hax::Vector2 btnSize;
        btnSize.X = Hax::Gui::GetContentRegionAvail().X;
        btnSize.Y = Hax::Gui::GetFontLineHeight(MAIN_FONT, fontSize) + padding.Y * 2.f;

        Hax::Rect btnBounds;
        btnBounds.Min = Hax::Gui::GetCursorPos();
        btnBounds.Max = btnBounds.Min + btnSize;

        Hax::Gui::PlaceItem(btnSize);
        if (!Hax::Gui::IsItemVisible(btnBounds))
            return false;

        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);

        // Interaction
        {
            auto res = Hax::Gui::Interact(id, btnBounds);
            const float deltaTime = Hax::Gui::GetDeltaTime();
            state.Elapse(res.Hovered ? deltaTime : -deltaTime, 0.1f);
            if (res.Hovered)
                Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
        }

        // Background
        {
            const Hax::Gui::Color btnColor = active ? Theme::BtnBgActive : Hax::Gui::LinearColor(Theme::BtnBg.R, Theme::BtnBg.G, Theme::BtnBg.B, state.Progress).ToColor();
            Hax::Gui::DrawRect(btnBounds.Min, btnBounds.Max, {.FillColor = btnColor, .Rounding = 10_px});
        }

        const Hax::WStringView icon = {&icon16, 1};
        const Hax::Gui::Color labelColor = active ? 0xFFF5F2F0 
            : Hax::Lerp(Hax::Gui::LinearColor(0x9299A8FF), Hax::Gui::LinearColor(0xF0F2F5FF), state.Progress).ToColor();

        Hax::Vector2 posCursor = btnBounds.Min;
        posCursor.Y += (btnSize.Y - iconHeight) / 2.f;
        posCursor.X += padding.X;
        Hax::Gui::DrawString(ICONS_FONT, icon, posCursor, iconHeight, {.Color = labelColor});

        const float capHeight = Hax::Gui::GetFontCapHeight(MAIN_FONT, fontSize);
        const Hax::Vector2 labelSize = Hax::Gui::CalcTextSize(MAIN_FONT, text, fontSize);
        posCursor.X += 18_px + padding.X;
        posCursor.Y += (iconHeight - capHeight) / 2.f - (Hax::Gui::GetFontAscent(MAIN_FONT, fontSize) - capHeight);
        Hax::Gui::DrawString(MAIN_FONT, text, posCursor, fontSize, {.Color = active ? 0xF0F2F5FF : 0xE4E7E9FF});

        return Hax::Gui::IsItemClicked(id);
    }

    void BeginPanel(size_t id)
    {
        Hax::Gui::BeginContainer(id, {.FitY = true});
        const Hax::Vector2 containerSize = Hax::Gui::GetContentRegionAvail();
        if (containerSize.Y > 0.f)
        {
            const Hax::Vector2 a = Hax::Gui::GetCursorPos();
            const Hax::Vector2 b = a + containerSize;
            Hax::Gui::DrawRect(a, b, {.FillColor = 0x18181AFF, .Rounding = 5_px});
        }
        else
            Hax::Gui::PushSkipDrawing();

        Hax::Gui::BeginHorizontal();
        const float padding = 12_px;
        Hax::Gui::Space(padding);
        Hax::Gui::BeginContainer(id + 1, {.W = containerSize.X - padding * 2.f, .H = 15.f, .FitY = true});
        Hax::Gui::BeginVertical(padding);
        Hax::Gui::Dummy({0.f, 0.f});
    }

    void EndPanel()
    {
        Hax::Gui::Dummy({0.f, 0.f});
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
        Hax::Gui::EndHorizontal();

        if (Hax::Gui::IsDrawingSkipped())
            Hax::Gui::PopSkipDrawing();

        Hax::Gui::EndContainer();
    }

    int SliderConvertInt(int min, int max, float m)
    {
        return Hax::Clamp(min + (int)Hax::Round((float)(max - min) * m), min, max);
    }

    float SliderConvertFloat(float min, float max, float m)
    {
        return Hax::Lerp(min, max, Hax::Clamp(m, 0.f, 1.f));
    }

    //template <float Step>
    //float SliderConvertFloatStep(float min, float max, float m)
    //{
    //    float rawVal = min + m * (max - min);
    //
    //    float stepsCount = Round((rawVal - min) / Step);
    //
    //    float finalVal = min + (stepsCount * Step);
    //    return Clamp(finalVal, min, max);
    //}

    void ButtonBeh(size_t id, Hax::Gui::Color& bg, Hax::Gui::Color& fg)
    {
        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);

        bg = 0x2B2B2BFF;
        fg = 0xC8C8C8FF;
        if (!Hax::Gui::IsItemActive(id))
        {
            constexpr Hax::Gui::LinearColor fromBg = 0x282828FF;
            constexpr Hax::Gui::LinearColor toBg = 0x343434FF;
            bg = Hax::Lerp(fromBg, toBg, state.Progress).ToColor();

            constexpr Hax::Gui::LinearColor fromFg = 0xC8C8C8FF;
            constexpr Hax::Gui::LinearColor toFg = 0xFFFFFFFF;
            fg = Hax::Lerp(fromFg, toFg, state.Progress).ToColor();
        }

        const float deltaTime = Hax::Gui::GetDeltaTime();
        if (Hax::Gui::IsItemHovered(id))
        {
            state.Elapse(deltaTime, 0.1f);
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
        }
        else
            state.Elapse(-deltaTime, 0.1f);
    }

    bool AutoLayoutButton(size_t id, Hax::WStringView text, const AutoLayoutButtonParams& params)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {12_px, 5_px};

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, text, fontH);
        Hax::Vector2 btnSize = textSize + padding * 2.f;

        Hax::Rect bounds; 
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + btnSize;

        if (params.Icon != 0)
        {
            Hax::Vector2 iconSize = Hax::Gui::GetImageSize(params.Icon);
            bounds.Max.X += padding.X + (textSize.Y * (iconSize.X / iconSize.Y));
            btnSize = bounds.GetSize();
        }

        if (btnSize.X > Hax::Gui::GetContentRegionAvail().X)
        {
            Hax::Gui::EndHorizontal();
            Hax::Gui::BeginHorizontal(5_px);

            bounds.Min = Hax::Gui::GetCursorPos();
            bounds.Max = bounds.Min + btnSize;
        }

        Hax::Gui::PlaceItem(btnSize);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        if (!params.Disabled)
            Hax::Gui::Interact(id, bounds);

        Hax::Gui::Color bg, fg;
        ButtonBeh(id, bg, fg);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = bg, .Rounding = 5_px});

        Hax::Vector2 drawPos = bounds.Min + padding;
        if (params.Icon != 0)
        {
            Hax::Vector2 iconSize = Hax::Gui::GetImageSize(params.Icon);

            Hax::Vector2 iconBR;
            iconBR.X = drawPos.X + textSize.Y * (iconSize.X / iconSize.Y);
            iconBR.Y = drawPos.Y + textSize.Y;
            Hax::Gui::DrawImage(params.Icon, drawPos, iconBR);

            drawPos.X = iconBR.X + padding.X / 2.f;
        }
        Hax::Gui::DrawString(MAIN_FONT, text, drawPos, fontH, {.Color = fg});

        if (params.Disabled)
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMaskCol});

        return Hax::Gui::IsItemClicked(id);
    }

    bool DropdownBtn(size_t id, Hax::WStringView preview, float w)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {12_px, 5_px};//!
        const float textH = Hax::Gui::GetFontLineHeight(MAIN_FONT, fontH);

        const Hax::Vector2 size = {w, textH + padding.Y * 2.f};
        Hax::Rect bounds; bounds.Min = Hax::Gui::GetCursorPos(); bounds.Max = bounds.Min + size;

        Hax::Gui::PlaceItem(size);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        Hax::Gui::Interact(id, bounds);

        Hax::Gui::Color bg, fg;
        ButtonBeh(id, bg, fg);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = bg, .Rounding = 5_px});
        Hax::Gui::DrawString(MAIN_FONT, preview, bounds.Min + padding, fontH, {.Color = fg});

        const Hax::Vector2 arrowSize = padding;
        Hax::Vector2 arrowPos;
        arrowPos.X = bounds.Max.X - arrowSize.X - padding.X;
        arrowPos.Y = bounds.Min.Y + (bounds.GetHeight() - arrowSize.Y) / 2.f;

        Hax::Gui::DrawTriangle(arrowPos, arrowPos + Hax::Vector2(arrowSize.X, 0.f), arrowPos + Hax::Vector2(arrowSize.X / 2.f, arrowSize.Y), {.FillColor = 0xC8C8C8FF});

        return Hax::Gui::IsItemClicked(id);
    }

    struct PopupState { bool Opened, IsFirstFrame; Hax::Vector2 Pos; };

    void OpenPopup(size_t id, const Hax::Vector2& pos)
    {
        PopupState& state = Hax::Gui::GetState<PopupState>(id);
        state.Opened = state.IsFirstFrame = true;
        state.Pos = pos;
    }

    void ClosePopup(size_t id)
    {
        PopupState& state = Hax::Gui::GetState<PopupState>(id);
        state.Opened = false;
    }

    bool BeginDropList(size_t id, const Hax::Vector2& size)
    {
        PopupState& state = Hax::Gui::GetState<PopupState>(id);

        if (!state.Opened)
            return false;

        Hax::Gui::SwitchLayer(L"Popups");
        Hax::Gui::SetCursorPos(state.Pos);

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + size;

        Hax::Gui::Interact(id, bounds);
        if (!state.IsFirstFrame && Hax::Gui::IsLmbJustPressed() && !bounds.Contains(Hax::Gui::GetMousePos()))
        {
            state.Opened = false;
            Hax::Gui::RestoreLayer();
            return false;
        }

        state.IsFirstFrame = false;

        const float r = 5_px;
        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::FrameColor, .Rounding = r});

        bounds.Min.Y += r;
        bounds.Max.Y -= r;

        Hax::Gui::SetCursorPos(bounds.Min);
        Hax::Gui::ScrollStyle ScrollSt = 
        {
            .TrackWidth = 6_px,
            .ThumbPadding = 0.f,
            .TrackCol = 0x0,
            .ThumbCol = 0x4F4F4FFF,
            .ThumbHovCol = 0x4F4F4FFF,
            .ThumbActiveCol = 0x4F4F4FFF
        };
        Hax::Gui::BeginContainer(id, {.W = size.X, .H = size.Y - r * 2.f, .Clip = true, .ScrollY = true, .Style = ScrollSt});
        Hax::Gui::BeginVertical();

        return true;
    }

    void EndDropList()
    {
        Hax::Gui::EndVertical();
        Hax::Gui::EndContainer();
        Hax::Gui::RestoreLayer();
    }

    bool BeginModal(size_t id)
    {
        PopupState& state = Hax::Gui::GetState<PopupState>(id);

        if (!state.Opened)
            return false;

        Hax::Gui::SwitchLayer(L"Popups");
        Hax::Gui::SetCursorPos(state.Pos);

        Hax::Gui::BeginContainer(id, {.FitX = true, .FitY = true});
        Hax::Rect bounds = Hax::Gui::GetContainerBounds();

        Hax::Gui::Interact(id + 1, Hax::Gui::GetViewportBounds());
        Hax::Gui::Interact(id, bounds);

        if (Hax::Gui::IsItemPressed(id + 1))
            state.Opened = false;

        if (Hax::Gui::IsItemActive(id))
            state.Pos += Hax::Gui::GetMouseDeltaPos();

        if (bounds.GetSize().X > 0)
        {
            const float r = 5_px;
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::PopupBg, .Rounding = r});
        }
        else
            Hax::Gui::PushSkipDrawing();

        return true;
    }

    void EndModal()
    {
        if (Hax::Gui::IsDrawingSkipped())
            Hax::Gui::PopSkipDrawing();

        Hax::Gui::EndContainer();
        Hax::Gui::RestoreLayer();
    }

    bool Selectable(size_t id, Hax::WStringView label, bool selected, SelectableParams params)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const float markH = fontH * 1.2f;
        const Hax::Vector2 padding = {12_px, 5_px};

        Hax::Vector2 markSize{};
        float markSpacing = 0.f;
        if (selected)
        {
            markSize = Hax::Gui::CalcTextSize(ICONS_FONT, L"\uF00C", markH);
            markSpacing = padding.X;
        }

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        const Hax::Vector2 btnSize = {Hax::Max(textSize.X + padding.X * 2.f + markSpacing + markSize.X, params.MinW), textSize.Y + padding.Y * 2.f};

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + btnSize;

        if (params.AutoLayout && btnSize.X > Hax::Gui::GetContentRegionAvail().X)
        {
            Hax::Gui::EndHorizontal();
            Hax::Gui::BeginHorizontal(5_px);

            bounds.Min = Hax::Gui::GetCursorPos();
            bounds.Max = bounds.Min + btnSize;
        }

        Hax::Gui::PlaceItem(bounds.GetSize());
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        if (!params.Disabled)
            Hax::Gui::Interact(id, bounds);

        bool active = Hax::Gui::IsItemActive(id);
        bool hovered = Hax::Gui::IsItemHovered(id);
        Hax::Gui::Color bg = selected ? 0x333333FF : (hovered ? 0x2B2B2BFF : 0x282828FF);
        Hax::Gui::Color fg = selected ? Hax::Gui::Color::White : 0xC8C8C8FF;

        if (hovered)
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = bg, .Rounding = params.R});
        Hax::Gui::DrawString(MAIN_FONT, label, bounds.Min + padding, fontH, {.Color = fg});
        if (selected)
        {
            Hax::Vector2 pos = {bounds.Max.X - padding.X - markSize.X, bounds.Min.Y + (btnSize.Y - markSize.Y) / 2.f};
            Hax::Gui::DrawString(ICONS_FONT, L"\uF00C", pos, markH, {.Color = 0xFFFFFFFF});
        }

        if (params.Disabled)
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMaskCol});

        return Hax::Gui::IsItemClicked(id);
    }

    float CalcWidgetEqWidth(size_t nWidgets)
    {
        const float spacing = Hax::Gui::GetLayoutSpacing();
        return (Hax::Gui::GetContentRegionAvail().X - spacing * float(nWidgets - 1)) / (float)nWidgets;
    }

    Hax::Vector2 CalcRepeatBtnSize(Hax::WStringView label)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {8_px, 3_px};

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        const Hax::Vector2 btnSize = textSize + padding * 2.f;
        return btnSize;
    }

    bool RepeatBtn(size_t id, Hax::WStringView label, bool enabled)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {8_px, 3_px};

        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, label, fontH);
        const Hax::Vector2 btnSize = textSize + padding * 2.f;

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + btnSize;

        Hax::Gui::PlaceItem(bounds.GetSize());
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        if (enabled)
            Hax::Gui::Interact(id, bounds);

        Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);

        Hax::Gui::Color bg = 0x2B2B2BFF;
        Hax::Gui::Color fg = 0xC8C8C8FF;
        if (!Hax::Gui::IsItemActive(id))
        {
            constexpr Hax::Gui::LinearColor fromBg = 0x282828FF;
            constexpr Hax::Gui::LinearColor toBg = 0x343434FF;
            bg = Hax::Lerp(fromBg, toBg, state.Progress).ToColor();

            constexpr Hax::Gui::LinearColor fromFg = 0xC8C8C8FF;
            constexpr Hax::Gui::LinearColor toFg = 0xFFFFFFFF;
            fg = Hax::Lerp(fromFg, toFg, state.Progress).ToColor();
        }

        const float deltaTime = Hax::Gui::GetDeltaTime();
        if (Hax::Gui::IsItemHovered(id))
        {
            state.Elapse(deltaTime, 0.1f);
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);
        }
        else
            state.Elapse(-deltaTime, 0.1f);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = bg, .Rounding = 5_px});
        Hax::Gui::DrawString(MAIN_FONT, label, bounds.Min + (btnSize - textSize) / 2.f, fontH, {.Color = fg});

        if (!enabled)
            Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = Theme::DisabledMaskCol});

        return Hax::Gui::IsItemPressedRepeat(id);
    }

    bool Hotkey(size_t id, int& key)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {8_px, 4_px};

        Hax::WStringView keyName = Hax::Gui::GetKeyName(key);
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, keyName, fontH);
        const Hax::Vector2 btnSize = textSize + padding * 2.f;

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + btnSize;

        Hax::Gui::PlaceItem(btnSize);
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        double& state = Hax::Gui::GetState<double>(id);

        Hax::Gui::Interact(id, bounds);
        if (Hax::Gui::IsItemClicked(id))
        {
            GCheat->KeyListenerId = (GCheat->KeyListenerId == 0) ? id : 0;
            state = Hax::Gui::GetTime();
        }
        if (Hax::Gui::IsItemHovered(id))
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        bool changed = false;

        if (GCheat->KeyListenerId == id)
        {
            auto& keys = Hax::Gui::GetJustPressedKeys();
            if (keys.Size() > 0)
            {
                Hax::uint8 pressedKey = keys.Last();
                if (pressedKey == 0x1B) // VK_ESCAPE
                    pressedKey = 0;

                changed = key != pressedKey;
                key = pressedKey;

                GCheat->KeyListenerId = 0;
            }
        }

        Hax::Gui::LinearColor color = 0x2B69FF00;
        color.A = (Hax::Sin(Hax::kPi * (float)(Hax::Gui::GetTime() - state) / 0.5f) + 1.f) * 0.5f;

        float px3 = 3_px; float px1 = 1_px; float px5 = 5_px;
        if (GCheat->KeyListenerId == id)
        {
            Hax::Gui::DrawRect(bounds.Min - Hax::Vector2(px3, px3), bounds.Max + Hax::Vector2(px3, px3), { .FillColor = color.ToColor(), .Rounding = px5 });
            Hax::Gui::DrawRect(bounds.Min - Hax::Vector2(px1, px1), bounds.Max + Hax::Vector2(px1, px1), { .FillColor = 0x181818FF, .Rounding = px5 });
        }
        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = 0x282828FF, .Rounding = px5});
        Hax::Gui::DrawString(MAIN_FONT, keyName, bounds.Min + padding, fontH, {.Color = Theme::MainCol});

        Hax::Gui::DrawString(ICONS_FONT, L"\uf11c", bounds.Max - Hax::Vector2(8_px, 8_px), Theme::DescFontSize * Hax::Gui::G.ScaleFactor, {.Color = Theme::DescColor});

        return changed;
    }

    bool HotkeyEx(size_t id, int& key, Hax::WStringView label, Hax::WStringView desc)
    {
        const float fontH = Theme::DescFontSize * Hax::Gui::G.ScaleFactor;
        const Hax::Vector2 padding = {8_px, 4_px};
        const Hax::Vector2 textSize = Hax::Gui::CalcTextSize(MAIN_FONT, Hax::Gui::GetKeyName(key), fontH);
        Hax::Vector2 btnSize = textSize + padding * 2.f;

        Hax::Gui::BeginVertical();
        Hax::Gui::BeginHorizontal();
        LabelAlignedByH(MAIN_FONT, label, Hax::Gui::Scale(Theme::MainFontSize), Theme::MainCol, btnSize.Y);
        Hax::Gui::Space(Hax::Gui::GetContentRegionAvail().X - btnSize.X);
        bool changed = Hotkey(id, key);
        Hax::Gui::EndHorizontal();
        if (!desc.Empty())
            Label(DESC_FONT, desc, Hax::Gui::Scale(Theme::DescFontSize), Theme::DescColor);
        Hax::Gui::EndVertical();
        return changed;
    }

    bool Checkbox(size_t id, bool& val)
    {
        float size = Theme::CheckboxSize * Hax::Gui::G.ScaleFactor;

        Hax::Rect bounds;
        bounds.Min = Hax::Gui::GetCursorPos();
        bounds.Max = bounds.Min + Hax::Vector2(size, size);

        Hax::Gui::PlaceItem(bounds.GetSize());
        if (!Hax::Gui::IsItemVisible(bounds))
            return false;

        Hax::Gui::Interact(id, bounds);

        if (Hax::Gui::IsItemClicked(id))
            val = !val;

        if (Hax::Gui::IsItemHovered(id))
            Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

        Hax::Gui::DrawRect(bounds.Min, bounds.Max, {.FillColor = val ? Theme::ActiveColor : Theme::FrameColor, .Rounding = 5_px});
        return Hax::Gui::IsItemClicked(id);
    }

    bool CheckboxEx(size_t id, bool& val, Hax::WStringView text)
    {
        bool changed = false;
        Hax::Gui::BeginHorizontal(5_px);
        {
            changed = Checkbox(id, val);
            LabelAlignedByH(MAIN_FONT, text, Theme::DescFontSize * Hax::Gui::G.ScaleFactor, Hax::Gui::Color(val ? 0xB0B0B0FF : 0x8D8D8DFF), Hax::Gui::Scale(Theme::CheckboxSize));
            Hax::Gui::Interact(id, Hax::Gui::GetLayoutBounds());
        }
        Hax::Gui::EndHorizontal();

        return changed;
    }

    void HorizontalLine(float th, float w)
    {
        Hax::Gui::HorizontalLine(th, Theme::SeparatorCol, w);
    }
}