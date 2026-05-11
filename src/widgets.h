#pragma once

#include "haxsdk/gui/hax_gui.h"

namespace Cheat
{
    namespace Widgets
    {
        struct ButtonParams 
        {
            bool Enabled = true;
            float MinW = 0.f;
        };

        struct ToggleExParams
        {
            bool Disabled;
            size_t SettingsId;
        };

        struct SelectableParams 
        { 
            bool AutoLayout;
            bool Disabled;
            float MinW;
            float R = 0.f;
        };

        struct AutoLayoutButtonParams
        {
            bool Disabled;
            Hax::Gui::Texture2D Icon;
        };

        void Label(Hax::Gui::FontHandle hFont, Hax::WStringView text, float fontH, Hax::Gui::Color col);
        void MainLabel(Hax::WStringView text);
        void MainLabel(Hax::WStringView text, const Hax::Gui::Color& col);
        Hax::Vector2 CalcMainLabelSize(Hax::WStringView text);
        void DescLabel(Hax::WStringView text);
        void LabelAlignedByH(Hax::Gui::FontHandle hFont, Hax::WStringView label, float fontH, const Hax::Gui::Color& col, float h);
        void MainLabelAlignedByH(Hax::WStringView label, float h);
        void BeginWindow();
        void EndWindow();
        bool BeginSidePanel();
        void EndSidePanel();
        void PanelHeader(Hax::WStringView text, Hax::WStringView desc = {});
        bool SettingsBtn(size_t id, bool disabled = false);
        bool Toggle(size_t id, bool& value, bool enabled = true);
        void OpenPopup(size_t id, const Hax::Vector2& pos);
        bool ToggleEx(size_t id, bool& value, Hax::WStringView text, Hax::WStringView desc = {}, ToggleExParams params = {});
        Hax::Vector2 CalcButtonSize(Hax::WStringView label);
        float CalcButtonHeight();
        bool Button(size_t id, Hax::WStringView label, Hax::WStringView desc = {}, const ButtonParams& params = {});
        void Image(Hax::Gui::Texture2D tex);
        bool TabButton(size_t id, Hax::WStringView text, Hax::char16 icon16, bool active);
        void BeginPanel(size_t id);
        void EndPanel();
        int SliderConvertInt(int min, int max, float m);
        float SliderConvertFloat(float min, float max, float m);
        bool AutoLayoutButton(size_t id, Hax::WStringView text, const AutoLayoutButtonParams & = {});
        bool DropdownBtn(size_t id, Hax::WStringView preview, float w);
        void OpenPopup(size_t id, const Hax::Vector2& pos);
        void ClosePopup(size_t id);
        bool BeginDropList(size_t id, const Hax::Vector2& size);
        void EndDropList();
        bool BeginModal(size_t id);
        void EndModal();
        bool Selectable(size_t id, Hax::WStringView label, bool selected, SelectableParams params = {});
        float CalcWidgetEqWidth(size_t nWidgets);
        Hax::Vector2 CalcRepeatBtnSize(Hax::WStringView label);
        bool RepeatBtn(size_t id, Hax::WStringView label, bool enabled = true);
        bool Hotkey(size_t id, int& key);
        bool HotkeyEx(size_t id, int& key, Hax::WStringView label, Hax::WStringView desc = {});

        enum class TextInputFilter
        {
            Any,
            Int,
            Float,
        };

        struct TextInputParams
        {
            Hax::WStringView Hint;
            TextInputFilter Filter = TextInputFilter::Any;
            float MinW = 100.f;
            bool Disabled = false;
        };

        bool TextInput(size_t id, wchar_t* buf, size_t bufSize, const TextInputParams& params = {});
        float CalcTextInputHeight();
        bool Checkbox(size_t id, bool& val);
        bool CheckboxEx(size_t id, bool& val, Hax::WStringView text);
        void HorizontalLine(float th, float w = 0.f);

        template <typename T>
        bool Slider(size_t id, T* val, T min, T max, T(*func)(T min, T max, float mousePosNorm), float w)
        {
            T tmp{};
            if (!val) val = &tmp;
            else tmp = *val;

            float r = 6_px;
            const float trackH = 8_px;

            Hax::Rect bounds;
            bounds.Min = Hax::Gui::GetCursorPos();// bounds.Min.Y += (trackH - r) / 2.f;
            bounds.Max.X = bounds.Min.X + w;
            bounds.Max.Y = bounds.Min.Y + r * 2.f;

            Hax::Gui::PlaceItem(bounds.GetSize());
            if (!Hax::Gui::IsItemVisible(bounds))
                return false;

            float minMousePos = bounds.Min.X + r;
            float maxMousePos = bounds.Max.X - r;

            Hax::Gui::Interact(id, bounds);

            if (Hax::Gui::IsItemHovered(id))
                Hax::Gui::SetMouseIcon(Hax::Gui::MouseIcon_Hand);

            Hax::Gui::LinearAnim& state = Hax::Gui::GetState<Hax::Gui::LinearAnim>(id);
            const float deltaTime = Hax::Gui::GetDeltaTime();

            if (Hax::Gui::IsItemActive(id))
            {
                state.Elapse(deltaTime, 0.2f);
                float mousePos = Hax::Gui::GetMousePos().X;
                float mousePosNorm = (mousePos - minMousePos) / (maxMousePos - minMousePos);

                *val = func(min, max, mousePosNorm);
            }
            else
                state.Elapse(-deltaTime, 0.2f);

            float t = (float)(*val - min) / (float)(max - min);
            float thumbPosX = Hax::Lerp(minMousePos, maxMousePos, t);

            Hax::Vector2 trackMin = bounds.Min; trackMin.Y += r - trackH / 2.f;
            Hax::Vector2 trackMax = bounds.Max; trackMax.Y -= r - trackH / 2.f;
            Hax::Gui::DrawRect(trackMin, trackMax, {.FillColor = 0x282829FF, .Rounding = 8_px});

            r *= Hax::Lerp(1.f, 1.3f, state.Progress);

            trackMax.X = Hax::Max(thumbPosX, Hax::Lerp(bounds.Min.X, bounds.Max.X, t));
            Hax::Gui::DrawRect(trackMin, trackMax, {.FillColor = 0x2B69FFFF, .Rounding = 8_px});
            Hax::Gui::DrawCircle(Hax::Vector2(thumbPosX, bounds.GetCenter().Y), r, {.FillColor = Hax::Gui::Color::White});

            return *val != tmp;
        }

        template <typename T>
        bool SliderEx(size_t id, Hax::WStringView text, Hax::WStringView valTxt, T* val, T min, T max, T(*func)(T min, T max, float mousePosNorm))
        {
            Hax::Gui::BeginVertical(4_px);
            Hax::Gui::BeginHorizontal();
            Widgets::MainLabel(text, 0xC2C2C2FF);
            const Hax::Vector2 valTextSize = Widgets::CalcMainLabelSize(valTxt);
            Hax::Gui::Space(Hax::Gui::GetContentRegionAvail().X - valTextSize.X);
            Widgets::MainLabel(valTxt, Hax::Gui::Color::White);
            Hax::Gui::EndHorizontal();
            bool changed = Slider(id, val, min, max, func, Hax::Gui::GetContentRegionAvail().X);
            Hax::Gui::EndVertical();
            return changed;
        }
    }
}