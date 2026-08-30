# JppDarkMode DLL

Turn the large `JppDarkMode` C++ implementation into a reusable DLL so client applications only need to load the binary (or link against the import library) instead of compiling the 80 kB+ source into every project.

## Layout

```
JppDarkModeDLL/
├── include/
│   └── JppDarkModeAPI.h          ← public C API (the only header clients need)
├── src/
│   ├── JppDarkMode.h             ← original header (internal)
│   ├── JppDarkMode.cpp           ← original implementation (internal)
│   └── JppDarkModeAPI.cpp        ← thin C wrapper that exports the API
├── client/
│   └── ClientExample.cpp         ← dynamic-load demo
└── README.md
```

## Public C API (summary)

| Function | Purpose |
|----------|---------|
| `JppDarkMode_Initialize` | Call once at startup |
| `JppDarkMode_Shutdown` | Optional cleanup |
| `JppDarkMode_SetDarkMode(BOOL, BOOL)` | Enable/disable dark mode |
| `JppDarkMode_IsEnabled` | Query current state |
| `JppDarkMode_DoDarkMode` | One-shot apply dark mode to a window |
| `JppDarkMode_Refresh` | Force re-theme a window tree |
| `JppDarkMode_SetDarkTitleBar` | Dark title bar (Win10+) |
| `JppDarkMode_SetDarkExplorerTheme` | Dark explorer theme |
| `JppDarkMode_SetDarkScrollBar` | Dark scrollbars |
| `JppDarkMode_AutoSubclassAndThemeChildControls` | Subclass + theme all children |
| Colour getters / `OnCtlColor*` helpers | Use inside `WM_CTLCOLOR*` handlers |

Full declarations are in `include/JppDarkModeAPI.h`.

## Building the DLL (MSVC)

1. Create a new **Dynamic-Link Library (DLL)** project in Visual Studio.
2. Add the three files under `src/` to the project.
3. Add the **include** directory to Additional Include Directories.
4. Define `JPPDARKMODE_EXPORTS`.
5. Link: `user32.lib gdi32.lib uxtheme.lib dwmapi.lib shlwapi.lib`
6. Supply missing internal headers (`DarkMode.h`, `UAHMenuBar.h`, DPI managers) from the original project.
7. Build → `JppDarkMode.dll` + `JppDarkMode.lib`.

## Using the DLL from a client

### Dynamic load

```cpp
HMODULE h = LoadLibraryW(L"JppDarkMode.dll");
auto Init = (int(*)())GetProcAddress(h, "JppDarkMode_Initialize");
auto Set  = (int(*)(BOOL,BOOL))GetProcAddress(h, "JppDarkMode_SetDarkMode");
auto Do   = (void(*)(HWND))GetProcAddress(h, "JppDarkMode_DoDarkMode");
Init();
Set(TRUE, TRUE);
// In WM_CREATE:
Do(hwnd);
```

See `client/ClientExample.cpp` for a complete sample with many child controls.

### Static import

```cpp
#include "JppDarkModeAPI.h"
#pragma comment(lib, "JppDarkMode.lib")

JppDarkMode_Initialize();
JppDarkMode_SetDarkMode(TRUE, TRUE);
JppDarkMode_DoDarkMode(hwnd);
```

## Typical usage

```cpp
case WM_CREATE:
    JppDarkMode_DoDarkMode(hwnd);  // title bar + children + scrollbars + notify
    break;

case WM_CTLCOLORSTATIC:
case WM_CTLCOLOREDIT:
case WM_CTLCOLORBTN:
case WM_CTLCOLORLISTBOX:
case WM_CTLCOLORDLG:
{
    LRESULT r = JppDarkMode_OnColorControl(msg, wParam, lParam);
    if (r) return r;
    break;
}
```

## Notes

* GDI objects (brushes, pens) are owned by the DLL — do not DeleteObject them from the client.
* Windows 10 build 19041+ required for immersive dark title bar; older builds use experimental APIs.
* To extend the API: declare in `JppDarkModeAPI.h`, wrap in `JppDarkModeAPI.cpp`, rebuild.
