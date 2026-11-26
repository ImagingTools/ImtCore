// The enums of the Qt namespace a QML document sees (qnamespace.h: Q_ENUM_NS, and the enums of Q_FLAG_NS flags), with the
// values of Qt 6.12: generated from what Qt prints for every name (tests/diff/cases/qt_namespace_enums.qml compares them).
// A scoped enum (enum class) is an object: Qt.ColorScheme.Dark.
module.exports = {
    // GlobalColor
    color0: 0, color1: 1, black: 2, white: 3, darkGray: 4, gray: 5, lightGray: 6, red: 7, green: 8, blue: 9, cyan: 10,
    magenta: 11, yellow: 12, darkRed: 13, darkGreen: 14, darkBlue: 15, darkCyan: 16, darkMagenta: 17, darkYellow: 18,
    transparent: 19,
    // ColorScheme
    ColorScheme: { Unknown: 0, Light: 1, Dark: 2 },
    // ContrastPreference
    ContrastPreference: { NoPreference: 0, HighContrast: 1 },
    // MotionPreference
    MotionPreference: { NoPreference: 0, ReducedMotion: 1 },
    // MouseButton
    NoButton: 0, LeftButton: 1, RightButton: 2, MiddleButton: 4, BackButton: 8, XButton1: 8, ExtraButton1: 8,
    ForwardButton: 16, XButton2: 16, ExtraButton2: 16, TaskButton: 32, ExtraButton3: 32, ExtraButton4: 64,
    ExtraButton5: 128, ExtraButton6: 0x100, ExtraButton7: 0x200, ExtraButton8: 0x400, ExtraButton9: 0x800,
    ExtraButton10: 0x1000, ExtraButton11: 0x2000, ExtraButton12: 0x4000, ExtraButton13: 0x8000,
    ExtraButton14: 0x10000, ExtraButton15: 0x20000, ExtraButton16: 0x40000, ExtraButton17: 0x80000,
    ExtraButton18: 0x100000, ExtraButton19: 0x200000, ExtraButton20: 0x400000, ExtraButton21: 0x800000,
    ExtraButton22: 0x1000000, ExtraButton23: 0x2000000, ExtraButton24: 0x4000000, AllButtons: 0x7ffffff,
    MaxMouseButton: 0x4000000, MouseButtonMask: -1,
    // Orientation
    Horizontal: 1, Vertical: 2,
    // FocusPolicy
    NoFocus: 0, TabFocus: 1, ClickFocus: 2, StrongFocus: 11, WheelFocus: 15,
    // TabFocusBehavior
    NoTabFocus: 0, TabFocusTextControls: 1, TabFocusListControls: 2, TabFocusAllControls: 255,
    // SortOrder
    AscendingOrder: 0, DescendingOrder: 1,
    // SplitBehaviorFlags
    KeepEmptyParts: 0, SkipEmptyParts: 1,
    // AlignmentFlag
    AlignLeft: 1, AlignLeading: 1, AlignRight: 2, AlignTrailing: 2, AlignHCenter: 4, AlignJustify: 8,
    AlignAbsolute: 16, AlignHorizontal_Mask: 31, AlignTop: 32, AlignBottom: 64, AlignVCenter: 128,
    AlignBaseline: 0x100, AlignVertical_Mask: 0x1e0, AlignCenter: 132,
    // TextFlag
    TextSingleLine: 0x100, TextDontClip: 0x200, TextExpandTabs: 0x400, TextShowMnemonic: 0x800, TextWordWrap: 0x1000,
    TextWrapAnywhere: 0x2000, TextDontPrint: 0x4000, TextIncludeTrailingSpaces: 0x8000000, TextHideMnemonic: 0x8000,
    TextJustificationForced: 0x10000, TextForceLeftToRight: 0x20000, TextForceRightToLeft: 0x40000,
    TextLongestVariant: 0x80000,
    // TextElideMode
    ElideLeft: 0, ElideRight: 1, ElideMiddle: 2, ElideNone: 3,
    // WindowType
    Widget: 0, Window: 1, Dialog: 3, Sheet: 5, Drawer: 7, Popup: 9, Tool: 11, ToolTip: 13, SplashScreen: 15,
    SubWindow: 18, ForeignWindow: 33, CoverWindow: 65, WindowType_Mask: 255, MSWindowsFixedSizeDialogHint: 0x100,
    MSWindowsOwnDC: 0x200, BypassWindowManagerHint: 0x400, X11BypassWindowManagerHint: 0x400,
    FramelessWindowHint: 0x800, WindowTitleHint: 0x1000, WindowSystemMenuHint: 0x2000,
    WindowMinimizeButtonHint: 0x4000, WindowMaximizeButtonHint: 0x8000, WindowMinMaxButtonsHint: 0xc000,
    WindowContextHelpButtonHint: 0x10000, WindowShadeButtonHint: 0x20000, WindowStaysOnTopHint: 0x40000,
    WindowTransparentForInput: 0x80000, WindowOverridesSystemGestures: 0x100000, WindowDoesNotAcceptFocus: 0x200000,
    ExpandedClientAreaHint: 0x400000, NoTitleBarBackgroundHint: 0x800000, CustomizeWindowHint: 0x2000000,
    WindowStaysOnBottomHint: 0x4000000, WindowCloseButtonHint: 0x8000000, BypassGraphicsProxyWidget: 0x20000000,
    NoDropShadowWindowHint: 0x40000000, WindowFullscreenButtonHint: -2147483648,
    // WindowState
    WindowNoState: 0, WindowMinimized: 1, WindowMaximized: 2, WindowFullScreen: 4, WindowActive: 8,
    // ApplicationState
    ApplicationSuspended: 0, ApplicationHidden: 1, ApplicationInactive: 2, ApplicationActive: 4,
    // ScreenOrientation
    PrimaryOrientation: 0, PortraitOrientation: 1, LandscapeOrientation: 2, InvertedPortraitOrientation: 4,
    InvertedLandscapeOrientation: 8,
    // WidgetAttribute
    WA_Disabled: 0, WA_UnderMouse: 1, WA_MouseTracking: 2, WA_OpaquePaintEvent: 4, WA_StaticContents: 5,
    WA_LaidOut: 7, WA_PaintOnScreen: 8, WA_NoSystemBackground: 9, WA_UpdatesDisabled: 10, WA_Mapped: 11,
    WA_InputMethodEnabled: 14, WA_WState_Visible: 15, WA_WState_Hidden: 16, WA_ForceDisabled: 32,
    WA_KeyCompression: 33, WA_PendingMoveEvent: 34, WA_PendingResizeEvent: 35, WA_SetPalette: 36, WA_SetFont: 37,
    WA_SetCursor: 38, WA_NoChildEventsFromChildren: 39, WA_WindowModified: 41, WA_Resized: 42, WA_Moved: 43,
    WA_PendingUpdate: 44, WA_InvalidSize: 45, WA_CustomWhatsThis: 47, WA_LayoutOnEntireRect: 48,
    WA_OutsideWSRange: 49, WA_GrabbedShortcut: 50, WA_TransparentForMouseEvents: 51, WA_PaintUnclipped: 52,
    WA_SetWindowIcon: 53, WA_NoMouseReplay: 54, WA_DeleteOnClose: 55, WA_RightToLeft: 56, WA_SetLayoutDirection: 57,
    WA_NoChildEventsForParent: 58, WA_ForceUpdatesDisabled: 59, WA_WState_Created: 60, WA_WState_CompressKeys: 61,
    WA_WState_InPaintEvent: 62, WA_WState_Reparented: 63, WA_WState_ConfigPending: 64, WA_WState_Polished: 66,
    WA_WState_OwnSizePolicy: 68, WA_WState_ExplicitShowHide: 69, WA_ShowModal: 70, WA_MouseNoMask: 71,
    WA_NoMousePropagation: 73, WA_Hover: 74, WA_InputMethodTransparent: 75, WA_QuitOnClose: 76,
    WA_KeyboardFocusChange: 77, WA_AcceptDrops: 78, WA_DropSiteRegistered: 79, WA_WindowPropagation: 80,
    WA_NoX11EventCompression: 81, WA_TintedBackground: 82, WA_X11OpenGLOverlay: 83, WA_AlwaysShowToolTips: 84,
    WA_MacOpaqueSizeGrip: 85, WA_SetStyle: 86, WA_SetLocale: 87, WA_MacShowFocusRect: 88, WA_MacNormalSize: 89,
    WA_MacSmallSize: 90, WA_MacMiniSize: 91, WA_LayoutUsesWidgetRect: 92, WA_StyledBackground: 93,
    WA_CanHostQMdiSubWindowTitleBar: 95, WA_MacAlwaysShowToolWindow: 96, WA_StyleSheet: 97,
    WA_ShowWithoutActivating: 98, WA_X11BypassTransientForHint: 99, WA_NativeWindow: 100,
    WA_DontCreateNativeAncestors: 101, WA_DontShowOnScreen: 103, WA_X11NetWmWindowTypeDesktop: 104,
    WA_X11NetWmWindowTypeDock: 105, WA_X11NetWmWindowTypeToolBar: 106, WA_X11NetWmWindowTypeMenu: 107,
    WA_X11NetWmWindowTypeUtility: 108, WA_X11NetWmWindowTypeSplash: 109, WA_X11NetWmWindowTypeDialog: 110,
    WA_X11NetWmWindowTypeDropDownMenu: 111, WA_X11NetWmWindowTypePopupMenu: 112, WA_X11NetWmWindowTypeToolTip: 113,
    WA_X11NetWmWindowTypeNotification: 114, WA_X11NetWmWindowTypeCombo: 115, WA_X11NetWmWindowTypeDND: 116,
    WA_SetWindowModality: 118, WA_WState_WindowOpacitySet: 119, WA_TranslucentBackground: 120,
    WA_AcceptTouchEvents: 121, WA_WState_AcceptedTouchBeginEvent: 122, WA_TouchPadAcceptSingleTouchEvents: 123,
    WA_X11DoNotAcceptFocus: 126, WA_AlwaysStackOnTop: 128, WA_TabletTracking: 129,
    WA_ContentsMarginsRespectsSafeArea: 130, WA_StyleSheetTarget: 131, WA_AttributeCount: 132,
    // ApplicationAttribute
    AA_QtQuickUseDefaultSizePolicy: 1, AA_DontShowIconsInMenus: 2, AA_NativeWindows: 3,
    AA_DontCreateNativeWidgetSiblings: 4, AA_PluginApplication: 5, AA_DontUseNativeMenuBar: 6,
    AA_MacDontSwapCtrlAndMeta: 7, AA_Use96Dpi: 8, AA_DisableNativeVirtualKeyboard: 9, AA_DontUseNativeMenuWindows: 10,
    AA_SynthesizeTouchForUnhandledMouseEvents: 11, AA_SynthesizeMouseForUnhandledTouchEvents: 12,
    AA_ForceRasterWidgets: 14, AA_UseDesktopOpenGL: 15, AA_UseOpenGLES: 16, AA_UseSoftwareOpenGL: 17,
    AA_ShareOpenGLContexts: 18, AA_SetPalette: 19, AA_DisableHighDpiScaling: 21,
    AA_UseStyleSheetPropagationInWidgetStyles: 22, AA_DontUseNativeDialogs: 23,
    AA_SynthesizeMouseForUnhandledTabletEvents: 24, AA_CompressHighFrequencyEvents: 25,
    AA_DontCheckOpenGLContextThreadAffinity: 26, AA_DisableShaderDiskCache: 27,
    AA_DontShowShortcutsInContextMenus: 28, AA_CompressTabletEvents: 29, AA_DisableSessionManager: 31,
    AA_AttributeCount: 32,
    // ImageConversionFlag
    ColorMode_Mask: 3, AutoColor: 0, ColorOnly: 3, MonoOnly: 2, AlphaDither_Mask: 12, ThresholdAlphaDither: 0,
    OrderedAlphaDither: 4, DiffuseAlphaDither: 8, NoAlpha: 12, Dither_Mask: 48, DiffuseDither: 0, OrderedDither: 16,
    ThresholdDither: 32, DitherMode_Mask: 192, AutoDither: 0, PreferDither: 64, AvoidDither: 128,
    NoOpaqueDetection: 0x100, NoFormatConversion: 0x200,
    // BGMode
    TransparentMode: 0, OpaqueMode: 1,
    // Key
    Key_Space: 32, Key_Any: 32, Key_Exclam: 33, Key_QuoteDbl: 34, Key_NumberSign: 35, Key_Dollar: 36, Key_Percent: 37,
    Key_Ampersand: 38, Key_Apostrophe: 39, Key_ParenLeft: 40, Key_ParenRight: 41, Key_Asterisk: 42, Key_Plus: 43,
    Key_Comma: 44, Key_Minus: 45, Key_Period: 46, Key_Slash: 47, Key_0: 48, Key_1: 49, Key_2: 50, Key_3: 51,
    Key_4: 52, Key_5: 53, Key_6: 54, Key_7: 55, Key_8: 56, Key_9: 57, Key_Colon: 58, Key_Semicolon: 59, Key_Less: 60,
    Key_Equal: 61, Key_Greater: 62, Key_Question: 63, Key_At: 64, Key_A: 65, Key_B: 66, Key_C: 67, Key_D: 68,
    Key_E: 69, Key_F: 70, Key_G: 71, Key_H: 72, Key_I: 73, Key_J: 74, Key_K: 75, Key_L: 76, Key_M: 77, Key_N: 78,
    Key_O: 79, Key_P: 80, Key_Q: 81, Key_R: 82, Key_S: 83, Key_T: 84, Key_U: 85, Key_V: 86, Key_W: 87, Key_X: 88,
    Key_Y: 89, Key_Z: 90, Key_BracketLeft: 91, Key_Backslash: 92, Key_BracketRight: 93, Key_AsciiCircum: 94,
    Key_Underscore: 95, Key_QuoteLeft: 96, Key_BraceLeft: 123, Key_Bar: 124, Key_BraceRight: 125, Key_AsciiTilde: 126,
    Key_nobreakspace: 160, Key_exclamdown: 161, Key_cent: 162, Key_sterling: 163, Key_currency: 164, Key_yen: 165,
    Key_brokenbar: 166, Key_section: 167, Key_diaeresis: 168, Key_copyright: 169, Key_ordfeminine: 170,
    Key_guillemotleft: 171, Key_notsign: 172, Key_hyphen: 173, Key_registered: 174, Key_macron: 175, Key_degree: 176,
    Key_plusminus: 177, Key_twosuperior: 178, Key_threesuperior: 179, Key_acute: 180, Key_micro: 181,
    Key_paragraph: 182, Key_periodcentered: 183, Key_cedilla: 184, Key_onesuperior: 185, Key_masculine: 186,
    Key_guillemotright: 187, Key_onequarter: 188, Key_onehalf: 189, Key_threequarters: 190, Key_questiondown: 191,
    Key_Agrave: 192, Key_Aacute: 193, Key_Acircumflex: 194, Key_Atilde: 195, Key_Adiaeresis: 196, Key_Aring: 197,
    Key_AE: 198, Key_Ccedilla: 199, Key_Egrave: 200, Key_Eacute: 201, Key_Ecircumflex: 202, Key_Ediaeresis: 203,
    Key_Igrave: 204, Key_Iacute: 205, Key_Icircumflex: 206, Key_Idiaeresis: 207, Key_ETH: 208, Key_Ntilde: 209,
    Key_Ograve: 210, Key_Oacute: 211, Key_Ocircumflex: 212, Key_Otilde: 213, Key_Odiaeresis: 214, Key_multiply: 215,
    Key_Ooblique: 216, Key_Ugrave: 217, Key_Uacute: 218, Key_Ucircumflex: 219, Key_Udiaeresis: 220, Key_Yacute: 221,
    Key_THORN: 222, Key_ssharp: 223, Key_division: 247, Key_ydiaeresis: 255, Key_Escape: 0x1000000,
    Key_Tab: 0x1000001, Key_Backtab: 0x1000002, Key_Backspace: 0x1000003, Key_Return: 0x1000004, Key_Enter: 0x1000005,
    Key_Insert: 0x1000006, Key_Delete: 0x1000007, Key_Pause: 0x1000008, Key_Print: 0x1000009, Key_SysReq: 0x100000a,
    Key_Clear: 0x100000b, Key_Home: 0x1000010, Key_End: 0x1000011, Key_Left: 0x1000012, Key_Up: 0x1000013,
    Key_Right: 0x1000014, Key_Down: 0x1000015, Key_PageUp: 0x1000016, Key_PageDown: 0x1000017, Key_Shift: 0x1000020,
    Key_Control: 0x1000021, Key_Meta: 0x1000022, Key_Alt: 0x1000023, Key_CapsLock: 0x1000024, Key_NumLock: 0x1000025,
    Key_ScrollLock: 0x1000026, Key_F1: 0x1000030, Key_F2: 0x1000031, Key_F3: 0x1000032, Key_F4: 0x1000033,
    Key_F5: 0x1000034, Key_F6: 0x1000035, Key_F7: 0x1000036, Key_F8: 0x1000037, Key_F9: 0x1000038, Key_F10: 0x1000039,
    Key_F11: 0x100003a, Key_F12: 0x100003b, Key_F13: 0x100003c, Key_F14: 0x100003d, Key_F15: 0x100003e,
    Key_F16: 0x100003f, Key_F17: 0x1000040, Key_F18: 0x1000041, Key_F19: 0x1000042, Key_F20: 0x1000043,
    Key_F21: 0x1000044, Key_F22: 0x1000045, Key_F23: 0x1000046, Key_F24: 0x1000047, Key_F25: 0x1000048,
    Key_F26: 0x1000049, Key_F27: 0x100004a, Key_F28: 0x100004b, Key_F29: 0x100004c, Key_F30: 0x100004d,
    Key_F31: 0x100004e, Key_F32: 0x100004f, Key_F33: 0x1000050, Key_F34: 0x1000051, Key_F35: 0x1000052,
    Key_Super_L: 0x1000053, Key_Super_R: 0x1000054, Key_Menu: 0x1000055, Key_Hyper_L: 0x1000056,
    Key_Hyper_R: 0x1000057, Key_Help: 0x1000058, Key_Direction_L: 0x1000059, Key_Direction_R: 0x1000060,
    Key_AltGr: 0x1001103, Key_Multi_key: 0x1001120, Key_Codeinput: 0x1001137, Key_SingleCandidate: 0x100113c,
    Key_MultipleCandidate: 0x100113d, Key_PreviousCandidate: 0x100113e, Key_Mode_switch: 0x100117e,
    Key_Kanji: 0x1001121, Key_Muhenkan: 0x1001122, Key_Henkan: 0x1001123, Key_Romaji: 0x1001124,
    Key_Hiragana: 0x1001125, Key_Katakana: 0x1001126, Key_Hiragana_Katakana: 0x1001127, Key_Zenkaku: 0x1001128,
    Key_Hankaku: 0x1001129, Key_Zenkaku_Hankaku: 0x100112a, Key_Touroku: 0x100112b, Key_Massyo: 0x100112c,
    Key_Kana_Lock: 0x100112d, Key_Kana_Shift: 0x100112e, Key_Eisu_Shift: 0x100112f, Key_Eisu_toggle: 0x1001130,
    Key_Hangul: 0x1001131, Key_Hangul_Start: 0x1001132, Key_Hangul_End: 0x1001133, Key_Hangul_Hanja: 0x1001134,
    Key_Hangul_Jamo: 0x1001135, Key_Hangul_Romaja: 0x1001136, Key_Hangul_Jeonja: 0x1001138,
    Key_Hangul_Banja: 0x1001139, Key_Hangul_PreHanja: 0x100113a, Key_Hangul_PostHanja: 0x100113b,
    Key_Hangul_Special: 0x100113f, Key_Dead_Grave: 0x1001250, Key_Dead_Acute: 0x1001251,
    Key_Dead_Circumflex: 0x1001252, Key_Dead_Tilde: 0x1001253, Key_Dead_Macron: 0x1001254, Key_Dead_Breve: 0x1001255,
    Key_Dead_Abovedot: 0x1001256, Key_Dead_Diaeresis: 0x1001257, Key_Dead_Abovering: 0x1001258,
    Key_Dead_Doubleacute: 0x1001259, Key_Dead_Caron: 0x100125a, Key_Dead_Cedilla: 0x100125b,
    Key_Dead_Ogonek: 0x100125c, Key_Dead_Iota: 0x100125d, Key_Dead_Voiced_Sound: 0x100125e,
    Key_Dead_Semivoiced_Sound: 0x100125f, Key_Dead_Belowdot: 0x1001260, Key_Dead_Hook: 0x1001261,
    Key_Dead_Horn: 0x1001262, Key_Dead_Stroke: 0x1001263, Key_Dead_Abovecomma: 0x1001264,
    Key_Dead_Abovereversedcomma: 0x1001265, Key_Dead_Doublegrave: 0x1001266, Key_Dead_Belowring: 0x1001267,
    Key_Dead_Belowmacron: 0x1001268, Key_Dead_Belowcircumflex: 0x1001269, Key_Dead_Belowtilde: 0x100126a,
    Key_Dead_Belowbreve: 0x100126b, Key_Dead_Belowdiaeresis: 0x100126c, Key_Dead_Invertedbreve: 0x100126d,
    Key_Dead_Belowcomma: 0x100126e, Key_Dead_Currency: 0x100126f, Key_Dead_a: 0x1001280, Key_Dead_A: 0x1001281,
    Key_Dead_e: 0x1001282, Key_Dead_E: 0x1001283, Key_Dead_i: 0x1001284, Key_Dead_I: 0x1001285, Key_Dead_o: 0x1001286,
    Key_Dead_O: 0x1001287, Key_Dead_u: 0x1001288, Key_Dead_U: 0x1001289, Key_Dead_Small_Schwa: 0x100128a,
    Key_Dead_Capital_Schwa: 0x100128b, Key_Dead_Greek: 0x100128c, Key_Dead_Lowline: 0x1001290,
    Key_Dead_Aboveverticalline: 0x1001291, Key_Dead_Belowverticalline: 0x1001292,
    Key_Dead_Longsolidusoverlay: 0x1001293, Key_Back: 0x1000061, Key_Forward: 0x1000062, Key_Stop: 0x1000063,
    Key_Refresh: 0x1000064, Key_VolumeDown: 0x1000070, Key_VolumeMute: 0x1000071, Key_VolumeUp: 0x1000072,
    Key_BassBoost: 0x1000073, Key_BassUp: 0x1000074, Key_BassDown: 0x1000075, Key_TrebleUp: 0x1000076,
    Key_TrebleDown: 0x1000077, Key_MediaPlay: 0x1000080, Key_MediaStop: 0x1000081, Key_MediaPrevious: 0x1000082,
    Key_MediaNext: 0x1000083, Key_MediaRecord: 0x1000084, Key_MediaPause: 0x1000085,
    Key_MediaTogglePlayPause: 0x1000086, Key_HomePage: 0x1000090, Key_Favorites: 0x1000091, Key_Search: 0x1000092,
    Key_Standby: 0x1000093, Key_OpenUrl: 0x1000094, Key_LaunchMail: 0x10000a0, Key_LaunchMedia: 0x10000a1,
    Key_Launch0: 0x10000a2, Key_Launch1: 0x10000a3, Key_Launch2: 0x10000a4, Key_Launch3: 0x10000a5,
    Key_Launch4: 0x10000a6, Key_Launch5: 0x10000a7, Key_Launch6: 0x10000a8, Key_Launch7: 0x10000a9,
    Key_Launch8: 0x10000aa, Key_Launch9: 0x10000ab, Key_LaunchA: 0x10000ac, Key_LaunchB: 0x10000ad,
    Key_LaunchC: 0x10000ae, Key_LaunchD: 0x10000af, Key_LaunchE: 0x10000b0, Key_LaunchF: 0x10000b1,
    Key_MonBrightnessUp: 0x10000b2, Key_MonBrightnessDown: 0x10000b3, Key_KeyboardLightOnOff: 0x10000b4,
    Key_KeyboardBrightnessUp: 0x10000b5, Key_KeyboardBrightnessDown: 0x10000b6, Key_PowerOff: 0x10000b7,
    Key_WakeUp: 0x10000b8, Key_Eject: 0x10000b9, Key_ScreenSaver: 0x10000ba, Key_WWW: 0x10000bb, Key_Memo: 0x10000bc,
    Key_LightBulb: 0x10000bd, Key_Shop: 0x10000be, Key_History: 0x10000bf, Key_AddFavorite: 0x10000c0,
    Key_HotLinks: 0x10000c1, Key_BrightnessAdjust: 0x10000c2, Key_Finance: 0x10000c3, Key_Community: 0x10000c4,
    Key_AudioRewind: 0x10000c5, Key_BackForward: 0x10000c6, Key_ApplicationLeft: 0x10000c7,
    Key_ApplicationRight: 0x10000c8, Key_Book: 0x10000c9, Key_CD: 0x10000ca, Key_Calculator: 0x10000cb,
    Key_ToDoList: 0x10000cc, Key_ClearGrab: 0x10000cd, Key_Close: 0x10000ce, Key_Copy: 0x10000cf, Key_Cut: 0x10000d0,
    Key_Display: 0x10000d1, Key_DOS: 0x10000d2, Key_Documents: 0x10000d3, Key_Excel: 0x10000d4,
    Key_Explorer: 0x10000d5, Key_Game: 0x10000d6, Key_Go: 0x10000d7, Key_iTouch: 0x10000d8, Key_LogOff: 0x10000d9,
    Key_Market: 0x10000da, Key_Meeting: 0x10000db, Key_MenuKB: 0x10000dc, Key_MenuPB: 0x10000dd,
    Key_MySites: 0x10000de, Key_News: 0x10000df, Key_OfficeHome: 0x10000e0, Key_Option: 0x10000e1,
    Key_Paste: 0x10000e2, Key_Phone: 0x10000e3, Key_Calendar: 0x10000e4, Key_Reply: 0x10000e5, Key_Reload: 0x10000e6,
    Key_RotateWindows: 0x10000e7, Key_RotationPB: 0x10000e8, Key_RotationKB: 0x10000e9, Key_Save: 0x10000ea,
    Key_Send: 0x10000eb, Key_Spell: 0x10000ec, Key_SplitScreen: 0x10000ed, Key_Support: 0x10000ee,
    Key_TaskPane: 0x10000ef, Key_Terminal: 0x10000f0, Key_Tools: 0x10000f1, Key_Travel: 0x10000f2,
    Key_Video: 0x10000f3, Key_Word: 0x10000f4, Key_Xfer: 0x10000f5, Key_ZoomIn: 0x10000f6, Key_ZoomOut: 0x10000f7,
    Key_Away: 0x10000f8, Key_Messenger: 0x10000f9, Key_WebCam: 0x10000fa, Key_MailForward: 0x10000fb,
    Key_Pictures: 0x10000fc, Key_Music: 0x10000fd, Key_Battery: 0x10000fe, Key_Bluetooth: 0x10000ff,
    Key_WLAN: 0x1000100, Key_UWB: 0x1000101, Key_AudioForward: 0x1000102, Key_AudioRepeat: 0x1000103,
    Key_AudioRandomPlay: 0x1000104, Key_Subtitle: 0x1000105, Key_AudioCycleTrack: 0x1000106, Key_Time: 0x1000107,
    Key_Hibernate: 0x1000108, Key_View: 0x1000109, Key_TopMenu: 0x100010a, Key_PowerDown: 0x100010b,
    Key_Suspend: 0x100010c, Key_ContrastAdjust: 0x100010d, Key_LaunchG: 0x100010e, Key_LaunchH: 0x100010f,
    Key_TouchpadToggle: 0x1000110, Key_TouchpadOn: 0x1000111, Key_TouchpadOff: 0x1000112, Key_MicMute: 0x1000113,
    Key_Red: 0x1000114, Key_Green: 0x1000115, Key_Yellow: 0x1000116, Key_Blue: 0x1000117, Key_ChannelUp: 0x1000118,
    Key_ChannelDown: 0x1000119, Key_Guide: 0x100011a, Key_Info: 0x100011b, Key_Settings: 0x100011c,
    Key_MicVolumeUp: 0x100011d, Key_MicVolumeDown: 0x100011e, Key_Keyboard: 0x100011f, Key_New: 0x1000120,
    Key_Open: 0x1000121, Key_Find: 0x1000122, Key_Undo: 0x1000123, Key_Redo: 0x1000124, Key_MediaLast: 0x100ffff,
    Key_Select: 0x1010000, Key_Yes: 0x1010001, Key_No: 0x1010002, Key_Cancel: 0x1020001, Key_Printer: 0x1020002,
    Key_Execute: 0x1020003, Key_Sleep: 0x1020004, Key_Play: 0x1020005, Key_Zoom: 0x1020006, Key_Exit: 0x102000a,
    Key_Context1: 0x1100000, Key_Context2: 0x1100001, Key_Context3: 0x1100002, Key_Context4: 0x1100003,
    Key_Call: 0x1100004, Key_Hangup: 0x1100005, Key_Flip: 0x1100006, Key_ToggleCallHangup: 0x1100007,
    Key_VoiceDial: 0x1100008, Key_LastNumberRedial: 0x1100009, Key_Camera: 0x1100020, Key_CameraFocus: 0x1100021,
    Key_unknown: 0x1ffffff,
    // KeyboardModifier
    NoModifier: 0, ShiftModifier: 0x2000000, ControlModifier: 0x4000000, AltModifier: 0x8000000,
    MetaModifier: 0x10000000, KeypadModifier: 0x20000000, GroupSwitchModifier: 0x40000000,
    KeyboardModifierMask: -33554432,
    // Modifier
    META: 0x10000000, SHIFT: 0x2000000, CTRL: 0x4000000, ALT: 0x8000000, MODIFIER_MASK: -33554432,
    // ArrowType
    NoArrow: 0, UpArrow: 1, DownArrow: 2, LeftArrow: 3, RightArrow: 4,
    // PenStyle
    NoPen: 0, SolidLine: 1, DashLine: 2, DotLine: 3, DashDotLine: 4, DashDotDotLine: 5, CustomDashLine: 6,
    // PenCapStyle
    FlatCap: 0, SquareCap: 16, RoundCap: 32, MPenCapStyle: 48,
    // PenJoinStyle
    MiterJoin: 0, BevelJoin: 64, RoundJoin: 128, SvgMiterJoin: 0x100, MPenJoinStyle: 0x1c0,
    // BrushStyle
    NoBrush: 0, SolidPattern: 1, Dense1Pattern: 2, Dense2Pattern: 3, Dense3Pattern: 4, Dense4Pattern: 5,
    Dense5Pattern: 6, Dense6Pattern: 7, Dense7Pattern: 8, HorPattern: 9, VerPattern: 10, CrossPattern: 11,
    BDiagPattern: 12, FDiagPattern: 13, DiagCrossPattern: 14, LinearGradientPattern: 15, RadialGradientPattern: 16,
    ConicalGradientPattern: 17, TexturePattern: 24,
    // SizeMode
    AbsoluteSize: 0, RelativeSize: 1,
    // CursorShape
    ArrowCursor: 0, UpArrowCursor: 1, CrossCursor: 2, WaitCursor: 3, IBeamCursor: 4, SizeVerCursor: 5,
    SizeHorCursor: 6, SizeBDiagCursor: 7, SizeFDiagCursor: 8, SizeAllCursor: 9, BlankCursor: 10, SplitVCursor: 11,
    SplitHCursor: 12, PointingHandCursor: 13, ForbiddenCursor: 14, WhatsThisCursor: 15, BusyCursor: 16,
    OpenHandCursor: 17, ClosedHandCursor: 18, DragCopyCursor: 19, DragMoveCursor: 20, DragLinkCursor: 21,
    LastCursor: 21, CustomCursor: 25, BitmapCursor: 24,
    // TextFormat
    PlainText: 0, RichText: 1, AutoText: 2, MarkdownText: 3,
    // AspectRatioMode
    IgnoreAspectRatio: 0, KeepAspectRatio: 1, KeepAspectRatioByExpanding: 2,
    // DockWidgetArea
    LeftDockWidgetArea: 1, RightDockWidgetArea: 2, TopDockWidgetArea: 4, BottomDockWidgetArea: 8,
    DockWidgetArea_Mask: 15, AllDockWidgetAreas: 15, NoDockWidgetArea: 0,
    // ToolBarArea
    LeftToolBarArea: 1, RightToolBarArea: 2, TopToolBarArea: 4, BottomToolBarArea: 8, ToolBarArea_Mask: 15,
    AllToolBarAreas: 15, NoToolBarArea: 0,
    // DateFormat
    TextDate: 0, ISODate: 1, RFC2822Date: 8, ISODateWithMs: 9,
    // TimeSpec
    LocalTime: 0, UTC: 1, OffsetFromUTC: 2, TimeZone: 3,
    // DayOfWeek
    Monday: 1, Tuesday: 2, Wednesday: 3, Thursday: 4, Friday: 5, Saturday: 6, Sunday: 7,
    // ScrollBarPolicy
    ScrollBarAsNeeded: 0, ScrollBarAlwaysOff: 1, ScrollBarAlwaysOn: 2,
    // CaseSensitivity
    CaseInsensitive: 0, CaseSensitive: 1,
    // Corner
    TopLeftCorner: 0, TopRightCorner: 1, BottomLeftCorner: 2, BottomRightCorner: 3,
    // Edge
    TopEdge: 1, LeftEdge: 2, RightEdge: 4, BottomEdge: 8,
    // ConnectionType
    AutoConnection: 0, DirectConnection: 1, QueuedConnection: 2, BlockingQueuedConnection: 3, UniqueConnection: 128,
    SingleShotConnection: 0x100,
    // ShortcutContext
    WidgetShortcut: 0, WindowShortcut: 1, ApplicationShortcut: 2, WidgetWithChildrenShortcut: 3,
    // FillRule
    OddEvenFill: 0, WindingFill: 1,
    // MaskMode
    MaskInColor: 0, MaskOutColor: 1,
    // ClipOperation
    NoClip: 0, ReplaceClip: 1, IntersectClip: 2,
    // ItemSelectionMode
    ContainsItemShape: 0, IntersectsItemShape: 1, ContainsItemBoundingRect: 2, IntersectsItemBoundingRect: 3,
    // ItemSelectionOperation
    ReplaceSelection: 0, AddToSelection: 1,
    // TransformationMode
    FastTransformation: 0, SmoothTransformation: 1,
    // Axis
    XAxis: 0, YAxis: 1, ZAxis: 2,
    // FocusReason
    MouseFocusReason: 0, TabFocusReason: 1, BacktabFocusReason: 2, ActiveWindowFocusReason: 3, PopupFocusReason: 4,
    ShortcutFocusReason: 5, MenuBarFocusReason: 6, OtherFocusReason: 7, NoFocusReason: 8,
    // ContextMenuPolicy
    NoContextMenu: 0, DefaultContextMenu: 1, ActionsContextMenu: 2, CustomContextMenu: 3, PreventContextMenu: 4,
    // ContextMenuTrigger
    ContextMenuTrigger: { Press: 0, Release: 1 },
    // InputMethodQuery
    ImEnabled: 1, ImCursorRectangle: 2, ImFont: 4, ImCursorPosition: 8, ImSurroundingText: 16, ImCurrentSelection: 32,
    ImMaximumTextLength: 64, ImAnchorPosition: 128, ImHints: 0x100, ImPreferredLanguage: 0x200,
    ImAbsolutePosition: 0x400, ImTextBeforeCursor: 0x800, ImTextAfterCursor: 0x1000, ImEnterKeyType: 0x2000,
    ImAnchorRectangle: 0x4000, ImInputItemClipRectangle: 0x8000, ImReadOnly: 0x10000, ImPlatformData: -2147483648,
    ImQueryInput: 0x40ba, ImQueryAll: -1,
    // InputMethodHint
    ImhNone: 0, ImhHiddenText: 1, ImhSensitiveData: 2, ImhNoAutoUppercase: 4, ImhPreferNumbers: 8,
    ImhPreferUppercase: 16, ImhPreferLowercase: 32, ImhNoPredictiveText: 64, ImhDate: 128, ImhTime: 0x100,
    ImhPreferLatin: 0x200, ImhMultiLine: 0x400, ImhNoEditMenu: 0x800, ImhNoTextHandles: 0x1000,
    ImhNoFullscreen: 0x2000, ImhDigitsOnly: 0x10000, ImhFormattedNumbersOnly: 0x20000, ImhUppercaseOnly: 0x40000,
    ImhLowercaseOnly: 0x80000, ImhDialableCharactersOnly: 0x100000, ImhEmailCharactersOnly: 0x200000,
    ImhUrlCharactersOnly: 0x400000, ImhLatinOnly: 0x800000, ImhDecimalNumbersOnly: 0x1000000,
    ImhExclusiveInputMask: -65536,
    // EnterKeyType
    EnterKeyDefault: 0, EnterKeyReturn: 1, EnterKeyDone: 2, EnterKeyGo: 3, EnterKeySend: 4, EnterKeySearch: 5,
    EnterKeyNext: 6, EnterKeyPrevious: 7,
    // ToolButtonStyle
    ToolButtonIconOnly: 0, ToolButtonTextOnly: 1, ToolButtonTextBesideIcon: 2, ToolButtonTextUnderIcon: 3,
    ToolButtonFollowStyle: 4,
    // LayoutDirection
    LeftToRight: 0, RightToLeft: 1, LayoutDirectionAuto: 2,
    // DropAction
    CopyAction: 1, MoveAction: 2, LinkAction: 4, ActionMask: 255, TargetMoveAction: 0x8002, IgnoreAction: 0,
    // CheckState
    Unchecked: 0, PartiallyChecked: 1, Checked: 2,
    // ItemDataRole
    DisplayRole: 0, DecorationRole: 1, EditRole: 2, ToolTipRole: 3, StatusTipRole: 4, WhatsThisRole: 5, FontRole: 6,
    TextAlignmentRole: 7, BackgroundRole: 8, ForegroundRole: 9, CheckStateRole: 10, AccessibleTextRole: 11,
    AccessibleDescriptionRole: 12, SizeHintRole: 13, InitialSortOrderRole: 14, DisplayPropertyRole: 27,
    DecorationPropertyRole: 28, ToolTipPropertyRole: 29, StatusTipPropertyRole: 30, WhatsThisPropertyRole: 31,
    RangeModelDataRole: 40, RangeModelAdapterRole: 41, UserRole: 0x100, StandardItemFlagsRole: 255, FileInfoRole: 252,
    RemoteObjectsCacheRole: 255,
    // ItemFlag
    NoItemFlags: 0, ItemIsSelectable: 1, ItemIsEditable: 2, ItemIsDragEnabled: 4, ItemIsDropEnabled: 8,
    ItemIsUserCheckable: 16, ItemIsEnabled: 32, ItemIsAutoTristate: 64, ItemNeverHasChildren: 128,
    ItemIsUserTristate: 0x100,
    // MatchFlag
    MatchExactly: 0, MatchContains: 1, MatchStartsWith: 2, MatchEndsWith: 3, MatchRegularExpression: 4,
    MatchWildcard: 5, MatchFixedString: 8, MatchTypeMask: 15, MatchCaseSensitive: 16, MatchWrap: 32,
    MatchRecursive: 64,
    // WindowModality
    NonModal: 0, WindowModal: 1, ApplicationModal: 2,
    // TextInteractionFlag
    NoTextInteraction: 0, TextSelectableByMouse: 1, TextSelectableByKeyboard: 2, LinksAccessibleByMouse: 4,
    LinksAccessibleByKeyboard: 8, TextEditable: 16, TextEditorInteraction: 19, TextBrowserInteraction: 13,
    // SizeHint
    MinimumSize: 0, PreferredSize: 1, MaximumSize: 2, MinimumDescent: 3, NSizeHints: 4,
    // TouchPointState
    TouchPointUnknownState: 0, TouchPointPressed: 1, TouchPointMoved: 2, TouchPointStationary: 4,
    TouchPointReleased: 8,
    // GestureState
    NoGesture: 0, GestureStarted: 1, GestureUpdated: 2, GestureFinished: 3, GestureCanceled: 4,
    // GestureType
    TapGesture: 1, TapAndHoldGesture: 2, PanGesture: 3, PinchGesture: 4, SwipeGesture: 5, CustomGesture: 0x100,
    LastGestureType: -1,
    // NativeGestureType
    BeginNativeGesture: 0, EndNativeGesture: 1, PanNativeGesture: 2, ZoomNativeGesture: 3, SmartZoomNativeGesture: 4,
    RotateNativeGesture: 5, SwipeNativeGesture: 6,
    // CursorMoveStyle
    LogicalMoveStyle: 0, VisualMoveStyle: 1,
    // TimerType
    PreciseTimer: 0, CoarseTimer: 1, VeryCoarseTimer: 2,
    // TimerId
    TimerId: { Invalid: 0 },
    // ScrollPhase
    NoScrollPhase: 0, ScrollBegin: 1, ScrollUpdate: 2, ScrollEnd: 3, ScrollMomentum: 4,
    // MouseEventSource
    MouseEventNotSynthesized: 0, MouseEventSynthesizedBySystem: 1, MouseEventSynthesizedByQt: 2,
    MouseEventSynthesizedByApplication: 3,
    // MouseEventFlag
    NoMouseEventFlag: 0, MouseEventCreatedDoubleClick: 1, MouseEventFlagMask: 255,
    // ChecksumType
    ChecksumIso3309: 0, ChecksumItuV41: 1,
    // HighDpiScaleFactorRoundingPolicy
    HighDpiScaleFactorRoundingPolicy: { Unset: 0, Round: 1, Ceil: 2, Floor: 3, RoundPreferFloor: 4, PassThrough: 5 },
    // PermissionStatus
    PermissionStatus: { Undetermined: 0, Granted: 1, Denied: 2 },

    // not of the Qt namespace: what v5 kept here before (the easing curves of PropertyAnimation)
    Easing: {
        Linear: 0,
        InQuad: 1, OutQuad: 2, InOutQuad: 3, OutInQuad: 4,
        InCubic: 5, OutCubic: 6, InOutCubic: 7, OutInCubic: 8,
        InQuart: 9, OutQuart: 10, InOutQuart: 11, OutInQuart: 12,
        InQuint: 13, OutQuint: 14, InOutQuint: 15, OutInQuint: 16,
        InSine: 17, OutSine: 18, InOutSine: 19, OutInSine: 20,
        InExpo: 21, OutExpo: 22, InOutExpo: 23, OutInExpo: 24,
        InCirc: 25, OutCirc: 26, InOutCirc: 27, OutInCirc: 28,
        InElastic: 29, OutElastic: 30, InOutElastic: 31, OutInElastic: 32,
        InBack: 33, OutBack: 34, InOutBack: 35, OutInBack: 36,
        InBounce: 37, OutBounce: 38, InOutBounce: 39, OutInBounce: 40,
    },
}
