// Run: node --test tests/unit
// MouseArea hover and position as Qt 6.8 delivers them (qtdeclarative 6.8: qquickmousearea.cpp, qquickdeliveryagent.cpp):
// - mouseX / mouseY return lastPos, stored before entered / pressed are emitted (saveEvent, hoverEnterEvent)
// - hoverEnterEvent: lastPos, entered, mouseXChanged, mouseYChanged, positionChanged
// - itemChange(ItemVisibleHasChanged): an area hidden under the pointer is no longer hovered; one shown under it gets
//   the hover delivered at the pointer and is hovered when it is among the hovered items (not when covered)
// - deliverHoverEvent: an item still hovered gets HoverMove, an item no longer under the pointer HoverLeave
// - flushFrameSynchronousEvents: with no mouse grabber, a last pointer position and dirty items, the pointer gets a
//   hover event at its last position with the next frame
// The cases run on the sources (Runtime/) and on the built runtime the bundles take (dist/core.js).
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

const LeftButton = 1

function geometry(item, x, y, width, height){
    item.x = x
    item.y = y
    item.width = width
    item.height = height
    return item
}

function cases(name, runtime){
    const { engine, Item, MouseArea, delivery } = runtime

    function reset(){
        delivery.leaveWindow()
        delivery.scene.roots.length = 0
        delivery.scene.grabber = null
        delivery.scene.hovered = []
        engine.flushFrame()
    }

    function window(){
        reset()
        let root
        engine.batch(() => { root = geometry(new Item(null), 0, 0, 400, 300) })
        delivery.scene.roots.push(root)
        return root
    }

    function area(parent, x, y, width, height, setup){
        let it
        engine.batch(() => {
            it = geometry(new MouseArea(parent), x, y, width, height)
            it.hoverEnabled = true
            if(setup) setup(it)
        })
        return it
    }

    // the signals of an area in order, with what mouseX / mouseY read while each is handled
    function record(it){
        let log = []
        let names = ['entered', 'exited', 'positionChanged', 'mouseXChanged', 'mouseYChanged', 'pressed', 'containsMouseChanged']
        for(let signal of names){
            engine.connect(it, MouseArea['SIGNAL_' + signal], () => log.push(signal + '@' + it.mouseX + ',' + it.mouseY))
        }
        return log
    }

    const signals = (log, signal) => log.filter(entry => entry.startsWith(signal + '@'))

    test(name + ': onEntered reads the position the pointer entered at', () => {
        let root = window()
        let it = area(root, 100, 100, 100, 100)
        let log = record(it)
        delivery.move(130, 140, 0, 0)
        assert.deepEqual(signals(log, 'entered'), ['entered@30,40'])
        assert.equal(it.containsMouse, true)
    })

    test(name + ': entering emits entered, mouseXChanged, mouseYChanged and positionChanged in this order', () => {
        let root = window()
        let it = area(root, 100, 100, 100, 100)
        let log = record(it)
        delivery.move(130, 140, 0, 0)
        assert.deepEqual(log.filter(entry => !entry.startsWith('containsMouseChanged')),
            ['entered@30,40', 'mouseXChanged@30,40', 'mouseYChanged@30,40', 'positionChanged@30,40'])
    })

    test(name + ': a move over the area emits positionChanged, a move to the same point does not', () => {
        let root = window()
        let it = area(root, 100, 100, 100, 100)
        delivery.move(130, 140, 0, 0)
        let log = record(it)
        delivery.move(131, 140, 0, 0)
        delivery.move(131, 140, 0, 0)
        assert.deepEqual(signals(log, 'positionChanged'), ['positionChanged@31,40'])
    })

    test(name + ': onPressed reads the press position', () => {
        let root = window()
        let it = area(root, 100, 100, 100, 100, (a) => { a.hoverEnabled = false })
        let log = record(it)
        delivery.press(150, 160, LeftButton, LeftButton, 0, false, null)
        delivery.release(150, 160, LeftButton, 0, 0)
        assert.deepEqual(signals(log, 'pressed'), ['pressed@50,60'])
    })

    test(name + ': an area hidden under the pointer is left, shown again under it it is entered', () => {
        let root = window()
        let it = area(root, 100, 100, 100, 100)
        delivery.move(130, 140, 0, 0)
        let log = record(it)
        engine.batch(() => { it.visible = false })
        assert.deepEqual(signals(log, 'exited'), ['exited@30,40'])
        assert.equal(it.containsMouse, false)
        engine.batch(() => { it.visible = true })
        assert.deepEqual(signals(log, 'entered'), ['entered@30,40'])
        assert.equal(it.containsMouse, true)
    })

    test(name + ': an area shown under the pointer at another place reads the pointer position in onEntered', () => {
        let root = window()
        let it = area(root, 100, 100, 100, 100, (a) => { a.visible = false })
        delivery.move(130, 140, 0, 0)
        engine.batch(() => { it.x = 120 })
        let log = record(it)
        engine.batch(() => { it.visible = true })
        assert.deepEqual(signals(log, 'entered'), ['entered@10,40'])
    })

    test(name + ': a hidden area the pointer moves onto again is entered, not just moved over', () => {
        let root = window()
        let it = area(root, 100, 100, 100, 100)
        delivery.move(130, 140, 0, 0)
        engine.batch(() => { it.visible = false })
        delivery.move(20, 20, 0, 0)
        engine.batch(() => { it.visible = true })
        let log = record(it)
        delivery.move(135, 140, 0, 0)
        assert.deepEqual(signals(log, 'entered'), ['entered@35,40'])
        assert.equal(it.containsMouse, true)
    })

    test(name + ': an area shown away from the pointer is not entered', () => {
        let root = window()
        let it = area(root, 100, 100, 100, 100, (a) => { a.visible = false })
        delivery.move(20, 20, 0, 0)
        let log = record(it)
        engine.batch(() => { it.visible = true })
        assert.deepEqual(signals(log, 'entered'), [])
    })

    test(name + ': an area that takes no hover is not entered when shown under the pointer', () => {
        let root = window()
        let it = area(root, 100, 100, 100, 100, (a) => {
            a.visible = false
            a.hoverEnabled = false
        })
        delivery.move(130, 140, 0, 0)
        let log = record(it)
        engine.batch(() => { it.visible = true })
        assert.deepEqual(signals(log, 'entered'), [])
    })

    test(name + ': an area shown above the hovered one under the pointer takes the hover from it', () => {
        let root = window()
        let below = area(root, 100, 100, 100, 100)
        let cover = area(root, 0, 0, 400, 300, (a) => { a.visible = false })
        delivery.move(130, 140, 0, 0)
        let log = record(below)
        engine.batch(() => { cover.visible = true })
        assert.equal(cover.containsMouse, true)
        assert.deepEqual(signals(log, 'exited'), ['exited@30,40'])
        assert.equal(below.containsMouse, false)
    })

    test(name + ': an area shown under the pointer below a hovered area is not entered', () => {
        let root = window()
        let below = area(root, 100, 100, 100, 100, (a) => { a.visible = false })
        let cover = area(root, 0, 0, 400, 300)
        delivery.move(130, 140, 0, 0)
        let log = record(below)
        engine.batch(() => { below.visible = true })
        assert.deepEqual(signals(log, 'entered'), [])
        assert.equal(below.containsMouse, false)
        assert.equal(cover.containsMouse, true)
    })

    test(name + ': an area that moves over the hovered one under a still pointer takes the hover with the next frame', () => {
        let root = window()
        let below = area(root, 100, 100, 100, 100)
        let cover = area(root, 300, 0, 100, 300)
        delivery.move(130, 140, 0, 0)
        let log = record(below)
        engine.batch(() => {
            cover.x = 0
            cover.width = 400
        })
        assert.deepEqual(signals(log, 'exited'), [])
        engine.flushFrame()
        assert.deepEqual(signals(log, 'exited'), ['exited@30,40'])
        assert.equal(cover.containsMouse, true)
    })

    test(name + ': an area moved under a still pointer is entered with the next frame', () => {
        let root = window()
        let it = area(root, 300, 200, 50, 50)
        delivery.move(130, 140, 0, 0)
        let log = record(it)
        engine.batch(() => {
            it.x = 100
            it.y = 100
        })
        assert.deepEqual(signals(log, 'entered'), [])
        engine.flushFrame()
        assert.deepEqual(signals(log, 'entered'), ['entered@30,40'])
    })

    test(name + ': no frame hover while an area grabs the pointer', () => {
        let root = window()
        let grabber = area(root, 0, 0, 50, 50)
        let it = area(root, 300, 200, 50, 50)
        delivery.move(10, 10, 0, 0)
        delivery.press(10, 10, LeftButton, LeftButton, 0, false, null)
        let log = record(it)
        engine.batch(() => {
            it.x = 0
            it.y = 0
        })
        engine.flushFrame()
        assert.deepEqual(signals(log, 'entered'), [])
        delivery.release(10, 10, LeftButton, 0, 0)
        assert.equal(delivery.scene.grabber, null)
        assert.ok(grabber)
    })

    test(name + ': no frame hover once the pointer left the window', () => {
        let root = window()
        let it = area(root, 300, 200, 50, 50)
        delivery.move(130, 140, 0, 0)
        delivery.leaveWindow()
        let log = record(it)
        engine.batch(() => {
            it.x = 100
            it.y = 100
        })
        engine.flushFrame()
        assert.deepEqual(signals(log, 'entered'), [])
    })

    // TabPanelDecorator: the close button of a tab is enabled while the tab or the button is hovered; the pointer rests
    // on it. Qt sends the enter of the button before the leave of the tab area below it (deliverHoverEventRecursive
    // enters the leaf, the tab area is not visited and is left after the pass), so the tab stays hovered.
    function tab(root){
        let tabArea = area(root, 0, 0, 200, 40)
        let decorator
        engine.batch(() => { decorator = geometry(new Item(root), 0, 0, 200, 40) })
        let close = area(decorator, 170, 10, 20, 20, (a) => {
            a.enabled = false
            a.opacity = 0
        })
        let state = { closeHovered: false, changes: 0 }
        // the button shows (opacity) and is enabled with the hover: a visible change, the scene is dirty
        let update = () => engine.batch(() => {
            close.enabled = tabArea.containsMouse || state.closeHovered
            close.opacity = close.enabled ? 1 : 0
        })
        engine.connect(tabArea, MouseArea.SIGNAL_containsMouseChanged, () => {
            state.changes++
            update()
        })
        engine.connect(close, MouseArea.SIGNAL_containsMouseChanged, () => {
            state.changes++
            state.closeHovered = close.containsMouse
            update()
        })
        return { tabArea, close, state }
    }

    test(name + ': a close button enabled by the hover of its tab takes the hover and the tab stays hovered', () => {
        let root = window()
        let { tabArea, close, state } = tab(root)
        delivery.move(180, 20, 0, 0)
        for(let i = 0; i < 5; i++) engine.flushFrame()
        assert.equal(close.enabled, true)
        assert.equal(close.containsMouse, true)
        assert.equal(tabArea.containsMouse, false)
        let changes = state.changes
        for(let i = 0; i < 5; i++) engine.flushFrame()
        delivery.move(181, 20, 0, 0)
        for(let i = 0; i < 5; i++) engine.flushFrame()
        assert.equal(state.changes, changes, 'the hover settles, it does not flip with every frame')
        assert.equal(close.containsMouse, true)
    })

    // the button is above the tab area: the pass visits it first and leaves it there, then enters the tab area
    test(name + ': moving from the close button back onto its tab leaves the button, enters the tab and keeps it enabled', () => {
        let root = window()
        let { tabArea, close } = tab(root)
        delivery.move(180, 20, 0, 0)
        for(let i = 0; i < 3; i++) engine.flushFrame()
        let log = []
        engine.connect(tabArea, MouseArea.SIGNAL_entered, () => log.push('tab entered'))
        engine.connect(close, MouseArea.SIGNAL_exited, () => log.push('close exited'))
        delivery.move(50, 20, 0, 0)
        for(let i = 0; i < 3; i++) engine.flushFrame()
        assert.deepEqual(log, ['close exited', 'tab entered'])
        assert.equal(tabArea.containsMouse, true)
        assert.equal(close.enabled, true)
    })

    test(name + ': an area above the one the pointer leaves is left before the one below is entered, as the pass visits them', () => {
        let root = window()
        let below = area(root, 0, 0, 100, 100)
        let above = area(root, 50, 0, 100, 100)
        delivery.move(120, 50, 0, 0)
        let log = []
        engine.connect(below, MouseArea.SIGNAL_entered, () => log.push('below entered'))
        engine.connect(above, MouseArea.SIGNAL_exited, () => log.push('above exited'))
        delivery.move(20, 50, 0, 0)
        assert.deepEqual(log, ['above exited', 'below entered'])
    })

    // FilterableSelectPopup: a hover block over the list, shown under a still pointer, ends with a real move only
    test(name + ': a block shown under the pointer ends with the first real move, not with its own enter', () => {
        let root = window()
        let row = area(root, 0, 100, 400, 40)
        let block = area(root, 0, 0, 400, 300, (a) => { a.visible = false })
        let entered = null
        let blocked = true
        engine.connect(block, MouseArea.SIGNAL_entered, () => { entered = { x: block.mouseX, y: block.mouseY } })
        engine.connect(block, MouseArea.SIGNAL_positionChanged, () => {
            if(Math.abs(block.mouseX - entered.x) >= 1 || Math.abs(block.mouseY - entered.y) >= 1) blocked = false
        })
        delivery.move(50, 110, 0, 0)
        engine.batch(() => { block.visible = true })
        engine.flushFrame()
        assert.deepEqual(entered, { x: 50, y: 110 })
        assert.equal(blocked, true)
        delivery.move(52, 112, 0, 0)
        assert.equal(blocked, false)
        engine.batch(() => { block.visible = false })
        engine.flushFrame()
        assert.equal(row.containsMouse, true)
    })
}

for(let [name, load] of runtimes) cases(name, load())
