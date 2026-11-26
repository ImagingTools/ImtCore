// Run: node --test tests/web   (Playwright: npm install playwright in v5, or JQML_PLAYWRIGHT=<path to the package>)
// A plain TextEdit (and the TextArea of the controls) in Chromium: the native control shows the document's text; typing,
// Return, Backspace, Delete, undo and redo as QTextDocument steps, an input method, a paste, the arrows of the control,
// a selection by the mouse, a binding of text kept while typing.
const test = require('node:test')
const assert = require('node:assert/strict')
const { browser, playwright } = require('./lib')

const SKIP = playwright() ? false : 'no Playwright (npm install playwright, or JQML_PLAYWRIGHT=<path>)'

let b = null
test.before(async () => { if(!SKIP) b = await browser() })
test.after(async () => { if(b) await b.close() })

const page = (extra = '', text = 'abc def') => `import QtQuick
Rectangle {
    width: 600; height: 400; color: "white"
    property alias edit: edit
    property int edits: 0
    TextEdit {
        id: edit
        x: 20; y: 20; width: 300; height: 200
        font.family: "Arial"; font.pixelSize: 16
        text: ${JSON.stringify(text)}
        onTextChanged: parent.edits++
        ${extra}
    }
}`

const state = (p) => p.evaluate(() => {
    let e = window.__root.edit
    return { text: e.text, cursor: e.cursorPosition, start: e.selectionStart, end: e.selectionEnd, canUndo: e.canUndo, canRedo: e.canRedo, value: document.querySelector('textarea').value }
})

async function focusAt(p, pos){
    await p.click('textarea')
    await p.evaluate((pos) => { window.__root.edit.cursorPosition = pos }, pos)
}

test('typing, Return and Backspace edit the text; the control shows it; undo takes the typed word back at once', { skip: SKIP }, async () => {
    let p = await b.open(page())
    await focusAt(p, 3)
    await p.keyboard.type('xy')
    let s = await state(p)
    assert.deepEqual([s.text, s.value, s.cursor], ['abcxy def', 'abcxy def', 5])
    await p.keyboard.press('Enter')
    await p.keyboard.type('z')
    s = await state(p)
    assert.deepEqual([s.text, s.value, s.cursor], ['abcxy\nz def', 'abcxy\nz def', 7])
    await p.keyboard.press('Backspace')
    await p.keyboard.press('Backspace')
    assert.equal((await state(p)).text, 'abcxy def')
    // undo by QTextDocument's steps: the removed separator, the removed z, the z, the Return, xy
    await p.keyboard.press('ControlOrMeta+z')
    s = await state(p)
    assert.deepEqual([s.text, s.cursor], ['abcxy\n def', 6])
    await p.keyboard.press('ControlOrMeta+z')
    s = await state(p)
    assert.deepEqual([s.text, s.cursor], ['abcxy\nz def', 7])
    for(let i = 0; i < 3; i++) await p.keyboard.press('ControlOrMeta+z')
    s = await state(p)
    assert.deepEqual([s.text, s.value, s.canUndo], ['abc def', 'abc def', false])
    await p.keyboard.press('ControlOrMeta+Shift+z')
    assert.equal((await state(p)).text, 'abcxy def')
    assert.deepEqual(p.errors, [])
})

test('the arrows of the control move the cursor; Shift selects; Delete removes after the cursor', { skip: SKIP }, async () => {
    let p = await b.open(page('', 'first\nsecond'))
    await focusAt(p, 2)
    // the control moves its caret, the selectionchange that follows tells the TextEdit
    let settle = () => p.evaluate(() => new Promise(r => setTimeout(r, 30)))
    await p.keyboard.press('ArrowDown')
    await settle()
    let s = await state(p)
    assert.ok(s.cursor > 6 && s.cursor < 10, String(s.cursor))
    await p.keyboard.press('Home')
    await settle()
    assert.equal((await state(p)).cursor, 6)
    await p.keyboard.press('Shift+End')
    await settle()
    s = await state(p)
    assert.deepEqual([s.start, s.end], [6, 12])
    await p.keyboard.press('Delete')
    assert.equal((await state(p)).text, 'first\n')
})

test('an input method composes into the text', { skip: SKIP }, async () => {
    let p = await b.open(page())
    await focusAt(p, 3)
    let cdp = await p.context().newCDPSession(p)
    await cdp.send('Input.imeSetComposition', { text: 'ñ', selectionStart: 1, selectionEnd: 1 })
    await cdp.send('Input.insertText', { text: 'ñ' })
    let s = await state(p)
    assert.deepEqual([s.text, s.cursor], ['abcñ def', 4])
})

test('a paste replaces the selection with the plain text of the clipboard', { skip: SKIP }, async () => {
    let p = await b.open(page(), { clipboard: true })
    await focusAt(p, 0)
    await p.evaluate(async () => {
        await navigator.clipboard.writeText('PASTED')
        window.__root.edit.select(0, 3)
    })
    await p.keyboard.press('ControlOrMeta+v')
    let s = await state(p)
    assert.deepEqual([s.text, s.cursor], ['PASTED def', 6])
})

test('a drag of the mouse selects in the control', { skip: SKIP }, async () => {
    let p = await b.open(page())
    let box = await p.locator('textarea').boundingBox()
    await p.mouse.move(box.x + 2, box.y + 10)
    await p.mouse.down()
    await p.mouse.move(box.x + 40, box.y + 10, { steps: 5 })
    await p.mouse.up()
    let s = await state(p)
    assert.equal(s.start, 0)
    assert.ok(s.end >= 3, String(s.end))
})

test('read-only: no typing, no undo of the history', { skip: SKIP }, async () => {
    let p = await b.open(page('readOnly: true'))
    await p.click('textarea')
    await p.keyboard.type('q')
    await p.keyboard.press('Enter')
    assert.equal((await state(p)).text, 'abc def')
})

test('TextArea of the controls: typing, Return, undo', { skip: SKIP }, async () => {
    let qml = `import QtQuick
import QtQuick.Controls
Rectangle {
    width: 600; height: 400
    property alias edit: area
    TextArea { id: area; x: 10; y: 10; width: 300; height: 200; text: "one" }
}`
    let p = await b.open(qml)
    await p.click('textarea')
    await p.evaluate(() => { window.__root.edit.cursorPosition = 3 })
    await p.keyboard.type(' two')
    await p.keyboard.press('Enter')
    await p.keyboard.type('three')
    assert.equal(await p.evaluate(() => window.__root.edit.text), 'one two\nthree')
    // the word typed after the Return is a step of its own
    await p.keyboard.press('ControlOrMeta+z')
    assert.equal(await p.evaluate(() => window.__root.edit.text), 'one two\n')
    await p.keyboard.press('ControlOrMeta+z')
    assert.equal(await p.evaluate(() => window.__root.edit.text), 'one two')
    assert.deepEqual(p.errors, [])
})
