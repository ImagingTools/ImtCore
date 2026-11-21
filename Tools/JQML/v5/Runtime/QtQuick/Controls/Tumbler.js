// Tumbler of QtQuick.Templates (qquicktumbler.cpp, Qt 6.12) and its view (QQuickTumblerView). Checked against Qt 6.12
// (tests/diff/cases/tumbler.qml):
//  - the view of the content: a path view when it wraps, else a ListView (StrictlyEnforceRange, the current item in the
//    middle, SnapToItem); wrap is count >= visibleItemCount unless set;
//  - every delegate is as wide as the content and availableHeight / visibleItemCount high (_q_updateItemWidths/Heights);
//  - the path view (QQuickPathView with pathItemCount visibleItemCount + 1, the highlight at 0.5): the item of displacement
//    d is d delegates above the middle, the items of displacement in (-n/2, n/2] are shown; offset: currentIndex is
//    count - round(offset); a new current index moves the offset there in highlightMoveDuration (1000, InOutQuad, the
//    shorter way round); a drag past startDragDistance moves the offset with the pointer (moving), the release snaps;
//    positionViewAtIndex: QQuickPathView::positionViewAtIndex;
//  - Up / Down: the index before / after (round when it wraps); the attached Tumbler.tumbler and Tumbler.displacement
//    (QQuickTumblerAttachedPrivate::calculateDisplacement).
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { GroupObject } = require('../../Core/QtObject')
const { Item } = require('../Item')
const { Control, withSetters } = require('./Control')
const { ModelContext, modelRows, sameModel } = require('../../QtQml/Models/ModelContext')
const { clock, DRAG_THRESHOLD } = require('../Flickable')
const QtEnums = require('../../Qt/enums')

const Beginning = 0, Center = 1, End = 2, Visible = 3, Contain = 4, SnapPosition = 5
const easeInOutQuad = (t) => (t < 0.5 ? 2 * t * t : 1 - Math.pow(-2 * t + 2, 2) / 2)
const easeOutQuad = (t) => 1 - (1 - t) * (1 - t)
const mod = (a, n) => ((a % n) + n) % n
const LeftButton = 1

// ---- the path view of a wrapping Tumbler (QQuickPathView as QQuickTumblerView makes it) ----
class TumblerPathView extends Item {
    constructor(parent = null){
        super(parent)
        this.$items = new Map()        // index -> { item, ctx }
        this.$model = null
        this.$count = 0
        this.$offset = 0
        this.$animation = null
        let layout = () => this.$layout()
        engine.connect(this, Item.SIGNAL_widthChanged, layout, null, engine.PRIORITY_NORMAL, this)
        engine.connect(this, Item.SIGNAL_heightChanged, layout, null, engine.PRIORITY_NORMAL, this)
    }
    get offset(){ engine.track(this, TumblerPathView.SIGNAL_offsetChanged); return this.$offset }
    get count(){ return this.$count }
    get currentIndex(){ return this.$tumbler ? this.$tumbler.currentIndex : 0 }
    get pathItemCount(){ return this.$pathItemCount() }
    $pathItemCount(){ return (this.$tumbler ? this.$tumbler.$v[Tumbler.VISIBLEITEMCOUNT] : 5) + 1 }
    $delegateHeight(){ return this.$tumbler ? this.$tumbler.$delegateHeight() : 40 }
    // the model of the Tumbler: the delegates are made again
    $setModel(model){
        if(this.$model && this.$model.$removeView) this.$model.$removeView(this)
        this.$model = model
        if(model && model.$addView) model.$addView(this)
        this.$reset()
    }
    $modelChanged(){ this.$reset() }
    $reset(){
        for(let entry of this.$items.values()) entry.item.$releaseLater()
        this.$items.clear()
        this.$count = modelRows(this.$model).count
        this.$layout()
    }
    // QQuickTumblerAttachedPrivate::calculateDisplacement (a PathView): count - index - offset, folded into the half of
    // the visible items around 0 (one more when there are more items than shown)
    $displacementOf(index){
        let count = this.$count
        if(count <= 1) return 0
        let d = count - index - this.$offset
        let visible = this.$tumbler ? this.$tumbler.$v[Tumbler.VISIBLEITEMCOUNT] : 5
        let half = Math.floor(visible / 2) + (visible < count ? 1 : 0)
        if(d > half) d -= count
        else if(d < -half) d += count
        return d
    }
    // QQuickPathViewPrivate::positionOfIndex: the percent of the path of an index (the highlight range at 0.5)
    $percentOf(index){
        let count = this.$count, n = this.$pathItemCount()
        let mappedRange = n < count ? count / n : 1
        let globalPos = ((index + this.$offset) % count) / count
        if(n < count){
            globalPos += 0.5 / mappedRange
            return (globalPos % 1) * mappedRange
        }
        return (globalPos + 0.5) % 1
    }
    // QQuickPathViewPrivate::layoutItems / updateItem: the items of the path (those under 100%), at the point of their
    // percent on the line from -dh / 2 to (visibleItemCount + 1) * dh - dh / 2
    $layout(){
        if(this.$f & engine.DESTROYED || !this.$tumbler) return
        let count = this.$count, n = this.$pathItemCount(), dh = this.$delegateHeight()
        let startY = -dh / 2, endY = n * dh - dh / 2, x = this.$v[Item.WIDTH] / 2
        let shown = new Set()
        let delegate = this.$tumbler.$v[Tumbler.DELEGATE]
        let rows = modelRows(this.$model)
        if(delegate) for(let index = 0; index < count; index++){
            let percent = this.$percentOf(index)
            if(!(percent < 1)) continue
            shown.add(index)
            let entry = this.$items.get(index)
            if(!entry){
                let ctx = new ModelContext(rows.row(index), index, rows.data(index))
                ctx.$view = this
                let item = delegate.createObject(this, null, ctx)
                if(!item) continue
                item.$tumblerIndex = index
                entry = { item, ctx }
                this.$items.set(index, entry)
                this.$tumbler.$setupItem(item)
            }
            let item = entry.item
            let y = startY + (endY - startY) * percent
            item.$write(Item.X, x - item.$v[Item.WIDTH] / 2)
            item.$write(Item.Y, y - item.$v[Item.HEIGHT] / 2)
        }
        for(let [index, entry] of [...this.$items]){
            if(shown.has(index)) continue
            this.$items.delete(index)
            entry.item.$releaseLater()
        }
        this.$tumbler.$updateCurrentItem()
        this.$tumbler.$updateDisplacements()
    }
    $width(){ return this.$v[Item.WIDTH] }
    itemAt(index){ let entry = this.$items.get(index); return entry ? entry.item : null }
    // follow: the current index follows the offset (a drag, a flick, positionViewAtIndex; not the move to a new index)
    $setOffset(value, follow = true){
        let count = this.$count
        value = count > 0 ? mod(value, count) : 0
        if(Math.abs(value - count) < 1e-9) value = 0
        if(value === this.$offset) return
        this.$offset = value
        engine.emit(this, TumblerPathView.SIGNAL_offsetChanged)
        this.$layout()
        // StrictlyEnforceRange: the current index follows the offset
        if(follow && count > 0 && this.$tumbler) this.$tumbler.$viewIndexChanged(mod(count - Math.round(value), count))
    }
    $targetOffset(index){ let count = this.$count; return count > 0 ? mod(count - index, count) : 0 }
    // QQuickPathViewPrivate::snapToIndex: the shorter way round in highlightMoveDuration, InOutQuad
    $snapToIndex(index, animate){
        this.$stopAnimation()
        let count = this.$count
        if(count <= 0) return
        let target = this.$targetOffset(index)
        if(!animate){
            this.$setOffset(target, false)
            return
        }
        let from = this.$offset, to = target
        if(to - from > count / 2) to -= count
        else if(to - from < -count / 2) to += count
        this.$animate(from, to, 1000, easeInOutQuad, null, false)
    }
    $animate(from, to, duration, easing, done, follow = true){
        let animation = { from, to, start: clock.now(), duration }
        this.$animation = animation
        if(from === to || duration <= 0){
            this.$setOffset(to, follow)
            this.$animation = null
            if(done) done()
            return
        }
        let step = () => {
            if(this.$animation !== animation || this.$f & engine.DESTROYED || engine.isStopped()) return
            let t = Math.min(1, (clock.now() - animation.start) / duration)
            engine.batch(() => this.$setOffset(from + (to - from) * easing(t), follow))
            if(t < 1) clock.frame(step)
            else {
                this.$animation = null
                if(done) engine.batch(done)
            }
        }
        clock.frame(step)
    }
    $stopAnimation(){ this.$animation = null }
    // QQuickPathView::positionViewAtIndex (snapping, pathItemCount items, the highlight range at 0.5)
    positionViewAtIndex(index, mode){
        let count = this.$count
        if(count <= 0) return
        let n = Math.min(this.$pathItemCount(), count)
        let idx = mod(index, count)
        let begin = count - idx - Math.floor(n * 0.5)
        let end = begin + n - 1
        let offset = this.$offset
        switch(mode){
        case Beginning: offset = begin; break
        case End: offset = end; break
        case Center: offset = Math.round(begin < end ? (begin + end) / 2 : (begin + end + count) / 2); break
        case Contain:
            if((begin < end && (offset < begin || offset > end)) || (offset < begin && offset > end)){
                let d1 = mod(begin - offset, count), d2 = mod(offset - end, count)
                offset = d1 < d2 ? begin : end
            }
            break
        case SnapPosition: offset = count - idx; break
        }
        this.$stopAnimation()
        this.$setOffset(offset)
    }
    incrementCurrentIndex(){ if(this.$tumbler) this.$tumbler.$step(1) }
    decrementCurrentIndex(){ if(this.$tumbler) this.$tumbler.$step(-1) }
    // a drag past startDragDistance moves the offset with the pointer (QQuickPathViewPrivate::handleMouseMoveEvent)
    $mousePress(x, y, button){
        if(button !== LeftButton || !this.$count) return false
        this.$stopAnimation()
        this.$press = { y, dragging: false, startY: y, startOffset: this.$offset, samples: [{ y, time: clock.now() }] }
        return true
    }
    $mouseMove(x, y){
        let press = this.$press
        if(!press) return
        if(!press.dragging){
            if(Math.abs(y - press.y) <= DRAG_THRESHOLD) return
            press.dragging = true
            press.startY = y
            press.startOffset = this.$offset
            if(this.$tumbler) this.$tumbler.$setMoving(true)
        }
        press.samples.push({ y, time: clock.now() })
        if(press.samples.length > 5) press.samples.shift()
        engine.batch(() => this.$setOffset(press.startOffset + (y - press.startY) / this.$delegateHeight()))
    }
    get $keepMouseGrab(){ return !!(this.$press && this.$press.dragging) }
    // the release: the offset goes on with the speed of the last moves, then snaps to an item
    $mouseRelease(){
        let press = this.$press
        this.$press = null
        if(!press || !press.dragging) return
        let samples = press.samples, now = clock.now()
        let first = samples[0], last = samples[samples.length - 1]
        let velocity = last.time > first.time && now - last.time < 100 ? (last.y - first.y) / (last.time - first.time) * 1000 : 0
        let dh = this.$delegateHeight()
        let decel = this.$tumbler ? this.$tumbler.flickDeceleration : 100
        let distance = Math.abs(velocity) > 75 ? Math.sign(velocity) * velocity * velocity / (2 * Math.max(decel, 1)) : 0
        let to = Math.round(this.$offset + distance / dh)
        let duration = distance ? Math.min(1000, Math.abs(velocity) / Math.max(decel, 1) * 1000) : 200
        this.$animate(this.$offset, to, duration, distance ? easeOutQuad : easeInOutQuad, () => { if(this.$tumbler) this.$tumbler.$setMoving(false) })
    }
    $ungrabMouse(){ if(this.$press) this.$mouseRelease() }
    $disposeNative(){
        if(this.$model && this.$model.$removeView) this.$model.$removeView(this)
        this.$model = null
        this.$animation = null
        super.$disposeNative()
    }
}
defineType(TumblerPathView, { signals: ['offsetChanged'] })

// ---- the content of the Basic Tumbler (QQuickTumblerView): a path view or a ListView as the Tumbler wraps ----
class TumblerView extends Item {
    constructor(parent = null){
        super(parent)
        this.$view = null
        this.$wraps = null
    }
    $tumblerOf(){ for(let it = this.$parent; it; it = it.$parent) if(it instanceof Tumbler) return it; return null }
    // QQuickTumblerView::createView
    $createView(tumbler){
        let wrap = tumbler.$wraps()
        if(this.$view && this.$wraps === wrap) return this.$view
        if(this.$view){
            let old = this.$view
            this.$view = null
            old.$tumbler = null
            old.$setParent(null)
            old.destroy()
        }
        this.$wraps = wrap
        let view
        if(wrap){
            view = new TumblerPathView(this)
            view.$tumbler = tumbler
        } else {
            // made as a view of a component (its delegates after componentComplete): at its start, the current index
            // kept (checked)
            let { ListView } = require('../ListView')
            let previous = engine.beginCreate()
            try {
                view = new ListView(this)
                view.$tumbler = tumbler
                view.highlightRangeMode = ListView.StrictlyEnforceRange
                view.snapMode = ListView.SnapToItem
                view.highlightMoveDuration = 1000
                view.clip = true
                view.$bind(ListView.PREFERREDHIGHLIGHTBEGIN, () => view.height / 2 - tumbler.$delegateHeightTracked() / 2, null, view)
                view.$bind(ListView.PREFERREDHIGHLIGHTEND, () => view.height / 2 + tumbler.$delegateHeightTracked() / 2, null, view)
                view.$bind(Item.WIDTH, () => this.width, null, view)
                view.$bind(Item.HEIGHT, () => this.height, null, view)
                // the delegates are sized as they come (QQuickTumblerPrivate::itemChildAdded)
                view.contentItem.$childAdded = (child) => tumbler.$setupItem(child)
                view.delegate = tumbler.$v[Tumbler.DELEGATE]
                view.model = tumbler.$v[Tumbler.MODEL]
                let index = tumbler.$v[Tumbler.CURRENTINDEX]
                if(index > 0) view.currentIndex = index
            } finally {
                engine.endCreate(previous)
            }
            engine.connect(view, ListView.SIGNAL_contentYChanged, () => tumbler.$updateDisplacements(), null, engine.PRIORITY_NORMAL, tumbler)
            engine.connect(view, ListView.SIGNAL_currentIndexChanged, () => tumbler.$viewIndexChanged(view.currentIndex), null, engine.PRIORITY_NORMAL, tumbler)
            engine.connect(view, ListView.SIGNAL_currentItemChanged, () => tumbler.$updateCurrentItem(), null, engine.PRIORITY_NORMAL, tumbler)
            engine.connect(view, ListView.SIGNAL_movingChanged, () => tumbler.$setMoving(view.moving), null, engine.PRIORITY_NORMAL, tumbler)
            this.$view = view
            return view
        }
        view.$bind(Item.WIDTH, () => this.width, null, view)
        view.$bind(Item.HEIGHT, () => this.height, null, view)
        this.$view = view
        return view
    }
}
defineType(TumblerView, { props: { model: 'var', delegate: { kind: 'component', initial: null }, path: { kind: 'var', initial: null } } })

// ---- Tumbler (QQuickTumbler) ----
class Tumbler extends Control {
    constructor(parent = null){
        super(parent)
        this.$plain(Tumbler.VISIBLEITEMCOUNT, 5)
        this.$plain(Tumbler.CURRENTINDEX, -1)
        // QQuickTumblerPrivate: wrap true until the count says otherwise (setWrapBasedOnCount), unless set
        this.$wrapValue = true
        this.$wrapSet = false
        this.$flickDeceleration = 0
        this.$attached = new Set()
        // QQuickTumbler: setActiveFocusOnTab(true)
        this.$write(Control.FOCUSPOLICY, 1)
    }
    // QQuickTumblerPrivate::setWrapBasedOnCount
    $setWrapBasedOnCount(){
        let count = this.$v[Tumbler.COUNT]
        if(count === 0 || this.$wrapSet || this.$modelBeingSet) return
        this.$setWrap(count >= this.$v[Tumbler.VISIBLEITEMCOUNT])
    }
    // QQuickTumblerPrivate::setWrap: the view of the new kind, with the current index
    $setWrap(wrap){
        if(this.$f & engine.COMPLETED && wrap === this.$wrapValue) return
        let index = this.$v[Tumbler.CURRENTINDEX]
        let deceleration = this.flickDeceleration
        this.$wrapValue = wrap
        engine.emit(this, Tumbler.SIGNAL_wrapChanged)
        this.$setupView()
        let view = this.$view()
        if(view instanceof TumblerPathView) view.$snapToIndex(index, false)
        if(this.flickDeceleration !== deceleration) engine.emit(this, Tumbler.SIGNAL_flickDecelerationChanged)
    }
    $delegateHeight(){
        let v = this.$v
        return (v[Item.HEIGHT] - v[Control.TOPPADDING] - v[Control.BOTTOMPADDING]) / v[Tumbler.VISIBLEITEMCOUNT]
    }
    $delegateHeightTracked(){ return this.availableHeight / this.visibleItemCount }
    $wraps(){ return this.$wrapValue }
    $view(){
        let content = this.$v[Control.CONTENTITEM]
        if(content instanceof TumblerView) return content.$view
        return this.$extView && this.$extView.content === content ? this.$extView.view : null
    }

    // QQuickTumblerPrivate::setupViewData for a content item of the document: the PathView or ListView it is, or the
    // first one in it (determineViewType); the Tumbler follows its current index, count and moving, sizes its delegates
    // and computes their displacement from its offset / contentY
    $setupExternalView(){
        let content = this.$v[Control.CONTENTITEM]
        if(!content || content instanceof TumblerView || !(this.$f & engine.COMPLETED)) return
        if(this.$extView && this.$extView.content === content) return
        const { PathView } = require('../PathView')
        const { ListView } = require('../ListView')
        let find = (item) => {
            if(item instanceof PathView || item instanceof ListView) return item
            for(let child of item.$children || []){
                let found = child instanceof Item ? find(child) : null
                if(found) return found
            }
            return null
        }
        let view = find(content)
        if(!view){
            engine.reportError('Tumbler: contentItem must contain either a PathView or a ListView', 'Tumbler')
            return
        }
        let path = view instanceof PathView
        let viewContent = path ? view : view.contentItem
        this.$extView = { content, view, path, viewContent }
        let C = view.constructor
        engine.connect(view, C.SIGNAL_currentIndexChanged, () => { if(!this.$syncing) this.$viewIndexChanged(view.currentIndex) }, null, engine.PRIORITY_NORMAL, this)
        engine.connect(view, C.SIGNAL_currentItemChanged, () => this.$updateCurrentItem(), null, engine.PRIORITY_NORMAL, this)
        engine.connect(view, C.SIGNAL_countChanged, () => this.$onViewCountChanged(), null, engine.PRIORITY_NORMAL, this)
        engine.connect(view, C.SIGNAL_movingChanged, () => this.$setMoving(view.moving), null, engine.PRIORITY_NORMAL, this)
        engine.connect(view, path ? C.SIGNAL_offsetChanged : C.SIGNAL_contentYChanged, () => this.$updateDisplacements(), null, engine.PRIORITY_NORMAL, this)
        // QQuickTumblerPrivate::itemChildAdded: the delegates take the width and the height of a row
        let added = viewContent.$childAdded
        viewContent.$childAdded = (child) => {
            if(added) added.call(viewContent, child)
            this.$sizeItem(child)
        }
        // itemGeometryChanged of the Tumbler: the rows are sized again
        for(let signal of [Item.SIGNAL_widthChanged, Item.SIGNAL_heightChanged, Control.SIGNAL_availableWidthChanged, Control.SIGNAL_availableHeightChanged]){
            if(signal !== undefined) engine.connect(this, signal, () => { this.$updateItemSizes(); this.$updateDisplacements() }, null, engine.PRIORITY_NORMAL, this)
        }
        // syncCurrentIndex
        let index = this.$v[Tumbler.CURRENTINDEX]
        if(view.currentIndex !== index && !(view.count === 0 && view.currentIndex <= 0)){
            this.$syncing = true
            try { if(index >= 0) view.currentIndex = index } finally { this.$syncing = false }
        }
        this.$updateItemSizes()
        this.$updateDisplacements()
    }

    $sizeItem(item){
        if(!(item instanceof Item) || item === (this.$extView && this.$extView.view.$highlightItem)) return
        item.width = this.availableWidth
        item.height = this.$delegateHeight()
    }

    // _q_updateItemWidths / _q_updateItemHeights
    $updateItemSizes(){
        let ext = this.$extView
        if(!ext || ext.content !== this.$v[Control.CONTENTITEM]) return
        for(let child of ext.viewContent.$children || []) this.$sizeItem(child)
    }

    // _q_onViewCountChanged
    $onViewCountChanged(){
        let view = this.$view()
        if(!view) return
        let count = view.count
        if(count !== this.$v[Tumbler.COUNT]){
            this.$plain(Tumbler.COUNT, count)
            this.$setWrapBasedOnCount()
        }
        if(count > 0){
            if(this.$v[Tumbler.CURRENTINDEX] === -1) this.currentIndex = 0
        } else this.$plain(Tumbler.CURRENTINDEX, -1)
        this.$updateCurrentItem()
    }
    // the view of the content as the Tumbler wraps, with the model and the current index
    $setupView(){
        let content = this.$v[Control.CONTENTITEM]
        if(!(content instanceof TumblerView) || !(this.$f & engine.COMPLETED)) return
        let old = content.$view
        let index = this.$v[Tumbler.CURRENTINDEX]
        this.$syncing = true
        try {
            let view = content.$createView(this)
            if(view === old) return
            if(view instanceof TumblerPathView){
                view.$setModel(this.$v[Tumbler.MODEL])
                view.$snapToIndex(index, false)
                view.$layout()
            }
        } finally {
            this.$syncing = false
        }
        this.$updateCurrentItem()
        this.$updateDisplacements()
    }
    $componentComplete(){
        super.$componentComplete()
        this.$updateCount()
        // a non-empty Tumbler has a current index (_q_onViewCountChanged)
        if(this.$v[Tumbler.COUNT] > 0 && this.$v[Tumbler.CURRENTINDEX] < 0) this.$plain(Tumbler.CURRENTINDEX, 0)
        this.$setupView()
        this.$setupExternalView()
        if(this.$extView) this.$onViewCountChanged()
        this.$updateCurrentItem()
    }
    // QQuickTumblerPrivate::setCount
    $updateCount(){
        let ext = this.$extView && this.$view()
        let count = ext ? ext.count : modelRows(this.$v[Tumbler.MODEL]).count
        if(count === this.$v[Tumbler.COUNT]) return
        this.$plain(Tumbler.COUNT, count)
        this.$setWrapBasedOnCount()
    }
    // the delegate of an index is as wide as the content and a delegate height high (_q_updateItemWidths / Heights)
    $setupItem(item){
        if(!item.$isItem) return
        let content = this.$v[Control.CONTENTITEM]
        item.$bind(Item.WIDTH, () => (content ? content.width : 0), null, item)
        item.$bind(Item.HEIGHT, () => this.$delegateHeightTracked(), null, item)
    }
    // the current index of the view (a drag, a snap): the Tumbler's
    $viewIndexChanged(index){
        if(this.$syncing || index === this.$v[Tumbler.CURRENTINDEX]) return
        this.$plain(Tumbler.CURRENTINDEX, index)
        this.$updateCurrentItem()
    }
    $updateCurrentItem(){
        let view = this.$view()
        let item = view ? (view instanceof TumblerPathView ? view.itemAt(this.$v[Tumbler.CURRENTINDEX]) : view.currentItem) : null
        if(item === undefined) item = null
        this.$plain(Tumbler.CURRENTITEM, item || null)
    }
    $setMoving(moving){ this.$plain(Tumbler.MOVING, moving) }
    // Up / Down: the index before / after the current one
    $step(delta){
        let count = this.$v[Tumbler.COUNT]
        if(count <= 0) return
        let index = this.$v[Tumbler.CURRENTINDEX] + delta
        if(this.$wraps()) index = mod(index, count)
        else index = Math.max(0, Math.min(count - 1, index))
        this.currentIndex = index
    }
    $keyEvent(event, kind){
        if(event.key !== QtEnums.Key_Up && event.key !== QtEnums.Key_Down) return 0
        if(kind === 'pressed') this.$step(event.key === QtEnums.Key_Up ? -1 : 1)
        return 1
    }
    // QQuickTumbler::positionViewAtIndex
    positionViewAtIndex(index, mode){
        let view = this.$view()
        if(!view) return
        if(view instanceof TumblerPathView) view.positionViewAtIndex(index, mode)
        else view.positionViewAtIndex(index, mode)
    }
    // QQuickTumblerAttachedPrivate::calculateDisplacement, for each attached object of a delegate
    $updateDisplacements(){
        for(let attached of this.$attached) attached.$update()
    }
    $displacementOf(item){
        let view = this.$view()
        if(!view || !item) return 0
        if(view instanceof TumblerPathView) return item.$tumblerIndex === undefined ? 0 : view.$displacementOf(item.$tumblerIndex)
        // a PathView of the document: count - index - offset, folded into the visible half
        if(view.$positionOfIndex){
            let count = view.count
            if(count <= 1) return 0
            let index = view.$indexOf(item)
            if(index < 0 && view.$requested) index = view.$requested.index
            if(index < 0) return 0
            let d = count - index - view.offset
            let visible = this.$v[Tumbler.VISIBLEITEMCOUNT]
            let half = Math.floor(visible / 2) + (visible < count ? 1 : 0)
            if(d > half) d -= count
            else if(d < -half) d += count
            return d
        }
        // the distance from the current item, less where the current item is from the highlight range
        let dh = this.$delegateHeight()
        if(!dh || !view.count) return 0
        let current = view.currentItem
        let currentY = current ? current.$v[Item.Y] : 0
        let relative = currentY - view.contentY - view.preferredHighlightBegin
        return (currentY - item.$v[Item.Y] - relative) / dh
    }
}
defineType(Tumbler, {
    props: {
        model: 'var', count: { kind: 'int', readonly: true }, currentIndex: 'int', currentItem: { kind: 'var', readonly: true, initial: null },
        delegate: { kind: 'component', initial: null }, visibleItemCount: 'int', wrap: 'bool', moving: { kind: 'bool', readonly: true },
        flickDeceleration: 'real',
    },
})
Tumbler.Beginning = Beginning; Tumbler.Center = Center; Tumbler.End = End; Tumbler.Visible = Visible; Tumbler.Contain = Contain; Tumbler.SnapPosition = SnapPosition
const relayout = (t) => {
    let view = t.$view()
    if(view instanceof TumblerPathView) view.$layout()
    t.$updateItemSizes()
    t.$updateDisplacements()
}
Tumbler.$setters = withSetters(Control, [
    [Tumbler.MODEL, (t, v) => {
        if(sameModel(v, t.$v[Tumbler.MODEL])) return
        // QQuickTumbler::setModel: the wrap from the count once it is set; the view starts at its first item
        t.$modelBeingSet = true
        let wrapped = t.$wraps()
        try {
            t.$plain(Tumbler.MODEL, v)
            if(!(t.$f & engine.COMPLETED)) return
            let view = t.$view()
            t.$syncing = true
            try {
                if(view instanceof TumblerPathView){ view.$setModel(v); view.$snapToIndex(0, false) }
                else if(view){ view.model = v; view.currentIndex = 0 }
            } finally { t.$syncing = false }
        } finally {
            t.$modelBeingSet = false
        }
        let count = modelRows(v).count
        t.$plain(Tumbler.CURRENTINDEX, count > 0 ? 0 : -1)
        t.$plain(Tumbler.COUNT, count)
        t.$setWrapBasedOnCount()
        if(wrapped === t.$wraps()) t.$setupView()
        t.$updateCurrentItem()
    }],
    [Tumbler.CURRENTINDEX, (t, v) => {
        // an index out of the model: the view keeps its own (checked)
        let count = t.$v[Tumbler.COUNT]
        if(v < -1 || (t.$f & engine.COMPLETED && count > 0 && (v < 0 || v >= count))) return
        if(v === t.$v[Tumbler.CURRENTINDEX]) return
        t.$plain(Tumbler.CURRENTINDEX, v)
        let view = t.$view()
        if(view){
            t.$syncing = true
            try {
                if(view instanceof TumblerPathView) view.$snapToIndex(v, true)
                else view.currentIndex = v
            } finally { t.$syncing = false }
        }
        t.$updateCurrentItem()
    }],
    [Tumbler.WRAP, (t, v) => {
        if(v === undefined){
            // resetWrap
            t.$wrapSet = false
            t.$setWrapBasedOnCount()
            return
        }
        t.$wrapSet = true
        t.$setWrap(!!v)
    }],
    [Tumbler.VISIBLEITEMCOUNT, (t, v) => {
        t.$plain(Tumbler.VISIBLEITEMCOUNT, v)
        relayout(t)
    }],
    [Tumbler.FLICKDECELERATION, (t, v) => {
        let old = t.flickDeceleration
        t.$flickDeceleration = v || 0
        if(t.flickDeceleration !== old) engine.emit(t, Tumbler.SIGNAL_flickDecelerationChanged)
    }],
    [Tumbler.DELEGATE, (t, v) => {
        t.$plain(Tumbler.DELEGATE, v)
        let view = t.$view()
        if(view instanceof TumblerPathView) view.$reset()
        else if(view) view.delegate = v
    }],
])
// wrap reads the effective value; flickDeceleration: 100 when it wraps, else the platform's (QPlatformTheme 1500)
Object.defineProperty(Tumbler.prototype, 'wrap', {
    get(){ engine.track(this, Tumbler.SIGNAL_wrapChanged); return this.$wraps() },
    set(value){ this.$write(Tumbler.WRAP, value) },
    configurable: true,
})
Object.defineProperty(Tumbler.prototype, 'flickDeceleration', {
    get(){ engine.track(this, Tumbler.SIGNAL_flickDecelerationChanged); return this.$flickDeceleration || (this.$wrapValue ? 100 : 1500) },
    set(value){ this.$write(Tumbler.FLICKDECELERATION, value) },
    configurable: true,
})

// ---- the attached Tumbler.tumbler, Tumbler.displacement (QQuickTumblerAttached) ----
class TumblerAttached extends GroupObject {
    constructor(owner){
        super(owner)
        this.$tumbler = null
        this.$find()
    }
    $find(){
        if(this.$tumbler) return this.$tumbler
        for(let it = this.$owner && this.$owner.$parent; it; it = it.$parent){
            if(it instanceof Tumbler){
                this.$tumbler = it
                it.$attached.add(this)
                engine.emit(this, TumblerAttached.SIGNAL_tumblerChanged)
                this.$update()
                return it
            }
        }
        return null
    }
    get tumbler(){ engine.track(this, TumblerAttached.SIGNAL_tumblerChanged); return this.$find() }
    get displacement(){
        engine.track(this, TumblerAttached.SIGNAL_displacementChanged)
        if(!this.$tumbler) this.$find()
        return this.$displacement || 0
    }
    $update(){
        let owner = this.$owner
        if(!this.$tumbler || !owner || owner.$f & engine.DESTROYED){
            if(this.$tumbler && owner && owner.$f & engine.DESTROYED) this.$tumbler.$attached.delete(this)
            return
        }
        let d = this.$tumbler.$displacementOf(owner)
        if(d === this.$displacement) return
        this.$displacement = d
        engine.emit(this, TumblerAttached.SIGNAL_displacementChanged)
    }
}
defineType(TumblerAttached, { signals: ['tumblerChanged', 'displacementChanged'] })
// Tumbler.Center... in a delegate reads the enums through the attached object (as ListView's)
Object.assign(TumblerAttached.prototype, { Beginning, Center, End, Visible, Contain, SnapPosition })

{
    let prop = Item.$type.propByName.get('Tumbler')
    prop.group = TumblerAttached
    Object.defineProperty(Item.prototype, 'Tumbler', {
        get(){ return this.$v[prop.id] || (this.$v[prop.id] = new TumblerAttached(this)) },
        configurable: true,
    })
}

module.exports = { Tumbler, TumblerAttached, TumblerView, TumblerPathView }
