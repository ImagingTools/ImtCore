// Run: node --test tests/web   (Playwright: npm install playwright in v5, or JQML_PLAYWRIGHT=<path to the package>)
// TextEdit with RichText in Chromium: the document drawn as DOM (fonts, colors, backgrounds, links, lists, tables,
// images), the layout against the drawn text, the mouse (press, drag, double and triple click, Shift+click), the keys
// (typing, Return, Shift+Return, Backspace, Delete, arrows, Home / End, undo), the clipboard (copy and paste of HTML),
// links (hover, activation), the cursor, an input method, read-only, a change of textFormat and the speed of append.
const test = require('node:test')
const assert = require('node:assert/strict')
const { browser, playwright } = require('./lib')

const SKIP = playwright() ? false : 'no Playwright (npm install playwright, or JQML_PLAYWRIGHT=<path>)'

const RICH = '<p>Some <b>bold</b> and <span style="color:#c00000">red</span> and <span style="background-color:#ffff00">yellow</span>'
    + ' <a href="http://example.com/x">link</a> end</p><ul><li>first item</li><li>second</li></ul>'

function page(text, extra = ''){
    return `import QtQuick
Rectangle {
    width: 600; height: 400; color: "white"
    property alias edit: edit
    property var activated: []
    property var hovered: []
    TextEdit {
        id: edit
        x: 20; y: 30; width: 400
        wrapMode: TextEdit.Wrap
        textFormat: TextEdit.RichText
        font.family: "Arial"; font.pixelSize: 16
        text: ${JSON.stringify(text)}
        onLinkActivated: (link) => parent.activated.push(link)
        onLinkHovered: (link) => parent.hovered.push(link)
        ${extra}
    }
}`
}

let b = null
test.before(async () => { if(!SKIP) b = await browser() })
test.after(async () => { if(b) await b.close() })

// the page point of a position of the TextEdit (the middle of its cursor rectangle)
async function pointOf(p, pos){
    return p.evaluate((pos) => {
        let e = window.__root.edit
        let r = e.positionToRectangle(pos)
        return { x: e.x + r.x + 0.5, y: e.y + r.y + r.height / 2 }
    }, pos)
}

const state = (p) => p.evaluate(() => {
    let e = window.__root.edit
    return { raw: e.getText(0, e.length), cursor: e.cursorPosition, start: e.selectionStart, end: e.selectionEnd, selected: e.selectedText, focus: e.activeFocus }
})

const body = (html) => html.slice(html.indexOf('<body'))

test('the document as DOM: words in their fonts and colors, backgrounds, the link, list markers; no errors', { skip: SKIP }, async () => {
    let p = await b.open(page(RICH))
    let spans = await p.evaluate(() => Array.from(document.querySelectorAll('.jq2-rich span')).map(s => {
        let cs = getComputedStyle(s)
        return { text: s.textContent, weight: cs.fontWeight, color: cs.color, decoration: cs.textDecorationLine, href: s.dataset.href || '' }
    }))
    let find = (t) => spans.find(s => s.text === t)
    assert.equal(find('bold').weight, '700')
    assert.equal(find('Some').weight, '400')
    assert.equal(find('red').color, 'rgb(192, 0, 0)')
    assert.equal(find('link').color, 'rgb(0, 0, 255)')
    assert.equal(find('link').decoration, 'underline')
    assert.equal(find('link').href, 'http://example.com/x')
    // the yellow background under "yellow"
    let yellow = await p.evaluate(() => Array.from(document.querySelectorAll('.jq2-rich div')).some(d => getComputedStyle(d).backgroundColor === 'rgb(255, 255, 0)'))
    assert.ok(yellow)
    // the bullets of the list: two discs
    let discs = await p.evaluate(() => Array.from(document.querySelectorAll('.jq2-rich div')).filter(d => d.style.borderRadius === '50%').length)
    assert.equal(discs, 2)
    // the hidden control shows nothing
    let control = await p.evaluate(() => getComputedStyle(document.querySelector('textarea')).color)
    assert.equal(control, 'rgba(0, 0, 0, 0)')
    assert.deepEqual(p.errors, [])
})

test('the layout matches the drawn text: each word where the cursor rectangles put it, as wide', { skip: SKIP }, async () => {
    let p = await b.open(page(RICH))
    let result = await p.evaluate(() => {
        let e = window.__root.edit
        let raw = e.getText(0, e.length)
        let view = document.querySelector('.jq2-rich').getBoundingClientRect()
        let out = []
        let from = 0
        for(let s of document.querySelectorAll('.jq2-rich span')){
            // the words in the order of the text (the list markers are not in it)
            let at = raw.indexOf(s.textContent, from)
            if(at < 0 || !s.textContent.trim()) continue
            from = at + s.textContent.length
            let r = s.getBoundingClientRect()
            let a = e.positionToRectangle(at), z = e.positionToRectangle(at + s.textContent.length)
            out.push({ text: s.textContent, left: r.left - view.left + e.leftPadding, x: a.x, width: r.width, layoutWidth: z.x - a.x, sameLine: a.y === z.y })
        }
        return out
    })
    for(let w of result.filter(w => w.sameLine)){
        assert.ok(Math.abs(w.left - w.x) < 1, w.text + ' at ' + w.left + ', layout ' + w.x)
        assert.ok(Math.abs(w.width - w.layoutWidth) < 1, w.text + ' ' + w.width + ' wide, layout ' + w.layoutWidth)
    }
})

test('a press puts the cursor where it is; a drag selects; the selection is drawn with the selected text color', { skip: SKIP }, async () => {
    let p = await b.open(page(RICH))
    let at = await pointOf(p, 7)
    await p.mouse.click(at.x, at.y)
    let s = await state(p)
    assert.equal(s.cursor, 7)
    assert.ok(s.focus)
    let from = await pointOf(p, 5), to = await pointOf(p, 9)
    await p.mouse.move(from.x, from.y)
    await p.mouse.down()
    await p.mouse.move(to.x, to.y, { steps: 4 })
    await p.mouse.up()
    s = await state(p)
    assert.deepEqual([s.start, s.end, s.selected], [5, 9, 'bold'])
    let selected = await p.evaluate(() => Array.from(document.querySelectorAll('.jq2-rich span')).filter(sp => getComputedStyle(sp).color === 'rgb(255, 255, 255)').map(sp => sp.textContent).join(''))
    assert.equal(selected, 'bold')
    // Shift+click moves the cursor, the anchor stays
    let further = await pointOf(p, 13)
    await p.keyboard.down('Shift')
    await p.mouse.click(further.x, further.y)
    await p.keyboard.up('Shift')
    s = await state(p)
    assert.deepEqual([s.start, s.end], [5, 13])
})

test('a double click selects the word, a triple click the block', { skip: SKIP }, async () => {
    let p = await b.open(page(RICH))
    let at = await pointOf(p, 6)
    await p.mouse.dblclick(at.x, at.y)
    assert.equal((await state(p)).selected, 'bold')
    await p.mouse.click(at.x, at.y, { clickCount: 3 })
    let s = await state(p)
    assert.equal(s.start, 0)
    assert.equal(s.selected, s.raw.split('\u2029')[0])
})

test('typing takes the format before the cursor; Return, Shift+Return, Backspace and Delete edit the document', { skip: SKIP }, async () => {
    let p = await b.open(page('<b>Hi</b> there'))
    let at = await pointOf(p, 1)
    await p.mouse.click(at.x, at.y)
    await p.keyboard.type('xy')
    let html = await p.evaluate(() => window.__root.edit.text)
    assert.ok(body(html).includes('<span style=" font-weight:700;">Hxyi</span> there'), body(html))
    await p.keyboard.press('End')
    await p.keyboard.press('Enter')
    await p.keyboard.type('z')
    await p.keyboard.press('Shift+Enter')
    await p.keyboard.type('w')
    let s = await state(p)
    assert.equal(s.raw, 'Hxyi there\u2029z\u2028w')
    await p.keyboard.press('Backspace')
    await p.keyboard.press('Backspace')
    await p.keyboard.press('Home')
    await p.keyboard.press('Backspace')
    s = await state(p)
    assert.equal(s.raw, 'Hxyi therez')
    assert.equal(s.cursor, 10)
    await p.keyboard.press('Delete')
    assert.equal((await state(p)).raw, 'Hxyi there')
    // the drawn text follows
    let drawn = await p.evaluate(() => Array.from(document.querySelectorAll('.jq2-rich span')).map(s => s.textContent).join(' '))
    assert.equal(drawn, 'Hxyi there')
})

test('the arrows and Home / End move by the layout; Up and Down keep the column', { skip: SKIP }, async () => {
    let p = await b.open(page('<p>first line here</p><p>second line here</p>'))
    let at = await pointOf(p, 6)
    await p.mouse.click(at.x, at.y)
    // the position under the column in the line below, as positionAt finds it
    let below = await p.evaluate(() => {
        let e = window.__root.edit
        let a = e.positionToRectangle(6), b = e.positionToRectangle(16)
        return e.positionAt(a.x, b.y + b.height / 2)
    })
    assert.ok(below > 16 && below < 26, String(below))
    await p.keyboard.press('ArrowDown')
    assert.equal((await state(p)).cursor, below)
    await p.keyboard.press('ArrowUp')
    assert.equal((await state(p)).cursor, 6)
    await p.keyboard.press('End')
    assert.equal((await state(p)).cursor, 15)
    await p.keyboard.press('Home')
    assert.equal((await state(p)).cursor, 0)
    await p.keyboard.press('Shift+ArrowDown')
    let s = await state(p)
    assert.deepEqual([s.start, s.end], [0, 16])
    // Right with a selection: to its end, then on by a character
    await p.keyboard.press('ArrowRight')
    assert.equal((await state(p)).cursor, 16)
    await p.keyboard.press('ArrowRight')
    assert.equal((await state(p)).cursor, 17)
    await p.keyboard.press('ArrowLeft')
    await p.keyboard.press('Shift+ArrowLeft')
    s = await state(p)
    assert.deepEqual([s.start, s.end, s.cursor], [15, 16, 15])
})

test('Backspace at the start of a list item takes it out of the list, the marker goes', { skip: SKIP }, async () => {
    let p = await b.open(page('<ul><li>one</li><li>two</li></ul>'))
    let at = await pointOf(p, 4)
    await p.mouse.click(at.x - 1, at.y)
    assert.equal((await state(p)).cursor, 4)
    await p.keyboard.press('Backspace')
    let discs = await p.evaluate(() => Array.from(document.querySelectorAll('.jq2-rich div')).filter(d => d.style.borderRadius === '50%').length)
    assert.equal(discs, 1)
    assert.equal((await state(p)).raw, 'one\u2029two')
})

test('undo and redo with the keys', { skip: SKIP }, async () => {
    let p = await b.open(page('<i>ab</i>'))
    let at = await pointOf(p, 2)
    await p.mouse.click(at.x, at.y)
    await p.keyboard.type('cd')
    assert.equal((await state(p)).raw, 'abcd')
    await p.keyboard.press('ControlOrMeta+z')
    assert.equal((await state(p)).raw, 'ab')
    await p.keyboard.press('ControlOrMeta+Shift+z')
    assert.equal((await state(p)).raw, 'abcd')
})

test('copy gives the plain text and the HTML of the selection; a paste of HTML is rich text', { skip: SKIP }, async () => {
    let p = await b.open(page('<p>plain <b>bold</b></p><p>next</p>'), { clipboard: true })
    let at = await pointOf(p, 6)
    await p.mouse.click(at.x, at.y)
    await p.evaluate(() => window.__root.edit.select(6, 12))
    await p.keyboard.press('ControlOrMeta+c')
    let clip = await p.evaluate(async () => {
        let items = await navigator.clipboard.read()
        let out = {}
        for(let item of items) for(let type of item.types) out[type] = await (await item.getType(type)).text()
        return out
    })
    assert.equal(clip['text/plain'], 'bold\nn')
    // the browser gives the HTML back sanitized: the bold span in its own words
    assert.ok(/<span style="font-weight: 700;">bold<\/span>/.test(clip['text/html']), clip['text/html'])
    await p.evaluate(async () => {
        let html = new Blob(['<i>it</i>'], { type: 'text/html' })
        let text = new Blob(['it'], { type: 'text/plain' })
        await navigator.clipboard.write([new ClipboardItem({ 'text/html': html, 'text/plain': text })])
        window.__root.edit.cursorPosition = 0
    })
    await p.keyboard.press('ControlOrMeta+v')
    let html = await p.evaluate(() => window.__root.edit.text)
    assert.ok(body(html).includes('<span style=" font-style:italic;">it</span>plain'), body(html))
})

test('a link: the hand and hoveredLink over it, its activation by a click', { skip: SKIP }, async () => {
    let p = await b.open(page('go <a href="http://example.com/y">there</a> now'))
    let at = await pointOf(p, 5)
    await p.mouse.move(at.x, at.y)
    let hover = await p.evaluate(() => [window.__root.edit.hoveredLink, getComputedStyle(document.querySelector('textarea')).cursor, window.__root.hovered.join(',')])
    assert.deepEqual(hover, ['http://example.com/y', 'pointer', 'http://example.com/y'])
    await p.mouse.click(at.x, at.y)
    assert.deepEqual(await p.evaluate(() => window.__root.activated.slice()), ['http://example.com/y'])
    let off = await pointOf(p, 1)
    await p.mouse.move(off.x, off.y)
    assert.equal(await p.evaluate(() => window.__root.edit.hoveredLink), '')
})

test('the cursor: a blinking bar at cursorRectangle while focused, none when read-only', { skip: SKIP }, async () => {
    let p = await b.open(page('abc def'))
    let at = await pointOf(p, 4)
    await p.mouse.click(at.x, at.y)
    let caret = await p.evaluate(() => {
        let c = document.querySelector('.jq2-rich-caret')
        let e = window.__root.edit
        let r = e.cursorRectangle
        return { shown: c && c.style.display !== 'none', left: parseFloat(c.style.left), x: r.x, animation: getComputedStyle(c).animationName }
    })
    assert.ok(caret.shown)
    assert.ok(Math.abs(caret.left - caret.x) < 0.01)
    assert.equal(caret.animation, 'jq2-rich-blink')
    await p.evaluate(() => { window.__root.edit.readOnly = true })
    await p.waitForTimeout(50)
    assert.equal(await p.evaluate(() => document.querySelector('.jq2-rich-caret').style.display), 'none')
    // read-only: typing does nothing
    await p.keyboard.type('x')
    assert.equal((await state(p)).raw, 'abc def')
})

test('an input method composes into the document', { skip: SKIP }, async () => {
    let p = await b.open(page('<b>ab</b>'))
    let at = await pointOf(p, 1)
    await p.mouse.click(at.x, at.y)
    let cdp = await p.context().newCDPSession(p)
    await cdp.send('Input.imeSetComposition', { text: 'ç', selectionStart: 1, selectionEnd: 1 })
    await cdp.send('Input.insertText', { text: 'é' })
    let s = await state(p)
    assert.equal(s.raw, 'aéb')
    assert.equal(s.cursor, 2)
    assert.ok(body(await p.evaluate(() => window.__root.edit.text)).includes('font-weight:700;">aéb</span>'))
})

test('a table: cells drawn with borders, typing in a cell, Backspace at a cell start keeps the cell', { skip: SKIP }, async () => {
    let p = await b.open(page('<table border="1" cellpadding="2"><tr><td>a</td><td bgcolor="#ccffcc">b</td></tr></table>'))
    let cells = await p.evaluate(() => Array.from(document.querySelectorAll('.jq2-rich div')).filter(d => d.style.borderStyle || d.style.border).length)
    assert.ok(cells >= 3, String(cells))
    let green = await p.evaluate(() => Array.from(document.querySelectorAll('.jq2-rich div')).some(d => getComputedStyle(d).backgroundColor === 'rgb(204, 255, 204)'))
    assert.ok(green)
    let at = await pointOf(p, 4)
    await p.mouse.click(at.x, at.y)
    assert.equal((await state(p)).cursor, 4)
    await p.keyboard.type('X')
    assert.equal((await state(p)).raw, '\ufdd0a\ufdd0bX\ufdd1')
    await p.keyboard.press('Home')
    await p.keyboard.press('Backspace')
    assert.equal((await state(p)).raw, '\ufdd0a\ufdd0bX\ufdd1')
})

test('an image: drawn at its size, the layout takes the natural size of a loaded image', { skip: SKIP }, async () => {
    // a 4 x 2 PNG
    let png = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAQAAAACCAIAAADwyuo0AAAAEElEQVR4nGP4z8AARwzIHABvqgf5gNwAKAAAAABJRU5ErkJggg=='
    let p = await b.open(page('a<img src="' + png + '" width="20" height="10">b<img src="' + png + '">c'))
    await p.waitForTimeout(200)
    let images = await p.evaluate(() => Array.from(document.querySelectorAll('.jq2-rich img')).map(i => [i.style.width, i.style.height, i.complete]))
    assert.deepEqual(images.map(i => i.slice(0, 2)), [['20px', '10px'], ['4px', '2px']])
    let s = await state(p)
    assert.equal(s.raw, 'a\ufffcb\ufffcc')
    let x = await p.evaluate(() => [window.__root.edit.positionToRectangle(2).x - window.__root.edit.positionToRectangle(1).x])
    assert.equal(x[0], 20)
})

test('a terminal line: the cursor block, a no-break space on a background, stays drawn at the end', { skip: SKIP }, async () => {
    let p = await b.open(page('<p style="background-color:#000000; color:#ffffff">$ ls<span style="background-color:#ffffff">&nbsp;</span></p>'))
    let block = await p.evaluate(() => {
        let e = window.__root.edit
        let r = e.positionToRectangle(4)
        let view = document.querySelector('.jq2-rich').getBoundingClientRect()
        return Array.from(document.querySelectorAll('.jq2-rich div')).filter(d => getComputedStyle(d).backgroundColor === 'rgb(255, 255, 255)').map(d => {
            let rect = d.getBoundingClientRect()
            return { x: rect.left - view.left, width: rect.width, at: r.x }
        })
    })
    assert.equal(block.length, 1)
    assert.ok(block[0].width > 2)
    assert.ok(Math.abs(block[0].x - block[0].at) < 1)
})

test('textFormat: back to plain text the control shows the HTML itself', { skip: SKIP }, async () => {
    let p = await b.open(page('<b>bold</b>'))
    await p.evaluate(() => { window.__root.edit.textFormat = 0 })
    await p.waitForTimeout(50)
    let shown = await p.evaluate(() => {
        let t = document.querySelector('textarea')
        return { view: !!document.querySelector('.jq2-rich'), color: getComputedStyle(t).color, value: t.value.slice(0, 15) }
    })
    assert.deepEqual(shown, { view: false, color: 'rgb(0, 0, 0)', value: '<!DOCTYPE HTML ' })
})

test('a width that changes wraps the text again', { skip: SKIP }, async () => {
    let p = await b.open(page('one two three four five six seven eight nine ten'))
    let lines = await p.evaluate(() => window.__root.edit.lineCount)
    assert.equal(lines, 1)
    await p.evaluate(() => { window.__root.edit.width = 80 })
    let narrow = await p.evaluate(() => [window.__root.edit.lineCount, document.querySelectorAll('.jq2-rich span').length])
    assert.ok(narrow[0] >= 4, String(narrow[0]))
    assert.equal(narrow[1], 10)
})

test('appending 500 lines as a terminal does stays fast', { skip: SKIP }, async () => {
    let p = await b.open(page(''))
    let ms = await p.evaluate(async () => {
        let e = window.__root.edit
        let t0 = performance.now()
        for(let i = 0; i < 500; i++){
            e.append('<span style="color:#c0c0c0">line ' + i + ' of the output</span>')
            if(i % 50 === 0) await new Promise(r => requestAnimationFrame(r))
        }
        await new Promise(r => requestAnimationFrame(r))
        return performance.now() - t0
    })
    assert.equal(await p.evaluate(() => window.__root.edit.lineCount), 500)
    assert.ok(ms < 5000, ms + ' ms')
    assert.deepEqual(p.errors, [])
})

test('padding and verticalAlignment move the drawn document as the cursor rectangles say', { skip: SKIP }, async () => {
    let p = await b.open(page('<b>x</b>', 'height: 100; padding: 7; leftPadding: 11; verticalAlignment: TextEdit.AlignBottom'))
    let r = await p.evaluate(() => {
        let e = window.__root.edit
        let span = document.querySelector('.jq2-rich span').getBoundingClientRect()
        let rect = e.positionToRectangle(0)
        return { spanLeft: span.left, left: e.x + rect.x, bottom: e.y + rect.y + rect.height, limit: e.y + e.height - 7 }
    })
    assert.ok(Math.abs(r.spanLeft - r.left) < 1, JSON.stringify(r))
    assert.equal(r.left, 20 + 11)
    assert.ok(Math.abs(r.bottom - r.limit) < 1, JSON.stringify(r))
})

test('linkAt in the TextEdit: the link under a point', { skip: SKIP }, async () => {
    let p = await b.open(page('go <a href="http://example.com/z">there</a>'))
    let links = await p.evaluate(() => {
        let e = window.__root.edit
        let r = e.positionToRectangle(5), s = e.positionToRectangle(1)
        return [e.linkAt(r.x + 1, r.y + 2), e.linkAt(s.x + 1, s.y + 2)]
    })
    assert.deepEqual(links, ['http://example.com/z', ''])
})

test('the selection shows while focused; persistentSelection keeps it shown without the focus', { skip: SKIP }, async () => {
    let qml = `import QtQuick
Rectangle {
    width: 600; height: 300
    property alias edit: edit
    TextEdit { id: edit; x: 10; y: 10; width: 300; textFormat: TextEdit.RichText; text: "<b>select me</b>"; font.pixelSize: 16 }
    TextEdit { id: other; objectName: "other"; x: 10; y: 100; width: 300; text: "other" }
}`
    let p = await b.open(qml)
    let shown = () => p.evaluate(() => Array.from(document.querySelectorAll('.jq2-rich div')).some(d => getComputedStyle(d).backgroundColor === 'rgb(0, 120, 215)'))
    await p.evaluate(() => { window.__root.edit.forceActiveFocus(); window.__root.edit.select(0, 6) })
    await p.waitForTimeout(30)
    assert.equal(await shown(), true)
    await p.mouse.click(30, 105)
    await p.waitForTimeout(30)
    assert.equal(await shown(), false)
    await p.evaluate(() => { window.__root.edit.persistentSelection = true; window.__root.edit.forceActiveFocus(); window.__root.edit.select(0, 6) })
    await p.mouse.click(30, 105)
    await p.waitForTimeout(30)
    assert.equal(await shown(), true)
})

test('a cursorDelegate is the cursor: no bar of its own, the delegate at the cursor rectangle', { skip: SKIP }, async () => {
    let p = await b.open(page('abc', 'cursorDelegate: Rectangle { objectName: "caret"; width: 3; color: "red" }'))
    let at = await pointOf(p, 2)
    await p.mouse.click(at.x, at.y)
    let r = await p.evaluate(() => {
        let e = window.__root.edit
        let own = document.querySelector('.jq2-rich-caret')
        let delegate = e.children.find(c => c.objectName === 'caret')
        return { own: own ? own.style.display : 'none', x: delegate.x, rect: e.cursorRectangle.x }
    })
    assert.equal(r.own, 'none')
    assert.equal(r.x, r.rect)
})

test('Tab types a tab, Ctrl+A selects all, cut takes the selection to the clipboard', { skip: SKIP }, async () => {
    let p = await b.open(page('<i>ab</i>'), { clipboard: true })
    let at = await pointOf(p, 1)
    await p.mouse.click(at.x, at.y)
    await p.keyboard.press('Tab')
    assert.equal((await state(p)).raw, 'a\tb')
    await p.keyboard.press('ControlOrMeta+a')
    let s = await state(p)
    assert.deepEqual([s.start, s.end], [0, 3])
    await p.keyboard.press('ControlOrMeta+x')
    assert.equal((await state(p)).raw, '')
    assert.equal(await p.evaluate(() => navigator.clipboard.readText()), 'a\tb')
})

test('a mouse drag over table cells selects the cells between, typing clears them', { skip: SKIP }, async () => {
    let p = await b.open(page('<table><tr><td>aa</td><td>bb</td></tr><tr><td>cc</td><td>dd</td></tr></table>'))
    // the cells: aa at 1, bb at 4, cc at 7, dd at 10
    let from = await pointOf(p, 2), to = await pointOf(p, 8)
    await p.mouse.move(from.x, from.y)
    await p.mouse.down()
    await p.mouse.move(to.x, to.y, { steps: 5 })
    await p.mouse.up()
    let s = await state(p)
    assert.equal(s.selected, 'aacc')
    await p.keyboard.type('Z')
    assert.equal((await state(p)).raw, '\ufdd0\ufdd0bb\ufdd0Z\ufdd0dd\ufdd1')
})
