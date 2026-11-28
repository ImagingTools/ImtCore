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
        key(0x01000015)
        assert.equal(view.currentIndex, 1)
        key(0x01000013)
        key(0x01000013)
        assert.equal(view.currentIndex, 0)
        engine.batch(() => { view.keyNavigationWraps = true })
        key(0x01000013)
        assert.equal(view.currentIndex, 9)
        engine.batch(() => { view.keyNavigationEnabled = false })
        key(0x01000015)
        assert.equal(view.currentIndex, 9)
        let reversed = list((it) => { it.verticalLayoutDirection = ListView.BottomToTop })
        engine.batch(() => reversed.$keyEvent({ key: 0x01000013, modifiers: 0, text: '' }, 'pressed'))
        assert.equal(reversed.currentIndex, 1)
        // keyNavigationEnabled follows interactive until it is set
        let fixed = list((it) => { it.interactive = false })
        engine.batch(() => fixed.$keyEvent({ key: 0x01000015, modifiers: 0, text: '' }, 'pressed'))
        assert.equal(fixed.currentIndex, 0)
    })

    // QQuickItemView::trackedPositionChanged: after a new current index (moveReason SetIndex) the view follows the
    // highlight - an empty item without a highlight component (createHighlightItem, createDefault) - which moves to the
    // current item by a SmoothedAnimation (highlightMoveVelocity 400); the view moved by hand stops following
    test(name + ': a new current index: the view follows the highlight moving to the item', () => {
        let view = list()
        assert.ok(view.highlightItem)
        engine.batch(() => { view.currentIndex = 9 })
        assert.equal(view.contentY, 0)
        let animations = view.$highlightAnimations
        assert.equal(animations.pos.to, 180)
        engine.batch(() => animations.pos.complete())
        assert.equal(view.highlightItem.y, 180)
        assert.equal(view.contentY, 100)
        engine.batch(() => { view.currentIndex = 2 })
        engine.batch(() => animations.pos.complete())
        assert.equal(view.contentY, 40)
        // by hand: the view does not follow the highlight any more
        engine.batch(() => { view.contentY = 0 })
        engine.batch(() => { view.highlightItem.y = 150 })
        assert.equal(view.contentY, 0)
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
        engine.batch(() => { view.currentIndex = 1 })
        assert.equal(current.ListView.isCurrentItem, false)
        assert.equal(view.itemAtIndex(1).ListView.isCurrentItem, true)
    })

    // the popup of the filters (PopupDecorator): a view of no size gets its model; setModel creates the current item
    // and ends with updateViewport, which estimates the rows with the current item's size (initializeCurrentItem);
    // setting the current index does not refill (QQuickItemView::trackedPositionChanged), the rows come in the polish
    test(name + ': a new model of a view of no size: the current item only, the content estimated from its size', () => {
        let root = window()
        let view
        engine.batch(() => {
            view = new ListView(root)
            view.cacheBuffer = 0
            view.spacing = 2
            view.delegate = box(20)
        })
        engine.batch(() => {
            view.model = 3
            assert.equal(view.contentHeight, 3 * 20 + 2 * 2)
            assert.ok(view.currentItem)
            assert.equal(view.itemAtIndex(0), null)
            assert.equal(view.itemAtIndex(1), null)
            view.width = 100
            view.height = 100
        })
        engine.flushFrame()
        assert.equal(view.contentHeight, 3 * 20 + 2 * 2)
        assert.ok(view.itemAtIndex(2))
    })

    // QQuickListViewPrivate::averageSize: 100 until items are laid out, then the rounded mean (qRound) of the built ones
    test(name + ': rows not built yet are estimated with the rounded mean size of the built ones', () => {
        const rows = (height) => component((parent, ctx) => {
            let item = new Item(parent)
            item.width = 100
            item.height = ctx.modelData.h
            return item
        })
        let model = []
        for(let i = 0; i < 10; i++) model.push({ h: i % 2 ? 26 : 25 })
        let view = list((it) => {
            it.delegate = rows()
            it.model = model
        })
        // built: 25, 26, 25, 26 (0..76, the next starts at 102 > 100); the mean 25.5 rounds to 26
        assert.ok(view.itemAtIndex(3))
        assert.equal(view.itemAtIndex(4), null)
        assert.equal(view.contentHeight, 102 + 6 * 26)
    })

    // QQuickListViewPrivate::createHighlight / updateHighlight: made at the current item's size along the flow and
    // its position; then the position moves by highlightMove*, the width and the height by highlightResize*
    test(name + ': the highlight moves by highlightMove* and resizes by highlightResize*', () => {
        let view = list((it) => {
            it.delegate = component((parent, ctx) => {
                let item = new Item(parent)
                item.width = ctx.modelData.w
                item.height = ctx.modelData.h
                return item
            })
            it.model = [{ w: 100, h: 20 }, { w: 80, h: 40 }]
            it.highlight = box(0, 0)
            it.highlightMoveVelocity = 300
            it.highlightResizeVelocity = 50
            it.highlightResizeDuration = 120
        })
        let highlight = view.highlightItem
        assert.equal(highlight.height, 20)
        assert.equal(highlight.width, 100)
        assert.equal(highlight.y, 0)
        engine.batch(() => { view.currentIndex = 1 })
        let animations = view.$highlightAnimations
        assert.equal(highlight.height, 20)
        assert.equal(animations.pos.running, true)
        assert.equal(animations.pos.to, 20)
        assert.equal(animations.pos.velocity, 300)
        for(let [key, to] of [['height', 40], ['width', 80]]){
            assert.equal(animations[key].running, true)
            assert.equal(animations[key].to, to)
            assert.equal(animations[key].velocity, 50)
            assert.equal(animations[key].duration, 120)
        }
        engine.batch(() => { for(let key in animations) animations[key].complete() })
        assert.deepEqual([highlight.y, highlight.width, highlight.height], [20, 80, 40])
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

    // QQuickListViewPrivate: the visible items keep their positions, the rows not built are estimated from them with the
    // average size (positionAt, originPosition, lastPosition). A view of rows of different heights jumped far down
    // settles at once, and a later layout with another average does not move the rows in the view (the estimate moves
    // the origin instead). Before, every row was positioned from the first one with the average: each fill built other
    // rows, a new average, and the polish never ended
    test(name + ': a ListView of rows of different heights jumped far down settles, its rows stay when the average changes', () => {
        let root, view
        let heights = Array.from({ length: 3000 }, (_, i) => 10 + (i * 37) % 41)
        engine.batch(() => {
            root = new Item(null)
            view = new ListView(root)
            view.width = 300
            view.height = 400
            view.spacing = 2
            view.delegate = component((parent, ctx) => {
                let item = new Item(parent)
                item.width = 300
                item.height = heights[ctx.index]
                return item
            })
            view.model = heights.length
        })
        let warnings = []
        let warn = console.warn
        console.warn = (message) => warnings.push(String(message))
        try {
            engine.batch(() => { view.contentY = 52000 })
            engine.flushFrame()
            engine.flushFrame()
        } finally {
            console.warn = warn
        }
        assert.deepEqual(warnings, [])
        let top = view.indexAt(10, 52001)
        assert.ok(top > 0)
        let item = view.itemAtIndex(top)
        let y = item.y
        // another row of the view grows: the average changes, the rows in the view stay, the origin moves
        let origin = view.originY
        heights[top + 2] += 300
        engine.batch(() => { view.itemAtIndex(top + 2).height = heights[top + 2] })
        engine.flushFrame()
        assert.equal(view.itemAtIndex(top), item)
        assert.equal(item.y, y)
        assert.equal(view.itemAtIndex(top + 3).y, view.itemAtIndex(top + 2).y + heights[top + 2] + 2)
        assert.notEqual(view.originY, origin)
        // scrolled back to the origin the first row is built where the estimate puts it
        engine.batch(() => { view.contentY = view.originY })
        engine.flushFrame()
        assert.equal(view.indexAt(10, view.originY + 1), 0)
    })

    // QQmlChangeSet move (QQmlListModel::move -> rowsMoved): the delegates of the moved rows are the same objects at
    // their new place, the rows moved out of the view release theirs, the ones moved in are made; the current index
    // follows its row and the view does not move (tests/diff/cases/listview_move.qml: the same as Qt 6)
    test(name + ': ListModel.move moves the delegates of a ListView, the current item follows its row', () => {
        const { ListModel } = types
        let root, model, view
        engine.batch(() => {
            root = new Item(null)
            model = new ListModel(root)
            for(let i = 0; i < 60; i++) model.append({ n: i })
            view = new ListView(root)
            view.width = 300
            view.height = 200
            view.cacheBuffer = 0
            view.delegate = box(20, 300)
            view.model = model
        })
        engine.flushFrame()
        let items = Array.from({ length: 10 }, (_, i) => view.itemAtIndex(i))
        engine.batch(() => { view.currentIndex = 3 })
        engine.batch(() => model.move(2, 5, 1))
        engine.flushFrame()
        assert.equal(view.itemAtIndex(5), items[2])
        assert.equal(view.itemAtIndex(2), items[3])
        assert.equal(view.itemAtIndex(5).y, 100)
        assert.equal(view.currentIndex, 2)
        assert.equal(view.currentItem, items[3])
        // to the top, before the first visible row: put back there
        engine.batch(() => model.move(3, 0, 2))
        engine.flushFrame()
        assert.equal(view.itemAtIndex(0), items[4])
        assert.equal(view.itemAtIndex(0).y, 0)
        assert.equal(view.contentY, 0)
        // out of the view: released; in from far: made
        let out = view.itemAtIndex(1)
        engine.batch(() => model.move(1, 40, 1))
        engine.flushFrame()
        assert.notEqual(view.itemAtIndex(1), out)
        assert.equal(view.itemAtIndex(40), null)
        engine.batch(() => model.move(50, 2, 3))
        engine.flushFrame()
        assert.equal(model.get(2).n, 50)
        assert.equal(view.itemAtIndex(2).y, 40)
        assert.equal(view.contentY, 0)
    })

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
        key(0x01000014)
        assert.equal(view.currentIndex, 1)
        key(0x01000015)
        assert.equal(view.currentIndex, 4)
        key(0x01000015)
        assert.equal(view.currentIndex, 4)
        key(0x01000013)
        assert.equal(view.currentIndex, 1)
        // the view follows the highlight (an empty item without a component), moved in highlightMoveDuration (150 ms)
        engine.batch(() => { view.currentIndex = 6 })
        assert.ok(view.highlightItem)
        assert.equal(view.$highlightAnimations.y.duration, 150)
        engine.batch(() => { for(let key in view.$highlightAnimations) view.$highlightAnimations[key].complete() })
        assert.equal(view.highlightItem.y, 200)
        assert.equal(view.contentY, 100)
    })
}

for(let [name, load] of runtimes) cases(name, load())
