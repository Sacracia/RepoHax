#include "win_hooks.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "cheat.h"
#include "safetyhook/safetyhook.hpp"

namespace Cheat
{
    extern void Hook(void* ptr, void* detour, SafetyHookInline& out, const char* name);
}

namespace Cheat::WinHooks
{
    static HCURSOR WINAPI               Hooked_SetCursor(HCURSOR cursor);
    static BOOL WINAPI                  Hooked_GetCursorPos(LPPOINT lpPoint);
    static int WINAPI                   Hooked_ShowCursor(BOOL bShow);
    static BOOL WINAPI                  Hooked_ClipCursor(const RECT* lpRect);
    static BOOL WINAPI                  Hooked_SetCursorPos(int x, int y);
    static SHORT WINAPI                 Hooked_GetAsyncKeyState(int vKey);
    static SHORT WINAPI                 Hooked_GetKeyState(int vKey);
    static BOOL WINAPI                  Hooked_GetKeyboardState(PBYTE lpKeyState);
    static BOOL WINAPI                  Hooked_PeekMessageA(LPMSG lpMsg, HWND  hWnd, UINT  wMsgFilterMin, UINT  wMsgFilterMax, UINT  wRemoveMsg);
    static BOOL WINAPI                  Hooked_PeekMessageW(LPMSG lpMsg, HWND  hWnd, UINT  wMsgFilterMin, UINT  wMsgFilterMax, UINT  wRemoveMsg);
    static BOOL WINAPI                  Hooked_GetMessageA(LPMSG lpMsg, HWND  hWnd, UINT  wMsgFilterMin, UINT  wMsgFilterMax);
    static BOOL WINAPI                  Hooked_GetMessageW(LPMSG lpMsg, HWND  hWnd, UINT  wMsgFilterMin, UINT  wMsgFilterMax);
    static BOOL                         Hooked_TerminateProcess(HANDLE handle, UINT exitCode);

    static void                         OpenMenu();
    static void                         CloseMenu();

    void Install()
    {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        HookModuleProc(user32, "SetCursor", Hooked_SetCursor, GCheat->SetCursorHook);
        HookModuleProc(user32, "ClipCursor", Hooked_ClipCursor, GCheat->ClipCursorHook);
        HookModuleProc(user32, "GetCursorPos", Hooked_GetCursorPos, GCheat->GetCursorPosHook);
        HookModuleProc(user32, "ShowCursor", Hooked_ShowCursor, GCheat->ShowCursorHook);
        HookModuleProc(user32, "SetCursorPos", Hooked_SetCursorPos, GCheat->SetCursorPosHook);
        HookModuleProc(user32, "GetAsyncKeyState", Hooked_GetAsyncKeyState, GCheat->GetAsyncKeyStateHook);
        HookModuleProc(user32, "GetKeyState", Hooked_GetKeyState, GCheat->GetKeyStateHook);
        HookModuleProc(user32, "GetKeyboardState", Hooked_GetKeyboardState, GCheat->GetKeyboardStateHook);
        HookModuleProc(user32, "PeekMessageA", Hooked_PeekMessageA, GCheat->PeekMessageAHook);
        HookModuleProc(user32, "PeekMessageW", Hooked_PeekMessageW, GCheat->PeekMessageWHook);
        HookModuleProc(user32, "GetMessageA", Hooked_GetMessageA, GCheat->GetMessageAHook);
        HookModuleProc(user32, "GetMessageW", Hooked_GetMessageW, GCheat->GetMessageWHook);
        HookModuleProc(user32, "GetMessageW", Hooked_GetMessageW, GCheat->GetMessageWHook);

        HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
        HookModuleProc(kernel32, "TerminateProcess", Hooked_TerminateProcess, GCheat->TerminateProcessHook);
    }

    static HCURSOR WINAPI Hooked_SetCursor(HCURSOR icon)
    {
        if (GCheat->ForceCursorTex)
        {
            GCheat->OrigMouseTexture = icon;
            icon = (HCURSOR)Hax::Gui::GetMouseTexture();
        }

        return GCheat->SetCursorHook.unsafe_stdcall<HCURSOR, HCURSOR>(icon);
    }

    static BOOL WINAPI Hooked_GetCursorPos(LPPOINT lpPoint)
    {
        Hax::Optional<POINT> lockedPos = GCheat->LockedCursorPos;
        if (lockedPos.HasValue())
        {
            *lpPoint = lockedPos.Value();
            return TRUE;
        }

        return GCheat->GetCursorPosHook.unsafe_stdcall<BOOL, LPPOINT>(lpPoint);
    }

    static int WINAPI Hooked_ShowCursor(BOOL bShow)
    {
        if (GCheat->ForceCursorVisible)
        {
            GCheat->OrigCursorVisible = bShow;
            return bShow ? 1 : -1;
        }

        return GCheat->ShowCursorHook.unsafe_stdcall<int, BOOL>(bShow);
    }

    static BOOL WINAPI Hooked_ClipCursor(const RECT* lpRect)
    {
        if (GCheat->ForceCursorUnclip)
        {
            GCheat->OrigClipArea.Reset();
            if (lpRect)
                GCheat->OrigClipArea = *lpRect;

            lpRect = nullptr;
        }

        return GCheat->ClipCursorHook.unsafe_stdcall<BOOL, const RECT*>(lpRect);
    }

    static BOOL WINAPI Hooked_SetCursorPos(int x, int y)
    {
        if (GCheat->LockedCursorPos.HasValue())
        {
            /*cursor.PosExpected.value().x;
            cursor.PosExpected.value().y;*/
            return TRUE;
        }

        return GCheat->SetCursorPosHook.unsafe_stdcall<BOOL, int, int>(x, y);
    }

    static SHORT WINAPI Hooked_GetAsyncKeyState(int vKey)
    {
        if (GCheat->GameInputPrevented)
            return 0;

        return GCheat->GetAsyncKeyStateHook.unsafe_stdcall<SHORT, int>(vKey);
    }

    static SHORT WINAPI Hooked_GetKeyState(int vKey)
    {
        if (GCheat->GameInputPrevented)
            return 0;

        return GCheat->GetKeyStateHook.unsafe_stdcall<SHORT, int>(vKey);
    }

    static BOOL WINAPI Hooked_GetKeyboardState(PBYTE lpKeyState)
    {
        BOOL result = GCheat->GetKeyboardStateHook.unsafe_stdcall<BOOL, PBYTE>(lpKeyState);

        if (GCheat->GameInputPrevented)
            memset(lpKeyState, 0, 256);

        return result;
    }

    static void HandleMessage(MSG* msg)
    {
        if (msg->message == WM_USER)
            GCheat->MenuVisible ? CloseMenu() : OpenMenu();

        if (Hax::Gui::HandleWndMsg(msg->hwnd, msg->message, msg->wParam, msg->lParam))
            msg->message = WM_NULL;

        if (GCheat->GameInputPrevented)
        {
            switch (msg->message)
            {
            case WM_SYSKEYDOWN:
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP:
            case WM_XBUTTONDBLCLK:
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDBLCLK:
            case WM_MBUTTONDBLCLK:
            case WM_INPUT:
                msg->message = WM_NULL;
            }
        }
    }

    static BOOL WINAPI Hooked_PeekMessageA(LPMSG lpMsg, HWND  hWnd, UINT  wMsgFilterMin, UINT  wMsgFilterMax, UINT  wRemoveMsg)
    {
        if (!GCheat->PeekMessageAHook.unsafe_stdcall<BOOL, LPMSG, HWND, UINT, UINT, UINT>(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg))
            return FALSE;

        if (lpMsg != nullptr)
            HandleMessage(lpMsg);

        return TRUE;
    }

    static BOOL WINAPI Hooked_PeekMessageW(LPMSG lpMsg, HWND  hWnd, UINT  wMsgFilterMin, UINT  wMsgFilterMax, UINT  wRemoveMsg)
    {
        if (!GCheat->PeekMessageWHook.unsafe_stdcall<BOOL, LPMSG, HWND, UINT, UINT, UINT>(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg))
            return FALSE;

        if (lpMsg != nullptr)
            HandleMessage(lpMsg);

        return TRUE;
    }

    static BOOL WINAPI Hooked_GetMessageW(LPMSG lpMsg, HWND  hWnd, UINT wMsgFilterMin, UINT  wMsgFilterMax)
    {
        if (!GCheat->GetMessageWHook.unsafe_stdcall<BOOL, LPMSG, HWND, UINT, UINT>(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax))
            return FALSE;

        if (lpMsg != nullptr)
            HandleMessage(lpMsg);

        return TRUE;
    }

    static BOOL WINAPI Hooked_GetMessageA(LPMSG lpMsg, HWND  hWnd, UINT wMsgFilterMin, UINT  wMsgFilterMax)
    {
        if (!GCheat->GetMessageAHook.unsafe_stdcall<BOOL, LPMSG, HWND, UINT, UINT>(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax))
            return FALSE;

        if (lpMsg != nullptr)
            HandleMessage(lpMsg);

        return TRUE;
    }

    static BOOL Hooked_TerminateProcess(HANDLE handle, UINT exitCode)
    {
        std::unique_lock<std::shared_mutex> lock{GCheat->ShutdownMutex};
        //UVM::ThreadDetach(*GCheat->UvmThread);
        Hax::Log(GCheat->LogFile, L"Saving ini...");
        Hax::IniSave(GCheat->IniFile);

        return GCheat->TerminateProcessHook.unsafe_stdcall<BOOL, HANDLE, UINT>(handle, exitCode);
    }

    static void OpenMenu()
    {
        POINT pos;
        ::GetCursorPos(&pos);
        GCheat->LockedCursorPos = pos;

        if (!GCheat->ForceCursorVisible)
        {
            int count = ::ShowCursor(TRUE);
            GCheat->OrigCursorVisible = count > 0;

            while (count != 0)
                count = ::ShowCursor(count > 0 ? FALSE : TRUE);

            GCheat->ForceCursorVisible = true;
        }

        if (!GCheat->ForceCursorTex)
        {
            GCheat->OrigMouseTexture = ::GetCursor();
            ::SetCursor((HCURSOR)Hax::Gui::GetMouseTexture());
            GCheat->ForceCursorTex = true;
        }

        if (!GCheat->ForceCursorUnclip)
        {
            RECT clipArea;
            if (::GetClipCursor(&clipArea))
                GCheat->OrigClipArea = clipArea;

            ::ClipCursor(nullptr);

            GCheat->ForceCursorUnclip = true;
        }

        GCheat->GameInputPrevented = true;
        GCheat->MenuVisible = true;
    }

    static void CloseMenu()
    {
        if (GCheat->ForceCursorVisible)
        {
            GCheat->ForceCursorVisible = false;

            int count = ::ShowCursor(TRUE);
            while (count != 0) 
                count = ::ShowCursor(count > 0 ? FALSE : TRUE);

            if (!GCheat->OrigCursorVisible)
                ::ShowCursor(FALSE);
        }

        if (GCheat->ForceCursorTex)
        {
            GCheat->ForceCursorTex = false;
            ::SetCursor(GCheat->OrigMouseTexture);
        }

        GCheat->ForceCursorUnclip = false;
        ::ClipCursor(GCheat->OrigClipArea.HasValue() ? &GCheat->OrigClipArea.Value() : nullptr);

        GCheat->GameInputPrevented = false;
        GCheat->LockedCursorPos.Reset();
        GCheat->MenuVisible = false;

        //Hax::IniSave(GCheat->IniFile);
    }

}
