const engine = require('../Core/engine')
const { toCss } = require('../Core/color')
const { QRectF } = require('../Core/ValueTypes')
const { Font } = require('./Font')
const { Item, bits } = require('./Item')
const { familyCss, measure, wrapParagraph } = require('./TextLayout')
const { ACCEPTABLE, INVALID } = require('./Validator')
const { isSyncingDom, CONSUMED, ACCEPTED, agent: keyAgent } = require('./KeyDelivery')
const QtEnums = require('../Qt/enums')

const STYLE = bits.STYLE

const TEXT = 64

const DEFAULT_FONT = { pixelSize: 12, family: 'Segoe UI', weight: 400, italic: false, underline: false, strikeout: false, capitalization: 0, letterSpacing: 0, wordSpacing: 0 }

function fontOf(item){
    let font = item.$v[item.constructor.FONT]
    return font ? font.$info() : DEFAULT_FONT
}

// QFont::Capitalization as the text is drawn (and measured)
function capitalize(text, mode){
    if(mode === 1) return text.toUpperCase()
    if(mode === 2) return text.toLowerCase()
    if(mode === 4) return text.replace(/(^|\s)(\S)/g, (m, s, c) => s + c.toUpperCase())
    return text
}

const ALIGN_RIGHT = 0x2, ALIGN_HCENTER = 0x4, ALIGN_JUSTIFY = 0x8, ALIGN_BOTTOM = 0x40, ALIGN_VCENTER = 0x80

// QQuickTextUtil::alignedX / alignedY
function alignedX(textWidth, availableWidth, alignment){
    if(alignment & ALIGN_RIGHT) return availableWidth - textWidth
    if(alignment & ALIGN_HCENTER) return (availableWidth - textWidth) / 2
    return 0
}

function alignedY(textHeight, availableHeight, alignment){
    if(alignment & ALIGN_BOTTOM) return availableHeight - textHeight
    if(alignment & ALIGN_VCENTER) return (availableHeight - textHeight) / 2
    return 0
}

const NORMAL = 0, NO_ECHO = 1, PASSWORD = 2, PASSWORD_ECHO_ON_EDIT = 3

// ---- input mask (QQuickTextInputPrivate::parseInputMask, maskString, clearString, stripString, findInMask) ----

const MASK_CHARS = new Set(['A', 'a', 'N', 'n', 'X', 'x', '9', '0', 'D', 'd', '#', 'H', 'h', 'B', 'b'])
const NO_CASE = 0, LOWER = 1, UPPER = 2

function parseInputMask(maskFields){
    let delimiter = maskFields ? maskFields.indexOf(';') : 0
    if(!maskFields || delimiter === 0) return null
    let inputMask = delimiter === -1 ? maskFields : maskFields.slice(0, delimiter)
    let blank = delimiter !== -1 && delimiter + 1 < maskFields.length ? maskFields[delimiter + 1] : ' '
    let data = []
    let caseMode = NO_CASE
    let escape = false
    for(let c of inputMask){
        if(escape){
            data.push({ maskChar: c, separator: true, caseMode })
            escape = false
        } else if(c === '<') caseMode = LOWER
        else if(c === '>') caseMode = UPPER
        else if(c === '!') caseMode = NO_CASE
        else if(c === '\\') escape = true
        else if(c !== '{' && c !== '}' && c !== '[' && c !== ']') data.push({ maskChar: c, separator: !MASK_CHARS.has(c), caseMode })
    }
    return { data, blank, maxLength: data.length }
}

const isLetter = (c) => /\p{L}/u.test(c)
const isNumber = (c) => /\p{N}/u.test(c)
const isPrint = (c) => !/[\p{Cc}\p{Cf}\p{Cs}\p{Co}\p{Cn}]/u.test(c)
const isHex = (c) => isNumber(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')

function isValidInput(key, maskChar, blank){
    switch(maskChar){
    case 'A': return isLetter(key)
    case 'a': return isLetter(key) || key === blank
    case 'N': return isLetter(key) || isNumber(key)
    case 'n': return isLetter(key) || isNumber(key) || key === blank
    case 'X': return isPrint(key) && key !== blank
    case 'x': return isPrint(key) || key === blank
    case '9': return isNumber(key)
    case '0': return isNumber(key) || key === blank
    case 'D': return isNumber(key) && Number(key) > 0
    case 'd': return (isNumber(key) && Number(key) > 0) || key === blank
    case '#': return isNumber(key) || key === '+' || key === '-' || key === blank
    case 'B': return key === '0' || key === '1'
    case 'b': return key === '0' || key === '1' || key === blank
    case 'H': return isHex(key)
    case 'h': return isHex(key) || key === blank
    }
    return false
}

function applyCase(c, mode){
    return mode === UPPER ? c.toUpperCase() : mode === LOWER ? c.toLowerCase() : c
}

function findInMask(mask, pos, forward, findSeparator, searchChar){
    if(pos >= mask.maxLength || pos < 0) return -1
    let end = forward ? mask.maxLength : -1
    let step = forward ? 1 : -1
    for(let i = pos; i !== end; i += step){
        let d = mask.data[i]
        if(findSeparator){
            if(d.separator && d.maskChar === searchChar) return i
        } else if(!d.separator){
            if(searchChar === null || searchChar === undefined) return i
            if(isValidInput(searchChar, d.maskChar, mask.blank)) return i
        }
    }
    return -1
}

function clearString(mask, pos, len){
    if(pos >= mask.maxLength) return ''
    let s = ''
    let end = Math.min(mask.maxLength, pos + len)
    for(let i = pos; i < end; i++) s += mask.data[i].separator ? mask.data[i].maskChar : mask.blank
    return s
}

// str laid over the mask from pos; clear: a separator met takes blanks (else the current content)
function maskString(mask, pos, str, clear, current){
    if(pos >= mask.maxLength) return ''
    let fill = clear ? clearString(mask, 0, mask.maxLength) : current
    let s = ''
    let strIndex = 0
    let i = pos
    while(i < mask.maxLength && strIndex < str.length){
        let d = mask.data[i]
        let c = str[strIndex]
        if(d.separator){
            s += d.maskChar
            if(c === d.maskChar) strIndex++
            ++i
            continue
        }
        if(isValidInput(c, d.maskChar, mask.blank)){
            s += applyCase(c, d.caseMode)
            ++i
        } else {
            // a separator first, else the next blank this character fits
            let n = findInMask(mask, i, true, true, c)
            if(n !== -1){
                if(str.length !== 1 || i === 0 || !mask.data[i - 1].separator || mask.data[i - 1].maskChar !== c){
                    s += fill.slice(i, n + 1)
                    i = n + 1
                }
            } else {
                n = findInMask(mask, i, true, false, c)
                if(n !== -1){
                    s += fill.slice(i, n) + applyCase(c, mask.data[n].caseMode)
                    i = n + 1
                }
            }
        }
        ++strIndex
    }
    return s
}

// the text without blanks, separators kept: "127.0__.0__.1__" is "127.0.0.1"
function stripString(mask, str){
    let s = ''
    let end = Math.min(mask.maxLength, str.length)
    for(let i = 0; i < end; i++){
        if(mask.data[i].separator) s += mask.data[i].maskChar
        else if(str[i] !== mask.blank) s += str[i]
    }
    return s
}

function maskAcceptable(mask, str){
    if(str.length !== mask.maxLength) return false
    for(let i = 0; i < mask.maxLength; i++){
        let d = mask.data[i]
        if(d.separator ? str[i] !== d.maskChar : !isValidInput(str[i], d.maskChar, mask.blank)) return false
    }
    return true
}

function nextMaskBlank(mask, pos){
    let c = findInMask(mask, pos, true, false, null)
    return c !== -1 ? c : mask.maxLength
}

function prevMaskBlank(mask, pos){
    let c = findInMask(mask, pos, false, false, null)
    return c !== -1 ? c : 0
}

// ---- words (QTextBoundaryFinder::Word, QTextLayout::SkipWords) ----

const isWord = (c) => c !== undefined && /[\p{L}\p{N}\p{M}_]/u.test(c)
const isSpace = (c) => c !== undefined && /\s/.test(c)
const kind = (c) => isWord(c) ? 1 : isSpace(c) ? 2 : 3

function isBoundary(text, p){
    return p <= 0 || p >= text.length || kind(text[p - 1]) !== kind(text[p]) || kind(text[p]) === 3
}

function previousBoundary(text, p){
    if(p <= 0) return 0
    p--
    while(p > 0 && !isBoundary(text, p)) p--
    return p
}

function nextBoundary(text, p){
    if(p >= text.length) return text.length
    p++
    while(p < text.length && !isBoundary(text, p)) p++
    return p
}

// QTextLayout::previousCursorPosition(SkipWords): over the spaces, then the word before
function previousWordStart(text, p){
    while(p > 0 && isSpace(text[p - 1])) p--
    if(p > 0 && isWord(text[p - 1])){
        while(p > 0 && isWord(text[p - 1])) p--
    } else if(p > 0) p--
    return p
}

// QTextLayout::nextCursorPosition(SkipWords): over the word, then the spaces after it
function nextWordStart(text, p){
    if(p < text.length && isWord(text[p])){
        while(p < text.length && isWord(text[p])) p++
    } else if(p < text.length && !isSpace(text[p])) p++
    while(p < text.length && isSpace(text[p])) p++
    return p
}

const RTL = /[\p{Script=Hebrew}\p{Script=Arabic}\p{Script=Syriac}\p{Script=Thaana}\p{Script=Nko}\p{Script=Samaritan}\p{Script=Mandaic}]/u

// ---- mouse ----

// A mouse selection in a single-line field, as QQuickTextInput::mousePressEvent and mouseMoveEvent: the press puts
// the cursor at the x of the mouse, every move selects from there to the x of the mouse (positionAt, clamped to the
// text) wherever the mouse is. The field does it itself, at once: the browser snaps a selection dragged out of the
// field to whichever end the point maps to in document order, and correcting it afterwards painted the two in turn.
let selectionDrag = null
// a press in a field that does not select by mouse: the browser's selection collapses while the button is down
let pressedNoSelect = null

function dragSelectionMove(e){
    let field = selectionDrag
    if(!field || !(e.buttons & 1)) return
    engine.batch(() => field.$dragSelectTo(e.clientX))
}

function listenMouse(){
    if(listenMouse.done) return
    listenMouse.done = true
    window.addEventListener('mousemove', dragSelectionMove, true)
    window.addEventListener('mouseup', () => {
        selectionDrag = null
        pressedNoSelect = null
    }, true)
}

// ---- keys ----

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

// QKeySequence::Undo / Redo (Command on macOS)
function undoKey(event){
    let mods = event.modifiers & ~0x20000000
    let command = mods & (QtEnums.ControlModifier | QtEnums.MetaModifier)
    if(!command || (mods & ~(QtEnums.ControlModifier | QtEnums.MetaModifier | QtEnums.ShiftModifier))) return 0
    if(event.key === QtEnums.Key_Z) return mods & QtEnums.ShiftModifier ? 2 : 1
    if(event.key === QtEnums.Key_Y && !(mods & QtEnums.ShiftModifier)) return 2
    return 0
}

// Qt::InputMethodHints as the attributes of the control
const IMH_HIDDEN = 0x1, IMH_SENSITIVE = 0x2, IMH_NO_AUTO_UPPERCASE = 0x4, IMH_PREFER_NUMBERS = 0x8, IMH_PREFER_UPPERCASE = 0x10,
    IMH_NO_PREDICTIVE = 0x40, IMH_DIGITS = 0x10000, IMH_NUMBERS = 0x20000, IMH_UPPERCASE = 0x40000, IMH_DIALABLE = 0x100000,
    IMH_EMAIL = 0x200000, IMH_URL = 0x400000

function setAttribute(element, name, value){
    if(value === null){
        if(element.hasAttribute(name)) element.removeAttribute(name)
    } else if(element.getAttribute(name) !== value) element.setAttribute(name, value)
}

function applyInputMethodHints(input, hints){
    let mode = hints & IMH_DIGITS || hints & IMH_PREFER_NUMBERS ? 'numeric' : hints & IMH_NUMBERS ? 'decimal'
        : hints & IMH_DIALABLE ? 'tel' : hints & IMH_EMAIL ? 'email' : hints & IMH_URL ? 'url' : null
    setAttribute(input, 'inputmode', mode)
    setAttribute(input, 'autocapitalize', hints & IMH_NO_AUTO_UPPERCASE ? 'off' : hints & (IMH_UPPERCASE | IMH_PREFER_UPPERCASE) ? 'characters' : null)
    let noPrediction = hints & (IMH_NO_PREDICTIVE | IMH_SENSITIVE | IMH_HIDDEN)
    setAttribute(input, 'autocomplete', noPrediction ? 'off' : null)
    setAttribute(input, 'autocorrect', noPrediction ? 'off' : null)
    setAttribute(input, 'spellcheck', noPrediction ? 'false' : null)
}

const PADDINGS = ['LEFTPADDING', 'RIGHTPADDING', 'TOPPADDING', 'BOTTOMPADDING']

class TextField extends Item {
    // QQuickTextInputPrivate::init, QQuickTextEditPrivate::init: setAcceptedMouseButtons(Qt::LeftButton). The field
    // takes the press in the scene (PointerDelivery), so an item below it does not get it; its native control edits
    get $acceptsMouse(){
        return true
    }

    // the focus of a press (QQuickTextInput::mousePressEvent: activeFocusOnPress)
    $pressFocus(){
        if(this.$v[this.constructor.ACTIVEFOCUSONPRESS] && !this.$v[Item.ACTIVEFOCUS]) this.forceActiveFocus(require('./KeyDelivery').MouseFocusReason)
    }

    // QQuickTextInput::keyPressEvent where there is no native control to edit (a page without a document: the tests in
    // node): the characters typed replace the selection, Backspace / Delete, the arrows, Home / End, Return accepts
    $editWithoutNative(event, kind){
        if(this.$input || kind !== 'pressed') return 0
        let C = this.constructor
        let K = require('../Qt/enums')
        let content = this.$content()
        let cursor = this.$v[C.CURSORPOSITION]
        let start = this.$v[C.SELECTIONSTART], end = this.$v[C.SELECTIONEND]
        let readOnly = this.$v[C.READONLY]
        let text = event.text || ''
        let ctrl = event.modifiers & (K.ControlModifier | K.MetaModifier)
        if((event.key === K.Key_Return || event.key === K.Key_Enter) && this.$multiline()){
            if(readOnly) return 0
            if(start !== end) this.remove(start, end)
            this.insert(this.$v[C.CURSORPOSITION], '\n')
            return 1
        }
        if(event.key === K.Key_Backspace || event.key === K.Key_Delete){
            if(readOnly) return 1
            if(start !== end) this.remove(start, end)
            else if(event.key === K.Key_Backspace && cursor > 0) this.remove(cursor - 1, cursor)
            else if(event.key === K.Key_Delete && cursor < content.length) this.remove(cursor, cursor + 1)
            return 1
        }
        if(event.key === K.Key_Left || event.key === K.Key_Right || event.key === K.Key_Home || event.key === K.Key_End){
            let to = event.key === K.Key_Home ? 0 : event.key === K.Key_End ? content.length : cursor + (event.key === K.Key_Left ? -1 : 1)
            to = Math.max(0, Math.min(content.length, to))
            if(event.modifiers & K.ShiftModifier) this.moveCursorSelection(to)
            else {
                this.deselect()
                this.cursorPosition = to
            }
            return 1
        }
        if(text && !ctrl && text >= ' ' && text !== '\x7f' && !readOnly){
            if(start !== end) this.remove(start, end)
            this.insert(this.$v[C.CURSORPOSITION], text)
            engine.emit(this, C.SIGNAL_textEdited)
            return 1
        }
        return 0
    }

    constructor(parent = null, multiline = false){
        super(parent)
        this.$input = null
        this.$syncing = false
        this.$editing = false
        this.$selecting = false
        this.$mask = null
        this.$maskText = ''
        this.$paddingSet = 0
        this.$hs = 0
        this.$undoStack = []
        this.$redoStack = []
        this.$lastEdit = null
        this.$lastEditCursor = -1
        this.$pending = null
        // cursorDelegate (QQuickTextUtil::setCursorDelegate / createCursor): made once the cursor first shows
        this.$cursorItem = null
        this.$cursorPending = false
        engine.scheduleComponentComplete(this)
        // QQuickTextInput::focusOutEvent; QQuickTextInputPrivate::handleFocusEvent: the cursor shows with the focus
        // (QQuickTextEditPrivate::handleFocusEvent the same): the cursor shows with the focus unless read-only; losing it,
        // the password echo ends, the selection goes unless persistentSelection or the window lost the focus
        // (Qt::ActiveWindowFocusReason), then editingFinished
        engine.connect(this, Item.SIGNAL_activeFocusChanged, (old, active) => {
            let C = this.constructor
            if(!this.$v[C.READONLY]) this.$write(C.CURSORVISIBLE, !!active)
            if(C.ECHOMODE !== undefined && this.$v[C.ECHOMODE] === PASSWORD_ECHO_ON_EDIT){
                this.$updateDisplayText()
                engine.markDirty(this, TEXT)
            }
            if(active) return
            if(!keyAgent.windowFocusReason && !this.$v[C.PERSISTENTSELECTION]
                && this.$v[C.SELECTIONSTART] !== this.$v[C.SELECTIONEND]) this.deselect()
            if(this.$acceptOrFixup()) engine.emitArgs(this, C.SIGNAL_editingFinished, [])
        })
        if(this.$dom){
            let input = document.createElement(multiline ? 'textarea' : 'input')
            input.className = 'jq2-input'
            // the cursor is the scene's (the topmost item with a cursor, QQuickWindowPrivate::findCursorItemAndHandler):
            // the control takes the DOM events even where an item covers it, its own text cursor would show through
            input.style.cssText = 'position:absolute;left:0;top:0;width:100%;height:100%;margin:0;padding:0;border:0;outline:none;background:transparent;resize:none;pointer-events:auto;font:inherit;color:inherit;cursor:inherit;box-sizing:border-box;line-height:normal;'
            this.$dom.appendChild(input)
            this.$input = input
            input.$item = this
            input.addEventListener('beforeinput', (event) => engine.batch(() => this.$beforeInput(event)))
            input.addEventListener('input', () => engine.batch(() => this.$nativeInput()))
            // a click or Tab into the input: the field takes active focus (activeFocusOnPress); the input itself
            // is focused and blurred by KeyDelivery as the active focus moves
            input.addEventListener('focus', () => {
                if(!isSyncingDom() && !(this.$f & engine.DESTROYED)) this.forceActiveFocus()
            })
            input.addEventListener('select', () => engine.batch(() => this.$syncSelection()))
            // the caret moved by the keys or a click, nothing selected: no select event
            input.addEventListener('selectionchange', () => engine.batch(() => this.$syncSelection()))
            // the scroll of the control, kept here: reading it from the DOM would lay the page out
            input.addEventListener('scroll', () => engine.batch(() => {
                this.$hs = input.scrollLeft
                this.$vs = input.scrollTop
                this.$updateCursorRectangle()
            }))
            listenMouse()
            input.addEventListener('mousedown', (event) => this.$mouseDown(event))
        }
        this.$updateImplicitSize()
    }

    // QQuickTextInput::mousePressEvent: no focus by a press unless activeFocusOnPress; no selection by the mouse
    // unless selectByMouse (a double click does not select a word, a drag does not select)
    $mouseDown(event){
        selectionDrag = null
        if(event.button !== 0) return
        let C = this.constructor
        if(!this.$v[C.ACTIVEFOCUSONPRESS] && !this.$v[Item.ACTIVEFOCUS]){
            event.preventDefault()
            return
        }
        if(!this.$v[C.SELECTBYMOUSE]){
            pressedNoSelect = this
            if(event.detail > 1) event.preventDefault()
            return
        }
        if(this.$input.tagName === 'TEXTAREA' || event.detail !== 1) return
        // the browser neither focuses nor selects: QQuickTextInput::mousePressEvent takes the focus, then moves the
        // cursor there, keeping the anchor with Shift (moveCursor(pos, mark))
        event.preventDefault()
        let input = this.$input
        if(typeof document === 'undefined' || document.activeElement !== input){
            if(input.focus) input.focus({ preventScroll: true })
            if(!this.$v[Item.ACTIVEFOCUS]) this.forceActiveFocus()
        }
        let position = this.$positionAtClientX(event.clientX)
        engine.batch(() => {
            if(event.shiftKey) this.moveCursorSelection(position, 0)
            else this.$applySelection(position, position, true)
        })
        selectionDrag = this
    }

    // the position at the x of the mouse (in the viewport), on the middle of the line: a single-line field
    $positionAtClientX(clientX){
        let rect = this.$input.getBoundingClientRect()
        return this.$positionAtPoint(clientX - rect.left, (rect.bottom - rect.top) / 2, false)
    }

    // QQuickTextInput::mouseMoveEvent: moveCursorSelection(positionAt(mouse), mouseSelectionMode)
    $dragSelectTo(clientX){
        if(this.$f & engine.DESTROYED || !this.$input) return
        let C = this.constructor
        this.moveCursorSelection(this.$positionAtClientX(clientX), this.$v[C.MOUSESELECTIONMODE] || 0)
        // QQuickTextInputPrivate::updateHorizontalScroll: the text scrolls just enough to show the cursor
        let x = this.$caretRect(this.$v[C.CURSORPOSITION]).x
        let left = this.$padding('LEFTPADDING'), right = this.$v[Item.WIDTH] - this.$padding('RIGHTPADDING') - 1
        let delta = x > right ? x - right : x < left ? x - left : 0
        if(delta === 0) return
        let input = this.$input
        input.scrollLeft = Math.max(0, (this.$hs || 0) + delta)
        this.$hs = input.scrollLeft || 0
        this.$updateCursorRectangle()
    }

    // the text the control shows: with an input mask its separators and blanks (QQuickTextInputPrivate::m_text)
    $content(){
        return this.$mask ? this.$maskText : this.$v[this.constructor.TEXT]
    }

    // the text as it is laid out: hidden by the echo mode, capitalized by the font
    $layoutText(){
        let content = this.$content()
        let echo = this.constructor.ECHOMODE !== undefined ? this.$v[this.constructor.ECHOMODE] : NORMAL
        if(echo === NO_ECHO) return ''
        if(echo === PASSWORD || (echo === PASSWORD_ECHO_ON_EDIT && !this.$v[Item.ACTIVEFOCUS])){
            let c = this.$v[this.constructor.PASSWORDCHARACTER] || '●'
            return c[0].repeat(content.length)
        }
        return capitalize(content, fontOf(this).capitalization)
    }

    $padding(name){
        return this.$v[this.constructor[name]] || 0
    }

    $write(id, value){
        let C = this.constructor
        if(id === C.TEXT && !this.$editing) return this.$setText(value)
        if(id === C.CURSORPOSITION && !this.$selecting) return this.$moveCursorTo(value)
        if(id === C.PADDING){
            super.$write(id, value)
            for(let i = 0; i < PADDINGS.length; i++) if(!(this.$paddingSet & (1 << i))) super.$write(C[PADDINGS[i]], value)
            return
        }
        // QQuickTextInputPrivate::setLeftPadding...: a side set explicitly no longer follows padding
        if(id >= C.LEFTPADDING && id <= C.BOTTOMPADDING){
            for(let i = 0; i < PADDINGS.length; i++) if(C[PADDINGS[i]] === id) this.$paddingSet |= 1 << i
        }
        super.$write(id, value)
        if(id === Item.WIDTH || id === Item.HEIGHT){
            engine.markDirty(this, TEXT)
            this.$updateCursorRectangle()
        }
    }

    // QQuickTextInput::setText (internalSetText): the undo history is cleared, the cursor goes to the end; with an
    // input mask the text is laid over it. QQuickTextEdit::setText: the cursor goes to the start
    $setText(value){
        let C = this.constructor
        value = value === undefined || value === null ? '' : String(value)
        let content = this.$normalizeText(value)
        let old = this.$content()
        let oldText = this.$v[C.TEXT]
        this.$undoStack = []
        this.$redoStack = []
        this.$lastEdit = null
        if(this.$mask) this.$maskText = content
        let text = this.$mask ? stripString(this.$mask, content) : content
        this.$editing = true
        try {
            super.$write(C.TEXT, text)
        } finally {
            this.$editing = false
        }
        if(content !== old && text === oldText) this.$textChanged()
        this.$updateUndoFlags()
        if(content !== old){
            let cursor = this.$cursorAfterSetText(content)
            this.$applySelection(cursor, cursor, true)
        }
    }

    $normalizeText(value){
        return value
    }

    $cursorAfterSetText(content){
        return content.length
    }

    // QQuickTextInput::setCursorPosition: a position outside the text is ignored; with an input mask the cursor
    // goes to the blank in the direction it moves
    $moveCursorTo(value){
        let content = this.$content()
        value = Math.trunc(Number(value))
        if(!(value >= 0 && value <= content.length)) return
        let cursor = this.$v[this.constructor.CURSORPOSITION]
        if(this.$mask && value !== cursor) value = value > cursor ? nextMaskBlank(this.$mask, value) : prevMaskBlank(this.$mask, value)
        this.$applySelection(value, value, true)
    }

    // the selection from anchor to cursor, in the properties and in the control
    $applySelection(anchor, cursor, toDom){
        let C = this.constructor
        let content = this.$content()
        anchor = Math.max(0, Math.min(anchor, content.length))
        cursor = Math.max(0, Math.min(cursor, content.length))
        let start = Math.min(anchor, cursor), end = Math.max(anchor, cursor)
        let input = this.$input
        if(toDom && input){
            if(input.value !== content) input.value = content
            let direction = cursor < anchor ? 'backward' : 'forward'
            if(input.selectionStart !== start || input.selectionEnd !== end || (start !== end && input.selectionDirection !== direction)) input.setSelectionRange(start, end, direction)
        }
        this.$selecting = true
        try {
            this.$write(C.SELECTIONSTART, start)
            this.$write(C.SELECTIONEND, end)
            this.$write(C.CURSORPOSITION, cursor)
            this.$write(C.SELECTEDTEXT, content.slice(start, end))
        } finally {
            this.$selecting = false
        }
        // a cursor moved away from the last edit separates the undo steps (QQuickTextInputPrivate::separate)
        if(!this.$editing && cursor !== this.$lastEditCursor) this.$lastEdit = null
        this.$updateCursorRectangle()
    }

    $syncSelection(){
        let input = this.$input
        if(!input) return
        let start = input.selectionStart || 0
        let end = input.selectionEnd || 0
        if(pressedNoSelect === this && start !== end){
            let caret = input.selectionDirection === 'backward' ? start : end
            input.setSelectionRange(caret, caret)
            start = end = caret
        }
        let backward = input.selectionDirection === 'backward' && start !== end
        this.$applySelection(backward ? end : start, backward ? start : end, false)
    }

    $anchor(){
        let C = this.constructor
        let start = this.$v[C.SELECTIONSTART], end = this.$v[C.SELECTIONEND]
        return this.$v[C.CURSORPOSITION] === start ? end : start
    }

    $state(){
        return { content: this.$content(), anchor: this.$anchor(), cursor: this.$v[this.constructor.CURSORPOSITION] }
    }

    // ---- editing ----

    // an edit about to be made: the state before it is an undo step, unless the edit continues the last one
    // (typing or deleting on from where the last edit left the cursor)
    $recordUndo(kind, state){
        state = state || this.$state()
        let continues = kind !== null && kind === this.$lastEdit && state.anchor === state.cursor && state.cursor === this.$lastEditCursor
        if(!continues) this.$undoStack.push(state)
        this.$redoStack = []
        this.$lastEdit = kind
    }

    $updateUndoFlags(){
        let C = this.constructor
        if(C.CANUNDO === undefined) return
        this.$write(C.CANUNDO, this.$undoStack.length > 0)
        this.$write(C.CANREDO, this.$redoStack.length > 0)
    }

    // the new content and selection of an edit; the text binding is broken as by typing; textEdited unless the
    // edit is a programmatic clear (QQuickTextInputPrivate::clear: finishChange(..., edited = false))
    $commitEdit(content, anchor, cursor, edited = true){
        let C = this.constructor
        let changed = content !== this.$content()
        if(changed){
            let text = this.$mask ? stripString(this.$mask, content) : content
            let oldText = this.$v[C.TEXT]
            this.$editing = true
            try {
                if(this.$mask) this.$maskText = content
                let binding = this.$takeBinding(C.TEXT)
                if(binding) engine.disposeBinding(binding)
                this.$write(C.TEXT, text)
                if(text === oldText) this.$textChanged()
                this.$applySelection(anchor, cursor, true)
            } finally {
                this.$editing = false
            }
        } else this.$applySelection(anchor, cursor, true)
        this.$lastEditCursor = this.$v[C.CURSORPOSITION]
        this.$updateUndoFlags()
        if(changed && edited) engine.emitArgs(this, C.SIGNAL_textEdited, [])
        return changed
    }

    // [start, end) replaced by inserted as QQuickTextInputPrivate edits: with an input mask the range is cleared to
    // blanks and inserted is laid over the mask from start, the cursor goes to the next blank; without one the
    // maximum length holds
    $spliced(start, end, inserted){
        let content = this.$content()
        let mask = this.$mask
        if(mask){
            content = content.slice(0, start) + clearString(mask, start, end - start) + content.slice(end)
            if(!inserted) return { content, cursor: start }
            let ms = maskString(mask, start, inserted, false, content)
            return { content: content.slice(0, start) + ms + content.slice(start + ms.length), cursor: nextMaskBlank(mask, start + ms.length) }
        }
        let max = this.constructor.MAXIMUMLENGTH !== undefined ? this.$v[this.constructor.MAXIMUMLENGTH] : Infinity
        inserted = inserted.slice(0, Math.max(0, max - (content.length - (end - start))))
        return { content: content.slice(0, start) + inserted + content.slice(end), cursor: start + inserted.length }
    }

    // an edit by the user (typing, deleting, cut, paste): an Invalid result is undone (finishChange)
    $userEdit(start, end, inserted, kind){
        if(this.$v[this.constructor.READONLY]) return false
        let content = this.$content()
        start = Math.max(0, Math.min(start, content.length))
        end = Math.max(start, Math.min(end, content.length))
        let result = this.$spliced(start, end, inserted)
        if(result.content !== content && this.$validateText && this.$validateText(result.content) === INVALID) return false
        if(result.content !== content) this.$recordUndo(kind)
        return this.$commitEdit(result.content, result.cursor, result.cursor)
    }

    // the browser is about to edit: with an input mask or in overwrite mode the edit is the field's, else the state
    // before it is kept for the undo history
    $beforeInput(event){
        let type = event.inputType || ''
        if(type === 'historyUndo' || type === 'historyRedo'){
            event.preventDefault()
            if(type === 'historyUndo') this.undo()
            else this.redo()
            return
        }
        let input = this.$input
        let start = input.selectionStart || 0, end = input.selectionEnd || 0
        let overwrite = this.$v[this.constructor.OVERWRITEMODE] && type === 'insertText' && start === end
        if((this.$mask || overwrite) && !event.isComposing && type !== 'insertCompositionText'){
            event.preventDefault()
            if(type.startsWith('insert')) this.$typed(start, end, this.$insertedData(event, type), type === 'insertText' ? 'insert' : null)
            else if(type.startsWith('delete')) this.$deleted(start, end, type)
            return
        }
        this.$pending = { state: this.$state(), kind: type === 'insertText' ? 'insert' : type === 'deleteContentBackward' ? 'backspace' : type === 'deleteContentForward' ? 'delete' : null }
    }

    $insertedData(event, type){
        if(type === 'insertLineBreak' || type === 'insertParagraph') return this.$input.tagName === 'TEXTAREA' ? '\n' : ''
        if(typeof event.data === 'string') return event.data
        return event.dataTransfer ? event.dataTransfer.getData('text/plain') : ''
    }

    // QQuickTextInputPrivate::insert; in overwrite mode the characters after the cursor are replaced (not past
    // the end of the line)
    $typed(start, end, data, kind){
        if(!data) return
        if(start === end && this.$v[this.constructor.OVERWRITEMODE] && !this.$mask){
            let content = this.$content()
            let stop = content.indexOf('\n', start)
            end = Math.min(stop < 0 ? content.length : stop, start + data.length)
        }
        this.$userEdit(start, end, data, kind)
    }

    // QQuickTextInputPrivate::backspace, del, deleteStartOfWord, deleteEndOfWord, deleteEndOfLine
    $deleted(start, end, type){
        if(start !== end) return this.$userEdit(start, end, '', null)
        let content = this.$content()
        if(type === 'deleteContentBackward'){
            if(start === 0) return false
            let p = this.$mask ? prevMaskBlank(this.$mask, start - 1) : start - 1
            return this.$userEdit(p, p + 1, '', 'backspace')
        }
        if(type === 'deleteContentForward') return start < content.length ? this.$userEdit(start, start + 1, '', 'delete') : false
        if(type === 'deleteWordBackward') return this.$userEdit(previousWordStart(content, start), start, '', null)
        if(type === 'deleteWordForward') return this.$userEdit(start, nextBoundary(content, start), '', null)
        if(type.endsWith('Backward')) return this.$userEdit(0, start, '', null)
        if(type.endsWith('Forward')) return this.$userEdit(start, content.length, '', null)
        return false
    }

    // the browser edited the control (no input mask, no overwrite)
    $nativeInput(){
        let C = this.constructor
        let input = this.$input
        let pending = this.$pending
        this.$pending = null
        if(this.$mask){
            // an edit the field could not take over (input method composition): laid over the mask as typed
            let content = maskString(this.$mask, 0, input.value, true, null)
            content += clearString(this.$mask, content.length, this.$mask.maxLength - content.length)
            let cursor = Math.min(input.selectionEnd || 0, content.length)
            this.$recordUndo(null, pending && pending.state)
            this.$commitEdit(content, cursor, cursor)
            return
        }
        // QQuickTextInputPrivate::finishChange: an Invalid edit is undone
        if(this.$validateText && this.$validateText(input.value) === INVALID){
            let previous = this.$v[C.TEXT]
            let caret = Math.max(0, Math.min(previous.length, this.$v[C.CURSORPOSITION]))
            input.value = previous
            input.setSelectionRange(caret, caret)
            return
        }
        if(input.value === this.$v[C.TEXT]) return
        this.$recordUndo(pending ? pending.kind : null, pending && pending.state)
        this.$syncing = true
        this.$editing = true
        try {
            let binding = this.$takeBinding(C.TEXT)
            if(binding) engine.disposeBinding(binding)
            this.$write(C.TEXT, input.value)
            this.$syncSelection()
        } finally {
            this.$editing = false
            this.$syncing = false
        }
        this.$lastEditCursor = this.$v[C.CURSORPOSITION]
        this.$updateUndoFlags()
        engine.emitArgs(this, C.SIGNAL_textEdited, [])
    }

    // hasAcceptableInput || fixup(): the validator may turn the text acceptable on Return / focus out
    $acceptOrFixup(){
        if(!this.$validateText) return true
        let text = this.$content()
        if(this.$validateText(text) === ACCEPTABLE) return !this.$mask || maskAcceptable(this.$mask, this.$maskText)
        let validator = this.$v[this.constructor.VALIDATOR]
        if(!validator || !validator.$fixup) return false
        let fixed = validator.$fixup(text)
        if(this.$validateText(fixed) !== ACCEPTABLE) return false
        if(fixed !== text) this.$write(this.constructor.TEXT, fixed)
        return true
    }

    $textChanged(){
        let C = this.constructor
        if(C.LENGTH !== undefined) this.$write(C.LENGTH, this.$content().length)
        if(this.$updateAcceptable) this.$updateAcceptable()
        this.$updateDisplayText()
        this.$updateImplicitSize()
    }

    // QQuickTextInputPrivate::updateDisplayText
    $updateDisplayText(){
        let C = this.constructor
        if(C.DISPLAYTEXT === undefined) return
        let echo = this.$v[C.ECHOMODE]
        let content = this.$content()
        let display = content
        if(echo === NO_ECHO) display = ''
        else if(echo === PASSWORD || (echo === PASSWORD_ECHO_ON_EDIT && !this.$v[Item.ACTIVEFOCUS])) display = (this.$v[C.PASSWORDCHARACTER] || '●')[0].repeat(content.length)
        this.$write(C.DISPLAYTEXT, display)
    }

    // ---- layout ----

    // the lines of the text: their start in the content, their text; a TextEdit with wrapMode and a set width
    // wraps to the width inside the padding
    $layout(){
        let font = fontOf(this)
        let C = this.constructor
        let text = String(this.$layoutText())
        let wrap = C.WRAPMODE !== undefined ? this.$v[C.WRAPMODE] : 0
        let available = Math.max(0, this.$v[Item.WIDTH] - this.$padding('LEFTPADDING') - this.$padding('RIGHTPADDING'))
        // the cursor moves far more often than the text, the font or the width change: the lines are kept for them
        let key = text + '\u0000' + font.family + '\u0000' + font.pixelSize + '\u0000' + font.weight + '\u0000' + font.italic + '\u0000' + font.letterSpacing
            + '\u0000' + wrap + '\u0000' + available + '\u0000' + (this.$explicit & 1)
        if(this.$layoutCache && this.$layoutCache.key === key) return this.$layoutCache.layout
        let paragraphs = text.split('\n')
        let lines = []
        let offset = 0
        for(let paragraph of paragraphs){
            let pieces = [paragraph]
            if(wrap && (this.$explicit & 1)){
                pieces = []
                wrapParagraph(paragraph, wrap, available, font, pieces)
            }
            let at = 0
            for(let piece of pieces){
                let found = paragraph.indexOf(piece, at)
                if(found < 0) found = at
                lines.push({ start: offset + found, text: piece, paragraphEnd: offset + paragraph.length })
                at = found + piece.length
            }
            offset += paragraph.length + 1
        }
        let lineHeight = measure('', font).height
        let layout = { font, lines, paragraphs, lineHeight }
        this.$layoutCache = { key, layout }
        return layout
    }

    // implicit width is the unwrapped text; a TextEdit with wrapMode and a set width wraps its content to it;
    // the padding is part of the implicit size
    $updateImplicitSize(){
        let layout = this.$layout()
        let font = layout.font
        let implicitWidth = 0
        for(let paragraph of layout.paragraphs) implicitWidth = Math.max(implicitWidth, measure(paragraph, font).width)
        let width = 0, height = 0
        for(let line of layout.lines){
            let size = measure(line.text, font)
            width = Math.max(width, size.width)
            height += size.height
        }
        let C = this.constructor
        engine.markDirty(this, TEXT)
        this.$write(C.CONTENTWIDTH, width)
        this.$write(C.CONTENTHEIGHT, height)
        if(C.PAINTEDWIDTH !== undefined){
            this.$write(C.PAINTEDWIDTH, width)
            this.$write(C.PAINTEDHEIGHT, height)
        }
        if(C.LINECOUNT !== undefined) this.$write(C.LINECOUNT, layout.lines.length)
        this.$write(Item.IMPLICITWIDTH, implicitWidth + this.$padding('LEFTPADDING') + this.$padding('RIGHTPADDING'))
        this.$write(Item.IMPLICITHEIGHT, height + this.$padding('TOPPADDING') + this.$padding('BOTTOMPADDING'))
        this.$updateCursorRectangle()
    }

    // the offset of the text in the area inside the padding (vscroll of QQuickTextInputPrivate::updateVerticalScroll,
    // yoff of QQuickTextEdit): a TextInput taller than its area scrolls instead
    $textOffsetY(contentHeight){
        let available = Math.max(0, this.$v[Item.HEIGHT] - this.$padding('TOPPADDING') - this.$padding('BOTTOMPADDING'))
        let y = alignedY(contentHeight, available, this.$v[this.constructor.VERTICALALIGNMENT])
        return this.$multiline() ? y : Math.max(0, y)
    }

    $multiline(){
        return this.constructor.WRAPMODE !== undefined
    }

    $scroll(){
        return { x: this.$hs || 0, y: this.$vs || 0 }
    }

    $lineIndexOf(lines, pos){
        let index = 0
        for(let i = 0; i < lines.length; i++) if(lines[i].start <= pos) index = i
        return index
    }

    // the rectangle of the cursor at pos in the item (QQuickTextInput::cursorRectangle, QQuickTextEdit::positionToRectangle)
    $caretRect(pos){
        let C = this.constructor
        let layout = this.$layout()
        let font = layout.font
        let lines = layout.lines
        if(C.ECHOMODE !== undefined && this.$v[C.ECHOMODE] === NO_ECHO) pos = 0
        let index = this.$lineIndexOf(lines, pos)
        let line = lines[index]
        let left = this.$padding('LEFTPADDING'), top = this.$padding('TOPPADDING')
        let available = Math.max(0, this.$v[Item.WIDTH] - left - this.$padding('RIGHTPADDING'))
        let column = Math.max(0, Math.min(pos - line.start, line.text.length))
        let lineX = Math.max(0, alignedX(measure(line.text, font).advance, available, this.$v[C.HORIZONTALALIGNMENT]))
        let scroll = this.$scroll()
        let x = left + lineX + measure(line.text.slice(0, column), font).advance - scroll.x
        let y = top + this.$textOffsetY(lines.length * layout.lineHeight) + index * layout.lineHeight - scroll.y
        let width = 1
        if(this.$v[C.OVERWRITEMODE]) width = column < line.text.length ? measure(line.text.slice(0, column + 1), font).advance - measure(line.text.slice(0, column), font).advance : measure(' ', font).advance
        return new QRectF(x, y, width, layout.lineHeight)
    }

    $updateCursorRectangle(){
        let C = this.constructor
        if(C.CURSORRECTANGLE === undefined || C.CURSORPOSITION === undefined) return
        let rect = this.$caretRect(this.$v[C.CURSORPOSITION] || 0)
        let old = this.$v[C.CURSORRECTANGLE]
        if(!(old && old.x === rect.x && old.y === rect.y && old.width === rect.width && old.height === rect.height)) this.$write(C.CURSORRECTANGLE, rect)
        // QQuickTextInput::updateCursorRectangle: the cursor delegate follows the rectangle, its height with it
        let cursor = this.$cursorItem
        if(cursor && !(cursor.$f & engine.DESTROYED)){
            cursor.x = rect.x
            cursor.y = rect.y
            cursor.height = rect.height
        }
    }

    $componentComplete(){
        super.$componentComplete()
        // QQuickTextInput::componentComplete: the cursor delegate is made if the cursor shows already
        if(this.$v[this.constructor.CURSORVISIBLE]) this.$createCursor()
    }

    // QQuickTextUtil::setCursorDelegate: the old cursor item goes; the new one is made now if the cursor shows
    $setCursorDelegate(){
        if(this.$cursorItem) this.$cursorItem.destroy()
        this.$cursorItem = null
        this.$cursorPending = true
        if(this.$v[this.constructor.CURSORVISIBLE] && (this.$f & engine.COMPLETED)) this.$createCursor()
        engine.markDirty(this, STYLE)
    }

    // QQuickTextUtil::createCursor: an instance of the delegate as a child of the field, at the cursor rectangle
    // with its height (set before its creation completes); the field draws no cursor of its own then
    $createCursor(){
        if(!this.$cursorPending) return
        this.$cursorPending = false
        let C = this.constructor
        let component = this.$v[C.CURSORDELEGATE]
        if(component && typeof component.createObject === 'function'){
            let rect = this.$caretRect(this.$v[C.CURSORPOSITION] || 0)
            let item = component.createObject(this, { x: rect.x, y: rect.y, height: rect.height })
            if(item && item instanceof Item) this.$cursorItem = item
            else if(item){
                console.warn('JQML: ' + C.name + ' does not support loading non-visual cursor delegates.')
                item.destroy()
            }
        }
        engine.markDirty(this, STYLE)
    }

    // the position at a point of the item; onCharacter: the character under x (TextInput.CursorOnCharacter), else
    // the nearest position between characters
    $positionAtPoint(x, y, onCharacter){
        let C = this.constructor
        let layout = this.$layout()
        let lines = layout.lines
        let font = layout.font
        let scroll = this.$scroll()
        let top = this.$padding('TOPPADDING') + this.$textOffsetY(lines.length * layout.lineHeight) - scroll.y
        let index = Math.max(0, Math.min(lines.length - 1, Math.floor((y - top) / layout.lineHeight)))
        let line = lines[index]
        let left = this.$padding('LEFTPADDING')
        let available = Math.max(0, this.$v[Item.WIDTH] - left - this.$padding('RIGHTPADDING'))
        let local = x - left - Math.max(0, alignedX(measure(line.text, font).advance, available, this.$v[C.HORIZONTALALIGNMENT])) + scroll.x
        if(local <= 0) return line.start
        let previous = 0
        for(let i = 1; i <= line.text.length; i++){
            let width = measure(line.text.slice(0, i), font).advance
            if(local < width) return line.start + (onCharacter ? i - 1 : local - previous < width - local ? i - 1 : i)
            previous = width
        }
        return line.start + line.text.length
    }

    // ---- the native control ----

    // the field's DOM value and selection are read back while it is hidden
    $flush(dirty){
        this.$paint(dirty)
    }

    $paint(dirty){
        super.$paint(dirty)
        let input = this.$input
        if(!input) return
        let C = this.constructor
        let s = input.style
        if(dirty & TEXT){
            let content = this.$content()
            if(input.value !== content){
                input.value = content
                let start = this.$v[C.SELECTIONSTART], end = this.$v[C.SELECTIONEND]
                let cursor = this.$v[C.CURSORPOSITION]
                input.setSelectionRange(start, end, cursor === start && start !== end ? 'backward' : 'forward')
            }
            let font = fontOf(this)
            s.fontSize = font.pixelSize + 'px'
            s.fontFamily = familyCss(font.family)
            s.fontWeight = font.weight
            s.fontStyle = font.italic ? 'italic' : ''
            s.textDecoration = [font.underline ? 'underline' : '', font.strikeout ? 'line-through' : ''].join(' ').trim()
            s.fontVariant = font.capitalization === 3 ? 'small-caps' : ''
            s.textTransform = font.capitalization === 1 ? 'uppercase' : font.capitalization === 2 ? 'lowercase' : font.capitalization === 4 ? 'capitalize' : ''
            s.letterSpacing = font.letterSpacing ? font.letterSpacing + 'px' : ''
            s.wordSpacing = font.wordSpacing ? font.wordSpacing + 'px' : ''
            input.readOnly = !!this.$v[C.READONLY]
            if(C.ECHOMODE !== undefined){
                let echo = this.$v[C.ECHOMODE]
                let hidden = echo === PASSWORD || echo === NO_ECHO || (echo === PASSWORD_ECHO_ON_EDIT && !this.$v[Item.ACTIVEFOCUS])
                let type = hidden ? 'password' : 'text'
                if(input.type !== type) input.type = type
            }
            let max = C.MAXIMUMLENGTH !== undefined ? this.$v[C.MAXIMUMLENGTH] : undefined
            if(max !== undefined && max < 32767) input.maxLength = max
            else if(input.hasAttribute('maxlength')) input.removeAttribute('maxlength')
            // the alignment: text-align across, the padding of the control down (QQuickTextUtil::alignedY)
            let h = this.$v[C.HORIZONTALALIGNMENT]
            s.textAlign = h & ALIGN_RIGHT ? 'right' : h & ALIGN_HCENTER ? 'center' : h & ALIGN_JUSTIFY ? 'justify' : 'left'
            let lineHeight = measure('', font).height
            let lines = this.$multiline() ? Math.max(1, this.$v[C.LINECOUNT] || 1) : 1
            let top = this.$padding('TOPPADDING') + Math.max(0, this.$textOffsetY(lines * lineHeight))
            s.paddingLeft = this.$padding('LEFTPADDING') + 'px'
            s.paddingRight = this.$padding('RIGHTPADDING') + 'px'
            s.paddingTop = top + 'px'
            s.paddingBottom = (this.$multiline() ? this.$padding('BOTTOMPADDING') : Math.max(0, this.$v[Item.HEIGHT] - top - lineHeight)) + 'px'
            if(this.$multiline()){
                let wrap = this.$v[C.WRAPMODE]
                s.whiteSpace = wrap ? 'pre-wrap' : 'pre'
                s.wordBreak = wrap === 3 ? 'break-all' : 'normal'
                s.overflowWrap = wrap === 4 ? 'anywhere' : 'normal'
                input.wrap = wrap ? 'soft' : 'off'
            }
            applyInputMethodHints(input, this.$v[C.INPUTMETHODHINTS] || 0)
        }
        if(dirty & (STYLE | TEXT)){
            let color = toCss(this.$v[C.COLOR])
            let noEcho = C.ECHOMODE !== undefined && this.$v[C.ECHOMODE] === NO_ECHO
            s.color = noEcho ? 'transparent' : color
            s.caretColor = this.$v[C.CURSORVISIBLE] && !this.$v[C.READONLY] && !this.$cursorItem ? color : 'transparent'
            s.setProperty('--jq2-selection', toCss(this.$v[C.SELECTIONCOLOR]))
            s.setProperty('--jq2-selected-text', toCss(this.$v[C.SELECTEDTEXTCOLOR]))
        }
    }

    // QQuickTextInput::keyPressEvent: the keys of editing stay in the field (the browser types them); Return
    // emits accepted and goes on to the parents like the keys a single line field does not use; Undo and Redo
    // are the field's
    $keyEvent(event, kind){
        // without a native control the key edits here
        let edited = this.$editWithoutNative(event, kind)
        if(edited) return edited
        if(kind !== 'pressed') return 0
        let key = event.key
        let multiline = this.$input ? this.$input.tagName === 'TEXTAREA' : this.$multiline()
        if(key === QtEnums.Key_Return && !multiline){
            if(this.$acceptOrFixup()){
                if(this.constructor.SIGNAL_accepted !== undefined) engine.emitArgs(this, this.constructor.SIGNAL_accepted, [])
                engine.emitArgs(this, this.constructor.SIGNAL_editingFinished, [])
            }
            return 0
        }
        // QQuickTextControlPrivate::keyPressEvent: a TextEdit types a Tab (QInputControl takes '\t' in a text edit);
        // a read-only one leaves it to the focus chain
        if(multiline && key === QtEnums.Key_Tab && !(event.modifiers & ~0x20000000) && !this.$v[this.constructor.READONLY]){
            let start = this.$v[this.constructor.SELECTIONSTART], end = this.$v[this.constructor.SELECTIONEND]
            this.$typed(start, end, '\t', 'insert')
            return ACCEPTED
        }
        let history = undoKey(event)
        if(history){
            if(history === 1) this.undo()
            else this.redo()
            return ACCEPTED
        }
        return takesKey(event, multiline, !!this.$v[this.constructor.READONLY]) ? CONSUMED : 0
    }

    // ---- methods ----

    selectAll(){ this.select(0, this.$content().length) }

    // QQuickTextInput::select (setSelection): from start (the anchor) to end (the cursor)
    select(start, end){
        let length = this.$content().length
        if(start < 0 || end < 0 || start > length || end > length) return
        this.$applySelection(start, end, true)
    }

    deselect(){
        let cursor = this.$v[this.constructor.CURSORPOSITION]
        this.$applySelection(cursor, cursor, true)
    }

    // QQuickTextInput::moveCursorSelection: the selection from the anchor to pos, by characters or whole words
    moveCursorSelection(pos, mode = 0){
        let C = this.constructor
        let content = this.$content()
        pos = Math.max(0, Math.min(Math.trunc(pos), content.length))
        let cursor = this.$v[C.CURSORPOSITION]
        let anchor = this.$anchor()
        if(mode !== 1){
            if(this.$mask && pos !== cursor) pos = pos > cursor ? nextMaskBlank(this.$mask, pos) : prevMaskBlank(this.$mask, pos)
            this.$applySelection(anchor, pos, true)
            return
        }
        if(pos === cursor) return
        if(anchor < pos || (anchor === pos && cursor < pos)){
            if(anchor < content.length && (!isBoundary(content, anchor) || isWord(content[anchor - 1]))) anchor = previousBoundary(content, anchor)
            if(pos > 0 && !isBoundary(content, pos)) pos = nextBoundary(content, pos)
        } else {
            if(anchor > 0 && (!isBoundary(content, anchor) || isWord(content[anchor]))) anchor = nextBoundary(content, anchor)
            if(pos < content.length && !isBoundary(content, pos)) pos = previousBoundary(content, pos)
        }
        this.$applySelection(anchor, pos, true)
    }

    // QQuickTextInputPrivate::selectWordAtPos
    selectWord(){
        let content = this.$content()
        let cursor = this.$v[this.constructor.CURSORPOSITION]
        let next = Math.min(cursor + 1, content.length)
        let start = previousWordStart(content, next)
        let end = nextWordStart(content, start)
        while(end > cursor && isSpace(content[end - 1])) end--
        this.$applySelection(start, Math.max(start, end), true)
    }

    positionAt(x, y = 0, position = 0){
        return this.$positionAtPoint(Number(x) || 0, Number(y) || 0, position === 1)
    }

    positionToRectangle(pos){
        return this.$caretRect(Math.max(0, Math.min(Math.trunc(pos), this.$content().length)))
    }

    isRightToLeft(start, end){
        let text = this.$v[this.constructor.TEXT].slice(Math.min(start, end), Math.max(start, end))
        for(let c of text){
            if(RTL.test(c)) return true
            if(/\p{L}/u.test(c)) return false
        }
        return false
    }

    // QQuickTextInput::clear: an undoable edit, not textEdited
    clear(){
        if(!this.$content()) return
        this.$recordUndo(null)
        let content = this.$mask ? clearString(this.$mask, 0, this.$mask.maxLength) : ''
        this.$commitEdit(content, 0, 0, false)
    }

    // QQuickTextInput::insert: at position, the cursor after it moves on; with an input mask laid over it
    insert(position, text){
        let content = this.$content()
        if(position < 0 || position > content.length) return
        text = String(text)
        let next, length
        if(this.$mask){
            let ms = maskString(this.$mask, position, text, false, content)
            next = content.slice(0, position) + ms + content.slice(position + ms.length)
            length = 0
        } else {
            let max = this.constructor.MAXIMUMLENGTH !== undefined ? this.$v[this.constructor.MAXIMUMLENGTH] : Infinity
            text = text.slice(0, Math.max(0, max - content.length))
            next = content.slice(0, position) + text + content.slice(position)
            length = text.length
        }
        if(next === content) return
        if(this.$validateText && this.$validateText(next) === INVALID) return
        let shift = (p) => p >= position ? p + length : p
        this.$recordUndo(null)
        this.$commitEdit(next, shift(this.$anchor()), shift(this.$v[this.constructor.CURSORPOSITION]))
    }

    // QQuickTextInput::remove: with an input mask the range turns to blanks
    remove(start, end){
        let content = this.$content()
        start = Math.max(0, Math.min(start, content.length))
        end = Math.max(0, Math.min(end, content.length))
        if(start > end) [start, end] = [end, start]
        if(start === end) return
        let next = this.$mask ? content.slice(0, start) + clearString(this.$mask, start, end - start) + content.slice(end) : content.slice(0, start) + content.slice(end)
        if(this.$validateText && this.$validateText(next) === INVALID) return
        let shift = (p) => this.$mask || p <= start ? p : p - (Math.min(p, end) - start)
        this.$recordUndo(null)
        this.$commitEdit(next, shift(this.$anchor()), shift(this.$v[this.constructor.CURSORPOSITION]))
    }

    getText(start, end){
        let content = this.$content()
        start = Math.max(0, Math.min(start, content.length))
        end = Math.max(0, Math.min(end, content.length))
        return content.slice(Math.min(start, end), Math.max(start, end))
    }

    // QQuickTextInput::undo / redo: the edits of the user and of the methods since the text was last set
    undo(){
        if(this.$v[this.constructor.READONLY] || !this.$undoStack.length) return
        let state = this.$undoStack.pop()
        this.$redoStack.push(this.$state())
        this.$lastEdit = null
        this.$commitEdit(state.content, state.anchor, state.cursor)
    }

    redo(){
        if(this.$v[this.constructor.READONLY] || !this.$redoStack.length) return
        let state = this.$redoStack.pop()
        this.$undoStack.push(this.$state())
        this.$lastEdit = null
        let redo = this.$redoStack
        this.$commitEdit(state.content, state.anchor, state.cursor)
        this.$redoStack = redo
        this.$updateUndoFlags()
    }

    // QQuickTextInputPrivate::copy: the selected text, not in the echo modes that hide it; QQuickTextEdit::copy
    copy(){
        let text = this.$v[this.constructor.SELECTEDTEXT]
        if(text && this.$echoesText()) this.$clipboardWrite(text)
    }

    // QQuickTextInput::cut: copy() and del() of the selection, unless read-only or hidden
    cut(){
        let start = this.$v[this.constructor.SELECTIONSTART], end = this.$v[this.constructor.SELECTIONEND]
        if(this.$v[this.constructor.READONLY] || !this.$echoesText() || start === end) return
        this.$clipboardWrite(this.$content().slice(start, end))
        this.$userEdit(start, end, '', null)
    }

    // QQuickTextInput::paste: the clipboard text replaces the selection, unless read-only. The browser gives
    // the clipboard asynchronously, the selection is the one of the call
    paste(){
        if(this.$v[this.constructor.READONLY]) return null
        let start = this.$v[this.constructor.SELECTIONSTART], end = this.$v[this.constructor.SELECTIONEND]
        let clipboard = typeof navigator !== 'undefined' && navigator.clipboard
        if(!clipboard || !clipboard.readText) return null
        return clipboard.readText().then((text) => {
            if(!(this.$f & engine.DESTROYED) && !engine.isStopped()) engine.batch(() => this.$userEdit(start, end, String(text), null))
        }, () => {})
    }

    $echoesText(){
        let id = this.constructor.ECHOMODE
        return id === undefined || !this.$v[id]
    }

    $clipboardWrite(text){
        let clipboard = typeof navigator !== 'undefined' && navigator.clipboard
        if(clipboard && clipboard.writeText){
            clipboard.writeText(text).catch(() => {})
            return
        }
        // not a secure context: the copy command of the document, with the text as its data
        if(typeof document === 'undefined') return
        let handler = (event) => {
            event.preventDefault()
            if(event.clipboardData) event.clipboardData.setData('text/plain', text)
        }
        document.addEventListener('copy', handler, true)
        try {
            document.execCommand('copy')
        } finally {
            document.removeEventListener('copy', handler, true)
        }
    }
}

const textChanged = (field) => field.$textChanged()
const style = (item) => engine.markDirty(item, STYLE)
const text = (item) => engine.markDirty(item, TEXT)
const relayout = (field) => field.$updateImplicitSize()

const common = {
    text: { kind: 'string', effect: textChanged },
    color: { kind: 'color', effect: style },
    font: { group: Font },
    readOnly: {
        kind: 'bool',
        effect(field, value){
            if(field.constructor.CANPASTE !== undefined) field.$write(field.constructor.CANPASTE, !value)
            engine.markDirty(field, TEXT)
        },
    },
    cursorPosition: 'int',
    selectionStart: { kind: 'int', readonly: true },
    selectionEnd: { kind: 'int', readonly: true },
    selectedText: { kind: 'string', readonly: true },
    selectionColor: { kind: 'color', initial: '#0078d7', effect: style },
    selectedTextColor: { kind: 'color', initial: '#ffffff', effect: style },
    cursorRectangle: { kind: 'var', readonly: true },
    // QQuickTextInput::setCursorVisible: showing the cursor makes the pending cursor delegate
    cursorVisible: { kind: 'bool', effect(field, on){ if(on && (field.$f & engine.COMPLETED)) field.$createCursor(); style(field) } },
    cursorDelegate: { kind: 'component', effect: (field) => field.$setCursorDelegate() },
    persistentSelection: 'bool',
    mouseSelectionMode: 'int',
    overwriteMode: { kind: 'bool', effect: (field) => field.$updateCursorRectangle() },
    leftPadding: { kind: 'real', effect: relayout },
    rightPadding: { kind: 'real', effect: relayout },
    topPadding: { kind: 'real', effect: relayout },
    bottomPadding: { kind: 'real', effect: relayout },
    padding: { kind: 'real', effect: relayout },
    inputMethodHints: { kind: 'int', effect: text },
    selectByMouse: { kind: 'bool', initial: true },
    activeFocusOnPress: { kind: 'bool', initial: true },
    horizontalAlignment: {
        kind: 'int',
        initial: 1,
        effect(field, value){
            field.$write(field.constructor.EFFECTIVEHORIZONTALALIGNMENT, value)
            relayout(field)
        },
    },
    effectiveHorizontalAlignment: { kind: 'int', initial: 1, readonly: true },
    verticalAlignment: { kind: 'int', initial: 32, effect: relayout },
    length: { kind: 'int', readonly: true },
    contentWidth: { kind: 'real', readonly: true },
    contentHeight: { kind: 'real', readonly: true },
    canUndo: { kind: 'bool', readonly: true },
    canRedo: { kind: 'bool', readonly: true },
    canPaste: { kind: 'bool', initial: true, readonly: true },
    inputMethodComposing: { kind: 'bool', readonly: true },
    preeditText: { kind: 'string', readonly: true },
}

module.exports = { TextField, text, relayout, common, parseInputMask, maskString, clearString, stripString, maskAcceptable, NO_ECHO, PASSWORD, PASSWORD_ECHO_ON_EDIT }
