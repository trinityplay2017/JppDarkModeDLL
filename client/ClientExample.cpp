/*
 * ClientExample.cpp
 *
 * Demo client that loads JppDarkMode.dll at runtime and shows dark-mode
 * styling on a variety of common child controls.
 *
 * Build (MSVC):
 *   cl /EHsc /DUNICODE /D_UNICODE ClientExample.cpp ^
 *      user32.lib gdi32.lib comctl32.lib
 *
 * Place JppDarkMode.dll next to the executable (or in PATH).
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

/* ------------------------------------------------------------------ */
/*  Control IDs                                                       */
/* ------------------------------------------------------------------ */

enum
{
    IDC_STATIC_LABEL    = 1001,
    IDC_EDIT            = 1002,
    IDC_EDIT_MULTI      = 1003,
    IDC_BUTTON          = 1004,
    IDC_CHECKBOX        = 1005,
    IDC_RADIO1          = 1006,
    IDC_RADIO2          = 1007,
    IDC_GROUPBOX        = 1008,
    IDC_COMBO           = 1009,
    IDC_LISTBOX         = 1010,
    IDC_LISTVIEW        = 1011,
    IDC_TREEVIEW        = 1012,
    IDC_TOGGLE_DARK     = 1013
};

/* ------------------------------------------------------------------ */
/*  Function pointer typedefs matching JppDarkModeAPI.h               */
/* ------------------------------------------------------------------ */

typedef int     (*PFN_Initialize)(void);
typedef int     (*PFN_Shutdown)(void);
typedef int     (*PFN_SetDarkMode)(BOOL, BOOL);
typedef int     (*PFN_IsEnabled)(void);
typedef void    (*PFN_DoDarkMode)(HWND);
typedef void    (*PFN_Refresh)(HWND, BOOL);
typedef COLORREF(*PFN_GetBgColor)(void);
typedef HBRUSH  (*PFN_GetBgBrush)(void);
typedef LRESULT (*PFN_OnColorControl)(UINT, WPARAM, LPARAM);

/* ------------------------------------------------------------------ */
/*  Globals                                                           */
/* ------------------------------------------------------------------ */

static HMODULE                  g_hDll              = nullptr;
static PFN_Initialize           g_Initialize        = nullptr;
static PFN_Shutdown             g_Shutdown          = nullptr;
static PFN_SetDarkMode          g_SetDarkMode       = nullptr;
static PFN_IsEnabled            g_IsEnabled         = nullptr;
static PFN_DoDarkMode           g_DoDarkMode        = nullptr;
static PFN_Refresh              g_Refresh           = nullptr;
static PFN_GetBgColor           g_GetBgColor        = nullptr;
static PFN_GetBgBrush           g_GetBgBrush        = nullptr;
static PFN_OnColorControl       g_OnColorControl    = nullptr;

static HFONT g_hFont = nullptr;

/* ------------------------------------------------------------------ */
/*  Loader                                                            */
/* ------------------------------------------------------------------ */

bool LoadJppDarkMode(const wchar_t* dllPath = L"JppDarkMode.dll")
{
    g_hDll = LoadLibraryW(dllPath);
    if (!g_hDll)
    {
        wprintf(L"Failed to load %s  (error %lu)\n", dllPath, GetLastError());
        return false;
    }

#define LOAD(name, var) \
    var = reinterpret_cast<decltype(var)>(GetProcAddress(g_hDll, name)); \
    if (!var) { wprintf(L"Missing export: %S\n", name); return false; }

    LOAD("JppDarkMode_Initialize",         g_Initialize);
    LOAD("JppDarkMode_Shutdown",           g_Shutdown);
    LOAD("JppDarkMode_SetDarkMode",        g_SetDarkMode);
    LOAD("JppDarkMode_IsEnabled",          g_IsEnabled);
    LOAD("JppDarkMode_DoDarkMode",         g_DoDarkMode);
    LOAD("JppDarkMode_Refresh",            g_Refresh);
    LOAD("JppDarkMode_GetBackgroundColor", g_GetBgColor);
    LOAD("JppDarkMode_GetBackgroundBrush", g_GetBgBrush);
    LOAD("JppDarkMode_OnColorControl",     g_OnColorControl);

#undef LOAD
    return true;
}

void UnloadJppDarkMode()
{
    if (g_Shutdown) g_Shutdown();
    if (g_hDll)
    {
        FreeLibrary(g_hDll);
        g_hDll = nullptr;
    }
}

/* ------------------------------------------------------------------ */
/*  Create all child controls                                         */
/* ------------------------------------------------------------------ */

static void CreateChildControls(HWND hwnd, HINSTANCE hInst)
{
    /* ---- left column ---- */

    CreateWindowExW(0, L"STATIC", L"Static label:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        20, 20, 120, 20, hwnd, (HMENU)(INT_PTR)IDC_STATIC_LABEL, hInst, nullptr);

    CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"Single-line edit",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
        150, 18, 200, 24, hwnd, (HMENU)(INT_PTR)IDC_EDIT, hInst, nullptr);

    CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT",
        L"Multi-line edit\r\nLine 2\r\nLine 3",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
        ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN,
        20, 55, 330, 70, hwnd, (HMENU)(INT_PTR)IDC_EDIT_MULTI, hInst, nullptr);

    CreateWindowExW(0, L"BUTTON", L"Push Button",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        20, 140, 110, 28, hwnd, (HMENU)(INT_PTR)IDC_BUTTON, hInst, nullptr);

    CreateWindowExW(0, L"BUTTON", L"Check Box",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
        150, 140, 110, 28, hwnd, (HMENU)(INT_PTR)IDC_CHECKBOX, hInst, nullptr);

    /* radio group inside a group box */
    CreateWindowExW(0, L"BUTTON", L" Options ",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        20, 180, 240, 80, hwnd, (HMENU)(INT_PTR)IDC_GROUPBOX, hInst, nullptr);

    CreateWindowExW(0, L"BUTTON", L"Radio A",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTORADIOBUTTON | WS_GROUP,
        35, 205, 100, 22, hwnd, (HMENU)(INT_PTR)IDC_RADIO1, hInst, nullptr);

    CreateWindowExW(0, L"BUTTON", L"Radio B",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTORADIOBUTTON,
        35, 230, 100, 22, hwnd, (HMENU)(INT_PTR)IDC_RADIO2, hInst, nullptr);

    /* combo box */
    HWND hCombo = CreateWindowExW(0, L"COMBOBOX", nullptr,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL,
        20, 275, 180, 200, hwnd, (HMENU)(INT_PTR)IDC_COMBO, hInst, nullptr);

    SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Item One");
    SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Item Two");
    SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Item Three");
    SendMessageW(hCombo, CB_SETCURSEL, 0, 0);

    /* ---- right column ---- */

    CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", nullptr,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | LBS_NOTIFY,
        370, 20, 200, 120, hwnd, (HMENU)(INT_PTR)IDC_LISTBOX, hInst, nullptr);

    HWND hList = GetDlgItem(hwnd, IDC_LISTBOX);
    SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)L"List item 1");
    SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)L"List item 2");
    SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)L"List item 3");
    SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)L"List item 4");
    SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)L"List item 5");

    /* List-View */
    HWND hLV = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEWW, nullptr,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL,
        370, 150, 200, 100, hwnd, (HMENU)(INT_PTR)IDC_LISTVIEW, hInst, nullptr);

    ListView_SetExtendedListViewStyle(hLV, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    LVCOLUMNW col = {};
    col.mask = LVCF_TEXT | LVCF_WIDTH;
    col.cx = 90;
    col.pszText = const_cast<wchar_t*>(L"Name");
    ListView_InsertColumn(hLV, 0, &col);
    col.pszText = const_cast<wchar_t*>(L"Value");
    ListView_InsertColumn(hLV, 1, &col);

    LVITEMW item = {};
    item.mask = LVIF_TEXT;
    item.iItem = 0;
    item.pszText = const_cast<wchar_t*>(L"Alpha");
    ListView_InsertItem(hLV, &item);
    ListView_SetItemText(hLV, 0, 1, const_cast<wchar_t*>(L"100"));

    item.iItem = 1;
    item.pszText = const_cast<wchar_t*>(L"Beta");
    ListView_InsertItem(hLV, &item);
    ListView_SetItemText(hLV, 1, 1, const_cast<wchar_t*>(L"200"));

    /* Tree-View */
    HWND hTV = CreateWindowExW(WS_EX_CLIENTEDGE, WC_TREEVIEWW, nullptr,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT,
        370, 260, 200, 100, hwnd, (HMENU)(INT_PTR)IDC_TREEVIEW, hInst, nullptr);

    TVINSERTSTRUCTW tvis = {};
    tvis.hParent = TVI_ROOT;
    tvis.hInsertAfter = TVI_LAST;
    tvis.item.mask = TVIF_TEXT;
    tvis.item.pszText = const_cast<wchar_t*>(L"Root");
    HTREEITEM hRoot = TreeView_InsertItem(hTV, &tvis);

    tvis.hParent = hRoot;
    tvis.item.pszText = const_cast<wchar_t*>(L"Child A");
    TreeView_InsertItem(hTV, &tvis);
    tvis.item.pszText = const_cast<wchar_t*>(L"Child B");
    TreeView_InsertItem(hTV, &tvis);
    TreeView_Expand(hTV, hRoot, TVE_EXPAND);

    /* Toggle button at the bottom */
    CreateWindowExW(0, L"BUTTON", L"Toggle Dark Mode",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        20, 320, 160, 30, hwnd, (HMENU)(INT_PTR)IDC_TOGGLE_DARK, hInst, nullptr);

    /* Apply a consistent font to every child */
    g_hFont = CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                          DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                          CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    if (g_hFont)
    {
        EnumChildWindows(hwnd, [](HWND hChild, LPARAM lParam) -> BOOL
        {
            SendMessageW(hChild, WM_SETFONT, (WPARAM)lParam, TRUE);
            return TRUE;
        }, (LPARAM)g_hFont);
    }
}

/* ------------------------------------------------------------------ */
/*  Window procedure                                                  */
/* ------------------------------------------------------------------ */

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        CreateChildControls(hwnd, cs->hInstance);

        /*
         * One call applies dark title bar, themes all children,
         * subclasses for custom-draw, and enables dark scrollbars.
         */
        if (g_DoDarkMode)
            g_DoDarkMode(hwnd);
        break;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_TOGGLE_DARK)
        {
            BOOL currentlyOn = g_IsEnabled ? (g_IsEnabled() != 0) : FALSE;
            if (g_SetDarkMode)
                g_SetDarkMode(!currentlyOn, TRUE);
            if (g_DoDarkMode)
                g_DoDarkMode(hwnd);
            if (g_Refresh)
                g_Refresh(hwnd, TRUE);
            InvalidateRect(hwnd, nullptr, TRUE);
        }
        break;

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORDLG:
        if (g_OnColorControl)
        {
            LRESULT r = g_OnColorControl(msg, wParam, lParam);
            if (r) return r;
        }
        break;

    case WM_ERASEBKGND:
        if (g_IsEnabled && g_IsEnabled() && g_GetBgBrush)
        {
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect(reinterpret_cast<HDC>(wParam), &rc, g_GetBgBrush());
            return 1;
        }
        break;

    case WM_DESTROY:
        if (g_hFont)
        {
            DeleteObject(g_hFont);
            g_hFont = nullptr;
        }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

/* ------------------------------------------------------------------ */
/*  Entry point                                                       */
/* ------------------------------------------------------------------ */

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int nCmdShow)
{
    /* Init common controls (list-view, tree-view, etc.) */
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES | ICC_STANDARD_CLASSES };
    InitCommonControlsEx(&icc);

    if (!LoadJppDarkMode())
        return 1;

    if (g_Initialize() != 0)
    {
        wprintf(L"JppDarkMode_Initialize failed\n");
        UnloadJppDarkMode();
        return 1;
    }

    /* Enable dark mode for the process */
    g_SetDarkMode(TRUE, TRUE);

    WNDCLASSW wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = L"JppDarkModeDemo";
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, L"JppDarkModeDemo", L"JppDarkMode DLL – Child Controls Demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 420,
        nullptr, nullptr, hInst, nullptr);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnloadJppDarkMode();
    return 0;
}
