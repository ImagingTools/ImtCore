// Run: node --test tests/unit
// The press goes down the scene as QQuickDeliveryAgent delivers it: topmost item first, an item that does not take
// it passes it on. A TextInput takes the left button (setAcceptedMouseButtons(Qt::LeftButton)), so its native
// control gets a press only when the press reaches it.
// The cases run on the sources (Runtime/) and on the built runtime the bundles take (dist/core.js).
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

const LeftButton = 1
const RightButton = 2

function geometry(item, x, y, width, height){
    item.x = x
    item.y = y
    item.width = width
    item.height = height
    return item
}

function cases(name, runtime){
    const { engine, Item, MouseArea, TextInput, TextEdit, delivery } = runtime

    // A window with a form (a field) and, declared after it, an overlay (a popup) that may cover it
    function scene(){
        delivery.scene.roots.length = 0
        delivery.scene.grabber = null
        delivery.scene.hovered = []
        let window, field, overlay
        engine.batch(() => {
            window = geometry(new Item(null), 0, 0, 400, 300)
            field = geometry(new TextInput(window), 10, 100, 200, 30)
            overlay = geometry(new Item(window), 0, 80, 400, 100)
        })
        delivery.scene.roots.push(window)
        return { window, field, overlay }
    }

    function mouseArea(parent, setup){
        let area
        engine.batch(() => {
            area = geometry(new MouseArea(parent), 0, 0, parent.width, parent.height)
            if(setup) setup(area)
        })
        return area
    }

    function counter(area, signal){
        let count = { value: 0 }
        engine.connect(area, signal, () => count.value++)
        return count
    }

    function click(x, y, native){
        let reached = delivery.press(x, y, LeftButton, LeftButton, 0, false, native)
        delivery.release(x, y, LeftButton, 0, 0)
        return reached
    }

    test(name + ': a field nothing covers takes the press', () => {
        let { field } = scene()
        assert.equal(click(50, 110, field), true)
        assert.equal(delivery.scene.grabber, null)
    })

    test(name + ': a MouseArea above the field takes the press, the field does not get it', () => {
        let { field, overlay } = scene()
        let area = mouseArea(overlay)
        let clicked = counter(area, MouseArea.SIGNAL_clicked)
        assert.equal(click(50, 110, field), false)
        assert.equal(clicked.value, 1)
    })

    test(name + ': a MouseArea above the field that takes no button lets the press through', () => {
        let { field, overlay } = scene()
        let area = mouseArea(overlay, (it) => { it.acceptedButtons = 0 })
        let pressed = counter(area, MouseArea.SIGNAL_pressed)
        assert.equal(click(50, 110, field), true)
        assert.equal(pressed.value, 0)
    })

    test(name + ': a MouseArea above the field that rejects the press in onPressed lets it through', () => {
        let { field, overlay } = scene()
        let area = mouseArea(overlay)
        engine.connect(area, MouseArea.SIGNAL_pressed, (mouse) => { mouse.accepted = false })
        let clicked = counter(area, MouseArea.SIGNAL_clicked)
        assert.equal(click(50, 110, field), true)
        assert.equal(clicked.value, 0)
    })

    test(name + ': an invisible or disabled MouseArea above the field does not cover it', () => {
        for(let property of ['visible', 'enabled']){
            let { field, overlay } = scene()
            mouseArea(overlay, (it) => { it[property] = false })
            assert.equal(click(50, 110, field), true, property)
        }
    })

    test(name + ': an item above the field that takes no mouse events does not cover it', () => {
        let { field, overlay } = scene()
        engine.batch(() => geometry(new Item(overlay), 0, 0, 400, 100))
        assert.equal(click(50, 110, field), true)
    })

    test(name + ': a MouseArea above the field elsewhere does not cover it', () => {
        let { field, overlay } = scene()
        let area
        engine.batch(() => { area = geometry(new MouseArea(overlay), 300, 0, 100, 100) })
        let pressed = counter(area, MouseArea.SIGNAL_pressed)
        assert.equal(click(50, 110, field), true)
        assert.equal(pressed.value, 0)
    })

    test(name + ': a field above a MouseArea takes the press, the MouseArea does not get it', () => {
        let { window } = scene()
        let below, field
        engine.batch(() => {
            below = geometry(new MouseArea(window), 0, 0, 400, 300)
            field = geometry(new TextInput(window), 10, 200, 200, 30)
        })
        let pressed = counter(below, MouseArea.SIGNAL_pressed)
        assert.equal(click(50, 210, field), true)
        assert.equal(pressed.value, 0)
    })

    test(name + ': a press off the native control stops at the field above a MouseArea, as Qt delivers it', () => {
        let { window, field } = scene()
        let below
        engine.batch(() => { below = geometry(new MouseArea(window), 0, 0, 400, 300) })
        engine.batch(() => { field.z = 1 })
        let pressed = counter(below, MouseArea.SIGNAL_pressed)
        assert.equal(click(50, 110, null), false)
        assert.equal(pressed.value, 0)
        assert.equal(delivery.scene.grabber, null)
    })

    test(name + ': a TextEdit takes the press like a TextInput', () => {
        let { window, overlay } = scene()
        let edit
        engine.batch(() => { edit = geometry(new TextEdit(window), 10, 250, 200, 40) })
        assert.equal(click(50, 260, edit), true)
        // the overlay is declared before the edit: z puts it above, as a popup is
        engine.batch(() => {
            overlay.height = 300
            overlay.z = 1
        })
        mouseArea(overlay)
        assert.equal(click(50, 260, edit), false)
    })

    // QQuickWindowPrivate::findCursorItemAndHandler: the cursor of the topmost item under the pointer that has one; an
    // item without a cursor does not stop the search. A TextInput has none, a TextEdit the I-beam
    // (QQuickTextEditPrivate::updateMouseCursorShape: the arrow when read-only and not selecting by mouse).
    function cursorAt(x, y){
        let element = { style: { cursor: 'unset' } }
        delivery.scene.element = element
        try {
            delivery.move(x, y, 0, 0)
        } finally {
            delivery.leaveWindow()
            delivery.scene.element = null
        }
        return element.style.cursor
    }

    test(name + ': the cursor over a field covered by a MouseArea with a cursor shape is that of the MouseArea', () => {
        let { overlay } = scene()
        mouseArea(overlay, (it) => { it.cursorShape = 'pointer' })
        assert.equal(cursorAt(50, 110), 'pointer')
    })

    test(name + ': a TextInput has no cursor of its own', () => {
        scene()
        assert.equal(cursorAt(50, 110), '')
    })

    test(name + ': a TextEdit shows the I-beam, the arrow when read-only and not selecting by mouse', () => {
        let { window } = scene()
        let edit
        engine.batch(() => { edit = geometry(new TextEdit(window), 10, 250, 200, 40) })
        assert.equal(cursorAt(50, 260), 'text')
        engine.batch(() => { edit.readOnly = true })
        assert.equal(cursorAt(50, 260), 'text')
        engine.batch(() => { edit.selectByMouse = false })
        assert.equal(cursorAt(50, 260), 'default')
    })

    // QQuickMouseArea::QQuickMouseArea: setCursor(Qt::ArrowCursor), "The default value is Qt.ArrowCursor"; a modal
    // background or a dialog header above a field shows the arrow, not the I-beam of the field below
    test(name + ': a MouseArea above a field shows its default arrow, not the I-beam of the field below', () => {
        let { window, overlay } = scene()
        let edit
        engine.batch(() => { edit = geometry(new TextEdit(window), 10, 250, 200, 40) })
        let below = mouseArea(overlay, (it) => { it.cursorShape = 'text' })
        engine.batch(() => {
            overlay.height = 300
            overlay.z = 1
        })
        engine.batch(() => geometry(new MouseArea(window), 0, 200, 400, 100).z = 2)
        assert.equal(cursorAt(50, 260), 'default')
        assert.equal(cursorAt(50, 110), 'text')
        assert.ok(edit && below)
    })

    // "If the cursorShape property is set to undefined, the MouseArea will not change the existing shape"
    test(name + ': a MouseArea with the cursor shape undefined leaves the cursor of the item below', () => {
        let { window } = scene()
        engine.batch(() => geometry(new TextEdit(window), 10, 250, 200, 40))
        engine.batch(() => { geometry(new MouseArea(window), 0, 200, 400, 100).cursorShape = undefined })
        assert.equal(cursorAt(50, 260), 'text')
    })

    test(name + ': a press while another button holds the grab goes to the grabber, not to the field', () => {
        let { field, overlay } = scene()
        let area = mouseArea(overlay)
        let pressed = counter(area, MouseArea.SIGNAL_pressed)
        delivery.press(300, 150, LeftButton, LeftButton, 0, false, null)
        assert.equal(delivery.scene.grabber, area)
        assert.equal(delivery.press(50, 110, RightButton, LeftButton | RightButton, 0, false, field), false)
        assert.equal(pressed.value, 1)
        delivery.release(50, 110, LeftButton, 0, 0)
    })
}

for(let [name, load] of runtimes) cases(name, load())
