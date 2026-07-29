#include <src/pch.h>

#include <src/cheat.h>

namespace Cheat
{
    static HCURSOR WINAPI   Hooked_SetCursor(HCURSOR cursor);
    static BOOL WINAPI      Hooked_GetCursorPos(LPPOINT lpPoint);
    static int WINAPI       Hooked_ShowCursor(BOOL bShow);
    static BOOL WINAPI      Hooked_ClipCursor(const RECT* lpRect);
    static BOOL WINAPI      Hooked_SetCursorPos(int x, int y);
    static SHORT WINAPI     Hooked_GetAsyncKeyState(int vKey);
    static SHORT WINAPI     Hooked_GetKeyState(int vKey);
    static BOOL WINAPI      Hooked_GetKeyboardState(PBYTE lpKeyState);
    static BOOL WINAPI      Hooked_PeekMessageA(LPMSG lpMsg, HWND  hWnd, UINT  wMsgFilterMin, UINT  wMsgFilterMax, UINT  wRemoveMsg);
    static BOOL WINAPI      Hooked_PeekMessageW(LPMSG lpMsg, HWND  hWnd, UINT  wMsgFilterMin, UINT  wMsgFilterMax, UINT  wRemoveMsg);
    static BOOL WINAPI      Hooked_GetMessageA(LPMSG lpMsg, HWND  hWnd, UINT  wMsgFilterMin, UINT  wMsgFilterMax);
    static BOOL WINAPI      Hooked_GetMessageW(LPMSG lpMsg, HWND  hWnd, UINT  wMsgFilterMin, UINT  wMsgFilterMax);
    static BOOL             Hooked_TerminateProcess(HANDLE handle, UINT exitCode);

    static void             OpenMenu();
    static void             CloseMenu();

    void Hook(void* ptr, void* detour, SafetyHookInline& out, const char* name)
    {
        auto res = SafetyHookInline::create(ptr, detour, out);
        HAX_PANIC(!res.has_value(), &G->Logger, L"Failed to hooks %hs. Error %d", name, (int)res->type);
    }

    void HookModuleProc(HMODULE module, const char* procName, void* procHook, SafetyHookInline& out)
    {
        void* procPtr = ::GetProcAddress(module, procName);
        HAX_PANIC(procPtr != nullptr, &G->Logger, L"Proc %s not found", procName);

        Hook(procPtr, procHook, out, procName);
    }

    void HookWinProcs()
    {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        HookModuleProc(user32, "SetCursor",         Hooked_SetCursor,        G->SetCursorHook);
        HookModuleProc(user32, "ClipCursor",        Hooked_ClipCursor,       G->ClipCursorHook);
        HookModuleProc(user32, "GetCursorPos",      Hooked_GetCursorPos,     G->GetCursorPosHook);
        HookModuleProc(user32, "ShowCursor",        Hooked_ShowCursor,       G->ShowCursorHook);
        HookModuleProc(user32, "SetCursorPos",      Hooked_SetCursorPos,     G->SetCursorPosHook);
        HookModuleProc(user32, "GetAsyncKeyState",  Hooked_GetAsyncKeyState, G->GetAsyncKeyStateHook);
        HookModuleProc(user32, "GetKeyState",       Hooked_GetKeyState,      G->GetKeyStateHook);
        HookModuleProc(user32, "GetKeyboardState",  Hooked_GetKeyboardState, G->GetKeyboardStateHook);
        HookModuleProc(user32, "PeekMessageA",      Hooked_PeekMessageA,     G->PeekMessageAHook);
        HookModuleProc(user32, "PeekMessageW",      Hooked_PeekMessageW,     G->PeekMessageWHook);
        HookModuleProc(user32, "GetMessageA",       Hooked_GetMessageA,      G->GetMessageAHook);
        HookModuleProc(user32, "GetMessageW",       Hooked_GetMessageW,      G->GetMessageWHook);

        HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
        HookModuleProc(kernel32, "TerminateProcess", Hooked_TerminateProcess, G->TerminateProcessHook);
    }

    static HCURSOR WINAPI Hooked_SetCursor(HCURSOR icon)
    {
        if (G->ForceCustomCursorTex)
        {
            G->OrigMouseTexture = icon;
            icon = (HCURSOR)Hax::Gui::GetMouseTexture();
        }

        return G->SetCursorHook.unsafe_stdcall<HCURSOR, HCURSOR>(icon);
    }

    static BOOL WINAPI Hooked_GetCursorPos(LPPOINT lpPoint)
    {
        Hax::Optional<POINT> lockedPos = G->LockedCursorPos;
        if (lockedPos.HasValue())
        {
            *lpPoint = lockedPos.Value();
            return TRUE;
        }

        return G->GetCursorPosHook.unsafe_stdcall<BOOL, LPPOINT>(lpPoint);
    }

    static int WINAPI Hooked_ShowCursor(BOOL bShow)
    {
        if (G->ForceCursorVisible)
        {
            G->OrigCursorVisible = bShow;
            return bShow ? 1 : -1;
        }

        return G->ShowCursorHook.unsafe_stdcall<int, BOOL>(bShow);
    }

    static BOOL WINAPI Hooked_ClipCursor(const RECT* lpRect)
    {
        if (G->ForceCursorUnclip)
        {
            G->OrigClipArea.Reset();
            if (lpRect)
                G->OrigClipArea = *lpRect;

            lpRect = nullptr;
        }

        return G->ClipCursorHook.unsafe_stdcall<BOOL, const RECT*>(lpRect);
    }

    static BOOL WINAPI Hooked_SetCursorPos(int x, int y)
    {
        if (G->LockedCursorPos.HasValue())
            return TRUE;

        return G->SetCursorPosHook.unsafe_stdcall<BOOL, int, int>(x, y);
    }

    static SHORT WINAPI Hooked_GetAsyncKeyState(int vKey)
    {
        if (G->GameInputPrevented)
            return 0;

        return G->GetAsyncKeyStateHook.unsafe_stdcall<SHORT, int>(vKey);
    }

    static SHORT WINAPI Hooked_GetKeyState(int vKey)
    {
        if (G->GameInputPrevented)
            return 0;

        return G->GetKeyStateHook.unsafe_stdcall<SHORT, int>(vKey);
    }

    static BOOL WINAPI Hooked_GetKeyboardState(PBYTE lpKeyState)
    {
        BOOL result = G->GetKeyboardStateHook.unsafe_stdcall<BOOL, PBYTE>(lpKeyState);

        if (G->GameInputPrevented)
            memset(lpKeyState, 0, 256);

        return result;
    }

    static void HandleMessage(MSG* msg)
    {
        if (msg->message == WM_USER)
            G->MenuVisible ? CloseMenu() : OpenMenu();

        if (Hax::Gui::HandleWndMsg(msg->hwnd, msg->message, msg->wParam, msg->lParam))
            msg->message = WM_NULL;

        if (G->GameInputPrevented)
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
        if (!G->PeekMessageAHook.unsafe_stdcall<BOOL, LPMSG, HWND, UINT, UINT, UINT>(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg))
            return FALSE;

        if (lpMsg != nullptr)
            HandleMessage(lpMsg);

        return TRUE;
    }

    static BOOL WINAPI Hooked_PeekMessageW(LPMSG lpMsg, HWND  hWnd, UINT  wMsgFilterMin, UINT  wMsgFilterMax, UINT  wRemoveMsg)
    {
        BOOL ret = G->PeekMessageWHook.unsafe_stdcall<BOOL, LPMSG, HWND, UINT, UINT, UINT>(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);

        if (!ret)
            return FALSE;

        if (lpMsg != nullptr)
            HandleMessage(lpMsg);

        return ret;
    }

    static BOOL WINAPI Hooked_GetMessageW(LPMSG lpMsg, HWND  hWnd, UINT wMsgFilterMin, UINT  wMsgFilterMax)
    {
        BOOL ret = G->GetMessageWHook.unsafe_stdcall<BOOL, LPMSG, HWND, UINT, UINT>(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
        if (ret <= 0)
            return ret;

        if (lpMsg != nullptr)
            HandleMessage(lpMsg);

        return ret;
    }

    static BOOL WINAPI Hooked_GetMessageA(LPMSG lpMsg, HWND  hWnd, UINT wMsgFilterMin, UINT  wMsgFilterMax)
    {
        if (!G->GetMessageAHook.unsafe_stdcall<BOOL, LPMSG, HWND, UINT, UINT>(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax))
            return FALSE;

        if (lpMsg != nullptr)
            HandleMessage(lpMsg);

        return TRUE;
    }

    static BOOL Hooked_TerminateProcess(HANDLE handle, UINT exitCode)
    {
        Hax::Log(G->Logger, L"Saving ini...");
        Hax::IniSave(G->Config);

        return G->TerminateProcessHook.unsafe_stdcall<BOOL, HANDLE, UINT>(handle, exitCode);
    }

    static void OpenMenu()
    {
        POINT pos;
        ::GetCursorPos(&pos);
        G->LockedCursorPos = pos;

        if (!G->ForceCursorVisible)
        {
            int count = ::ShowCursor(TRUE);
            G->OrigCursorVisible = count > 0;

            while (count != 0)
                count = ::ShowCursor(count > 0 ? FALSE : TRUE);

            G->ForceCursorVisible = true;
        }

        if (!G->ForceCustomCursorTex)
        {
            G->OrigMouseTexture = ::GetCursor();
            ::SetCursor((HCURSOR)Hax::Gui::GetMouseTexture());
            G->ForceCustomCursorTex = true;
        }

        if (!G->ForceCursorUnclip)
        {
            RECT clipArea;
            if (::GetClipCursor(&clipArea))
                G->OrigClipArea = clipArea;

            ::ClipCursor(nullptr);

            G->ForceCursorUnclip = true;
        }

        G->GameInputPrevented = true;
        G->MenuVisible = true;
    }

    static void CloseMenu()
    {
        if (G->ForceCursorVisible)
        {
            G->ForceCursorVisible = false;

            int count = ::ShowCursor(TRUE);
            while (count != 0) 
                count = ::ShowCursor(count > 0 ? FALSE : TRUE);

            if (!G->OrigCursorVisible)
                ::ShowCursor(FALSE);
        }

        if (G->ForceCustomCursorTex)
        {
            G->ForceCustomCursorTex = false;
            ::SetCursor(G->OrigMouseTexture);
        }

        G->ForceCursorUnclip = false;
        ::ClipCursor(G->OrigClipArea.HasValue() ? &G->OrigClipArea.Value() : nullptr);

        G->GameInputPrevented = false;
        G->LockedCursorPos.Reset();
        G->MenuVisible = false;
    }
}