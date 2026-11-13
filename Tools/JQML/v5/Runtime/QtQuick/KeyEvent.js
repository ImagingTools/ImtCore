const QtEnums = require('../Qt/enums')

const KEYPAD_MODIFIER = 0x20000000     // Qt::KeypadModifier

const MODIFIERS = { shiftKey: QtEnums.ShiftModifier, ctrlKey: QtEnums.ControlModifier, altKey: QtEnums.AltModifier, metaKey: QtEnums.MetaModifier }

// The Qt key of a DOM event: named keys are their DOM names (the Qt.Key_* values of this runtime),
// letters and digits their Qt codes by the physical key, so Ctrl+A is Key_A in any layout; Shift+Tab is Key_Backtab
function qtKey(e){
    let key = String(e.key || '')
    if(key === 'Tab' && e.shiftKey) return QtEnums.Key_Backtab
    let code = e.code || ''
    if(/^Key[A-Z]$/.test(code)) return code.charCodeAt(3)
    if(/^Digit[0-9]$/.test(code)) return code.charCodeAt(5)
    if(key === ' ') return QtEnums.Key_Space
    if(key.length === 1) return key.toUpperCase().charCodeAt(0)
    return key
}

// KeyEvent of QML (QQuickKeyEvent): accepted starts false for every receiver
class KeyEvent {
    constructor(e){
        this.key = qtKey(e)
        let modifiers = 0
        for(let name in MODIFIERS) if(e[name]) modifiers |= MODIFIERS[name]
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
            let key = typeof this.key === 'number' ? String.fromCharCode(this.key).toLowerCase() : String(this.key).toLowerCase()
            if(combo.mods === mods && combo.key === key) return true
        }
        return false
    }
}

module.exports = { KeyEvent, qtKey }
