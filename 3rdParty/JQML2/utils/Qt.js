var Qt = {
    NoButton: 0x00000000,
    AllButtons: 0x07ffffff,
    LeftButton: 0x00000001,
    RightButton: 0x00000002,
    MiddleButton: 0x00000004,

    NoModifier: 0x00000000,
    ShiftModifier: 0x02000000,
    ControlModifier: 0x04000000,
    AltModifier: 0x08000000,
    MetaModifier: 0x10000000,

    TabFocus: 0x01,
    ClickFocus: 0x2,
    StrongFocus: 0x8,
    WheelFocus: 0x4,
    NoFocus: 0,

    TopEdge: 0x00001,
    LeftEdge: 0x00002,
    RightEdge: 0x00004,
    BottomEdge: 0x00008,

    Key_Escape: 'Escape',//0x01000000, 
    Key_Tab: 'Tab',//0x01000001, 
    Key_Backtab: 0x01000002, 
    Key_Backspace: 'Backspace',//0x01000003, 
    Key_Return: 'Enter',//0x01000004, 
    Key_Enter: 'Enter',//0x01000005,
    Key_Insert: 'Insert',//0x01000006, 
    Key_Delete: 'Delete',//0x01000007, 
    Key_Pause: 'Pause',//0x01000008,
    Key_Print: 0x01000009, 
    Key_SysReq: 0x0100000a, 
    Key_Clear: 0x0100000b,
    Key_Home: 'Home',//0x01000010, 
    Key_End: 'End',//0x01000011, 
    Key_Left: 'ArrowLeft',//0x01000012, 
    Key_Up: 'ArrowUp',//0x01000013, 
    Key_Right: 'ArrowRight',//0x01000014, 
    Key_Down: 'ArrowDown',//0x01000015, 
    Key_PageUp: 'PageUp',//0x01000016, 
    Key_PageDown: 'PageDown',//0x01000017, 
    Key_Shift: 'Shift',//0x01000020, 
    Key_Control: 'Control',//0x01000021,
    Key_Meta: 0x01000022,
    Key_Alt: 'Alt',//0x01000023, 
    Key_AltGr: 0x01001103,
    Key_CapsLock: 'CapsLock',//0x01000024, 
    Key_NumLock: 'NumLock',//0x01000025, 
    Key_ScrollLock: 'ScrollLock',//0x01000026, 
    Key_F1: 'F1',//0x01000030, 
    Key_F2: 'F2',//0x01000031, 
    Key_F3: 'F3',//0x01000032, 
    Key_F4: 'F4',//0x01000033, 
    Key_F5: 'F5',//0x01000034, 
    Key_F6: 'F6',//0x01000035, 
    Key_F7: 'F7',//0x01000036, 
    Key_F8: 'F8',//0x01000037, 
    Key_F9: 'F9',//0x01000038, 
    Key_F10: 'F10',//0x01000039, 
    Key_F11: 'F11',//0x0100003a, 
    Key_F12: 'F12',//0x0100003b, 
    Key_F13: 'F13',//0x0100003c, 
    Key_F14: 'F14',//0x0100003d, 
    Key_F15: 'F15',//0x0100003e, 
    Key_F16: 'F16',//0x0100003f, 
    Key_F17: 'F17',//0x01000040, 
    Key_F18: 'F18',//0x01000041, 
    Key_F19: 'F19',//0x01000042, 
    Key_F20: 'F20',//0x01000043, 
    Key_F21: 'F21',//0x01000044, 
    Key_F22: 'F22',//0x01000045, 
    Key_F23: 'F23',//0x01000046, 
    Key_F24: 'F24',//0x01000047, 
    Key_F25: 'F25',//0x01000048, 
    Key_F26: 'F26',//0x01000049, 
    Key_F27: 'F27',//0x0100004a, 
    Key_F28: 'F28',//0x0100004b, 
    Key_F29: 'F29',//0x0100004c, 
    Key_F30: 'F30',//0x0100004d, 
    Key_F31: 'F31',//0x0100004e, 
    Key_F32: 'F32',//0x0100004f, 
    Key_F33: 'F33',//0x01000050, 
    Key_F34: 'F34',//0x01000051, 
    Key_F35: 'F35',//0x01000052, 
    Key_Super_L: 0x01000053, 
    Key_Super_R: 0x01000054, 
    Key_Menu: 0x01000055, 
    Key_Hyper_L: 0x01000056, 
    Key_Hyper_R: 0x01000057, 
    Key_Help: 0x01000058, 
    Key_Direction_L: 0x01000059, 
    Key_Direction_R: 0x01000060, 
    Key_Space: 'Space',//0x20, 
    Key_Any: 0x20,
    Key_Exclam: 0x21, 
    Key_QuoteDbl: 0x22, 
    Key_NumberSign: 0x23, 
    Key_Dollar: 0x24, 
    Key_Percent: 0x25, 
    Key_Ampersand: 0x26, 
    Key_Apostrophe: 0x27, 
    Key_ParenLeft: 0x28, 
    Key_ParenRight: 0x29, 
    Key_Asterisk: 0x2a, 
    Key_Plus: 0x2b, 
    Key_Comma: 0x2c, 
    Key_Minus: 0x2d, 
    Key_Period: 0x2e, 
    Key_Slash: 0x2f, 
    Key_0: 'Digit0', 
    Key_1: 'Digit1', 
    Key_2: 'Digit2', 
    Key_3: 'Digit3', 
    Key_4: 'Digit4', 
    Key_5: 'Digit5', 
    Key_6: 'Digit6', 
    Key_7: 'Digit7', 
    Key_8: 'Digit8', 
    Key_9: 'Digit9', 
    Key_Colon: 0x3a, 
    Key_Semicolon: 0x3b, 
    Key_Less: 0x3c, 
    Key_Equal: 0x3d, 
    Key_Greater: 0x3e, 
    Key_Question: 0x3f, 
    Key_At: 0x40, 
    Key_A: 'KeyA', 
    Key_B: 'KeyB', 
    Key_C: 'KeyC', 
    Key_D: 'KeyD', 
    Key_E: 'KeyE', 
    Key_F: 'KeyF', 
    Key_G: 'KeyG', 
    Key_H: 'KeyH', 
    Key_I: 'KeyI', 
    Key_J: 'KeyJ', 
    Key_K: 'KeyK', 
    Key_L: 'KeyL', 
    Key_M: 'KeyM', 
    Key_N: 'KeyN', 
    Key_O: 'KeyO', 
    Key_P: 'KeyP', 
    Key_Q: 'KeyQ', 
    Key_R: 'KeyR', 
    Key_S: 'KeyS', 
    Key_T: 'KeyT', 
    Key_U: 'KeyU', 
    Key_V: 'KeyV', 
    Key_W: 'KeyW', 
    Key_X: 'KeyX', 
    Key_Y: 'KeyY', 
    Key_Z: 'KeyZ', 
    Key_BracketLeft: 0x5b, 
    Key_Backslash: 0x5c, 
    Key_BracketRight: 0x5d, 
    Key_AsciiCircum: 0x5e, 
    Key_Underscore: 0x5f, 
    Key_QuoteLeft: 0x60, 
    Key_BraceLeft: 0x7b, 
    Key_Bar: 0x7c, 
    Key_BraceRight: 0x7d, 
    Key_AsciiTilde: 0x7e, 
    Key_nobreakspace: 0x0a0, 
    Key_exclamdown: 0x0a1, 
    Key_cent: 0x0a2, 
    Key_sterling: 0x0a3, 
    Key_currency: 0x0a4, 
    Key_yen: 0x0a5, 
    Key_brokenbar: 0x0a6, 
    Key_section: 0x0a7, 
    Key_diaeresis: 0x0a8, 
    Key_copyright: 0x0a9, 
    Key_ordfeminine: 0x0aa, 
    Key_guillemotleft: 0x0ab, 
    Key_notsign: 0x0ac, 
    Key_hyphen: 0x0ad, 
    Key_registered: 0x0ae, 
    Key_macron: 0x0af, 
    Key_degree: 0x0b0, 
    Key_plusminus: 0x0b1, 
    Key_twosuperior: 0x0b2, 
    Key_threesuperior: 0x0b3, 
    Key_acute: 0x0b4, 
    Key_mu: 0x0b5, 
    Key_paragraph: 0x0b6, 
    Key_periodcentered: 0x0b7, 
    Key_cedilla: 0x0b8, 
    Key_onesuperior: 0x0b9, 
    Key_masculine: 0x0ba, 
    Key_guillemotright: 0x0bb, 
    Key_onequarter: 0x0bc, 
    Key_onehalf: 0x0bd, 
    Key_threequarters: 0x0be, 
    Key_questiondown: 0x0bf, 
    Key_Agrave: 0x0c0, 
    Key_Aacute: 0x0c1, 
    Key_Acircumflex: 0x0c2, 
    Key_Atilde: 0x0c3, 
    Key_Adiaeresis: 0x0c4, 
    Key_Aring: 0x0c5, 
    Key_AE: 0x0c6, 
    Key_Ccedilla: 0x0c7, 
    Key_Egrave: 0x0c8, 
    Key_Eacute: 0x0c9, 
    Key_Ecircumflex: 0x0ca, 
    Key_Ediaeresis: 0x0cb, 
    Key_Igrave: 0x0cc, 
    Key_Iacute: 0x0cd, 
    Key_Icircumflex: 0x0ce, 
    Key_Idiaeresis: 0x0cf, 
    Key_ETH: 0x0d0, 
    Key_Ntilde: 0x0d1, 
    Key_Ograve: 0x0d2, 
    Key_Oacute: 0x0d3, 
    Key_Ocircumflex: 0x0d4, 
    Key_Otilde: 0x0d5, 
    Key_Odiaeresis: 0x0d6, 
    Key_multiply: 0x0d7, 
    Key_Ooblique: 0x0d8,
    Key_Ugrave: 0x0d9, 
    Key_Uacute: 0x0da, 
    Key_Ucircumflex: 0x0db, 
    Key_Udiaeresis: 0x0dc, 
    Key_Yacute: 0x0dd, 
    Key_THORN: 0x0de, 
    Key_ssharp: 0x0df, 
    Key_division: 0x0f7, 
    Key_ydiaeresis: 0x0ff, 
    Key_Multi_key: 0x01001120, 
    Key_Codeinput: 0x01001137, 
    Key_SingleCandidate: 0x0100113c, 
    Key_MultipleCandidate: 0x0100113d, 
    Key_PreviousCandidate: 0x0100113e, 
    Key_Mode_switch: 0x0100117e, 
    Key_Kanji: 0x01001121, 
    Key_Muhenkan: 0x01001122, 
    Key_Henkan: 0x01001123, 
    Key_Romaji: 0x01001124, 
    Key_Hiragana: 0x01001125, 
    Key_Katakana: 0x01001126, 
    Key_Hiragana_Katakana: 0x01001127, 
    Key_Zenkaku: 0x01001128, 
    Key_Hankaku: 0x01001129, 
    Key_Zenkaku_Hankaku: 0x0100112a, 
    Key_Touroku: 0x0100112b, 
    Key_Massyo: 0x0100112c, 
    Key_Kana_Lock: 0x0100112d, 
    Key_Kana_Shift: 0x0100112e, 
    Key_Eisu_Shift: 0x0100112f, 
    Key_Eisu_toggle: 0x01001130, 
    Key_Hangul: 0x01001131, 
    Key_Hangul_Start: 0x01001132, 
    Key_Hangul_End: 0x01001133, 
    Key_Hangul_Hanja: 0x01001134, 
    Key_Hangul_Jamo: 0x01001135, 
    Key_Hangul_Romaja: 0x01001136, 
    Key_Hangul_Jeonja: 0x01001138, 
    Key_Hangul_Banja: 0x01001139, 
    Key_Hangul_PreHanja: 0x0100113a, 
    Key_Hangul_PostHanja: 0x0100113b, 
    Key_Hangul_Special: 0x0100113f, 
    Key_Dead_Grave: 0x01001250, 
    Key_Dead_Acute: 0x01001251, 
    Key_Dead_Circumflex: 0x01001252, 
    Key_Dead_Tilde: 0x01001253, 
    Key_Dead_Macron: 0x01001254, 
    Key_Dead_Breve: 0x01001255, 
    Key_Dead_Abovedot: 0x01001256, 
    Key_Dead_Diaeresis: 0x01001257, 
    Key_Dead_Abovering: 0x01001258, 
    Key_Dead_Doubleacute: 0x01001259, 
    Key_Dead_Caron: 0x0100125a, 
    Key_Dead_Cedilla: 0x0100125b, 
    Key_Dead_Ogonek: 0x0100125c, 
    Key_Dead_Iota: 0x0100125d, 
    Key_Dead_Voiced_Sound: 0x0100125e, 
    Key_Dead_Semivoiced_Sound: 0x0100125f, 
    Key_Dead_Belowdot: 0x01001260, 
    Key_Dead_Hook: 0x01001261, 
    Key_Dead_Horn: 0x01001262, 
    Key_Dead_Stroke: 0x01001263, 
    Key_Dead_Abovecomma: 0x01001264, 
    Key_Dead_Abovereversedcomma: 0x01001265, 
    Key_Dead_Doublegrave: 0x01001266, 
    Key_Dead_Belowring: 0x01001267, 
    Key_Dead_Belowmacron: 0x01001268, 
    Key_Dead_Belowcircumflex: 0x01001269, 
    Key_Dead_Belowtilde: 0x0100126a, 
    Key_Dead_Belowbreve: 0x0100126b, 
    Key_Dead_Belowdiaeresis: 0x0100126c, 
    Key_Dead_Invertedbreve: 0x0100126d, 
    Key_Dead_Belowcomma: 0x0100126e, 
    Key_Dead_Currency: 0x0100126f, 
    Key_Dead_a: 0x01001280, 
    Key_Dead_A: 0x01001281, 
    Key_Dead_e: 0x01001282, 
    Key_Dead_E: 0x01001283, 
    Key_Dead_i: 0x01001284, 
    Key_Dead_I: 0x01001285, 
    Key_Dead_o: 0x01001286, 
    Key_Dead_O: 0x01001287, 
    Key_Dead_u: 0x01001288, 
    Key_Dead_U: 0x01001289, 
    Key_Dead_Small_Schwa: 0x0100128a, 
    Key_Dead_Capital_Schwa: 0x0100128b, 
    Key_Dead_Greek: 0x0100128c, 
    Key_Dead_Lowline: 0x01001290, 
    Key_Dead_Aboveverticalline: 0x01001291, 
    Key_Dead_Belowverticalline: 0x01001292, 
    Key_Dead_Longsolidusoverlay: 0x01001293, 
    Key_Back: 0x01000061, 
    Key_Forward: 0x01000062, 
    Key_Stop: 0x01000063, 
    Key_Refresh: 0x01000064, 
    Key_VolumeDown: 0x01000070, 
    Key_VolumeMute: 0x01000071, 
    Key_VolumeUp: 0x01000072, 
    Key_BassBoost: 0x01000073, 
    Key_BassUp: 0x01000074, 
    Key_BassDown: 0x01000075, 
    Key_TrebleUp: 0x01000076, 
    Key_TrebleDown: 0x01000077, 
    Key_MediaPlay: 0x01000080,
    Key_MediaStop: 0x01000081,
    Key_MediaPrevious: 0x01000082, 
    Key_MediaNext: 0x01000083, 
    Key_MediaRecord: 0x01000084, 
    Key_MediaPause: 0x01000085,
    Key_MediaTogglePlayPause: 0x01000086,
    Key_HomePage: 0x01000090, 
    Key_Favorites: 0x01000091, 
    Key_Search: 0x01000092, 
    Key_Standby: 0x01000093, 
    Key_OpenUrl: 0x01000094, 
    Key_LaunchMail: 0x010000a0, 
    Key_LaunchMedia: 0x010000a1, 
    Key_Launch0: 0x010000a2, 
    Key_Launch1: 0x010000a3, 
    Key_Launch2: 0x010000a4, 
    Key_Launch3: 0x010000a5, 
    Key_Launch4: 0x010000a6, 
    Key_Launch5: 0x010000a7, 
    Key_Launch6: 0x010000a8, 
    Key_Launch7: 0x010000a9, 
    Key_Launch8: 0x010000aa, 
    Key_Launch9: 0x010000ab, 
    Key_LaunchA: 0x010000ac, 
    Key_LaunchB: 0x010000ad, 
    Key_LaunchC: 0x010000ae, 
    Key_LaunchD: 0x010000af, 
    Key_LaunchE: 0x010000b0, 
    Key_LaunchF: 0x010000b1, 
    Key_LaunchG: 0x0100010e, 
    Key_LaunchH: 0x0100010f, 
    Key_MonBrightnessUp: 0x010000b2, 
    Key_MonBrightnessDown: 0x010000b3, 
    Key_KeyboardLightOnOff: 0x010000b4, 
    Key_KeyboardBrightnessUp: 0x010000b5, 
    Key_KeyboardBrightnessDown: 0x010000b6, 
    Key_PowerOff: 0x010000b7, 
    Key_WakeUp: 0x010000b8, 
    Key_Eject: 0x010000b9, 
    Key_ScreenSaver: 0x010000ba, 
    Key_WWW: 0x010000bb, 
    Key_Memo: 0x010000bc, 
    Key_LightBulb: 0x010000bd, 
    Key_Shop: 0x010000be, 
    Key_History: 0x010000bf, 
    Key_AddFavorite: 0x010000c0, 
    Key_HotLinks: 0x010000c1, 
    Key_BrightnessAdjust: 0x010000c2, 
    Key_Finance: 0x010000c3, 
    Key_Community: 0x010000c4, 
    Key_AudioRewind: 0x010000c5, 
    Key_BackForward: 0x010000c6, 
    Key_ApplicationLeft: 0x010000c7, 
    Key_ApplicationRight: 0x010000c8, 
    Key_Book: 0x010000c9, 
    Key_CD: 0x010000ca, 
    Key_Calculator: 0x010000cb, 
    Key_ToDoList: 0x010000cc, 
    Key_ClearGrab: 0x010000cd, 
    Key_Close: 0x010000ce, 
    Key_Copy: 0x010000cf, 
    Key_Cut: 0x010000d0, 
    Key_Display: 0x010000d1, 
    Key_DOS: 0x010000d2, 
    Key_Documents: 0x010000d3, 
    Key_Excel: 0x010000d4, 
    Key_Explorer: 0x010000d5, 
    Key_Game: 0x010000d6, 
    Key_Go: 0x010000d7, 
    Key_iTouch: 0x010000d8, 
    Key_LogOff: 0x010000d9, 
    Key_Market: 0x010000da, 
    Key_Meeting: 0x010000db, 
    Key_MenuKB: 0x010000dc, 
    Key_MenuPB: 0x010000dd, 
    Key_MySites: 0x010000de, 
    Key_News: 0x010000df, 
    Key_OfficeHome: 0x010000e0, 
    Key_Option: 0x010000e1, 
    Key_Paste: 0x010000e2,
    Key_Phone: 0x010000e3, 
    Key_Calendar: 0x010000e4, 
    Key_Reply: 0x010000e5, 
    Key_Reload: 0x010000e6, 
    Key_RotateWindows: 0x010000e7, 
    Key_RotationPB: 0x010000e8, 
    Key_RotationKB: 0x010000e9, 
    Key_Save: 0x010000ea, 
    Key_Send: 0x010000eb, 
    Key_Spell: 0x010000ec, 
    Key_SplitScreen: 0x010000ed, 
    Key_Support: 0x010000ee, 
    Key_TaskPane: 0x010000ef, 
    Key_Terminal: 0x010000f0, 
    Key_Tools: 0x010000f1, 
    Key_Travel: 0x010000f2, 
    Key_Video: 0x010000f3, 
    Key_Word: 0x010000f4, 
    Key_Xfer: 0x010000f5, 
    Key_ZoomIn: 0x010000f6, 
    Key_ZoomOut: 0x010000f7, 
    Key_Away: 0x010000f8, 
    Key_Messenger: 0x010000f9, 
    Key_WebCam: 0x010000fa, 
    Key_MailForward: 0x010000fb, 
    Key_Pictures: 0x010000fc, 
    Key_Music: 0x010000fd, 
    Key_Battery: 0x010000fe, 
    Key_Bluetooth: 0x010000ff, 
    Key_WLAN: 0x01000100, 
    Key_UWB: 0x01000101, 
    Key_AudioForward: 0x01000102, 
    Key_AudioRepeat: 0x01000103, 
    Key_AudioRandomPlay: 0x01000104, 
    Key_Subtitle: 0x01000105, 
    Key_AudioCycleTrack: 0x01000106, 
    Key_Time: 0x01000107, 
    Key_Hibernate: 0x01000108, 
    Key_View: 0x01000109, 
    Key_TopMenu: 0x0100010a, 
    Key_PowerDown: 0x0100010b, 
    Key_Suspend: 0x0100010c, 
    Key_ContrastAdjust: 0x0100010d, 
    Key_TouchpadToggle: 0x01000110, 
    Key_TouchpadOn: 0x01000111, 
    Key_TouchpadOff: 0x01000112, 
    Key_MicMute: 0x01000113, 
    Key_Red: 0x01000114, 
    Key_Green: 0x01000115, 
    Key_Yellow: 0x01000116, 
    Key_Blue: 0x01000117, 
    Key_ChannelUp: 0x01000118, 
    Key_ChannelDown: 0x01000119, 
    Key_Guide: 0x0100011a, 
    Key_Info: 0x0100011b, 
    Key_Settings: 0x0100011c, 
    Key_MicVolumeUp: 0x0100011d, 
    Key_MicVolumeDown: 0x0100011e, 
    Key_New: 0x01000120, 
    Key_Open: 0x01000121, 
    Key_Find: 0x01000122, 
    Key_Undo: 0x01000123, 
    Key_Redo: 0x01000124, 
    Key_MediaLast: 0x0100ffff, 
    Key_unknown: 0x01ffffff, 
    Key_Call: 0x01100004,
    Key_Camera: 0x01100020,
    Key_CameraFocus: 0x01100021,
    Key_Context1: 0x01100000, 
    Key_Context2: 0x01100001, 
    Key_Context3: 0x01100002, 
    Key_Context4: 0x01100003, 
    Key_Flip: 0x01100006, 
    Key_Hangup: 0x01100005,
    Key_No: 0x01010002, 
    Key_Select: 0x01010000, 
    Key_Yes: 0x01010001, 
    Key_ToggleCallHangup: 0x01100007,
    Key_VoiceDial: 0x01100008, 
    Key_LastNumberRedial: 0x01100009, 
    Key_Execute: 0x01020003, 
    Key_Printer: 0x01020002, 
    Key_Play: 0x01020005, 
    Key_Sleep: 0x01020004, 
    Key_Zoom: 0x01020006, 
    Key_Exit: 0x0102000a, 
    Key_Cancel: 0x01020001, 

    ArrowCursor: 'default',
    UpArrowCursor: 'none',
    CrossCursor: 'crosshair',
    IBeamCursor: 'text',
    WaitCursor: 'wait',
    BusyCursor: 'progress',
    ForbiddenCursor: 'not-allowed',
    PointingHandCursor: 'pointer',
    WhatsThisCursor: 'help',
    DragMoveCursor: 'none',
    DragLinkCursor: 'none',
    SizeVerCursor: 'n-resize',
    SizeHorCursor: 'e-resize',
    SizeBDiagCursor: 'ne-resize',
    SizeFDiagCursor: 'nw-resize',
    SizeAllCursor: 'all-scroll',
    SplitVCursor: 'row-resize',
    SplitHCursor: 'col-resize',
    OpenHandCursor: 'grab',
    ClosedHandCursor: 'grabbing',
    DragCopyCursor: 'none',

    WindowShortcut: 0,
    ApplicationShortcut: 1,

    Unchecked: 0,
    PartiallyChecked: 1,
    Checked: 2,

    Horizontal: 0,
    Vertical: 1,

    LeftToRight: 0,
    RightToLeft: 1,

    platform: {
        os: 'web',
    },

    md5(inputString) {
        var hc="0123456789abcdef";
        function rh(n) {var j,s="";for(j=0;j<=3;j++) s+=hc.charAt((n>>(j*8+4))&0x0F)+hc.charAt((n>>(j*8))&0x0F);return s;}
        function ad(x,y) {var l=(x&0xFFFF)+(y&0xFFFF);var m=(x>>16)+(y>>16)+(l>>16);return (m<<16)|(l&0xFFFF);}
        function rl(n,c)            {return (n<<c)|(n>>>(32-c));}
        function cm(q,a,b,x,s,t)    {return ad(rl(ad(ad(a,q),ad(x,t)),s),b);}
        function ff(a,b,c,d,x,s,t)  {return cm((b&c)|((~b)&d),a,b,x,s,t);}
        function gg(a,b,c,d,x,s,t)  {return cm((b&d)|(c&(~d)),a,b,x,s,t);}
        function hh(a,b,c,d,x,s,t)  {return cm(b^c^d,a,b,x,s,t);}
        function ii(a,b,c,d,x,s,t)  {return cm(c^(b|(~d)),a,b,x,s,t);}
        function sb(x) {
            var i;var nblk=((x.length+8)>>6)+1;var blks=new Array(nblk*16);for(i=0;i<nblk*16;i++) blks[i]=0;
            for(i=0;i<x.length;i++) blks[i>>2]|=x.charCodeAt(i)<<((i%4)*8);
            blks[i>>2]|=0x80<<((i%4)*8);blks[nblk*16-2]=x.length*8;return blks;
        }
        var i,x=sb(""+inputString),a=1732584193,b=-271733879,c=-1732584194,d=271733878,olda,oldb,oldc,oldd;
        for(i=0;i<x.length;i+=16) {olda=a;oldb=b;oldc=c;oldd=d;
            a=ff(a,b,c,d,x[i+ 0], 7, -680876936);d=ff(d,a,b,c,x[i+ 1],12, -389564586);c=ff(c,d,a,b,x[i+ 2],17,  606105819);
            b=ff(b,c,d,a,x[i+ 3],22,-1044525330);a=ff(a,b,c,d,x[i+ 4], 7, -176418897);d=ff(d,a,b,c,x[i+ 5],12, 1200080426);
            c=ff(c,d,a,b,x[i+ 6],17,-1473231341);b=ff(b,c,d,a,x[i+ 7],22,  -45705983);a=ff(a,b,c,d,x[i+ 8], 7, 1770035416);
            d=ff(d,a,b,c,x[i+ 9],12,-1958414417);c=ff(c,d,a,b,x[i+10],17,     -42063);b=ff(b,c,d,a,x[i+11],22,-1990404162);
            a=ff(a,b,c,d,x[i+12], 7, 1804603682);d=ff(d,a,b,c,x[i+13],12,  -40341101);c=ff(c,d,a,b,x[i+14],17,-1502002290);
            b=ff(b,c,d,a,x[i+15],22, 1236535329);a=gg(a,b,c,d,x[i+ 1], 5, -165796510);d=gg(d,a,b,c,x[i+ 6], 9,-1069501632);
            c=gg(c,d,a,b,x[i+11],14,  643717713);b=gg(b,c,d,a,x[i+ 0],20, -373897302);a=gg(a,b,c,d,x[i+ 5], 5, -701558691);
            d=gg(d,a,b,c,x[i+10], 9,   38016083);c=gg(c,d,a,b,x[i+15],14, -660478335);b=gg(b,c,d,a,x[i+ 4],20, -405537848);
            a=gg(a,b,c,d,x[i+ 9], 5,  568446438);d=gg(d,a,b,c,x[i+14], 9,-1019803690);c=gg(c,d,a,b,x[i+ 3],14, -187363961);
            b=gg(b,c,d,a,x[i+ 8],20, 1163531501);a=gg(a,b,c,d,x[i+13], 5,-1444681467);d=gg(d,a,b,c,x[i+ 2], 9,  -51403784);
            c=gg(c,d,a,b,x[i+ 7],14, 1735328473);b=gg(b,c,d,a,x[i+12],20,-1926607734);a=hh(a,b,c,d,x[i+ 5], 4,    -378558);
            d=hh(d,a,b,c,x[i+ 8],11,-2022574463);c=hh(c,d,a,b,x[i+11],16, 1839030562);b=hh(b,c,d,a,x[i+14],23,  -35309556);
            a=hh(a,b,c,d,x[i+ 1], 4,-1530992060);d=hh(d,a,b,c,x[i+ 4],11, 1272893353);c=hh(c,d,a,b,x[i+ 7],16, -155497632);
            b=hh(b,c,d,a,x[i+10],23,-1094730640);a=hh(a,b,c,d,x[i+13], 4,  681279174);d=hh(d,a,b,c,x[i+ 0],11, -358537222);
            c=hh(c,d,a,b,x[i+ 3],16, -722521979);b=hh(b,c,d,a,x[i+ 6],23,   76029189);a=hh(a,b,c,d,x[i+ 9], 4, -640364487);
            d=hh(d,a,b,c,x[i+12],11, -421815835);c=hh(c,d,a,b,x[i+15],16,  530742520);b=hh(b,c,d,a,x[i+ 2],23, -995338651);
            a=ii(a,b,c,d,x[i+ 0], 6, -198630844);d=ii(d,a,b,c,x[i+ 7],10, 1126891415);c=ii(c,d,a,b,x[i+14],15,-1416354905);
            b=ii(b,c,d,a,x[i+ 5],21,  -57434055);a=ii(a,b,c,d,x[i+12], 6, 1700485571);d=ii(d,a,b,c,x[i+ 3],10,-1894986606);
            c=ii(c,d,a,b,x[i+10],15,   -1051523);b=ii(b,c,d,a,x[i+ 1],21,-2054922799);a=ii(a,b,c,d,x[i+ 8], 6, 1873313359);
            d=ii(d,a,b,c,x[i+15],10,  -30611744);c=ii(c,d,a,b,x[i+ 6],15,-1560198380);b=ii(b,c,d,a,x[i+13],21, 1309151649);
            a=ii(a,b,c,d,x[i+ 4], 6, -145523070);d=ii(d,a,b,c,x[i+11],10,-1120210379);c=ii(c,d,a,b,x[i+ 2],15,  718787259);
            b=ii(b,c,d,a,x[i+ 9],21, -343485551);a=ad(a,olda);b=ad(b,oldb);c=ad(c,oldc);d=ad(d,oldd);
        }
        return rh(a)+rh(b)+rh(c)+rh(d);
    },
    point(x, y){
        return {
            x: x,
            y: y,
        }
    },
    rect(x, y, width, height){
        return {
            x: x,
            y: y,
            width: width,
            height: height,
            left: x,
            top: y,
            right: x + width,
            bottom: y + height
        }
    },
    rgba(r, g, b, a){
        let _a = Math.trunc(a*255)
        let _r = Math.trunc(r*255)
        let _g = Math.trunc(g*255)
        let _b = Math.trunc(b*255)

        _a = (_a < 16 ? '0' : '') + _a.toString(16)
        _r = (_r < 16 ? '0' : '') + _r.toString(16)
        _g = (_g < 16 ? '0' : '') + _g.toString(16)
        _b = (_b < 16 ? '0' : '') + _b.toString(16)

        return `#${_r}${_g}${_b}${_a}`
    },
    rgbToHsl(r, g, b) {
        r /= 255, g /= 255, b /= 255;
      
        var max = Math.max(r, g, b), min = Math.min(r, g, b);
        var h, s, l = (max + min) / 2;
      
        if (max == min) {
          h = s = 0;
        } else {
          var d = max - min;
          s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
      
          switch (max) {
            case r: h = (g - b) / d + (g < b ? 6 : 0); break;
            case g: h = (b - r) / d + 2; break;
            case b: h = (r - g) / d + 4; break;
          }
      
          h /= 6;
        }
      
        return [ h, s, l ];
    },
    hslToRgb(h, s, l) {
        var r, g, b;
      
        if (s == 0) {
          r = g = b = l; // achromatic
        } else {
          function hue2rgb(p, q, t) {
            if (t < 0) t += 1;
            if (t > 1) t -= 1;
            if (t < 1/6) return p + (q - p) * 6 * t;
            if (t < 1/2) return q;
            if (t < 2/3) return p + (q - p) * (2/3 - t) * 6;
            return p;
          }
      
          var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
          var p = 2 * l - q;
      
          r = hue2rgb(p, q, h + 1/3);
          g = hue2rgb(p, q, h);
          b = hue2rgb(p, q, h - 1/3);
        }
      
        return [ r * 255, g * 255, b * 255 ];
    },
    rgbToHsv(r, g, b) {
        r /= 255, g /= 255, b /= 255;
      
        var max = Math.max(r, g, b), min = Math.min(r, g, b);
        var h, s, v = max;
      
        var d = max - min;
        s = max == 0 ? 0 : d / max;
      
        if (max == min) {
          h = 0; // achromatic
        } else {
          switch (max) {
            case r: h = (g - b) / d + (g < b ? 6 : 0); break;
            case g: h = (b - r) / d + 2; break;
            case b: h = (r - g) / d + 4; break;
          }
      
          h /= 6;
        }
      
        return [ h, s, v ];
    },
    hsvToRgb(h, s, v) {
        var r, g, b;
      
        var i = Math.floor(h * 6);
        var f = h * 6 - i;
        var p = v * (1 - s);
        var q = v * (1 - f * s);
        var t = v * (1 - (1 - f) * s);
      
        switch (i % 6) {
          case 0: r = v, g = t, b = p; break;
          case 1: r = q, g = v, b = p; break;
          case 2: r = p, g = v, b = t; break;
          case 3: r = p, g = q, b = v; break;
          case 4: r = t, g = p, b = v; break;
          case 5: r = v, g = p, b = q; break;
        }
      
        return [ r * 255, g * 255, b * 255 ];
    },
    toRGBA(value){
        if(!value){
            return {
                r: 255,
                g: 255,
                b: 255,
                a: 0,
            }
        }
        let hexColor = '#ffffff'
        let alpha = false

        if(value[0] === '#') {
            if(value.length === 4){
                hexColor = `#${value[1]}${value[1]}${value[2]}${value[2]}${value[3]}${value[3]}`
            } else if(value.length === 7) {
                hexColor = value
            } else if(value.length === 5) {
                alpha = true
                hexColor = `#${value[1]}${value[1]}${value[2]}${value[2]}${value[3]}${value[3]}${value[4]}${value[4]}`
            } else if(value.length === 9) {
                alpha = true
                hexColor = value
            } else {
                hexColor = value
            }
        } else {
            return {
                r: 255,
                g: 255,
                b: 255,
                a: 0,
            }
        }
        
        if(alpha){
            return {
                r: parseInt(hexColor[3]+hexColor[4], 16),
                g: parseInt(hexColor[5]+hexColor[6], 16),
                b: parseInt(hexColor[7]+hexColor[8], 16),
                a: parseInt(hexColor[1]+hexColor[2], 16)/255,
            }
        } else {
            return {
                r: parseInt(hexColor[1]+hexColor[2], 16),
                g: parseInt(hexColor[3]+hexColor[4], 16),
                b: parseInt(hexColor[5]+hexColor[6], 16),
                a: 1,
            }
        }
        

    },
    darker(baseColor, factor = 1.5){
        let rgba = this.toRGBA(baseColor)
        let hsv = this.rgbToHsv(rgba.r, rgba.g, rgba.b)
        rgba = this.hsvToRgb(hsv[0], hsv[0], hsv[0] / factor)
        let rh = rgba[0] > 15 ? rgba[0].toString(16) : '0' + rgba[0].toString(16)
        let gh = rgba[1] > 15 ? rgba[1].toString(16) : '0' + rgba[1].toString(16)
        let bh = rgba[2] > 15 ? rgba[2].toString(16) : '0' + rgba[2].toString(16)
        return `#${rh}${gh}${bh}`
    },
    lighter(baseColor, factor = 2.0){
        let rgba = this.toRGBA(baseColor)
        let hsv = this.rgbToHsv(rgba.r, rgba.g, rgba.b)
        rgba = this.hsvToRgb(hsv[0], hsv[0], hsv[0] * factor)
        let rh = rgba[0] > 15 ? rgba[0].toString(16) : '0' + rgba[0].toString(16)
        let gh = rgba[1] > 15 ? rgba[1].toString(16) : '0' + rgba[1].toString(16)
        let bh = rgba[2] > 15 ? rgba[2].toString(16) : '0' + rgba[2].toString(16)
        return `#${rh}${gh}${bh}`
    },
    tint(baseColor, tintColor){

    },
    openUrlExternally: function(url){
        return window.open(url, '_blank')
    },
    locale: function(){
        return {
            decimalPoint: (1.1).toLocaleString().substring(1, 2)
        }
    },
    qsTr: function(sourceText){
        let translate = mainRoot.languages[mainRoot.language]
        let result = translate ? translate[sourceText] : sourceText
        return result ? result : sourceText
    },
    createComponent(namespace, path, parent){
        let className = namespace.split('_')
        for(let name of path.replaceAll('/', '_').replaceAll('\\', '_').replaceAll('.qml', '').split('_')){
            if(name === '..') {
                className.pop()
            } else {
                if(className.indexOf(name) < 0) className.push(name)
            }
        }
        let cls
        try {
            cls = eval(className.join('_'))
        } catch (error) {
            let name = path.replaceAll('/', '_').replaceAll('\\', '_').replaceAll('.qml', '').split('_').pop()
            for(let key in classList){
                if(key.split('_').pop() === name){
                    cls = classList[key]
                    break
                }
            }
        }
        
        return {
            createObject: (parent)=>{
                let obj = new cls(parent)
                for(let update of updateList.splice(0, updateList.length)){
                    update()
                }
                TransactionController.add(obj)
                return obj
            }
        }
    },

    atob: function(data){
        return atob(data)
    },
    btoa: function(data){
        return btoa(data)
    },

    get localStorage(){return localStorage},
}

module.exports.Qt = Qt