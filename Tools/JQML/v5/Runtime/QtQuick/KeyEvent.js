const QtEnums = require('../Qt/enums')

const KEYPAD_MODIFIER = 0x20000000     // Qt::KeypadModifier

// Qt on macOS (Qt::AA_MacDontSwapCtrlAndMeta off): the Command key is Qt::ControlModifier, the Control key Qt::MetaModifier
const isMac = () => typeof navigator !== 'undefined' && !!navigator && /Mac|iPhone|iPad|iPod/.test(String(navigator.platform || ''))

function modifiersOf(e){
    let mac = isMac()
    return (e.shiftKey ? QtEnums.ShiftModifier : 0) | (e.altKey ? QtEnums.AltModifier : 0)
        | (e.ctrlKey ? (mac ? QtEnums.MetaModifier : QtEnums.ControlModifier) : 0)
        | (e.metaKey ? (mac ? QtEnums.ControlModifier : QtEnums.MetaModifier) : 0)
}

// DOM KeyboardEvent.key names -> Qt::Key (the keys whose name is not their character)
const K = QtEnums
const DOM_KEYS = {
    Escape: K.Key_Escape, Esc: K.Key_Escape, Tab: K.Key_Tab, Backspace: K.Key_Backspace, Enter: K.Key_Return,
    Insert: K.Key_Insert, Delete: K.Key_Delete, Del: K.Key_Delete, Pause: K.Key_Pause, PrintScreen: K.Key_Print,
    Clear: K.Key_Clear, Home: K.Key_Home, End: K.Key_End, ArrowLeft: K.Key_Left, ArrowUp: K.Key_Up,
    ArrowRight: K.Key_Right, ArrowDown: K.Key_Down, Left: K.Key_Left, Up: K.Key_Up, Right: K.Key_Right, Down: K.Key_Down,
    PageUp: K.Key_PageUp, PageDown: K.Key_PageDown, Shift: K.Key_Shift, Control: K.Key_Control, Meta: K.Key_Meta,
    OS: K.Key_Meta, Alt: K.Key_Alt, AltGraph: K.Key_AltGr, CapsLock: K.Key_CapsLock, NumLock: K.Key_NumLock,
    ScrollLock: K.Key_ScrollLock, ContextMenu: K.Key_Menu, Help: K.Key_Help, Hyper: K.Key_Hyper_L, Super: K.Key_Super_L,
    Undo: K.Key_Undo, Redo: K.Key_Redo, Copy: K.Key_Copy, Cut: K.Key_Cut, Paste: K.Key_Paste, Find: K.Key_Find,
    Open: K.Key_Open, Save: K.Key_Save, Close: K.Key_Close, New: K.Key_New, Print: K.Key_Printer, Select: K.Key_Select,
    Execute: K.Key_Execute, Cancel: K.Key_Cancel, ZoomIn: K.Key_ZoomIn, ZoomOut: K.Key_ZoomOut,
    AudioVolumeUp: K.Key_VolumeUp, AudioVolumeDown: K.Key_VolumeDown, AudioVolumeMute: K.Key_VolumeMute,
    MediaPlayPause: K.Key_MediaTogglePlayPause, MediaPlay: K.Key_MediaPlay, MediaPause: K.Key_MediaPause,
    MediaStop: K.Key_MediaStop, MediaTrackNext: K.Key_MediaNext, MediaTrackPrevious: K.Key_MediaPrevious,
    MediaRecord: K.Key_MediaRecord, BrowserBack: K.Key_Back, BrowserForward: K.Key_Forward, BrowserRefresh: K.Key_Refresh,
    BrowserStop: K.Key_Stop, BrowserSearch: K.Key_Search, BrowserFavorites: K.Key_Favorites, BrowserHome: K.Key_HomePage,
    LaunchMail: K.Key_LaunchMail, LaunchApplication1: K.Key_Launch0, LaunchApplication2: K.Key_Launch1,
    Eject: K.Key_Eject, Power: K.Key_PowerOff, Sleep: K.Key_Sleep, WakeUp: K.Key_WakeUp, Standby: K.Key_Standby,
    BrightnessUp: K.Key_MonBrightnessUp, BrightnessDown: K.Key_MonBrightnessDown, Dead: K.Key_unknown,
    KanaMode: K.Key_Kana_Lock, HangulMode: K.Key_Hangul, HanjaMode: K.Key_Hangul_Hanja, Convert: K.Key_Henkan,
    NonConvert: K.Key_Muhenkan, Hiragana: K.Key_Hiragana, Katakana: K.Key_Katakana, Romaji: K.Key_Romaji,
    Zenkaku: K.Key_Zenkaku, Hankaku: K.Key_Hankaku, ZenkakuHankaku: K.Key_Zenkaku_Hankaku, Eisu: K.Key_Eisu_toggle,
}
for(let i = 1; i <= 35; i++) DOM_KEYS['F' + i] = K['Key_F' + i]

// Qt::Key -> the DOM key name a synthetic event carries (QtTest keyPress)
const QT_KEYS = new Map()
for(let name of Object.keys(DOM_KEYS)) if(!QT_KEYS.has(DOM_KEYS[name])) QT_KEYS.set(DOM_KEYS[name], name)
QT_KEYS.set(K.Key_Enter, 'Enter')

// The Qt key of a DOM event (QKeyEvent::key): letters and digits are their Qt codes by the physical key, so Ctrl+A is
// Key_A in any layout; another character its upper case code (QKeyMapper); a named key its Qt::Key; Shift+Tab is
// Key_Backtab, Enter of the keypad Key_Enter. On macOS the Command key is Key_Control and the Control key Key_Meta,
// as the modifiers (Qt::AA_MacDontSwapCtrlAndMeta off)
function qtKey(e){
    let key = String(e.key || '')
    if(key === 'Tab' && e.shiftKey) return K.Key_Backtab
    let code = e.code || ''
    if(/^Key[A-Z]$/.test(code)) return code.charCodeAt(3)
    if(/^Digit[0-9]$/.test(code)) return code.charCodeAt(5)
    if(key === ' ') return K.Key_Space
    if(key === 'Enter' && e.location === 3) return K.Key_Enter
    if((key === 'Control' || key === 'Meta' || key === 'OS') && isMac()) return key === 'Control' ? K.Key_Meta : K.Key_Control
    if(Object.prototype.hasOwnProperty.call(DOM_KEYS, key)) return DOM_KEYS[key]
    if(key.length === 1 || (key.length === 2 && key.codePointAt(0) > 0xffff)) return key.toUpperCase().codePointAt(0)
    return K.Key_unknown
}

// the DOM key name of a Qt key, for a synthetic event
function domKey(qtKeyValue){
    return QT_KEYS.get(qtKeyValue) || null
}

// KeyEvent of QML (QQuickKeyEvent): accepted starts false for every receiver
class KeyEvent {
    constructor(e){
        this.key = qtKey(e)
        // the key as Shortcut names it (QKeySequence matching in matches())
        this.$sequenceKey = require('./Shortcut').keyOf(e)
        let modifiers = modifiersOf(e)
        if(e.location === 3) modifiers |= KEYPAD_MODIFIER
        this.modifiers = modifiers
        this.$code = e.code || ''
        this.text = String(e.key || '').length === 1 && !(e.ctrlKey || e.metaKey) ? e.key : ''
        this.isAutoRepeat = !!e.repeat
        this.count = 1
        this.nativeScanCode = 0
        this.accepted = false
    }

    // QKeyEvent::matches: the key and modifiers form one of the bindings of the standard key
    matches(standardKey){
        let { parseSequences } = require('./Shortcut')
        for(let sequence of parseSequences(standardKey)){
            if(sequence.length !== 1) continue
            let combo = sequence[0]
            let mods = (this.modifiers & QtEnums.ControlModifier ? 1 : 0) | (this.modifiers & QtEnums.ShiftModifier ? 2 : 0)
                | (this.modifiers & QtEnums.AltModifier ? 4 : 0) | (this.modifiers & QtEnums.MetaModifier ? 8 : 0)
            if(combo.mods === mods && combo.key === this.$sequenceKey) return true
        }
        return false
    }
}

module.exports = { KeyEvent, qtKey, domKey, modifiersOf }
