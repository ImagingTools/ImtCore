// Run: node --test tests/unit
// The Drag attached property and DropArea as qquickdrag.cpp, qquickdroparea.cpp and
// QQuickDeliveryAgentPrivate::deliverDragEvent: an active drag enters the topmost DropArea under its hot spot, moves
// in it, leaves it for the next one, drops on it; keys filter the areas; cancel leaves.
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

const CopyAction = 1, MoveAction = 2

function geometry(item, x, y, width, height){
    item.x = x
    item.y = y
    item.width = width
    item.height = height
    return item
}

const settle = () => new Promise((resolve) => queueMicrotask(resolve))

function cases(name, runtime){
    const { engine, Item, DropArea, delivery } = runtime

    // a window with two drop areas, one above the other, and a dragged item
    function scene(){
        delivery.scene.roots.length = 0
        let window, upper, lower, dragged
        let log = []
        engine.batch(() => {
            window = geometry(new Item(null), 0, 0, 400, 300)
            upper = geometry(new DropArea(window), 0, 0, 200, 100)
            lower = geometry(new DropArea(window), 0, 100, 200, 100)
            dragged = geometry(new Item(window), 10, 10, 20, 20)
            dragged.Drag.hotSpot.x = 10
            dragged.Drag.hotSpot.y = 10
        })
        delivery.scene.roots.push(window)
        for(let [area, tag] of [[upper, 'upper'], [lower, 'lower']]){
            engine.connect(area, DropArea.SIGNAL_entered, (drag) => log.push(tag + ' entered ' + drag.x + ',' + drag.y))
            engine.connect(area, DropArea.SIGNAL_exited, () => log.push(tag + ' exited'))
            engine.connect(area, DropArea.SIGNAL_positionChanged, (drag) => log.push(tag + ' moved ' + drag.x + ',' + drag.y))
        }
        return { window, upper, lower, dragged, log }
    }

    test(name + ': an active drag enters the DropArea under its hot spot, the area contains it', () => {
        let { upper, dragged, log } = scene()
        engine.batch(() => { dragged.Drag.active = true })
        assert.deepEqual(log, ['upper entered 20,20'])
        assert.equal(upper.containsDrag, true)
        assert.equal(upper.drag.source, dragged)
        assert.equal(upper.drag.x, 20)
        assert.equal(dragged.Drag.target, upper)
    })

    test(name + ': moving the item moves in the area, then leaves it for the area below', async () => {
        let { upper, lower, dragged, log } = scene()
        engine.batch(() => { dragged.Drag.active = true })
        engine.batch(() => { dragged.y = 40 })
        await settle()
        engine.batch(() => { dragged.y = 140 })
        await settle()
        // the former target is left before the new one is entered
        assert.deepEqual(log, ['upper entered 20,20', 'upper moved 20,50', 'upper exited', 'lower entered 20,50'])
        assert.equal(upper.containsDrag, false)
        assert.equal(lower.containsDrag, true)
        assert.equal(dragged.Drag.target, lower)
    })

    test(name + ': setting active to false cancels: the area is left, there is no target', () => {
        let { upper, dragged, log } = scene()
        engine.batch(() => { dragged.Drag.active = true })
        engine.batch(() => { dragged.Drag.active = false })
        assert.deepEqual(log, ['upper entered 20,20', 'upper exited'])
        assert.equal(upper.containsDrag, false)
        assert.equal(dragged.Drag.target, null)
    })

    test(name + ': drop() gives the action the area accepted, IgnoreAction when it does not accept', () => {
        let { upper, dragged } = scene()
        engine.connect(upper, DropArea.SIGNAL_dropped, (drop) => drop.accept(CopyAction))
        engine.batch(() => { dragged.Drag.active = true })
        let action
        engine.batch(() => { action = dragged.Drag.drop() })
        assert.equal(action, CopyAction)
        assert.equal(dragged.Drag.active, false)
        assert.equal(dragged.Drag.target, upper)
        assert.equal(upper.containsDrag, false)
        let other = scene()
        engine.batch(() => { other.dragged.Drag.active = true })
        engine.batch(() => { action = other.dragged.Drag.drop() })
        assert.equal(action, 0)
        assert.equal(other.dragged.Drag.target, null)
    })

    test(name + ': an area whose keys the drag lacks is passed over, as is one whose handler rejects the drag', () => {
        let { upper, lower, dragged, log } = scene()
        engine.batch(() => {
            upper.keys = ['text/plain']
            lower.y = 0
            lower.z = -1
        })
        engine.batch(() => { dragged.Drag.active = true })
        assert.deepEqual(log, ['lower entered 20,20'])
        engine.batch(() => { dragged.Drag.active = false })
        engine.batch(() => { dragged.Drag.keys = ['text/plain'] })
        engine.batch(() => { dragged.Drag.active = true })
        assert.equal(dragged.Drag.target, upper)
        engine.batch(() => { dragged.Drag.active = false })
        engine.connect(upper, DropArea.SIGNAL_entered, (drag) => { drag.accepted = false })
        engine.batch(() => { dragged.Drag.active = true })
        assert.equal(dragged.Drag.target, lower)
    })

    test(name + ': the default source is the item, the actions Copy, Move and Link, Move proposed; Drag.YAxis', () => {
        let { dragged } = scene()
        assert.equal(dragged.Drag.source, dragged)
        assert.equal(dragged.Drag.supportedActions, 7)
        assert.equal(dragged.Drag.proposedAction, MoveAction)
        assert.equal(dragged.Drag.YAxis, 2)
    })
}

for(let [name, load] of runtimes) cases(name, load())
