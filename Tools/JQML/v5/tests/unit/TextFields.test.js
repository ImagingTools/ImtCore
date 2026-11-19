// Run: node --test tests/unit
// TextInput and TextEdit as QQuickTextInput / QQuickTextEdit (qtdeclarative 6.8): the input mask, the maximum length,
// the cursor and the selection, undo and redo, overwrite mode, echo modes, padding, alignment and the cursor
// rectangle, positionAt, ensureVisible, word selection, append.
// Without a DOM the text is measured at half the pixel size per character: 6 px at the default 12 px font, and
// a line is ceil(12 * 1.33) = 16 px high.
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

const CHAR = 6
const LINE = 16
const ControlModifier = 0x04000000
const ShiftModifier = 0x02000000
const Key_Z = 0x5a

// the native control as the browser has it: value, selection; beforeinput / input as the browser sends them
function nativeControl(field, tagName = 'INPUT'){
    let control = {
        tagName, value: field.$content(), selectionStart: 0, selectionEnd: 0, selectionDirection: 'none', scrollLeft: 0, scrollTop: 0,
        style: { setProperty(){} }, attributes: {},
        hasAttribute(name){ return name in this.attributes },
        getAttribute(name){ return name in this.attributes ? this.attributes[name] : null },
        setAttribute(name, value){ this.attributes[name] = String(value) },
        removeAttribute(name){ delete this.attributes[name] },
        setSelectionRange(start, end, direction){
            this.selectionStart = start
            this.selectionEnd = end
            this.selectionDirection = direction || 'forward'
        },
    }
    field.$input = control
    return control
}

// the browser typing data at the selection: a field that takes the edit over prevents it, else the browser edits
function type(field, data){
    let control = field.$input
    let prevented = false
    field.$beforeInput({ inputType: 'insertText', data, isComposing: false, preventDefault(){ prevented = true } })
    if(prevented) return
    let start = control.selectionStart, end = control.selectionEnd
    control.value = control.value.slice(0, start) + data + control.value.slice(end)
    control.selectionStart = control.selectionEnd = start + data.length
    field.$nativeInput()
}

function backspace(field){
    let control = field.$input
    let prevented = false
    field.$beforeInput({ inputType: 'deleteContentBackward', data: null, isComposing: false, preventDefault(){ prevented = true } })
    if(prevented) return
    let start = control.selectionStart, end = control.selectionEnd
    if(start === end && start > 0) start--
    control.value = control.value.slice(0, start) + control.value.slice(end)
    control.selectionStart = control.selectionEnd = start
    field.$nativeInput()
}

function cases(name, runtime){
    const { engine, TextInput, TextEdit } = runtime

    function make(Type, setup){
        let item
        engine.batch(() => {
            item = new Type(null)
            if(setup) setup(item)
        })
        return item
    }

    function counter(item, signal){
        let count = { value: 0 }
        engine.connect(item, signal, () => count.value++)
        return count
    }

    // ---- input mask ----

    test(name + ': an input mask shows its separators and blanks, text() has the separators only', () => {
        let input = make(TextInput, (it) => { it.inputMask = '00.00.0000' })
        assert.equal(input.displayText, '  .  .    ')
        assert.equal(input.text, '..')
        assert.equal(input.length, 10)
        assert.equal(input.maximumLength, 10)
        // '0' takes a blank, '9' needs a digit (QQuickTextInputPrivate::isValidInput)
        assert.equal(input.acceptableInput, true)
        let required = make(TextInput, (it) => { it.inputMask = '99.99.9999' })
        assert.equal(required.acceptableInput, false)
    })

    test(name + ': a text set on a masked input is laid over the mask', () => {
        let input = make(TextInput, (it) => { it.inputMask = '99.99.9999 99:99' })
        engine.batch(() => { input.text = '120520241530' })
        assert.equal(input.text, '12.05.2024 15:30')
        assert.equal(input.acceptableInput, true)
        engine.batch(() => { input.text = '1205' })
        assert.equal(input.text, '12.05. :')
        assert.equal(input.displayText, '12.05.       :  ')
        assert.equal(input.acceptableInput, false)
    })

    test(name + ': a mask set after the text takes the text', () => {
        let input = make(TextInput, (it) => {
            it.text = '31122025'
            it.inputMask = '00.00.0000'
        })
        assert.equal(input.text, '31.12.2025')
    })

    test(name + ': typing into a masked input fills the blanks and skips the separators', () => {
        let input = make(TextInput, (it) => { it.inputMask = '00.00.0000' })
        nativeControl(input)
        engine.batch(() => { input.cursorPosition = 0 })
        let edited = counter(input, TextInput.SIGNAL_textEdited)
        for(let c of '1205') engine.batch(() => type(input, c))
        assert.equal(input.displayText, '12.05.    ')
        assert.equal(input.cursorPosition, 6)
        assert.equal(input.$input.value, '12.05.    ')
        assert.equal(edited.value, 4)
        // a character the mask does not take is dropped
        engine.batch(() => type(input, 'x'))
        assert.equal(input.displayText, '12.05.    ')
        for(let c of '2024') engine.batch(() => type(input, c))
        assert.equal(input.text, '12.05.2024')
        assert.equal(input.acceptableInput, true)
    })

    test(name + ': a separator typed jumps to after it', () => {
        let input = make(TextInput, (it) => { it.inputMask = '00.00.0000' })
        nativeControl(input)
        engine.batch(() => { input.cursorPosition = 0 })
        engine.batch(() => type(input, '1'))
        engine.batch(() => type(input, '.'))
        assert.equal(input.cursorPosition, 3)
        assert.equal(input.displayText, '1 .  .    ')
    })

    test(name + ': backspace in a masked input blanks the character before the cursor, over a separator', () => {
        let input = make(TextInput, (it) => {
            it.inputMask = '00.00.0000'
            it.text = '12052024'
        })
        nativeControl(input)
        engine.batch(() => { input.cursorPosition = 3 })
        engine.batch(() => backspace(input))
        assert.equal(input.displayText, '1 .05.2024')
        assert.equal(input.cursorPosition, 1)
    })

    test(name + ': the case marks and the blank character of a mask', () => {
        let input = make(TextInput, (it) => { it.inputMask = '>AAA-<aa;_' })
        assert.equal(input.displayText, '___-__')
        engine.batch(() => { input.text = 'abcDE' })
        assert.equal(input.text, 'ABC-de')
    })

    test(name + ': removing the mask clears the text', () => {
        let input = make(TextInput, (it) => {
            it.inputMask = '000'
            it.text = '123'
        })
        engine.batch(() => { input.inputMask = '' })
        assert.equal(input.text, '')
        assert.equal(input.maximumLength, 32767)
    })

    // ---- maximum length, cursor and selection ----

    test(name + ': the text is cut to the maximum length, also when the maximum length drops', () => {
        let input = make(TextInput, (it) => {
            it.maximumLength = 5
            it.text = 'abcdefgh'
        })
        assert.equal(input.text, 'abcde')
        engine.batch(() => { input.maximumLength = 3 })
        assert.equal(input.text, 'abc')
    })

    test(name + ': a TextInput puts the cursor at the end of a new text, a TextEdit at the start', () => {
        let input = make(TextInput, (it) => { it.text = 'hello' })
        assert.equal(input.cursorPosition, 5)
        let edit = make(TextEdit, (it) => { it.text = 'hello' })
        assert.equal(edit.cursorPosition, 0)
    })

    test(name + ': cursorPosition moves the cursor and deselects; a position outside the text is ignored', () => {
        let input = make(TextInput, (it) => { it.text = 'hello world' })
        engine.batch(() => input.select(0, 5))
        engine.batch(() => { input.cursorPosition = 7 })
        assert.equal(input.cursorPosition, 7)
        assert.equal(input.selectedText, '')
        engine.batch(() => { input.cursorPosition = 40 })
        assert.equal(input.cursorPosition, 7)
    })

    test(name + ': select(start, end) puts the cursor at end, also before start', () => {
        let input = make(TextInput, (it) => { it.text = 'hello world' })
        engine.batch(() => input.select(5, 2))
        assert.equal(input.selectionStart, 2)
        assert.equal(input.selectionEnd, 5)
        assert.equal(input.cursorPosition, 2)
        assert.equal(input.selectedText, 'llo')
    })

    test(name + ': moveCursorSelection by characters and by words', () => {
        let input = make(TextInput, (it) => { it.text = 'hello big world' })
        engine.batch(() => { input.cursorPosition = 2 })
        engine.batch(() => input.moveCursorSelection(7))
        assert.equal(input.selectedText, 'llo b')
        assert.equal(input.cursorPosition, 7)
        engine.batch(() => { input.cursorPosition = 2 })
        engine.batch(() => input.moveCursorSelection(7, TextInput.SelectWords))
        assert.equal(input.selectedText, 'hello big')
    })

    test(name + ': selectWord selects the word at the cursor', () => {
        let input = make(TextInput, (it) => { it.text = 'hello world' })
        engine.batch(() => { input.cursorPosition = 8 })
        engine.batch(() => input.selectWord())
        assert.equal(input.selectedText, 'world')
    })

    test(name + ': isRightToLeft follows the first strong character', () => {
        let input = make(TextInput, (it) => { it.text = 'abc שלום' })
        assert.equal(input.isRightToLeft(0, 3), false)
        assert.equal(input.isRightToLeft(4, 8), true)
    })

    // ---- undo, redo, insert, remove ----

    test(name + ': typing is undone as one step, redo brings it back; a cursor move separates the steps', () => {
        let input = make(TextInput)
        nativeControl(input)
        for(let c of 'abc') engine.batch(() => type(input, c))
        assert.equal(input.text, 'abc')
        assert.equal(input.canUndo, true)
        engine.batch(() => input.undo())
        assert.equal(input.text, '')
        assert.equal(input.canRedo, true)
        engine.batch(() => input.redo())
        assert.equal(input.text, 'abc')
        engine.batch(() => { input.cursorPosition = 1 })
        engine.batch(() => type(input, 'X'))
        assert.equal(input.text, 'aXbc')
        engine.batch(() => input.undo())
        assert.equal(input.text, 'abc')
    })

    test(name + ': a new text clears the undo history', () => {
        let input = make(TextInput)
        nativeControl(input)
        engine.batch(() => type(input, 'a'))
        engine.batch(() => { input.text = 'new' })
        assert.equal(input.canUndo, false)
        engine.batch(() => input.undo())
        assert.equal(input.text, 'new')
    })

    test(name + ': Ctrl+Z and Ctrl+Shift+Z undo and redo in the field, the key is accepted', () => {
        let input = make(TextInput)
        nativeControl(input)
        engine.batch(() => type(input, 'a'))
        let result
        engine.batch(() => { result = input.$keyEvent({ key: Key_Z, modifiers: ControlModifier, text: '' }, 'pressed') })
        assert.equal(input.text, '')
        assert.ok(result)
        engine.batch(() => input.$keyEvent({ key: Key_Z, modifiers: ControlModifier | ShiftModifier, text: '' }, 'pressed'))
        assert.equal(input.text, 'a')
    })

    test(name + ': insert() and remove() edit at a position, move the cursor after it and can be undone', () => {
        let input = make(TextInput, (it) => { it.text = 'hello' })
        let edited = counter(input, TextInput.SIGNAL_textEdited)
        engine.batch(() => input.insert(0, '>> '))
        assert.equal(input.text, '>> hello')
        assert.equal(input.cursorPosition, 8)
        engine.batch(() => input.remove(0, 3))
        assert.equal(input.text, 'hello')
        assert.equal(input.cursorPosition, 5)
        assert.equal(edited.value, 2)
        engine.batch(() => input.undo())
        assert.equal(input.text, '>> hello')
    })

    test(name + ': insert() keeps the maximum length', () => {
        let input = make(TextInput, (it) => {
            it.maximumLength = 6
            it.text = 'abcd'
        })
        engine.batch(() => input.insert(2, 'XYZ'))
        assert.equal(input.text, 'abXYcd')
    })

    // ---- overwrite, echo ----

    test(name + ': in overwrite mode typing replaces the character after the cursor', () => {
        let input = make(TextInput, (it) => {
            it.text = 'abc'
            it.overwriteMode = true
        })
        nativeControl(input)
        engine.batch(() => { input.cursorPosition = 1 })
        engine.batch(() => type(input, 'X'))
        assert.equal(input.text, 'aXc')
        assert.equal(input.cursorPosition, 2)
    })

    test(name + ': displayText hides the text in the echo modes', () => {
        let input = make(TextInput, (it) => { it.text = 'secret' })
        assert.equal(input.displayText, 'secret')
        engine.batch(() => { input.echoMode = TextInput.Password })
        assert.equal(input.displayText, '●●●●●●')
        engine.batch(() => { input.passwordCharacter = '*' })
        assert.equal(input.displayText, '******')
        engine.batch(() => { input.echoMode = TextInput.NoEcho })
        assert.equal(input.displayText, '')
        engine.batch(() => { input.echoMode = TextInput.PasswordEchoOnEdit })
        assert.equal(input.displayText, '******')
    })

    test(name + ': canPaste is false for a read-only field', () => {
        let input = make(TextInput)
        assert.equal(input.canPaste, true)
        engine.batch(() => { input.readOnly = true })
        assert.equal(input.canPaste, false)
    })

    // ---- padding, alignment, cursor rectangle ----

    test(name + ': padding is the default of each side, a side set explicitly keeps its value', () => {
        let input = make(TextInput, (it) => {
            it.text = 'abc'
            it.leftPadding = 2
            it.padding = 5
        })
        assert.equal(input.leftPadding, 2)
        assert.equal(input.rightPadding, 5)
        assert.equal(input.topPadding, 5)
        assert.equal(input.implicitWidth, 3 * CHAR + 2 + 5)
        assert.equal(input.implicitHeight, LINE + 10)
    })

    test(name + ': the cursor rectangle is in the item: padding, alignment, the text before the cursor', () => {
        let input = make(TextInput, (it) => {
            it.width = 200
            it.height = 40
            it.text = 'hello'
            it.leftPadding = 3
            it.verticalAlignment = TextInput.AlignVCenter
        })
        engine.batch(() => { input.cursorPosition = 2 })
        let rect = input.cursorRectangle
        assert.equal(rect.x, 3 + 2 * CHAR)
        assert.equal(rect.y, (40 - LINE) / 2)
        assert.equal(rect.height, LINE)
        engine.batch(() => { input.horizontalAlignment = TextInput.AlignRight })
        assert.equal(input.cursorRectangle.x, 3 + (200 - 3 - 5 * CHAR) + 2 * CHAR)
        assert.equal(input.effectiveHorizontalAlignment, TextInput.AlignRight)
        // positionToRectangle of a TextInput does not count the padding
        assert.equal(input.positionToRectangle(2).x, 200 - 3 - 5 * CHAR + 2 * CHAR)
    })

    test(name + ': cursorRectangle changes as the cursor moves', () => {
        let input = make(TextInput, (it) => {
            it.width = 100
            it.text = 'hello'
        })
        let changed = counter(input, TextInput.SIGNAL_cursorRectangleChanged)
        engine.batch(() => { input.cursorPosition = 1 })
        engine.batch(() => { input.cursorPosition = 3 })
        assert.equal(changed.value, 2)
        assert.equal(input.cursorRectangle.x, 3 * CHAR)
    })

    test(name + ': the cursor rectangle of a TextEdit is on its line, the text aligned down the item', () => {
        let edit = make(TextEdit, (it) => {
            it.width = 200
            it.height = 100
            it.text = 'ab\ncd'
            it.verticalAlignment = TextEdit.AlignVCenter
        })
        engine.batch(() => { edit.cursorPosition = 4 })
        let rect = edit.cursorRectangle
        assert.equal(rect.x, CHAR)
        assert.equal(rect.y, (100 - 2 * LINE) / 2 + LINE)
        assert.deepEqual([edit.positionToRectangle(4).x, edit.positionToRectangle(4).y], [rect.x, rect.y])
    })

    test(name + ': a wrapped TextEdit has its cursor on the wrapped line', () => {
        let edit = make(TextEdit, (it) => {
            it.width = 30
            it.wrapMode = TextEdit.WordWrap
            it.text = 'aaa bbb'
        })
        assert.equal(edit.lineCount, 2)
        engine.batch(() => { edit.cursorPosition = 5 })
        assert.equal(edit.cursorRectangle.y, LINE)
        assert.equal(edit.cursorRectangle.x, CHAR)
    })

    test(name + ': positionAt gives the nearest position, or the character under x', () => {
        let input = make(TextInput, (it) => {
            it.width = 100
            it.text = 'hello'
        })
        assert.equal(input.positionAt(13, 0), 2)
        assert.equal(input.positionAt(16, 0), 3)
        assert.equal(input.positionAt(16, 0, TextInput.CursorOnCharacter), 2)
        assert.equal(input.positionAt(500, 0), 5)
        let edit = make(TextEdit, (it) => {
            it.width = 100
            it.text = 'ab\ncd'
        })
        assert.equal(edit.positionAt(7, LINE + 2), 4)
    })

    test(name + ': ensureVisible scrolls the text so that the position shows', () => {
        let input = make(TextInput, (it) => {
            it.width = 5 * CHAR
            it.text = 'abcdefghij'
        })
        engine.batch(() => input.ensureVisible(10))
        assert.equal(input.positionToRectangle(10).x, 5 * CHAR)
        engine.batch(() => input.ensureVisible(0))
        assert.equal(input.positionToRectangle(0).x, 0)
    })

    test(name + ': the layout kept for the cursor follows a new text and a new width', () => {
        let input = make(TextInput, (it) => {
            it.width = 100
            it.text = 'abc'
        })
        assert.equal(input.cursorRectangle.x, 3 * CHAR)
        engine.batch(() => { input.text = 'abcdef' })
        assert.equal(input.cursorRectangle.x, 6 * CHAR)
        engine.batch(() => { input.horizontalAlignment = TextInput.AlignRight })
        engine.batch(() => { input.width = 200 })
        assert.equal(input.cursorRectangle.x, 200)
    })

    // ---- TextEdit ----

    test(name + ': append adds a paragraph, the text itself when empty', () => {
        let edit = make(TextEdit)
        engine.batch(() => edit.append('first'))
        assert.equal(edit.text, 'first')
        engine.batch(() => edit.append('second'))
        assert.equal(edit.text, 'first\nsecond')
        assert.equal(edit.lineCount, 2)
    })

    // ---- focus out, cursor delegate ----

    // a window with two fields: the second takes the focus from the first
    function focusScene(setup){
        const { Item, keys } = runtime
        let root, first, second
        engine.batch(() => {
            root = new Item(null)
            first = new TextInput(root)
            first.text = 'hello world'
            second = new TextInput(root)
            if(setup) setup(first)
        })
        root.$isFocusScope = true
        keys.agent.root = root
        keys.agent.windowFocused = true
        engine.batch(() => root.$setFocus(true))
        engine.batch(() => first.forceActiveFocus())
        return { root, first, second, keys }
    }

    // QQuickTextInputPrivate::handleFocusEvent / QQuickTextEditPrivate::handleFocusEvent
    test(name + ': losing the focus deselects, unless persistentSelection or the window lost it', () => {
        let { first, second } = focusScene()
        engine.batch(() => first.select(0, 5))
        engine.batch(() => second.forceActiveFocus())
        assert.equal(first.selectedText, '')
        assert.equal(first.cursorPosition, 5)

        let kept = focusScene((it) => { it.persistentSelection = true })
        engine.batch(() => kept.first.select(0, 5))
        engine.batch(() => kept.second.forceActiveFocus())
        assert.equal(kept.first.selectedText, 'hello')

        let window = focusScene()
        engine.batch(() => window.first.select(0, 5))
        window.keys.setWindowFocused(false)
        assert.equal(window.first.activeFocus, false)
        assert.equal(window.first.selectedText, 'hello')
        window.keys.setWindowFocused(true)
    })

    test(name + ': the cursor shows with the focus, not in a read-only field', () => {
        let { first, second } = focusScene()
        assert.equal(first.cursorVisible, true)
        engine.batch(() => second.forceActiveFocus())
        assert.equal(first.cursorVisible, false)
        let readOnly = focusScene((it) => { it.readOnly = true })
        assert.equal(readOnly.first.activeFocus, true)
        assert.equal(readOnly.first.cursorVisible, false)
    })

    // QQuickTextUtil::setCursorDelegate / createCursor, QQuickTextInput::updateCursorRectangle
    test(name + ': a cursor delegate is made when the cursor first shows, at the cursor rectangle, and follows it', () => {
        const { Item, types } = runtime
        let made = 0
        let delegate = new types.Component(null, (parent) => {
            made++
            let item
            engine.batch(() => {
                item = new Item(parent)
                item.width = 2
            })
            return item
        })
        let input = make(TextInput, (it) => {
            it.text = 'abcdef'
            it.cursorDelegate = delegate
        })
        assert.equal(made, 0)
        engine.batch(() => { input.cursorVisible = true })
        assert.equal(made, 1)
        let cursor = input.children.find((child) => child.width === 2)
        assert.ok(cursor)
        assert.equal(cursor.parent, input)
        assert.equal(cursor.x, input.cursorRectangle.x)
        assert.equal(cursor.y, input.cursorRectangle.y)
        assert.equal(cursor.height, input.cursorRectangle.height)
        engine.batch(() => { input.cursorPosition = 3 })
        assert.equal(cursor.x, 3 * CHAR)
        // it stays when the cursor hides; a new delegate replaces it
        engine.batch(() => { input.cursorVisible = false })
        engine.batch(() => { input.cursorVisible = true })
        assert.equal(made, 1)
        engine.batch(() => { input.cursorDelegate = delegate.constructor === undefined ? null : new types.Component(null, delegate.$build) })
        assert.equal(made, 2)
    })
}

for(let [name, load] of runtimes) cases(name, load())
