#pragma once
#include <windows.h>
#include <string>
#define DARKMODEPLUS_USER_INTERNAL			(WM_USER + 0000)
#define DARK_INTERNAL_REFRESHDARKMODE		(DARKMODEPLUS_USER_INTERNAL + 59)
typedef std::basic_string<TCHAR> generic_string;
typedef std::basic_stringstream<TCHAR> generic_stringstream;
HWND CreateToolTipDark(int toolID, HWND hDlg, HINSTANCE hInst, const wchar_t* pszText, bool isRTL, bool isTracking);
HWND CreateToolTipDarkRect(int toolID, HWND hWnd, HINSTANCE hInst, const wchar_t* pszText, const RECT rc);
namespace JppDarkMode
{
	struct Colors
	{
		COLORREF background;
		COLORREF softerBackground;
		COLORREF hotBackground;
		COLORREF pureBackground;
		COLORREF errorBackground;
		COLORREF text;
		COLORREF darkerText;
		COLORREF disabledText;
		COLORREF linkText;
		COLORREF edge;
		COLORREF hotEdge;
		COLORREF disabledEdge;
		COLORREF quickFindBackground;
	};
	struct Options
	{
		bool enable = false;
		bool enableMenubar = false;
		bool enablePlugin = false;
	};
	struct JppDarkModeParams
	{
		const wchar_t* _themeClassName;
		bool _subclass;
		bool _theme;
	};
	enum class ToolTipsType
	{
		tooltip,
		toolbar,
		listview,
		treeview,
		tabbar
	};
	enum ColorTone {
		blackTone  = 0,
		redTone    = 1,
		greenTone  = 2,
		blueTone   = 3,
		purpleTone = 4,
		cyanTone   = 5,
		oliveTone  = 6,
		customizedTone = 32
	};
	enum class TreeViewStyle
	{
		classic = 0,
		light = 1,
		dark = 2
	};
	struct AdvOptDefaults
	{
		generic_string _xmlFileName;
		int _toolBarIconSet = -1;
		int _tabIconSet = -1;
		bool _tabUseTheme = false;
	};
	struct AdvancedOptions
	{
		bool _enableWindowsMode = false;
		JppDarkMode::AdvOptDefaults _darkDefaults;
		JppDarkMode::AdvOptDefaults _lightDefaults;
	};
	void initDarkMode();
	void refreshDarkMode(HWND hwnd, bool forceRefresh = false);
	void initAdvancedOptions();
	bool isEnabled();
	bool isDarkMenuEnabled();
	bool isEnabledForPlugins();
	bool isExperimentalActive();
	bool isExperimentalSupported();
	bool isWindowsModeEnabled();
	void setWindowsMode(bool enable);
	generic_string getThemeName();
	void setThemeName(const generic_string& newThemeName);
	int getToolBarIconSet(bool useDark);
	void setToolBarIconSet(int state2Set, bool useDark);
	int getTabIconSet(bool useDark);
	void setTabIconSet(bool useAltIcons, bool useDark);
	bool useTabTheme();
	void setAdvancedOptions();
	bool isWindows10();
	bool isWindows11();
	DWORD getWindowsBuildNumber();
	COLORREF invertLightness(COLORREF c);
	COLORREF invertLightnessSofter(COLORREF c);
	double calculatePerceivedLighness(COLORREF c);
	void setDarkTone(ColorTone colorToneChoice);
	COLORREF getQuickFindBackground();
	COLORREF getBackgroundColor();
	COLORREF getSofterBackgroundColor();
	COLORREF getHotBackgroundColor();
	COLORREF getDarkerBackgroundColor();
	COLORREF getErrorBackgroundColor();
	COLORREF getTextColor();
	COLORREF getDarkerTextColor();
	COLORREF getDisabledTextColor();
	COLORREF getLinkTextColor();
	COLORREF getEdgeColor();
	COLORREF getHotEdgeColor();
	COLORREF getDisabledEdgeColor();
	HBRUSH getBackgroundBrush();
	HBRUSH getDarkerBackgroundBrush();
	HBRUSH getSofterBackgroundBrush();
	HBRUSH getHotBackgroundBrush();
	HBRUSH getErrorBackgroundBrush();
	HBRUSH getEdgeBrush();
	HBRUSH getHotEdgeBrush();
	HBRUSH getDisabledEdgeBrush();
	HPEN getDarkerTextPen();
	HPEN getEdgePen();
	HPEN getHotEdgePen();
	HPEN getDisabledEdgePen();
	COLORREF getIndividualTabColour(int colourIndex, bool themeDependant, bool saturated);
	void setQuickFindBackgroundColor(COLORREF c);
	void setBackgroundColor(COLORREF c);
	void setSofterBackgroundColor(COLORREF c);
	void setHotBackgroundColor(COLORREF c);
	void setDarkerBackgroundColor(COLORREF c);
	void setErrorBackgroundColor(COLORREF c);
	void setTextColor(COLORREF c);
	void setDarkerTextColor(COLORREF c);
	void setDisabledTextColor(COLORREF c);
	void setLinkTextColor(COLORREF c);
	void setEdgeColor(COLORREF c);
	void setHotEdgeColor(COLORREF c);
	void setDisabledEdgeColor(COLORREF c);
	Colors getDarkModeDefaultColors();
	Colors getDarkRedModeColors();
	void changeCustomTheme(const Colors& colors);
	void handleSettingChange(HWND hwnd, LPARAM lParam, bool isFromBtn = false);
	bool isDarkModeReg();
	bool runUAHWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lr);
	void drawUAHMenuNCBottomLine(HWND hWnd);
	void initExperimentalDarkMode();
	void setDarkMode(bool useDark, bool fixDarkScrollbar);
	void allowDarkModeForApp(bool allow);
	bool allowDarkModeForWindow(HWND hWnd, bool allow);
	void setTitleBarThemeColor(HWND hWnd);
	void enableDarkScrollBarForWindowAndChildren(HWND hwnd);
	inline void paintRoundFrameRect(HDC hdc, const RECT rect, const HPEN hpen, int width = 0, int height = 0);
	void subclassButtonControl(HWND hwnd);
	void subclassGroupboxControl(HWND hwnd);
	void subclassTabControl(HWND hwnd);
	void subclassComboBoxControl(HWND hwnd);
	void subclassAndThemeButton(HWND hwnd, JppDarkModeParams p);
	void subclassAndThemeComboBox(HWND hwnd, JppDarkModeParams p);
	void subclassAndThemeListBoxOrEditControl(HWND hwnd, JppDarkModeParams p, bool isListBox);
	void subclassAndThemeListView(HWND hwnd, JppDarkModeParams p);
	void themeTreeView(HWND hwnd, JppDarkModeParams p);
	void themeToolbar(HWND hwnd, JppDarkModeParams p);
	void themeRichEdit(HWND hwnd, JppDarkModeParams p);
	void autoSubclassAndThemeChildControls(HWND hwndParent, bool subclass = true, bool theme = true);
	void autoThemeChildControls(HWND hwndParent);
	LRESULT darkToolBarNotifyCustomDraw(LPARAM lParam);
	LRESULT darkListViewNotifyCustomDraw(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool isPlugin);
	LRESULT darkTreeViewNotifyCustomDraw(LPARAM lParam);
	ULONG autoSubclassAndThemePlugin(HWND hwnd, ULONG dmFlags);
	void autoSubclassAndThemeWindowNotify(HWND hwnd);
	bool subclassTabUpDownControl(HWND hwnd);
	void setMenuPositionCustom(int pos,
		COLORREF normal = RGB(70, 130, 180),
		COLORREF hot = RGB(110, 170, 220),
		COLORREF pushed = RGB(40, 90, 140),
		COLORREF disabled = RGB(90, 90, 90));
	void setDarkTitleBar(HWND hwnd);
	void setDarkExplorerTheme(HWND hwnd);
	void setDarkScrollBar(HWND hwnd);
	void setDarkTooltips(HWND hwnd, ToolTipsType type);
	void setDarkLineAbovePanelToolbar(HWND hwnd);
	void setDarkListView(HWND hwnd);
	void disableVisualStyle(HWND hwnd, bool doDisable);
	void calculateTreeViewStyle();
	void setTreeViewStyle(HWND hwnd);
	bool isThemeDark();
	void RegisterScrollBar(HWND dlg, int control_id);
	void setBorderEdge(HWND hWnd, bool doWithBorderEdge);
	void setBorder(HWND hwnd, bool border = true);
	BOOL CALLBACK enumAutocompleteProc(HWND hwnd, LPARAM lParam);
	void setDarkAutoCompletion();
	LRESULT onColorControl(UINT message,WPARAM wParam, WPARAM lParam);
	LRESULT onCtlColor(HDC hdc);
	LRESULT onCtlColorSofter(HDC hdc);
	LRESULT onCtlColorDarker(HDC hdc);
	LRESULT onCtlColorError(HDC hdc);
	LRESULT onCtlColorDarkerBGStaticText(HDC hdc, bool isTextEnabled);
	INT_PTR onCtlColorListbox(WPARAM wParam, LPARAM lParam);
}
