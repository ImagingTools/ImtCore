// Run: node --test tests/unit
// Flickable, ListView and GridView as qquickflickable.cpp, qquickitemview.cpp, qquicklistview.cpp, qquickgridview.cpp
// (Qt 6.8): dragging past the threshold (taken from a child MouseArea, not from one that prevents stealing), flicking
// with deceleration, bounds behaviour and the return to bounds, snapping; header and footer of a ListView, BottomToTop,
// sections, the highlight, the attached view / isCurrentItem, key navigation, the current item brought into view.
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

const LeftButton = 1

function cases(name, runtime){
    const { engine, Item, MouseArea, delivery, types, flickable } = runtime
    const { ListView, GridView, Flickable, Component } = types

    // a manual clock: frames run when the time is advanced
    let time = 0
    let frames = []
    flickable.clock.now = () => time
    flickable.clock.frame = (fn) => frames.push(fn)
    function advance(ms){
        let end = time + ms
        while(time < end){
            time = Math.min(end, time + 16)
            let run = frames
            frames = []
            for(let fn of run) fn()
        }
    }
    function settle(){
        for(let i = 0; i < 500 && frames.length; i++) advance(16)
    }

    function component(build){
        return new Component(null, (parent, ctx) => {
            let item
            engine.batch(() => {
                item = build(parent, ctx)
                if(ctx) item.$ctx = ctx
            })
            return item
        })
    }

    const box = (height, width = 100) => component((parent) => {
        let item = new Item(parent)
        item.width = width
        item.height = height
        return item
    })

    function window(){
        delivery.scene.roots.length = 0
        delivery.scene.grabber = null
        delivery.scene.hovered = []
        let root
        engine.batch(() => {
            root = new Item(null)
            root.width = 400
            root.height = 400
        })
        delivery.scene.roots.push(root)
        return root
    }

    function list(setup){
        let root = window()
        let view
        engine.batch(() => {
            view = new ListView(root)
            view.width = 100
            view.height = 100
            view.cacheBuffer = 0
            view.delegate = box(20)
            view.model = 10
            if(setup) setup(view)
        })
        engine.batch(() => view.forceLayout())
        return view
    }

    // the pointer: press, moves 16 ms apart, release
    function drag(points, releaseAfter = 16){
        let [x, y] = points[0]
        delivery.press(x, y, LeftButton, LeftButton, 0, false, null)
        for(let i = 1; i < points.length; i++){
            time += 16;
            [x, y] = points[i]
            delivery.move(x, y, LeftButton, 0)
        }
        time += releaseAfter
        delivery.release(x, y, LeftButton, 0, 0)
    }

    // ---- ListView ----

    test(name + ': a ListView header is before the first item, the view starts at it; the footer after the last item', () => {
        let view = list((it) => {
            it.header = box(30)
            it.footer = box(10)
        })
        assert.equal(view.headerItem.y, -30)
        assert.equal(view.contentY, -30)
        assert.equal(view.originY, -30)
        assert.equal(view.itemAtIndex(0).y, 0)
        assert.equal(view.contentHeight, 10 * 20 + 30 + 10)
        view.positionViewAtEnd()
        engine.batch(() => view.forceLayout())
        assert.equal(view.footerItem.y, 200)
        assert.equal(view.contentY, 210 - 100)
        assert.equal(view.atYEnd, true)
    })

    test(name + ': BottomToTop lays the items upwards from the bottom of the view', () => {
        let view = list((it) => { it.verticalLayoutDirection = ListView.BottomToTop })
        assert.equal(view.contentY, -100)
        assert.equal(view.itemAtIndex(0).y, -20)
        assert.equal(view.itemAtIndex(1).y, -40)
        assert.equal(view.atYEnd, true)
        assert.equal(view.atYBeginning, false)
    })

    test(name + ': the arrows move the current index along the flow, reversed with BottomToTop; keyNavigationWraps', () => {
        let view = list()
        let key = (k) => engine.batch(() => view.$keyEvent({ key: k, modifiers: 0, text: '', isAutoRepeat: false }, 'pressed'))
        key('ArrowDown')
        assert.equal(view.currentIndex, 1)
        key('ArrowUp')
        key('ArrowUp')
        assert.equal(view.currentIndex, 0)
        engine.batch(() => { view.keyNavigationWraps = true })
        key('ArrowUp')
        assert.equal(view.currentIndex, 9)
        engine.batch(() => { view.keyNavigationEnabled = false })
        key('ArrowDown')
        assert.equal(view.currentIndex, 9)
        let reversed = list((it) => { it.verticalLayoutDirection = ListView.BottomToTop })
        engine.batch(() => reversed.$keyEvent({ key: 'ArrowUp', modifiers: 0, text: '' }, 'pressed'))
        assert.equal(reversed.currentIndex, 1)
        // keyNavigationEnabled follows interactive until it is set
        let fixed = list((it) => { it.interactive = false })
        engine.batch(() => fixed.$keyEvent({ key: 'ArrowDown', modifiers: 0, text: '' }, 'pressed'))
        assert.equal(fixed.currentIndex, 0)
    })

    test(name + ': a new current index brings its item into the view', () => {
        let view = list()
        engine.batch(() => { view.currentIndex = 9 })
        assert.equal(view.contentY, 100)
        engine.batch(() => { view.currentIndex = 2 })
        assert.equal(view.contentY, 40)
    })

    test(name + ': the delegates are at z 1, the highlight at z 0 under the current item; ListView.view and isCurrentItem', () => {
        let view = list((it) => { it.highlight = box(0, 0) })
        let current = view.currentItem
        assert.equal(current.z, 1)
        assert.equal(view.highlightItem.z, 0)
        assert.equal(view.highlightItem.y, current.y)
        assert.equal(view.highlightItem.width, current.width)
        assert.equal(current.ListView.view, view)
        assert.equal(current.ListView.isCurrentItem, true)
        assert.equal(view.itemAtIndex(1).ListView.isCurrentItem, false)
    })

    test(name + ': sections: a label before the first item of each section, the attached section properties', () => {
        let view = list((it) => {
            it.model = [{ group: 'A' }, { group: 'A' }, { group: 'B' }]
            it.section.property = 'group'
            it.section.delegate = box(10)
        })
        let items = [0, 1, 2].map((i) => view.itemAtIndex(i))
        assert.deepEqual(items.map((it) => it.y), [10, 30, 60])
        assert.equal(items[2].ListView.section, 'B')
        assert.equal(items[2].ListView.previousSection, 'A')
        assert.equal(items[0].ListView.nextSection, 'A')
        assert.equal(view.currentSection, 'A')
        assert.equal(view.contentHeight, 2 * 10 + 3 * 20)
    })

    test(name + ': dragging moves the content after the threshold, a quick release flicks on and stops in the bounds', () => {
        let view = list()
        let started = 0, ended = 0
        engine.connect(view, ListView.SIGNAL_movementStarted, () => started++)
        engine.connect(view, ListView.SIGNAL_movementEnded, () => ended++)
        delivery.press(50, 80, LeftButton, LeftButton, 0, false, null)
        time += 16
        delivery.move(50, 75, LeftButton, 0)
        assert.equal(view.contentY, 0)
        time += 16
        delivery.move(50, 60, LeftButton, 0)
        assert.equal(view.contentY, 0, 'the move past the threshold does not move yet')
        // QQuickFlickablePrivate::drag: the next move sets dragStartOffset, the content follows from the one after
        time += 16
        delivery.move(50, 40, LeftButton, 0)
        assert.equal(view.contentY, 0)
        time += 16
        delivery.move(50, 20, LeftButton, 0)
        assert.equal(view.contentY, 20)
        assert.equal(view.dragging, true)
        assert.equal(view.moving, true)
        time += 16
        delivery.release(50, 20, LeftButton, 0, 0)
        assert.equal(view.dragging, false)
        assert.equal(view.flicking, true)
        settle()
        assert.ok(view.contentY > 20)
        assert.ok(view.contentY <= 100)
        assert.equal(view.flicking, false)
        assert.equal(view.moving, false)
        assert.equal(started, 1)
        assert.equal(ended, 1)
    })

    test(name + ': a drag over a MouseArea of a delegate takes the mouse from it (canceled), not with preventStealing', () => {
        for(let preventStealing of [false, true]){
            let canceled = 0
            let view = list((it) => {
                it.delegate = component((parent) => {
                    let item = new Item(parent)
                    item.width = 100
                    item.height = 20
                    let area = new MouseArea(item)
                    area.width = 100
                    area.height = 20
                    area.preventStealing = preventStealing
                    engine.connect(area, MouseArea.SIGNAL_canceled, () => canceled++)
                    return item
                })
            })
            drag([[50, 80], [50, 60], [50, 40], [50, 30]], 300)
            settle()
            if(preventStealing){
                assert.equal(canceled, 0)
                assert.equal(view.contentY, 0)
            } else {
                assert.equal(canceled, 1)
                assert.equal(view.contentY, 10)
            }
        }
    })

    test(name + ': past a bound the drag goes half as far and returns after the release; StopAtBounds stops there', () => {
        let view = list()
        drag([[50, 20], [50, 40], [50, 60], [50, 80]], 300)
        assert.ok(view.contentY < 0)
        settle()
        assert.equal(view.contentY, 0)
        let stopping = list((it) => { it.boundsBehavior = Flickable.StopAtBounds })
        delivery.press(50, 20, LeftButton, LeftButton, 0, false, null)
        for(let y of [40, 60, 80]){
            time += 16
            delivery.move(50, y, LeftButton, 0)
            assert.equal(stopping.contentY, 0)
        }
        delivery.release(50, 80, LeftButton, 0, 0)
    })

    test(name + ': SnapToItem takes the view to the start of the nearest item when it stops', () => {
        let view = list((it) => { it.snapMode = ListView.SnapToItem })
        drag([[50, 80], [50, 60], [50, 40], [50, 25]], 300)
        settle()
        assert.equal(view.contentY % 20, 0)
        assert.equal(view.contentY, 20)
    })

    // ---- Flickable ----

    test(name + ': a Flickable flicks along the axes its content is larger in; the direction enums are Qt\'s', () => {
        assert.equal(Flickable.HorizontalAndVerticalFlick, 3)
        assert.equal(Flickable.AutoFlickIfNeeded, 0xc)
        let root = window()
        let f
        engine.batch(() => {
            f = new Flickable(root)
            f.width = 100
            f.height = 100
            f.contentWidth = 100
            f.contentHeight = 500
        })
        assert.equal(f.$flickAxis('y'), true)
        assert.equal(f.$flickAxis('x'), false)
        drag([[50, 90], [80, 70], [80, 50], [80, 30]], 300)
        settle()
        assert.equal(f.contentX, 0)
        assert.equal(f.contentY, 20)
        assert.equal(f.atYBeginning, false)
        engine.batch(() => { f.flickableDirection = Flickable.HorizontalFlick })
        assert.equal(f.$flickAxis('y'), false)
        engine.batch(() => { f.contentY = 400 })
        assert.equal(f.atYEnd, true)
    })

    test(name + ': flick() and cancelFlick() of a Flickable', () => {
        let root = window()
        let f
        engine.batch(() => {
            f = new Flickable(root)
            f.width = 100
            f.height = 100
            f.contentHeight = 5000
        })
        // a negative velocity moves the content towards its end
        engine.batch(() => f.flick(0, -1000))
        assert.equal(f.flicking, true)
        settle()
        assert.ok(f.contentY > 300)
        assert.equal(f.flicking, false)
        engine.batch(() => f.flick(0, -1000))
        advance(32)
        let stopped = f.contentY
        engine.batch(() => f.cancelFlick())
        settle()
        assert.equal(f.contentY, stopped)
    })

    // ---- GridView ----

    test(name + ': GridView arrows move the current index by a cell and by a row', () => {
        let root = window()
        let view
        engine.batch(() => {
            view = new GridView(root)
            view.width = 300
            view.height = 200
            view.cellWidth = 100
            view.cellHeight = 100
            view.delegate = box(100)
            view.model = 7
        })
        let key = (k) => engine.batch(() => view.$keyEvent({ key: k, modifiers: 0, text: '' }, 'pressed'))
        assert.equal(view.currentIndex, 0)
        key('ArrowRight')
        assert.equal(view.currentIndex, 1)
        key('ArrowDown')
        assert.equal(view.currentIndex, 4)
        key('ArrowDown')
        assert.equal(view.currentIndex, 4)
        key('ArrowUp')
        assert.equal(view.currentIndex, 1)
        engine.batch(() => { view.currentIndex = 6 })
        assert.equal(view.contentY, 100)
    })
}

for(let [name, load] of runtimes) cases(name, load())
