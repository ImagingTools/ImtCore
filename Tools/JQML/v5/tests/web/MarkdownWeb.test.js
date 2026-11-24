// Run: node --test tests/web   (Playwright: npm install playwright in v5, or JQML_PLAYWRIGHT=<path to the package>)
// TextEdit with MarkdownText in Chromium: the document drawn as DOM (heading, emphasis, code in the fixed font, link,
// list markers, task markers, table), typing into it (text is its Markdown), cursorSelection changing the formats,
// a TextSelection of its own, getFormattedText, textFormat changes, textDocument.source loading a .md file, modified.
const test = require('node:test')
const assert = require('node:assert/strict')
const { browser, playwright } = require('./lib')

const SKIP = playwright() ? false : 'no Playwright (npm install playwright, or JQML_PLAYWRIGHT=<path>)'

const MARKDOWN = '# Title\n\nSome **bold** and *em* and `code` and [link](http://example.com/x)\n\n- one\n- [x] done\n\n| a | b |\n|---|---|\n| 1 | 2 |'

function page(text, extra = ''){
    return `import QtQuick
Rectangle {
    width: 600; height: 500; color: "white"
    property alias edit: edit
    property alias own: own
    property int changes: 0
    TextEdit {
        id: edit
        x: 20; y: 20; width: 450
        wrapMode: TextEdit.Wrap
        textFormat: TextEdit.MarkdownText
        font.family: "Arial"; font.pixelSize: 16
        text: ${JSON.stringify(text)}
        onTextChanged: parent.changes++
        TextSelection { id: own }
        ${extra}
    }
}`
}

let b = null
test.before(async () => { if(!SKIP) b = await browser() })
test.after(async () => { if(b) await b.close() })

const spans = (p) => p.evaluate(() => Array.from(document.querySelectorAll('.jq2-rich span')).map(s => {
    let cs = getComputedStyle(s)
    return { text: s.textContent, weight: cs.fontWeight, style: cs.fontStyle, family: cs.fontFamily, size: parseFloat(cs.fontSize), color: cs.color, decoration: cs.textDecorationLine }
}))

test('the document as DOM: heading, emphasis, code, link, list and task markers, a table; text is the Markdown', { skip: SKIP }, async () => {
    let p = await b.open(page(MARKDOWN))
    let all = await spans(p)
    let find = (t) => all.find(s => s.text === t)
    assert.equal(find('Title').weight, '700')
    assert.ok(find('Title').size > 16 * 1.5, 'the heading is larger')
    assert.equal(find('bold').weight, '700')
    assert.equal(find('em').style, 'italic')
    assert.match(find('code').family, /monospace/)
    assert.equal(find('link').color, 'rgb(0, 0, 255)')
    let text = await p.evaluate(() => window.__root.edit.text)
    assert.equal(text, '# Title\n\nSome **bold** and *em* and `code` and [link](http://example.com/x)\n\n- one\n- [x] done\n\n|a|b|\n|-|-|\n|1|2|\n\n\n')
    let raw = await p.evaluate(() => { let e = window.__root.edit; return e.getText(0, e.length) })
    assert.ok(raw.startsWith('Title\u2029Some bold'))
    // the markers: a disc for "one", a checked box (no disc) for "done"; the cells of the table drawn
    let markers = await p.evaluate(() => {
        let all = Array.from(document.querySelectorAll('.jq2-rich div'))
        let discs = all.filter(d => getComputedStyle(d).borderRadius === '50%' && getComputedStyle(d).backgroundColor === 'rgb(0, 0, 0)')
        let checks = Array.from(document.querySelectorAll('.jq2-rich-check')).map(c => ({ checked: c.dataset.checked, lines: c.querySelectorAll('line').length }))
        return { discs: discs.length, checks }
    })
    assert.deepEqual(markers, { discs: 1, checks: [{ checked: '1', lines: 2 }] })
    let view = await p.evaluate(() => document.querySelector('.jq2-rich').textContent)
    assert.ok(view.includes('1') && view.includes('2'))
    assert.deepEqual(p.errors, [])
})

test('typing into a Markdown text: text is the Markdown of the document, in the format before the cursor', { skip: SKIP }, async () => {
    let p = await b.open(page('Some **bold** text'))
    await p.click('textarea')
    // after "bol"
    await p.evaluate(() => { window.__root.edit.cursorPosition = 8 })
    await p.keyboard.type('XY')
    assert.equal(await p.evaluate(() => window.__root.edit.text), 'Some **bolXYd** text\n\n')
    await p.keyboard.press('End')
    await p.keyboard.press('Enter')
    await p.keyboard.type('next')
    assert.equal(await p.evaluate(() => window.__root.edit.text), 'Some **bolXYd** text\n\nnext\n\n')
    // undo: the typed word (the empty block after it is no Markdown)
    await p.keyboard.press('ControlOrMeta+z')
    assert.equal(await p.evaluate(() => window.__root.edit.text), 'Some **bolXYd** text\n\n')
})

test('cursorSelection: the font and color of the selected text; a TextSelection of its own; drawn at once', { skip: SKIP }, async () => {
    let p = await b.open(page('alpha beta gamma'))
    let text = await p.evaluate(() => {
        let e = window.__root.edit
        e.select(6, 10)
        e.cursorSelection.font.bold = true
        e.select(0, 5)
        e.cursorSelection.font.italic = true
        return e.text
    })
    assert.equal(text, '*alpha* **beta** gamma\n\n')
    let all = await spans(p)
    assert.equal(all.find(s => s.text === 'beta').weight, '700')
    assert.equal(all.find(s => s.text === 'alpha').style, 'italic')
    text = await p.evaluate(() => {
        let own = window.__root.own
        own.selectionStart = 11
        own.selectionEnd = 16
        own.font.strikeout = true
        return [window.__root.edit.text, window.__root.edit.selectionStart, window.__root.edit.selectionEnd, own.text]
    })
    assert.deepEqual(text, ['*alpha* **beta** ~~gamma~~\n\n', 0, 5, 'gamma'])
    assert.equal((await spans(p)).find(s => s.text === 'gamma').decoration, 'line-through')
})

test('getFormattedText gives Markdown; textFormat RichText shows the HTML of the document, PlainText its Markdown', { skip: SKIP }, async () => {
    let p = await b.open(page('**b** and *i*'))
    let r = await p.evaluate(() => {
        let e = window.__root.edit
        let part = e.getFormattedText(0, 1)
        e.textFormat = 1
        let html = e.text
        e.textFormat = 3
        let md = e.text
        e.textFormat = 0
        return { part, html, md, plain: e.text, value: document.querySelector('textarea').value }
    })
    assert.equal(r.part, '**b**\n\n')
    assert.match(r.html, /<span style=" font-weight:700;">b<\/span> and <span style=" font-style:italic;">i<\/span>/)
    assert.equal(r.md, '**b** and *i*\n\n')
    assert.equal(r.plain, '**b** and *i*\n\n')
    assert.equal(r.value, '**b** and *i*\n\n')
})

test('textDocument.source loads a .md file as Markdown; modified after an edit, not after the load', { skip: SKIP }, async () => {
    let p = await b.open(page('', 'textDocument.source: "http://jqml.test/notes.md"'), {})
    await p.route('http://jqml.test/**', (route) => route.fulfill({ status: 200, contentType: 'text/markdown', headers: { 'Access-Control-Allow-Origin': '*' }, body: '# Notes\n\n- [ ] first\n- second' }))
    // the request of the page went before the route: load again
    await p.evaluate(() => {
        let d = window.__root.edit.textDocument
        d.source = ''
        d.source = 'http://jqml.test/notes.md'
    })
    await p.waitForFunction(() => window.__root.edit.textDocument.status === 2)
    let r = await p.evaluate(() => {
        let e = window.__root.edit
        return { text: e.text, modified: e.textDocument.modified, cursor: e.cursorPosition }
    })
    assert.deepEqual(r, { text: '# Notes\n\n- [ ] first\n- second\n', modified: false, cursor: 0 })
    assert.equal((await spans(p)).find(s => s.text === 'Notes').weight, '700')
    await p.click('textarea')
    await p.evaluate(() => { window.__root.edit.cursorPosition = 0 })
    await p.keyboard.type('My ')
    r = await p.evaluate(() => ({ text: window.__root.edit.text, modified: window.__root.edit.textDocument.modified }))
    assert.deepEqual(r, { text: '# My Notes\n\n- [ ] first\n- second\n', modified: true })
})
