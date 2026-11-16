const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { Item } = require('./Item')
const { ListModel } = require('../QtQml/Models/ListModel')
const { ModelContext, modelRows, sameModel } = require('../QtQml/Models/ModelContext')

const { GroupObject } = require('../Core/QtObject')

const HORIZONTAL = 1
const VERTICAL = 2
const VISIBILITY = require('./Item').bits.VISIBILITY

// ListView attached object of a delegate (QQuickItemViewAttached): the reuse signals
class ListViewAttached extends GroupObject {}

defineType(ListViewAttached, { signals: ['pooled', 'reused'] })
ListViewAttached.$handlersOnly = true

// Delegates exist only for [contentY - cacheBuffer, contentY + height + cacheBuffer], like Qt.
class ListView extends Item {
    constructor(parent = null){
        super(parent)
        this.$content = new Item(this)
        this.$content.$childGeometryChanged = () => engine.queuePolish(this)
        this.$entries = new Map()     // index -> { item, ctx }
        this.$sizes = []
        this.$model = null
        this.$filling = false
        this.$pool = []               // released { item, ctx } kept for reuseItems
        engine.scheduleComponentComplete(this)
    }

    get contentItem(){ return this.$content }

    itemAtIndex(index){
        let entry = this.$entries.get(index)
        return entry ? entry.item : null
    }

    // "the visible item containing the point x, y in content coordinates"
    $entryAt(x, y){
        for(let [index, entry] of this.$entries){
            let item = entry.item
            if(!item || !item.$v[Item.VISIBLE]) continue
            let ix = item.$v[Item.X], iy = item.$v[Item.Y]
            if(x >= ix && y >= iy && x < ix + item.$v[Item.WIDTH] && y < iy + item.$v[Item.HEIGHT]) return [index, item]
        }
        return null
    }

    itemAt(x, y){
        let found = this.$entryAt(x, y)
        return found ? found[1] : null
    }

    indexAt(x, y){
        let found = this.$entryAt(x, y)
        return found ? found[0] : -1
    }

    positionViewAtBeginning(){ this.$setContentPos(0) }

    positionViewAtEnd(){
        this.$fill()
        this.$setContentPos(Math.max(0, this.$contentSize() - this.$viewSize()))
    }

    // QQuickItemViewPrivate::positionViewAtIndex: the mode decides the new position, bounded by the extents
    positionViewAtIndex(index, mode = ListView.Beginning){
        let count = this.$v[ListView.COUNT]
        if(count <= 0) return
        let idx = Math.max(Math.min(index, count - 1), 0)
        let spacing = this.$v[ListView.SPACING]
        let itemPos = 0
        for(let i = 0; i < idx; i++) itemPos += (this.$sizes[i] || this.$averageSize()) + spacing
        let itemSize = this.$sizes[idx] || this.$averageSize()
        let itemEnd = itemPos + itemSize
        let viewSize = this.$viewSize()
        let pos = this.$vertical() ? this.$v[ListView.CONTENTY] : this.$v[ListView.CONTENTX]
        switch(mode){
            case ListView.Beginning: pos = itemPos; break
            case ListView.Center: pos = itemPos - (viewSize - itemSize) / 2; break
            case ListView.End: pos = itemPos - viewSize + itemSize; break
            case ListView.Visible:
                if(itemPos > pos + viewSize) pos = itemEnd - viewSize
                else if(itemEnd <= pos) pos = itemPos
                break
            case ListView.Contain:
                if(itemEnd >= pos + viewSize) pos = itemPos - viewSize + itemSize
                if(itemPos < pos) pos = itemPos
                break
            case ListView.SnapPosition: pos = itemPos - this.$v[ListView.PREFERREDHIGHLIGHTBEGIN]; break
            default: return
        }
        pos = Math.max(0, Math.min(pos, Math.max(0, this.$contentSize() - viewSize)))
        this.$setContentPos(pos)
    }

    forceLayout(){ this.$fill() }

    $frame(){ this.$fill() }

    incrementCurrentIndex(){
        if(this.$v[ListView.CURRENTINDEX] < this.$v[ListView.COUNT] - 1) this.currentIndex = this.$v[ListView.CURRENTINDEX] + 1
    }

    decrementCurrentIndex(){
        if(this.$v[ListView.CURRENTINDEX] > 0) this.currentIndex = this.$v[ListView.CURRENTINDEX] - 1
    }

    // updateViewport sets the size along the flow; across it the content item follows the view (Flickable contentWidth -1)
    $syncContentItem(){
        let content = this.$content
        if(!content) return
        if(this.$vertical()){
            content.$write(Item.WIDTH, this.$v[Item.WIDTH])
            content.$write(Item.HEIGHT, this.$v[ListView.CONTENTHEIGHT])
        } else {
            content.$write(Item.HEIGHT, this.$v[Item.HEIGHT])
            content.$write(Item.WIDTH, this.$v[ListView.CONTENTWIDTH])
        }
    }

    $vertical(){ return this.$v[ListView.ORIENTATION] !== HORIZONTAL }
    $viewSize(){ return this.$vertical() ? this.$v[Item.HEIGHT] : this.$v[Item.WIDTH] }
    $contentSize(){ return this.$vertical() ? this.$v[ListView.CONTENTHEIGHT] : this.$v[ListView.CONTENTWIDTH] }

    $setContentPos(pos){
        if(this.$vertical()) this.contentY = pos
        else this.contentX = pos
    }

    $averageSize(){
        let total = 0, count = 0
        for(let size of this.$sizes){
            if(size !== undefined){ total += size; count++ }
        }
        return count ? total / count : 0
    }

    $componentComplete(){
        super.$componentComplete()
        this.$sync()
    }

    $sync(){
        let model = this.$v[ListView.MODEL]
        if(model !== this.$model){
            if(this.$model instanceof ListModel) this.$model.$removeView(this)
            this.$model = model
            if(model instanceof ListModel) model.$addView(this)
        }
        // QQuickItemViewPrivate::clear releases NotReusable
        for(let index of Array.from(this.$entries.keys())) this.$release(index, false)
        this.$drainPool()
        this.$sizes = []
        let count = modelRows(this.$model).count
        this.$write(ListView.COUNT, count)
        // QQuickItemView::componentComplete: refill(), then updateCurrent(0) unless the index was cleared
        this.$fill(false)
        if(count > 0 && this.$v[ListView.CURRENTINDEX] < 0) this.$write(ListView.CURRENTINDEX, 0)
        if(this.$v[ListView.CURRENTINDEX] >= count) this.$write(ListView.CURRENTINDEX, count - 1)
        this.$ensureCurrentItem()
    }

    // updateCurrent() creates the current item even when it is outside the filled range
    $ensureCurrentItem(){
        let index = this.$v[ListView.CURRENTINDEX]
        if(index < 0 || this.$entries.has(index) || !this.$v[ListView.DELEGATE]) return this.$updateCurrentItem()
        let rows = modelRows(this.$model)
        if(index >= rows.count) return this.$updateCurrentItem()
        let item = this.$create(index, rows)
        if(item){
            let vertical = this.$vertical()
            let pos = 0
            let spacing = this.$v[ListView.SPACING]
            for(let i = 0; i < index; i++) pos += (this.$sizes[i] === undefined ? this.$averageSize() : this.$sizes[i]) + spacing
            this.$sizes[index] = vertical ? item.$v[Item.HEIGHT] : item.$v[Item.WIDTH]
            item.$place(vertical ? Item.Y : Item.X, pos)
        }
        this.$updateCurrentItem()
    }

    $create(index, rows){
        // QQmlDelegateModelPrivate::object: the oldest pooled item, its index and every role updated, then reused()
        if(this.$pool.length){
            let entry = this.$pool.shift()
            let ctx = entry.ctx
            ctx.$row = rows.row(index)
            ctx.$write(ModelContext.INDEX, index)
            ctx.$write(ModelContext.MODELDATA, rows.data(index))
            ctx.$rowChanged(null)
            this.$entries.set(index, entry)
            entry.item.$culled = false
            engine.markDirty(entry.item, VISIBILITY)
            let attached = entry.item.$v[Item.LISTVIEW]
            if(attached) engine.emitArgs(attached, ListViewAttached.SIGNAL_reused, [])
            return entry.item
        }
        let delegate = this.$v[ListView.DELEGATE]
        let ctx = new ModelContext(rows.row(index), index, rows.data(index))
        ctx.$view = this
        let item = delegate.createObject(this.$content, null, ctx)
        if(!item) return null
        this.$entries.set(index, { item, ctx })
        return item
    }

    // QQuickItemViewPrivate::releaseItem: with reuseItems the item goes to the pool, culled, then pooled()
    $release(index, reusable = true){
        let entry = this.$entries.get(index)
        if(!entry) return
        this.$entries.delete(index)
        if(reusable && this.$v[ListView.REUSEITEMS]){
            this.$pool.push(entry)
            entry.item.$culled = true
            engine.markDirty(entry.item, VISIBILITY)
            let attached = entry.item.$v[Item.LISTVIEW]
            if(attached) engine.emitArgs(attached, ListViewAttached.SIGNAL_pooled, [])
            return
        }
        entry.item.$releaseLater()
    }

    // QQmlDelegateModel::drainReusableItemsPool(0)
    $drainPool(){
        let pool = this.$pool
        if(!pool.length) return
        this.$pool = []
        for(let entry of pool) entry.item.$releaseLater()
    }

    // QQuickItemViewPrivate::refill: the visible range first; the cacheBuffer range only on a later pass
    // (bufferPause of 16 ms, then polish), or when the visible pass added nothing.
    // A null-size view without items does not fill at all.
    $fill(withBuffer = false){
        if(this.$filling || !(this.$f & engine.COMPLETED)) return
        let delegate = this.$v[ListView.DELEGATE]
        if(!delegate) return
        if(this.$v[Item.WIDTH] === 0 && this.$v[Item.HEIGHT] === 0 && this.$entries.size === 0) return
        this.$filling = true
        let added = false
        try {
            let rows = modelRows(this.$model)
            let count = rows.count
            let vertical = this.$vertical()
            let spacing = this.$v[ListView.SPACING]
            let cache = this.$v[ListView.CACHEBUFFER]
            let position = vertical ? this.$v[ListView.CONTENTY] : this.$v[ListView.CONTENTX]
            // QQuickItemViewPrivate::refill: a negative view size counts as 0
            let keepStart = position - cache
            let keepEnd = position + Math.max(0, this.$viewSize()) + cache
            let start = withBuffer ? keepStart : position
            let end = withBuffer ? keepEnd : position + Math.max(0, this.$viewSize())
            let current = this.$v[ListView.CURRENTINDEX]
            this.$releaseOnJump(keepStart, keepEnd, spacing, current)
            // QQuickItemViewPrivate::refill: addVisibleItems, then removeNonVisibleItems
            let released = []
            // QQuickListViewPrivate::addVisibleItems: rows not built yet are estimated with averageSize
            let average = this.$averageSize()
            let pos = 0
            let i = 0
            for(; i < count; i++){
                let entry = this.$entries.get(i)
                let size = this.$sizes[i]
                let reach = size === undefined ? average : size
                let needed = pos <= end && (size === undefined && average === 0 || pos + reach >= start)
                let kept = pos <= keepEnd && (size === undefined || pos + size >= keepStart)
                if(needed && !entry){
                    let item = this.$create(i, rows)
                    if(!item) break
                    entry = this.$entries.get(i)
                    added = true
                } else if(!kept && entry && i !== current){
                    released.push(i)
                    entry = null
                }
                if(entry){
                    let item = entry.item
                    size = this.$sizes[i] = vertical ? item.$v[Item.HEIGHT] : item.$v[Item.WIDTH]
                    item.$place(vertical ? Item.Y : Item.X, pos)
                }
                if(size === undefined){
                    if(average === 0 || pos > keepEnd) break
                    pos += average + spacing
                    continue
                }
                pos += size + spacing
            }
            for(let index of released) this.$release(index)
            for(let index of Array.from(this.$entries.keys())){
                if(index >= count) this.$release(index)
            }
            let known = i
            let estimated = pos + (count - known) * (this.$averageSize() + spacing)
            let contentSize = Math.max(0, count ? estimated - spacing : 0)
            this.$write(vertical ? ListView.CONTENTHEIGHT : ListView.CONTENTWIDTH, contentSize)
            this.$syncContentItem()
            this.$updateCurrentItem()
        } finally {
            this.$filling = false
        }
        if(!withBuffer && this.$v[ListView.CACHEBUFFER] > 0){
            if(added) this.$scheduleBuffer()
            else this.$fill(true)
        }
    }

    // QQuickListViewPrivate::addVisibleItems: a jump of more than an item past the built ones releases them before the fill,
    // so the fill reuses them
    $releaseOnJump(keepStart, keepEnd, spacing, current){
        let first = Infinity, last = -1
        for(let index of this.$entries.keys()){
            if(index === current) continue
            if(index < first) first = index
            if(index > last) last = index
        }
        if(last < 0) return
        let average = this.$averageSize()
        let pos = 0, visiblePos = 0
        for(let k = 0; k <= last; k++){
            if(k === first) visiblePos = pos
            pos += (this.$sizes[k] === undefined ? average : this.$sizes[k]) + spacing
        }
        if(keepStart <= pos + average + spacing && keepEnd >= visiblePos - average - spacing) return
        for(let index of Array.from(this.$entries.keys())){
            if(index !== current) this.$release(index)
        }
    }

    $scheduleBuffer(){
        if(this.$bufferTimer) return
        this.$bufferTimer = setTimeout(() => {
            this.$bufferTimer = null
            this.$fillBuffer = true
            engine.queuePolish(this)
        }, 16)
    }

    $polish(){
        let withBuffer = !!this.$fillBuffer
        this.$fillBuffer = false
        this.$fill(withBuffer)
    }

    // QQuickItemView::updateCurrent: the new current item takes the focus, then currentIndexChanged, then currentItemChanged
    $focusCurrentItem(){
        let entry = this.$entries.get(this.$v[ListView.CURRENTINDEX])
        if(entry && entry.item !== this.$v[ListView.CURRENTITEM]) entry.item.$setFocus(true)
    }

    $updateCurrentItem(){
        this.$focusCurrentItem()
        let entry = this.$entries.get(this.$v[ListView.CURRENTINDEX])
        this.$write(ListView.CURRENTITEM, entry ? entry.item : null)
    }

    $write(id, value){
        let old = this.$v[id]
        super.$write(id, value)
        if(id === ListView.CURRENTINDEX && this.$v[id] !== old) this.$updateCurrentItem()
        // QQuickListView::geometryChange drains the pool, setReuseItems(false) as well
        else if((id === Item.WIDTH || id === Item.HEIGHT || id === ListView.REUSEITEMS) && this.$v[id] !== old && this.$pool.length){
            if(id !== ListView.REUSEITEMS || !this.$v[id]) this.$drainPool()
        }
    }

    $modelChanged(kind, index, count, roles = null){
        if(kind === 'insert' || kind === 'remove'){
            let shifted = new Map()
            for(let [i, entry] of this.$entries){
                if(kind === 'remove' && i >= index && i < index + count){
                    entry.item.$releaseLater()
                    continue
                }
                let to = i >= index ? (kind === 'insert' ? i + count : i - count) : i
                entry.ctx.$write(ModelContext.INDEX, to)
                shifted.set(to, entry)
            }
            this.$entries = shifted
            if(kind === 'insert') this.$sizes.splice(index, 0, ...new Array(count))
            else this.$sizes.splice(index, count)
            let total = modelRows(this.$model).count
            this.$write(ListView.COUNT, total)
            let current = this.$v[ListView.CURRENTINDEX]
            if(kind === 'insert' && current >= index && current >= 0 && total > count) this.$write(ListView.CURRENTINDEX, current + count)
            else if(kind === 'remove' && current >= index + count) this.$write(ListView.CURRENTINDEX, current - count)
            else if(kind === 'remove' && current >= index) this.$write(ListView.CURRENTINDEX, Math.min(index, total - 1))
            if(total > 0 && this.$v[ListView.CURRENTINDEX] < 0) this.$write(ListView.CURRENTINDEX, 0)
            engine.queueFrame(this)
        } else if(kind === 'change'){
            let rows = modelRows(this.$model)
            for(let i = index; i < index + count; i++){
                let entry = this.$entries.get(i)
                if(!entry) continue
                entry.ctx.$row = rows.row(i)
                entry.ctx.$write(ModelContext.MODELDATA, rows.data(i))
                entry.ctx.$rowChanged(roles)
            }
        } else {
            this.$sync()
        }
    }

    $wheel(x, y, angleDelta, pixelDelta){
        if(!this.$v[ListView.INTERACTIVE]) return false
        let vertical = this.$vertical()
        return require('./Flickable').wheelScroll(this, ListView.CONTENTX, ListView.CONTENTY,
            vertical ? this.$v[Item.WIDTH] : this.$contentSize(), vertical ? this.$contentSize() : this.$v[Item.HEIGHT], angleDelta, pixelDelta)
    }

    $disposeNative(){
        if(this.$model instanceof ListModel) this.$model.$removeView(this)
        if(this.$bufferTimer) clearTimeout(this.$bufferTimer)
        super.$disposeNative()
    }
}

const resync = (view) => { if(view.$f & engine.COMPLETED) view.$sync() }
const modelSet = (view, value, old) => { if(!sameModel(value, old)) resync(view) }
const refill = (view) => engine.queuePolish(view)
const scrolled = (view) => {
    view.$content.$write(Item.X, -view.$v[ListView.CONTENTX])
    view.$content.$write(Item.Y, -view.$v[ListView.CONTENTY])
    engine.queuePolish(view)
}

ListView.prototype.$isFocusScope = true

defineType(ListView, {
    props: {
        model: { kind: 'var', effect: modelSet },
        delegate: { kind: 'component', effect: resync },
        count: { kind: 'int', readonly: true },
        currentIndex: { kind: 'int', initial: -1, effect: (view) => view.$focusCurrentItem() },
        currentItem: { kind: 'var', initial: null, readonly: true },
        orientation: { kind: 'int', initial: VERTICAL, effect: refill },
        spacing: { kind: 'real', effect: refill },
        cacheBuffer: { kind: 'real', initial: 320, effect: refill },
        contentX: { kind: 'real', effect: scrolled },
        contentY: { kind: 'real', effect: scrolled },
        contentWidth: 'real',
        contentHeight: 'real',
        interactive: { kind: 'bool', initial: true },
        keyNavigationEnabled: { kind: 'bool', initial: true },
        highlightFollowsCurrentItem: { kind: 'bool', initial: true },
        reuseItems: 'bool',
        boundsBehavior: { kind: 'int', initial: 3 },
        boundsMovement: { kind: 'int', initial: 1 },
        snapMode: 'int',
        flickableDirection: 'int',
        highlightRangeMode: 'int',
        preferredHighlightBegin: 'real',
        preferredHighlightEnd: 'real',
        highlightMoveDuration: { kind: 'int', initial: -1 },
        highlightMoveVelocity: { kind: 'real', initial: 400 },
        highlight: 'component',
        header: 'component',
        footer: 'component',
        section: 'var',
        verticalLayoutDirection: 'int',
        layoutDirection: 'int',
        maximumFlickVelocity: { kind: 'real', initial: 2500 },
        flickDeceleration: { kind: 'real', initial: 1500 },
        moving: { kind: 'bool', readonly: true },
        flicking: { kind: 'bool', readonly: true },
        dragging: { kind: 'bool', readonly: true },
        atYBeginning: { kind: 'bool', initial: true, readonly: true },
        atYEnd: { kind: 'bool', initial: true, readonly: true },
        atXBeginning: { kind: 'bool', initial: true, readonly: true },
        atXEnd: { kind: 'bool', initial: true, readonly: true },
        originX: { kind: 'real', readonly: true },
        originY: { kind: 'real', readonly: true },
    },
    signals: ['movementStarted', 'movementEnded', 'flickStarted', 'flickEnded', 'dragStarted', 'dragEnded'],
})

ListView.Horizontal = HORIZONTAL
Object.assign(ListView, { Beginning: 0, Center: 1, End: 2, Visible: 3, Contain: 4, SnapPosition: 5 })
ListView.Vertical = VERTICAL

// size changes of the view refill it
{
    let base = ListView.prototype.$write
    ListView.prototype.$write = function(id, value){
        base.call(this, id, value)
        if(id === Item.WIDTH || id === Item.HEIGHT){
            this.$syncContentItem()
            engine.queuePolish(this)
        }
    }
}

// The ListView attached group property of every Item, wired here because it needs ListViewAttached
{
    let prop = Item.$type.propByName.get('ListView')
    prop.group = ListViewAttached
    let id = prop.id
    Object.defineProperty(Item.prototype, 'ListView', {
        get(){ return this.$v[id] || (this.$v[id] = new ListViewAttached(this)) },
        configurable: true,
    })
}

module.exports = { ListView, ListViewAttached }
