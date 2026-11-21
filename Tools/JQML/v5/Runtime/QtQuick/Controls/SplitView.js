// SplitView of QtQuick.Templates (qquicksplitview.cpp, Qt 6.12). Checked against Qt 6.12 (tests/diff/cases/splitview.qml):
//  - its items in a row (a column), a handle (of the handle component) between two of them; the item that fills is the
//    one with SplitView.fillWidth (fillHeight), else the last; the items before it are laid out from the start, those after
//    it from the end, at their preferred size (else implicit) within their minimum and maximum; it takes the rest;
//  - a drag of a handle sets the preferred size of the item on the side away from the filling item (resizing meanwhile);
//    SplitHandle.pressed / hovered attached to the handles;
//  - saveState / restoreState: the preferred sizes.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { GroupObject } = require('../../Core/QtObject')
const { Item } = require('../Item')
const { Control, withSetters } = require('./Control')
const { Container } = require('./Containers')

const Horizontal = 1, Vertical = 2

// SplitView.preferredWidth ... on an item of a split view (QQuickSplitViewAttached); -1 is not set
class SplitViewAttached extends GroupObject {
    $write(id, value){
        super.$write(id, value === undefined ? (id === SplitViewAttached.FILLWIDTH || id === SplitViewAttached.FILLHEIGHT ? false : -1) : value)
        let view = this.$v[SplitViewAttached.VIEW]
        if(view && view.$layout) view.$requestLayout()
    }
}
const unset = { kind: 'real', initial: -1 }
defineType(SplitViewAttached, {
    props: {
        view: { kind: 'var', readonly: true, initial: null },
        minimumWidth: unset, minimumHeight: unset, preferredWidth: unset, preferredHeight: unset,
        maximumWidth: unset, maximumHeight: unset, fillWidth: 'bool', fillHeight: 'bool',
    },
})
// SplitHandle.hovered / pressed on a handle
class SplitHandleAttached extends GroupObject {}
defineType(SplitHandleAttached, { props: { hovered: { kind: 'bool', readonly: true }, pressed: { kind: 'bool', readonly: true } } })

class SplitView extends Container {
    constructor(parent = null){
        super(parent)
        this.$plain(SplitView.ORIENTATION, Horizontal)
        this.$handles = []
        this.$drag = null
        this.$layoutQueued = false
        // the items and handles are in a content item of its own (QQuickContentItem)
        let { ContentItem } = require('./Containers')
        let content = new ContentItem(null)
        content.objectName = 'SplitView'
        this.$setDelegate(Control.CONTENTITEM, content)
    }
    $horizontal(){ return this.$v[SplitView.ORIENTATION] === Horizontal }
    $itemAdded(item){
        let content = this.$v[Control.CONTENTITEM]
        if(content && item.$parent !== content) item.$setParent(content)
        GroupObject.prototype.$write.call(item.SplitView, SplitViewAttached.VIEW, this)
        let relayout = () => this.$requestLayout()
        engine.connect(item, Item.SIGNAL_implicitWidthChanged, relayout, null, engine.PRIORITY_NORMAL, this)
        engine.connect(item, Item.SIGNAL_implicitHeightChanged, relayout, null, engine.PRIORITY_NORMAL, this)
        engine.connect(item, Item.SIGNAL_visibleChanged, relayout, null, engine.PRIORITY_NORMAL, this)
        this.$requestLayout()
    }
    $itemRemoved(){ this.$requestLayout() }
    // QQuickSplitView: layout on the next polish; at once while it is built and dragged
    $requestLayout(){
        if(!(this.$f & engine.COMPLETED) || this.$drag) return this.$layout()
        if(this.$layoutQueued) return
        this.$layoutQueued = true
        engine.queuePolish(this)
    }
    $polish(){
        this.$layoutQueued = false
        this.$layout()
    }
    $write(id, value){
        super.$write(id, value)
        if(id === Item.WIDTH || id === Item.HEIGHT) this.$requestLayout()
    }
    // a handle between each two items (of the handle component)
    $syncHandles(count){
        while(this.$handles.length > Math.max(0, count - 1)) this.$handles.pop().destroy()
        let component = this.$v[SplitView.HANDLE]
        while(this.$handles.length < count - 1 && component && component.createObject){
            // made without a parent, marked, then a child of the view (not one of its items)
            let handle = component.createObject(null)
            if(!handle) break
            handle.$delegateOf = this
            handle.$setParent(this)
            this.$handles.push(handle)
        }
    }
    $size(item){ return item.$v[this.$horizontal() ? Item.WIDTH : Item.HEIGHT] }
    // QQuickSplitViewPrivate::layoutResizeSplitItems
    $layout(){
        let items = this.$v[Container.CONTENTMODEL].$items.filter(i => i.$v[Item.VISIBLE] || i.$ownVisible)
        this.$syncHandles(items.length)
        if(!items.length) return
        let horizontal = this.$horizontal()
        let A = SplitViewAttached
        let attached = (item) => item.SplitView
        let get = (item, name) => attached(item).$v[A[(name + (horizontal ? 'Width' : 'Height')).toUpperCase()]]
        let fill = items.findIndex(item => attached(item).$v[horizontal ? A.FILLWIDTH : A.FILLHEIGHT])
        if(fill < 0) fill = items.length - 1
        let total = horizontal ? this.$v[Item.WIDTH] : this.$v[Item.HEIGHT]
        let across = horizontal ? this.$v[Item.HEIGHT] : this.$v[Item.WIDTH]
        let handleSize = (h) => (h ? (horizontal ? h.$v[Item.IMPLICITWIDTH] || h.$v[Item.WIDTH] : h.$v[Item.IMPLICITHEIGHT] || h.$v[Item.HEIGHT]) : 0)
        let sizeOf = (item) => {
            let preferred = get(item, 'preferred')
            let size = preferred >= 0 ? preferred : (horizontal ? item.$v[Item.IMPLICITWIDTH] : item.$v[Item.IMPLICITHEIGHT])
            let min = get(item, 'minimum'), max = get(item, 'maximum')
            if(max >= 0) size = Math.min(size, max)
            if(min >= 0) size = Math.max(size, min)
            return size
        }
        let place = (item, pos, size) => {
            item.$explicit |= 3
            if(horizontal){
                item.$write(Item.X, pos)
                item.$write(Item.Y, 0)
                item.$write(Item.WIDTH, size)
                item.$write(Item.HEIGHT, across)
            } else {
                item.$write(Item.Y, pos)
                item.$write(Item.X, 0)
                item.$write(Item.HEIGHT, size)
                item.$write(Item.WIDTH, across)
            }
        }
        let placeHandle = (h, pos) => {
            if(!h) return
            if(horizontal){
                h.$write(Item.X, pos)
                h.$write(Item.Y, 0)
                h.$write(Item.WIDTH, handleSize(h))
                h.$write(Item.HEIGHT, across)
            } else {
                h.$write(Item.Y, pos)
                h.$write(Item.X, 0)
                h.$write(Item.HEIGHT, handleSize(h))
                h.$write(Item.WIDTH, across)
            }
        }
        // before the filling item: from the start
        let pos = 0
        for(let i = 0; i < fill; i++){
            let size = sizeOf(items[i])
            place(items[i], pos, size)
            pos += size
            placeHandle(this.$handles[i], pos)
            pos += handleSize(this.$handles[i])
        }
        let start = pos
        // after it: from the end
        pos = total
        for(let i = items.length - 1; i > fill; i--){
            let size = sizeOf(items[i])
            pos -= size
            place(items[i], pos, size)
            let h = this.$handles[i - 1]
            pos -= handleSize(h)
            placeHandle(h, pos)
        }
        // the filling item: the rest, within its minimum and maximum
        let rest = pos - start
        let min = get(items[fill], 'minimum'), max = get(items[fill], 'maximum')
        if(max >= 0) rest = Math.min(rest, max)
        if(min >= 0) rest = Math.max(rest, min)
        place(items[fill], start, Math.max(0, rest))
        this.$fillIndex = fill
        this.$items = items
    }

    // ---- dragging a handle ----
    $handleAt(x, y){
        let horizontal = this.$horizontal()
        for(let i = 0; i < this.$handles.length; i++){
            let h = this.$handles[i]
            let p = horizontal ? x - h.$v[Item.X] : y - h.$v[Item.Y]
            let size = horizontal ? h.$v[Item.WIDTH] : h.$v[Item.HEIGHT]
            if(p >= 0 && p <= size) return i
        }
        return -1
    }
    $setHandleState(index, name, value){
        let h = this.$handles[index]
        if(!h) return
        GroupObject.prototype.$write.call(h.SplitHandle, SplitHandleAttached[name.toUpperCase()], value)
    }
    $mousePress(x, y, button){
        if(button !== 1) return false
        let index = this.$handleAt(x, y)
        if(index < 0) return false
        let horizontal = this.$horizontal()
        let fill = this.$fillIndex
        // the item resized: the one before the handle when the filling item is after it, else the one after it
        let target = index < fill ? this.$items[index] : this.$items[index + 1]
        let before = index < fill
        this.$drag = { index, target, before, start: horizontal ? x : y, size: this.$size(target) }
        this.$setHandleState(index, 'pressed', true)
        this.$plain(SplitView.RESIZING, true)
        return true
    }
    $mouseMove(x, y){
        let d = this.$drag
        if(!d) return
        let delta = (this.$horizontal() ? x : y) - d.start
        let size = d.before ? d.size + delta : d.size - delta
        let horizontal = this.$horizontal()
        let A = SplitViewAttached
        let a = d.target.SplitView
        let min = a.$v[horizontal ? A.MINIMUMWIDTH : A.MINIMUMHEIGHT], max = a.$v[horizontal ? A.MAXIMUMWIDTH : A.MAXIMUMHEIGHT]
        if(max >= 0) size = Math.min(size, max)
        size = Math.max(size, min >= 0 ? min : 0)
        // the filling item keeps its minimum
        let fill = this.$items[this.$fillIndex]
        let fillMin = Math.max(0, fill.SplitView.$v[horizontal ? A.MINIMUMWIDTH : A.MINIMUMHEIGHT])
        let room = this.$size(fill) + this.$size(d.target) - fillMin
        size = Math.min(size, room)
        a.$write(horizontal ? A.PREFERREDWIDTH : A.PREFERREDHEIGHT, size)
        this.$layout()
    }
    $mouseRelease(x, y){
        let d = this.$drag
        if(!d) return
        this.$setHandleState(d.index, 'pressed', false)
        this.$drag = null
        this.$plain(SplitView.RESIZING, false)
    }
    $ungrabMouse(){ this.$mouseRelease() }
    get $keepMouseGrab(){ return !!this.$drag }
    $acceptsHover(){ return true }
    $hoverMove(x, y){
        let index = this.$handleAt(x, y)
        this.$handles.forEach((h, i) => this.$setHandleState(i, 'hovered', i === index))
    }
    $hoverEnter(x, y){ this.$hoverMove(x, y) }
    $hoverLeave(){ this.$handles.forEach((h, i) => this.$setHandleState(i, 'hovered', false)) }
    $cursor(){ return null }

    // the preferred sizes (QQuickSplitView::saveState: a byte array in Qt; an object here)
    saveState(){
        let horizontal = this.$horizontal()
        return { version: 1, items: this.$v[Container.CONTENTMODEL].$items.map(item => item.SplitView.$v[horizontal ? SplitViewAttached.PREFERREDWIDTH : SplitViewAttached.PREFERREDHEIGHT]) }
    }
    restoreState(state){
        if(!state || !Array.isArray(state.items)) return false
        let horizontal = this.$horizontal()
        engine.batch(() => {
            state.items.forEach((size, i) => {
                let item = this.itemAt(i)
                if(item && size >= 0) item.SplitView.$write(horizontal ? SplitViewAttached.PREFERREDWIDTH : SplitViewAttached.PREFERREDHEIGHT, size)
            })
        })
        return true
    }
    $componentComplete(){
        super.$componentComplete()
        this.$layout()
    }
}
defineType(SplitView, { props: { orientation: 'int', resizing: { kind: 'bool', readonly: true }, handle: { kind: 'component', initial: null } } })
SplitView.$setters = withSetters(Container, [[SplitView.ORIENTATION, (s, v) => { s.$plain(SplitView.ORIENTATION, v); s.$requestLayout() }],
    [SplitView.HANDLE, (s, v) => { s.$plain(SplitView.HANDLE, v); for(let h of s.$handles.splice(0)) h.destroy(); s.$requestLayout() }]])
SplitView.prototype.$controlName = 'SplitView'

for(let [name, Group] of [['SplitView', SplitViewAttached], ['SplitHandle', SplitHandleAttached]]){
    let prop = Item.$type.propByName.get(name)
    if(!prop) continue
    prop.group = Group
    Object.defineProperty(Item.prototype, name, {
        get(){ return this.$v[prop.id] || (this.$v[prop.id] = new Group(this)) },
        configurable: true,
    })
}

module.exports = { SplitView, SplitViewAttached, SplitHandleAttached }
