// Shortcut (qquickshortcut.cpp + QShortcutMap): a window-level key listener matches key sequences.
// Every Shortcut of the window is in context (the web page is one window); several matches are
// ambiguous: activatedAmbiguously goes to one of them per press, in turn.
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { QtObject } = require('../Core/QtObject')

const CTRL = 1, SHIFT = 2, ALT = 4, META = 8

// QKeySequence::StandardKey with the Windows / Linux bindings
const StandardKey = {
    UnknownKey: 0, HelpContents: 1, WhatsThis: 2, Open: 3, Close: 4, Save: 5, New: 6, Delete: 7, Cut: 8, Copy: 9,
    Paste: 10, Undo: 11, Redo: 12, Back: 13, Forward: 14, Refresh: 15, ZoomIn: 16, ZoomOut: 17, Print: 18, AddTab: 19,
    NextChild: 20, PreviousChild: 21, Find: 22, FindNext: 23, FindPrevious: 24, Replace: 25, SelectAll: 26,
    Bold: 27, Italic: 28, Underline: 29, SaveAs: 63, Preferences: 64, Quit: 65, FullScreen: 66, Deselect: 67,
    Backspace: 69, Cancel: 70,
}
const STANDARD = {
    1: ['F1'], 2: ['Shift+F1'], 3: ['Ctrl+O'], 4: ['Ctrl+F4', 'Ctrl+W'], 5: ['Ctrl+S'], 6: ['Ctrl+N'], 7: ['Del'],
    8: ['Ctrl+X', 'Shift+Del'], 9: ['Ctrl+C', 'Ctrl+Ins'], 10: ['Ctrl+V', 'Shift+Ins'], 11: ['Ctrl+Z', 'Alt+Backspace'],
    12: ['Ctrl+Y', 'Shift+Ctrl+Z'], 13: ['Alt+Left', 'Backspace'], 14: ['Alt+Right', 'Shift+Backspace'], 15: ['F5'],
    16: ['Ctrl++', 'Ctrl+='], 17: ['Ctrl+-'], 18: ['Ctrl+P'], 19: ['Ctrl+T'], 20: ['Ctrl+Tab', 'Forward', 'Ctrl+F6'],
    21: ['Ctrl+Shift+Tab', 'Back', 'Ctrl+Shift+F6'], 22: ['Ctrl+F'], 23: ['F3', 'Ctrl+G'], 24: ['Shift+F3', 'Ctrl+Shift+G'],
    25: ['Ctrl+H'], 26: ['Ctrl+A'], 27: ['Ctrl+B'], 28: ['Ctrl+I'], 29: ['Ctrl+U'], 63: ['Ctrl+Shift+S'], 65: ['Ctrl+Q'],
    66: ['F11'], 67: ['Ctrl+Shift+A'], 69: ['Backspace'], 70: ['Escape'],
}

// Qt key names -> normalized key ids compared with keyOf(event)
const NAMES = {
    esc: 'escape', escape: 'escape', return: 'return', enter: 'enter', tab: 'tab', backtab: 'tab', space: ' ',
    backspace: 'backspace', del: 'delete', delete: 'delete', ins: 'insert', insert: 'insert', home: 'home', end: 'end',
    pgup: 'pageup', pageup: 'pageup', pgdown: 'pagedown', pagedown: 'pagedown', up: 'arrowup', down: 'arrowdown',
    left: 'arrowleft', right: 'arrowright', menu: 'contextmenu', print: 'printscreen', pause: 'pause',
}

function parseCombo(text){
    text = text.trim()
    if(!text) return null
    let parts = []
    // "Ctrl++": the last '+' is the key
    let rest = text
    for(;;){
        let i = rest.indexOf('+')
        if(i <= 0 || i === rest.length - 1) break
        parts.push(rest.slice(0, i))
        rest = rest.slice(i + 1)
    }
    let mods = 0
    for(let m of parts){
        let n = m.trim().toLowerCase()
        if(n === 'ctrl' || n === 'control') mods |= CTRL
        else if(n === 'shift') mods |= SHIFT
        else if(n === 'alt') mods |= ALT
        else if(n === 'meta' || n === 'win') mods |= META
        else return null
    }
    let key = rest.trim().toLowerCase()
    key = NAMES[key] || key
    if(rest.trim().toLowerCase() === 'backtab') mods |= SHIFT
    return { mods, key }
}

// "Ctrl+K, Ctrl+C" -> [combo, combo]; a StandardKey -> its bindings
function parseSequences(value){
    if(value === undefined || value === null || value === '') return []
    if(typeof value === 'number') return (STANDARD[value] || []).map(text => [parseCombo(text)]).filter(seq => seq[0])
    let seq = String(value).split(',').map(parseCombo)
    // "Ctrl+," would split wrongly; such sequences are rare and skipped
    return seq.every(Boolean) && seq.length <= 4 ? [seq] : []
}

function keyOf(e){
    let code = e.code || ''
    if(/^Key[A-Z]$/.test(code)) return code.slice(3).toLowerCase()
    if(/^Digit\d$/.test(code)) return code.slice(5)
    if(code === 'NumpadEnter') return 'enter'
    let key = String(e.key || '').toLowerCase()
    if(key === 'enter') return 'return'
    if(key === 'esc') return 'escape'
    return key
}

function modsOf(e){
    return (e.ctrlKey ? CTRL : 0) | (e.shiftKey ? SHIFT : 0) | (e.altKey ? ALT : 0) | (e.metaKey ? META : 0)
}

function comboMatches(combo, key, mods){
    if(combo.key !== key) return false
    if(combo.mods === mods) return true
    // a symbol typed with Shift ("Ctrl++") does not need Shift in the sequence
    return key.length === 1 && !/[a-z0-9]/.test(key) && (combo.mods | SHIFT) === mods
}

// text editing keys a focused text field takes first (ShortcutOverride accepted by TextInput)
function textFieldTakes(key, mods){
    if(!(mods & (CTRL | ALT | META))) return key.length === 1 || ['backspace', 'delete', 'home', 'end', 'arrowleft', 'arrowright', 'arrowup', 'arrowdown'].indexOf(key) >= 0
    if((mods & ~SHIFT) === CTRL) return ['a', 'c', 'v', 'x', 'z', 'y', 'arrowleft', 'arrowright', 'home', 'end', 'backspace', 'delete', 'insert'].indexOf(key) >= 0
    return false
}

const shortcuts = new Set()
let partial = []       // sequences matched so far: { shortcut, sequence, index }
let ambiguity = new Map()

function onKeyDown(e){
    if(engine.isStopped()) return
    let key = keyOf(e)
    if(key === 'control' || key === 'shift' || key === 'alt' || key === 'meta') return
    let mods = modsOf(e)
    let target = e.target
    let inTextField = target && (target.tagName === 'INPUT' || target.tagName === 'TEXTAREA' || target.isContentEditable)
    if(inTextField && textFieldTakes(key, mods)){
        partial = []
        return
    }

    let candidates = partial.length ? partial : null
    let next = [], complete = []
    let consider = (shortcut, sequence, index) => {
        if(index < sequence.length && comboMatches(sequence[index], key, mods)){
            if(index + 1 === sequence.length) complete.push(shortcut)
            else next.push({ shortcut, sequence, index: index + 1 })
        }
    }
    if(candidates){
        for(let c of candidates) if(shortcuts.has(c.shortcut) && c.shortcut.$enabledNow()) consider(c.shortcut, c.sequence, c.index)
    }
    if(!complete.length && !next.length){
        for(let shortcut of shortcuts){
            if(!shortcut.$enabledNow()) continue
            if(e.repeat && !shortcut.$v[Shortcut.AUTOREPEAT]) continue
            for(let sequence of shortcut.$sequences) consider(shortcut, sequence, 0)
        }
    }
    complete = complete.filter((s, i) => complete.indexOf(s) === i)
    if(complete.length){
        partial = []
        e.preventDefault()
        engine.batch(() => {
            if(complete.length === 1){
                engine.emit(complete[0], Shortcut.SIGNAL_activated)
            } else {
                let id = key + ':' + mods
                let turn = ambiguity.get(id) || 0
                ambiguity.set(id, turn + 1)
                engine.emit(complete[turn % complete.length], Shortcut.SIGNAL_activatedAmbiguously)
            }
        })
        return
    }
    partial = next
    if(next.length) e.preventDefault()
}

let listening = false
function listen(){
    if(listening || typeof window === 'undefined') return
    listening = true
    window.addEventListener('keydown', onKeyDown, true)
}

class Shortcut extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$sequences = []
        shortcuts.add(this)
        listen()
    }

    $enabledNow(){
        return this.$v[Shortcut.ENABLED] && !(this.$f & engine.DESTROYED)
    }

    $update(){
        let list = parseSequences(this.$v[Shortcut.SEQUENCE])
        let more = this.$v[Shortcut.SEQUENCES]
        if(Array.isArray(more)) for(let s of more) list.push(...parseSequences(s))
        this.$sequences = list
        let text = (this.$v[Shortcut.SEQUENCE] === undefined ? '' : this.$v[Shortcut.SEQUENCE])
        this.$v[Shortcut.NATIVETEXT] = typeof text === 'number' ? (STANDARD[text] || [''])[0] : String(text)
        this.$v[Shortcut.PORTABLETEXT] = this.$v[Shortcut.NATIVETEXT]
    }

    $disposeNative(){
        shortcuts.delete(this)
        super.$disposeNative()
    }
}

const update = (shortcut) => shortcut.$update()

defineType(Shortcut, {
    props: {
        sequence: { kind: 'var', effect: update },
        sequences: { kind: 'var', effect: update },
        enabled: { kind: 'bool', initial: true },
        autoRepeat: { kind: 'bool', initial: true },
        context: { kind: 'int', initial: 1 },
        nativeText: { kind: 'string', readonly: true },
        portableText: { kind: 'string', readonly: true },
    },
    signals: ['activated', 'activatedAmbiguously'],
})

module.exports = { Shortcut, StandardKey, parseSequences }
