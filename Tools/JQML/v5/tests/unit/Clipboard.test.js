// Run: node --test tests/unit
// copy(), cut() and paste() of TextInput and TextEdit as in QQuickTextInput / QQuickTextEdit: copy takes the
// selected text (not in a password echo mode), cut and paste edit the text unless it is read-only. The page's
// "Copy" buttons call selectAll() and copy() of a hidden TextEdit.
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

const Password = 2

// the browser clipboard (navigator.clipboard) for the time of a test
function withClipboard(initial){
    let clipboard = { text: initial || '', writeText(text){ this.text = text; return Promise.resolve() }, readText(){ return Promise.resolve(this.text) } }
    let previous = Object.getOwnPropertyDescriptor(globalThis, 'navigator')
    Object.defineProperty(globalThis, 'navigator', { value: { clipboard }, configurable: true, writable: true })
    clipboard.restore = () => {
        if(previous) Object.defineProperty(globalThis, 'navigator', previous)
        else delete globalThis.navigator
    }
    return clipboard
}

function cases(name, runtime){
    const { engine, TextInput, TextEdit } = runtime

    function field(Type, text, setup){
        let item
        engine.batch(() => {
            item = new Type(null)
            item.text = text
            if(setup) setup(item)
        })
        return item
    }

    test(name + ': selectAll() and copy() of a hidden TextEdit put its text on the clipboard', () => {
        let clipboard = withClipboard()
        try {
            let helper = field(TextEdit, 'ABCD-1234', (it) => { it.visible = false })
            engine.batch(() => { helper.selectAll(); helper.copy() })
            assert.equal(clipboard.text, 'ABCD-1234')
        } finally {
            clipboard.restore()
        }
    })

    test(name + ': copy() takes the selection only, nothing when nothing is selected', () => {
        let clipboard = withClipboard('before')
        try {
            let input = field(TextInput, 'hello world')
            engine.batch(() => input.copy())
            assert.equal(clipboard.text, 'before')
            engine.batch(() => { input.select(6, 11); input.copy() })
            assert.equal(clipboard.text, 'world')
        } finally {
            clipboard.restore()
        }
    })

    test(name + ': copy() and cut() of a TextInput in the password echo mode do nothing', () => {
        let clipboard = withClipboard('before')
        try {
            let input = field(TextInput, 'secret', (it) => { it.echoMode = Password })
            engine.batch(() => { input.selectAll(); input.copy(); input.cut() })
            assert.equal(clipboard.text, 'before')
            assert.equal(input.text, 'secret')
        } finally {
            clipboard.restore()
        }
    })

    test(name + ': cut() moves the selection to the clipboard and emits textEdited, not when read-only', () => {
        let clipboard = withClipboard()
        try {
            let input = field(TextInput, 'hello world')
            let edited = 0
            engine.connect(input, TextInput.SIGNAL_textEdited, () => edited++)
            engine.batch(() => { input.select(5, 11); input.cut() })
            assert.equal(clipboard.text, ' world')
            assert.equal(input.text, 'hello')
            assert.equal(input.cursorPosition, 5)
            assert.equal(edited, 1)
            let readOnly = field(TextInput, 'fixed', (it) => { it.readOnly = true })
            engine.batch(() => { readOnly.selectAll(); readOnly.cut() })
            assert.equal(readOnly.text, 'fixed')
            assert.equal(clipboard.text, ' world')
        } finally {
            clipboard.restore()
        }
    })

    test(name + ': paste() replaces the selection with the clipboard text, within the maximum length', async () => {
        let clipboard = withClipboard('big ')
        try {
            let input = field(TextInput, 'a small dog', (it) => { it.maximumLength = 12 })
            engine.batch(() => input.select(2, 8))
            await input.paste()
            assert.equal(input.text, 'a big dog')
            assert.equal(input.cursorPosition, 6)
            clipboard.text = 'XXXXXXXX'
            engine.batch(() => input.select(0, 0))
            await input.paste()
            assert.equal(input.text, 'XXXa big dog')
        } finally {
            clipboard.restore()
        }
    })

    test(name + ': paste() into a read-only field does nothing', async () => {
        let clipboard = withClipboard('text')
        try {
            let edit = field(TextEdit, 'fixed', (it) => { it.readOnly = true })
            await edit.paste()
            assert.equal(edit.text, 'fixed')
        } finally {
            clipboard.restore()
        }
    })
}

for(let [name, load] of runtimes) cases(name, load())
