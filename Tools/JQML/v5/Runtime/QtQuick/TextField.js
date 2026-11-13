const engine = require('../Core/engine')
const { toCss } = require('../Core/color')
const { Font } = require('./Font')
const { Item, bits } = require('./Item')
const { measure, wrapParagraph } = require('./TextLayout')
const { ACCEPTABLE, INVALID } = require('./Validator')
const { isSyncingDom, CONSUMED } = require('./KeyDelivery')
const QtEnums = require('../Qt/enums')

const STYLE = bits.STYLE

const TEXT = 64

function fontOf(item){
    let font = item.$v[item.constructor.FONT]
    return {
        pixelSize: font ? font.$v[Font.PIXELSIZE] : 12,
        family: font ? font.$v[Font.FAMILY] : 'Segoe UI',
        bold: font ? font.$v[Font.BOLD] : false,
    }
}

// A mouse selection that leaves a single-line field: QQuickTextInput::mouseMoveEvent puts the cursor at the
// x of the mouse (positionAt, clamped to the text) wherever the mouse is; the browser instead snaps it to
// whichever end of the field the point outside maps to in document order
let selectionDrag = null

function caretAtX(field, clientX){
    let input = field.$input
    let rect = input.getBoundingClientRect()
    let font = fontOf(field)
    let f = { pixelSize: font.pixelSize, family: font.family, weight: font.bold ? 700 : 400, italic: false, letterSpacing: 0 }
    let text = input.value
    if(input.type === 'password') text = '•'.repeat(text.length)
    let x = clientX - rect.left + input.scrollLeft
    if(x <= 0) return 0
    let previous = 0
    for(let i = 1; i <= text.length; i++){
        let width = measure(text.slice(0, i), f).advance
        if(x < width) return x - previous < width - x ? i - 1 : i
        previous = width
    }
    return text.length
}

function dragSelectionMove(e){
    let drag = selectionDrag
    if(!drag || !(e.buttons & 1)) return
    let input = drag.field.$input
    if(drag.anchor === null) drag.anchor = input.selectionDirection === 'backward' ? input.selectionEnd : input.selectionStart
    let rect = input.getBoundingClientRect()
    if(e.clientX >= rect.left && e.clientX <= rect.right && e.clientY >= rect.top && e.clientY <= rect.bottom){
        drag.focus = null
        return
    }
    drag.focus = caretAtX(drag.field, e.clientX)
    // after the browser's own handling of this move
    setTimeout(applyDragSelection, 0)
}

function applyDragSelection(){
    let drag = selectionDrag
    if(!drag || drag.focus === null) return
    let input = drag.field.$input
    let start = Math.min(drag.anchor, drag.focus), end = Math.max(drag.anchor, drag.focus)
    let direction = drag.focus < drag.anchor ? 'backward' : 'forward'
    if(input.selectionStart !== start || input.selectionEnd !== end || input.selectionDirection !== direction) input.setSelectionRange(start, end, direction)
}

function listenSelectionDrag(){
    if(listenSelectionDrag.done) return
    listenSelectionDrag.done = true
    window.addEventListener('mousemove', dragSelectionMove, true)
    window.addEventListener('mouseup', () => { selectionDrag = null }, true)
    document.addEventListener('selectionchange', applyDragSelection)
}

const EDITING_KEYS = new Set([QtEnums.Key_Backspace, QtEnums.Key_Delete, QtEnums.Key_Left, QtEnums.Key_Right,
    QtEnums.Key_Home, QtEnums.Key_End, QtEnums.Key_Insert])
const MULTILINE_KEYS = new Set([QtEnums.Key_Up, QtEnums.Key_Down, QtEnums.Key_PageUp, QtEnums.Key_PageDown, QtEnums.Key_Return])
const CTRL_KEYS = new Set([0x41, 0x43, 0x56, 0x58, 0x59, 0x5a, QtEnums.Key_Left, QtEnums.Key_Right, QtEnums.Key_Home,
    QtEnums.Key_End, QtEnums.Key_Backspace, QtEnums.Key_Delete, QtEnums.Key_Insert])

// the keys QQuickTextInputPrivate::processKeyEvent (QQuickTextControl for TextEdit) accepts: typing, editing,
// moving the cursor, the clipboard and select all; Up, Down, Page keys, Escape, Tab and other shortcuts go on
function takesKey(event, multiline, readOnly){
    let mods = event.modifiers & ~(QtEnums.ShiftModifier | 0x20000000)
    if(mods === QtEnums.ControlModifier) return CTRL_KEYS.has(event.key)
    if(mods) return false
    if(multiline && MULTILINE_KEYS.has(event.key)) return true
    if(EDITING_KEYS.has(event.key)) return true
    return !readOnly && event.text !== ''
}

class TextField extends Item {
    constructor(parent = null, multiline = false){
        super(parent)
        this.$input = null
        this.$syncing = false
        // QQuickTextInput::focusOutEvent
        engine.connect(this, Item.SIGNAL_activeFocusChanged, (old, active) => {
            if(!active && this.$acceptOrFixup()) engine.emitArgs(this, this.constructor.SIGNAL_editingFinished, [])
        })
        if(this.$dom){
            let input = document.createElement(multiline ? 'textarea' : 'input')
            input.className = 'jq2-input'
            input.style.cssText = 'position:absolute;left:0;top:0;width:100%;height:100%;margin:0;padding:0;border:0;outline:none;background:transparent;resize:none;pointer-events:auto;font:inherit;color:inherit;'
            this.$dom.appendChild(input)
            this.$input = input
            input.$item = this
            input.addEventListener('input', () => engine.batch(() => {
                // QQuickTextInputPrivate::finishChange: an Invalid edit is undone
                if(this.$validateText && this.$validateText(input.value) === INVALID){
                    let previous = this.$v[this.constructor.TEXT]
                    let caret = Math.max(0, Math.min(previous.length, this.$v[this.constructor.CURSORPOSITION]))
                    input.value = previous
                    input.setSelectionRange(caret, caret)
                    return
                }
                this.$syncing = true
                try {
                    let binding = this.$takeBinding(this.constructor.TEXT)
                    if(binding) engine.disposeBinding(binding)
                    this.$write(this.constructor.TEXT, input.value)
                    this.$syncSelection()
                } finally {
                    this.$syncing = false
                }
                engine.emitArgs(this, this.constructor.SIGNAL_textEdited, [])
            }))
            // a click or Tab into the input: the field takes active focus (activeFocusOnPress); the input itself
            // is focused and blurred by KeyDelivery as the active focus moves
            input.addEventListener('focus', () => {
                if(!isSyncingDom() && !(this.$f & engine.DESTROYED)) this.forceActiveFocus()
            })
            input.addEventListener('select', () => engine.batch(() => this.$syncSelection()))
            // the caret moved by the keys or a click, nothing selected: no select event
            input.addEventListener('selectionchange', () => engine.batch(() => this.$syncSelection()))
            if(!multiline){
                listenSelectionDrag()
                input.addEventListener('mousedown', (event) => {
                    selectionDrag = event.button === 0 && event.detail === 1 && !event.shiftKey ? { field: this, anchor: null, focus: null } : null
                })
            }
        }
        this.$updateImplicitSize()
    }

    // hasAcceptableInput || fixup(): the validator may turn the text acceptable on Return / focus out
    $acceptOrFixup(){
        if(!this.$validateText) return true
        let text = this.$v[this.constructor.TEXT]
        if(this.$validateText(text) === ACCEPTABLE) return true
        let validator = this.$v[this.constructor.VALIDATOR]
        if(!validator || !validator.$fixup) return false
        let fixed = validator.$fixup(text)
        if(this.$validateText(fixed) !== ACCEPTABLE) return false
        if(fixed !== text) this.$write(this.constructor.TEXT, fixed)
        return true
    }

    $syncSelection(){
        if(!this.$input) return
        let start = this.$input.selectionStart || 0
        let end = this.$input.selectionEnd || 0
        this.$write(this.constructor.SELECTIONSTART, start)
        this.$write(this.constructor.SELECTIONEND, end)
        this.$write(this.constructor.CURSORPOSITION, end)
        this.$write(this.constructor.SELECTEDTEXT, this.$v[this.constructor.TEXT].slice(start, end))
    }

    // implicit width is the unwrapped text; a TextEdit with wrapMode and a set width wraps its content to it
    $updateImplicitSize(){
        let font = fontOf(this)
        let f = { pixelSize: font.pixelSize, family: font.family, weight: font.bold ? 700 : 400, italic: false, letterSpacing: 0 }
        let paragraphs = String(this.$v[this.constructor.TEXT]).split('\n')
        let wrap = this.constructor.WRAPMODE !== undefined ? this.$v[this.constructor.WRAPMODE] : 0
        let lines = paragraphs
        if(wrap && (this.$explicit & 1)){
            lines = []
            for(let paragraph of paragraphs) wrapParagraph(paragraph, wrap, this.$v[Item.WIDTH], f, lines)
        }
        let implicitWidth = 0
        for(let paragraph of paragraphs) implicitWidth = Math.max(implicitWidth, measure(paragraph, f).width)
        let width = 0, height = 0
        for(let line of lines){
            let size = measure(line, f)
            width = Math.max(width, size.width)
            height += size.height
        }
        engine.markDirty(this, TEXT)
        this.$write(this.constructor.CONTENTWIDTH, width)
        this.$write(this.constructor.CONTENTHEIGHT, height)
        if(this.constructor.PAINTEDWIDTH !== undefined){
            this.$write(this.constructor.PAINTEDWIDTH, width)
            this.$write(this.constructor.PAINTEDHEIGHT, height)
        }
        if(this.constructor.LINECOUNT !== undefined) this.$write(this.constructor.LINECOUNT, lines.length)
        this.$write(Item.IMPLICITWIDTH, implicitWidth)
        this.$write(Item.IMPLICITHEIGHT, height)
    }

    // the field's DOM value and selection are read back while it is hidden
    $flush(dirty){
        this.$paint(dirty)
    }

    $paint(dirty){
        super.$paint(dirty)
        let input = this.$input
        if(!input) return
        if(dirty & TEXT){
            let text = this.$v[this.constructor.TEXT]
            if(input.value !== text) input.value = text
            let font = fontOf(this)
            input.style.fontSize = font.pixelSize + 'px'
            input.style.fontFamily = font.family
            input.style.fontWeight = font.bold ? 'bold' : ''
            input.readOnly = !!this.$v[this.constructor.READONLY]
            if(this.constructor.ECHOMODE !== undefined) input.type = this.$v[this.constructor.ECHOMODE] === 2 ? 'password' : 'text'
            let max = this.$v[this.constructor.MAXIMUMLENGTH]
            if(max !== undefined && max < 32767) input.maxLength = max
        }
        if(dirty & STYLE) input.style.color = toCss(this.$v[this.constructor.COLOR])
    }

    // QQuickTextInput::keyPressEvent: the keys of editing stay in the field (the browser types them); Return
    // emits accepted and goes on to the parents like the keys a single line field does not use
    $keyEvent(event, kind){
        if(kind !== 'pressed') return 0
        let key = event.key
        let multiline = this.$input && this.$input.tagName === 'TEXTAREA'
        if(key === QtEnums.Key_Return && !multiline){
            if(this.$acceptOrFixup()){
                if(this.constructor.SIGNAL_accepted !== undefined) engine.emitArgs(this, this.constructor.SIGNAL_accepted, [])
                engine.emitArgs(this, this.constructor.SIGNAL_editingFinished, [])
            }
            return 0
        }
        return takesKey(event, multiline, !!this.$v[this.constructor.READONLY]) ? CONSUMED : 0
    }

    selectAll(){ this.select(0, this.$v[this.constructor.TEXT].length) }

    select(start, end){
        if(this.$input) this.$input.setSelectionRange(Math.min(start, end), Math.max(start, end))
        this.$write(this.constructor.SELECTIONSTART, Math.min(start, end))
        this.$write(this.constructor.SELECTIONEND, Math.max(start, end))
        this.$write(this.constructor.SELECTEDTEXT, this.$v[this.constructor.TEXT].slice(Math.min(start, end), Math.max(start, end)))
    }

    deselect(){ this.select(this.$v[this.constructor.CURSORPOSITION], this.$v[this.constructor.CURSORPOSITION]) }

    clear(){ this.text = '' }

    insert(position, text){
        let current = this.$v[this.constructor.TEXT]
        this.text = current.slice(0, position) + text + current.slice(position)
    }

    remove(start, end){
        let current = this.$v[this.constructor.TEXT]
        this.text = current.slice(0, Math.min(start, end)) + current.slice(Math.max(start, end))
    }

    getText(start, end){ return this.$v[this.constructor.TEXT].slice(start, end) }
}

const textChanged = (field) => {
    let id = field.constructor.LENGTH
    if(id !== undefined) field.$write(id, field.$v[field.constructor.TEXT].length)
    if(field.$updateAcceptable) field.$updateAcceptable()
    field.$updateImplicitSize()
}
const style = (item) => engine.markDirty(item, STYLE)
const text = (item) => engine.markDirty(item, TEXT)

const common = {
    text: { kind: 'string', effect: textChanged },
    color: { kind: 'color', effect: style },
    font: { group: Font },
    readOnly: { kind: 'bool', effect: text },
    cursorPosition: 'int',
    selectionStart: { kind: 'int', readonly: true },
    selectionEnd: { kind: 'int', readonly: true },
    selectedText: { kind: 'string', readonly: true },
    selectionColor: { kind: 'color', initial: '#0078d7' },
    selectedTextColor: { kind: 'color', initial: '#ffffff' },
    cursorRectangle: { kind: 'var', readonly: true },
    cursorVisible: 'bool',
    persistentSelection: 'bool',
    mouseSelectionMode: 'int',
    overwriteMode: 'bool',
    leftPadding: 'real',
    rightPadding: 'real',
    topPadding: 'real',
    bottomPadding: 'real',
    padding: 'real',
    inputMethodHints: 'int',
    selectByMouse: { kind: 'bool', initial: true },
    activeFocusOnPress: { kind: 'bool', initial: true },
    horizontalAlignment: { kind: 'int', initial: 1 },
    verticalAlignment: { kind: 'int', initial: 32 },
    length: { kind: 'int', readonly: true },
    contentWidth: { kind: 'real', readonly: true },
    contentHeight: { kind: 'real', readonly: true },
}

module.exports = { TextField, text, common }
