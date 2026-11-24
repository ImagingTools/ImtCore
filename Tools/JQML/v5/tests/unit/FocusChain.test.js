// Run: node --test tests/unit
// The tab focus chain as QQuickItemPrivate::nextPrevItemInTabFocusChain and deliverKeyEvent (Qt 6.8): depth first in
// the order of the children, only visible, enabled items with activeFocusOnTab, wrapping at the end; Tab and Backtab
// move the focus from the contentItem or an item with activeFocusOnTab; a TextEdit types a Tab.
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

const ShiftModifier = 0x02000000, ControlModifier = 0x04000000

function cases(name, runtime){
    const { engine, Item, TextInput, TextEdit, types } = runtime
    const keys = runtime.keys

    // a window: a form of three fields and a panel with a button inside; the middle field is disabled later
    function scene(){
        let root, a, b, panel, button, c, plain
        engine.batch(() => {
            root = new Item(null)
            a = new Item(root)
            b = new Item(root)
            panel = new Item(root)
            plain = new Item(panel)
            button = new Item(panel)
            c = new Item(root)
            for(let it of [a, b, button, c]) it.activeFocusOnTab = true
        })
        keys.agent.root = root
        keys.agent.windowFocused = true
        root.$isFocusScope = true
        return { root, a, b, panel, button, c, plain }
    }

    test(name + ': nextItemInFocusChain goes depth first through the items with activeFocusOnTab and wraps', () => {
        let { a, b, button, c, plain } = scene()
        assert.equal(a.nextItemInFocusChain(), b)
        assert.equal(b.nextItemInFocusChain(), button)
        assert.equal(button.nextItemInFocusChain(), c)
        assert.equal(c.nextItemInFocusChain(), a)
        assert.equal(button.nextItemInFocusChain(false), b)
        assert.equal(a.nextItemInFocusChain(false), c)
        // an item without activeFocusOnTab is passed over, also as the start
        assert.equal(plain.nextItemInFocusChain(), button)
    })

    test(name + ': invisible and disabled items, and the children of a disabled one, are passed over', () => {
        let { a, b, panel, c } = scene()
        engine.batch(() => {
            b.visible = false
            panel.enabled = false
        })
        assert.equal(a.nextItemInFocusChain(), c)
        assert.equal(c.nextItemInFocusChain(false), a)
    })

    test(name + ': Tab and Shift+Tab move the active focus, Ctrl+Tab does not', () => {
        let { a, b, button } = scene()
        engine.batch(() => a.forceActiveFocus())
        assert.equal(a.activeFocus, true)
        let result
        engine.batch(() => { result = keys.deliverTo(a, { key: 0x01000001, modifiers: 0, text: '' }, 'pressed') })
        assert.equal(result, keys.ACCEPTED)
        assert.equal(b.activeFocus, true)
        engine.batch(() => keys.deliverTo(b, { key: 0x01000001, modifiers: 0, text: '' }, 'pressed'))
        assert.equal(button.activeFocus, true)
        engine.batch(() => keys.deliverTo(button, { key: 0x01000002, modifiers: ShiftModifier, text: '' }, 'pressed'))
        assert.equal(b.activeFocus, true)
        engine.batch(() => { result = keys.deliverTo(b, { key: 0x01000001, modifiers: ControlModifier, text: '' }, 'pressed') })
        assert.equal(result, 0)
        assert.equal(b.activeFocus, true)
    })

    test(name + ': an item without activeFocusOnTab leaves Tab to its parents, the contentItem moves the focus', () => {
        let { root, a, plain } = scene()
        engine.batch(() => plain.forceActiveFocus())
        assert.equal(keys.deliverTo(plain, { key: 0x01000001, modifiers: 0, text: '' }, 'pressed'), 0)
        engine.batch(() => keys.deliverTo(root, { key: 0x01000001, modifiers: 0, text: '' }, 'pressed'))
        assert.equal(a.activeFocus || root.activeFocus, true)
    })

    test(name + ': a TextEdit types a Tab, a read-only one does not', () => {
        let edit
        engine.batch(() => {
            edit = new TextEdit(null)
            edit.text = 'ab'
            edit.cursorPosition = 1
        })
        engine.batch(() => edit.$keyEvent({ key: 0x01000001, modifiers: 0, text: '' }, 'pressed'))
        assert.equal(edit.text, 'a\tb')
        engine.batch(() => { edit.readOnly = true })
        assert.equal(edit.$keyEvent({ key: 0x01000001, modifiers: 0, text: '' }, 'pressed'), 0)
    })
}

for(let [name, load] of runtimes) cases(name, load())
