const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { Item } = require('./Item')
const { ListModel } = require('../QtQml/Models/ListModel')
const { ModelContext, modelRows, sameModel } = require('../QtQml/Models/ModelContext')
const { GroupObject } = require('../Core/QtObject')
const { installFlick, flickDirection, flickState, wheelScroll, externalMove, fixup, FLICK_PROPS, FLICK_SIGNALS, FLICK_ENUMS } = require('./Flickable')
const { SmoothedAnimation } = require('./SmoothedAnimation')
const QtEnums = require('../Qt/enums')

const HORIZONTAL = 1
const VERTICAL = 2
const VISIBILITY = require('./Item').bits.VISIBILITY

const TOP_TO_BOTTOM = 0, BOTTOM_TO_TOP = 1
const RIGHT_TO_LEFT = 1
const NO_SNAP = 0, SNAP_TO_ITEM = 1, SNAP_ONE_ITEM = 2
const NO_HIGHLIGHT_RANGE = 0, APPLY_RANGE = 1, STRICTLY_ENFORCE_RANGE = 2
const INLINE_HEADER = 0, OVERLAY_HEADER = 1, PULL_BACK_HEADER = 2
// QQuickItemViewPrivate::MovementReason: the view follows the highlight only after a new current index
const MOVE_OTHER = 0, MOVE_SET_INDEX = 1, MOVE_MOUSE = 2
const FULL_STRING = 0, FIRST_CHARACTER = 1

const ENUMS = {
    Horizontal: HORIZONTAL, Vertical: VERTICAL,
    Beginning: 0, Center: 1, End: 2, Visible: 3, Contain: 4, SnapPosition: 5,
    TopToBottom: TOP_TO_BOTTOM, BottomToTop: BOTTOM_TO_TOP, LeftToRight: 0, RightToLeft: RIGHT_TO_LEFT,
    NoSnap: NO_SNAP, SnapToItem: SNAP_TO_ITEM, SnapOneItem: SNAP_ONE_ITEM,
    NoHighlightRange: NO_HIGHLIGHT_RANGE, ApplyRange: APPLY_RANGE, StrictlyEnforceRange: STRICTLY_ENFORCE_RANGE,
    InlineHeader: INLINE_HEADER, OverlayHeader: OVERLAY_HEADER, PullBackHeader: PULL_BACK_HEADER,
    InlineFooter: 0, OverlayFooter: 1, PullBackFooter: 2,
}

// ListView attached object (QQuickListViewAttached): view, isCurrentItem, the sections; the reuse signals.
// It holds the enums of the type too: ListView.Horizontal in a delegate
class ListViewAttached extends GroupObject {}
defineType(ListViewAttached, {
    props: {
        view: { kind: 'var', readonly: true },
        isCurrentItem: { kind: 'bool', readonly: true },
        delayRemove: 'bool',
        section: { kind: 'string', readonly: true },
        previousSection: { kind: 'string', readonly: true },
        nextSection: { kind: 'string', readonly: true },
    },
    signals: ['pooled', 'reused', 'add', 'remove'],
})
Object.assign(ListViewAttached.prototype, ENUMS)
// ListView.view of a delegate while its bindings are first evaluated: the view it is made in (QQuickItemView::initItem
// sets it before the delegate is complete)
{
    let get = Object.getOwnPropertyDescriptor(ListViewAttached.prototype, 'view').get
    Object.defineProperty(ListViewAttached.prototype, 'view', {
        get(){
            let view = get.call(this)
            if(view) return view
            let parent = this.$owner && this.$owner.$parent
            let candidate = parent && parent.$parent
            return candidate instanceof ListView && candidate.$content === parent ? candidate : null
        },
        configurable: true,
    })
}

// ListView.section (QQuickViewSection)
class ViewSection extends GroupObject {}
const sectionChanged = (section) => { let view = section.$owner; if(view.$f & engine.COMPLETED) view.$sync() }
defineType(ViewSection, {
    props: {
        property: { kind: 'string', effect: sectionChanged },
        criteria: { kind: 'int', effect: sectionChanged },
        delegate: { kind: 'component', effect: sectionChanged },
        labelPositioning: { kind: 'int', effect: sectionChanged },
    },
})
Object.assign(ViewSection, { FullString: FULL_STRING, FirstCharacter: FIRST_CHARACTER, InlineLabels: 1, CurrentLabelAtStart: 2, NextLabelAtEnd: 4 })

function attachedOf(item){
    return item.ListView
}

// Delegates exist only for [position - cacheBuffer, position + size + cacheBuffer], like Qt. Positions are along the
// flow: the first item at 0, the header before it at -headerSize, the footer after the last item. With
// BottomToTop (RightToLeft) the flow runs up (left): an item at flow position p is at y = -p - size
// (QQuickListViewPrivate::setPosition, FxListItemSG::setPosition)
class ListView extends Item {
    constructor(parent = null){
        super(parent)
        this.$content = new Item(this)
        // QQuickListViewPrivate::itemGeometryChanged: a delegate (or its section label) changing its size along the
        // flow lays the view out again; not one made, released or moved by the view
        this.$content.$childGeometryChanged = (child) => {
            let entry = child.$lvEntry
            if(entry && entry.size !== undefined && this.$entries.get(entry.index) === entry && this.$entrySize(entry) !== entry.size) this.$forceLayoutPolish()
        }
        this.$entries = new Map()     // index -> { item, ctx, section }
        this.$visible = []            // QQuickItemViewPrivate::visibleItems: the entries built in a row, each at its position
        this.$visibleIndex = 0        // the index of the first of them (visibleIndex)
        this.$visiblePos = 0          // its position (visiblePos)
        this.$currentEntry = null     // the entry of the current item
        this.$moveReason = MOVE_OTHER
        this.$internalMove = 0        // the view moves itself (setPosition): not a move of the user
        this.$model = null
        this.$filling = false
        this.$pool = []               // released { item, ctx } kept for reuseItems
        this.$header = null
        this.$footer = null
        this.$highlight = null
        this.$highlightAnimations = null
        // QQuickListViewPrivate::averageSize: 100 until items are laid out, then the rounded mean of the built ones
        this.$average = 100
        this.$flickDirectionSet = false
        this.$keyNavigationSet = false
        engine.scheduleComponentComplete(this)
    }

    get contentItem(){ return this.$content }

    // ---- the flow ----

    $vertical(){ return this.$v[ListView.ORIENTATION] !== HORIZONTAL }
    $reversed(){
        return this.$vertical() ? this.$v[ListView.VERTICALLAYOUTDIRECTION] === BOTTOM_TO_TOP : this.$v[ListView.LAYOUTDIRECTION] === RIGHT_TO_LEFT
    }
    $viewSize(){ return this.$vertical() ? this.$v[Item.HEIGHT] : this.$v[Item.WIDTH] }
    $contentSize(){ return this.$vertical() ? this.$v[ListView.CONTENTHEIGHT] : this.$v[ListView.CONTENTWIDTH] }
    $headerSize(){ return this.$header ? this.$sizeOf(this.$header) : 0 }
    $footerSize(){ return this.$footer ? this.$sizeOf(this.$footer) : 0 }
    $sizeOf(item){ return this.$vertical() ? item.$v[Item.HEIGHT] : item.$v[Item.WIDTH] }

    // the flow position of the start of the view (QQuickItemViewPrivate::position)
    $position(){
        let pos = this.$vertical() ? this.$v[ListView.CONTENTY] : this.$v[ListView.CONTENTX]
        return this.$reversed() ? -pos - this.$viewSize() : pos
    }

    // QQuickListViewPrivate::setPosition
    $setPosition(pos){
        let content = this.$reversed() ? -pos - this.$viewSize() : pos
        this.$internalMove++
        try {
            if(this.$vertical()) this.contentY = content
            else this.contentX = content
        } finally {
            this.$internalMove--
        }
    }

    // QQuickItemView::setContentX / setContentY: positioning the view by hand overrides the movement state
    $assign(id, value){
        if((id === ListView.CONTENTX || id === ListView.CONTENTY) && !this.$internalMove) this.$moveReason = MOVE_OTHER
        super.$assign(id, value)
    }

    $placeFlow(item, pos){
        let size = this.$sizeOf(item)
        item.$place(this.$vertical() ? Item.Y : Item.X, this.$reversed() ? -pos - size : pos)
    }

    // ---- positions along the flow (QQuickListViewPrivate, FxListItemSG) ----
    // The view keeps the entries it built in a row ($visible, visibleItems), each at its own position; the rows not
    // built are estimated from them with the average size. An entry's position is where its section label, then its
    // item, start; its size is the two together

    $entrySize(entry){ return (entry.section ? this.$sizeOf(entry.section) : 0) + this.$sizeOf(entry.item) }
    $entryEnd(entry){ return entry.pos + this.$entrySize(entry) }

    // FxListItemSG::setPosition: the section label at pos, the item after it
    $setEntryPos(entry, pos){
        entry.pos = pos
        entry.item.$lvEntry = entry
        if(entry.section) entry.section.$lvEntry = entry
        if(entry.section){
            this.$placeFlow(entry.section, pos)
            pos += this.$sizeOf(entry.section)
        }
        this.$placeFlow(entry.item, pos)
    }

    // QQuickItemViewPrivate::visibleItem
    $visibleEntry(index){
        let i = index - this.$visibleIndex
        let entry = i >= 0 && i < this.$visible.length ? this.$visible[i] : null
        return entry && entry.index === index ? entry : null
    }

    // the current item when it is not one of the visible entries (QQuickItemViewPrivate::currentItem)
    $currentOutside(){
        let index = this.$v[ListView.CURRENTINDEX]
        let entry = index >= 0 ? this.$entries.get(index) : null
        return entry && !this.$visibleEntry(index) ? entry : null
    }

    // QQuickListViewPrivate::originPosition: where the first row would start
    $originPosition(){
        let visible = this.$visible
        if(!visible.length) return 0
        let pos = visible[0].pos
        if(this.$visibleIndex > 0) pos -= this.$visibleIndex * (this.$average + this.$v[ListView.SPACING])
        return pos
    }

    // QQuickListViewPrivate::lastPosition: where the last row would end
    $lastPosition(){
        let visible = this.$visible
        let count = this.$v[ListView.COUNT]
        let spacing = this.$v[ListView.SPACING]
        if(visible.length){
            let last = visible[visible.length - 1]
            let invisible = count - (last.index + 1)
            let pos = this.$entryEnd(last)
            if(invisible > 0) pos += invisible * (this.$average + spacing)
            return pos
        }
        return count ? count * this.$average + (count - 1) * spacing : 0
    }

    // ---- the Flickable side: bounds of contentX / contentY (QQuickItemViewPrivate::minExtentForAxis, maxExtentForAxis) ----

    $flickBounds(axis){
        let along = (axis === 'y') === this.$vertical()
        let viewSize = axis === 'y' ? this.$v[Item.HEIGHT] : this.$v[Item.WIDTH]
        if(!along) return { min: 0, max: 0 }
        let header = this.$headerSize(), footer = this.$footerSize()
        let last = this.$lastPosition()
        let count = this.$v[ListView.COUNT]
        let range = this.$highlightRange()
        let minExtent, maxExtent
        if(this.$reversed()){
            // the flow runs against the axis: the end of the last item is at the far end
            let highlightStart = viewSize - range.end, highlightEnd = viewSize - range.start
            // endPosition() of a reversed flow is -originPosition()
            let origin = this.$originPosition()
            maxExtent = origin + viewSize
            if(count && range.strict){
                maxExtent = origin + highlightStart
                if(highlightEnd !== highlightStart) maxExtent = Math.max(maxExtent, origin + highlightEnd)
            }
            maxExtent -= header
            minExtent = last + (count ? 0 : header)
            if(range.strict && count){
                minExtent += highlightStart
                minExtent = Math.min(minExtent, this.$positionAt(count - 1) + highlightEnd)
            }
            minExtent += footer
            if(minExtent < maxExtent) minExtent = maxExtent
        } else {
            // -startPosition(): the first row's estimated position
            minExtent = header - this.$originPosition()
            if(range.strict && count){
                minExtent += range.start
                let first = this.$visibleEntry(0)
                if(first && first.section) minExtent -= this.$sizeOf(first.section)
                minExtent = Math.max(minExtent, -(this.$endPositionAt(0) - range.end))
            }
            if(!count) maxExtent = viewSize
            else if(range.strict){
                maxExtent = -(this.$positionAt(count - 1) - range.start)
                if(range.end !== range.start) maxExtent = Math.min(maxExtent, -(last - range.end))
            } else maxExtent = -(last - viewSize)
            maxExtent -= footer
            if(maxExtent > minExtent) maxExtent = minExtent
        }
        return { min: -minExtent, max: -maxExtent }
    }

    $flickAxis(axis){
        let content = axis === 'y' ? this.$v[ListView.CONTENTHEIGHT] : this.$v[ListView.CONTENTWIDTH]
        return flickDirection(this.$v[ListView.FLICKABLEDIRECTION], content, axis === 'y' ? this.$v[Item.HEIGHT] : this.$v[Item.WIDTH], axis)
    }

    $contentPos(axis){ return this.$v[axis === 'x' ? ListView.CONTENTX : ListView.CONTENTY] }

    // the flick engine moves the view: QQuickListView::viewportMoved takes it for a move of the mouse while flicking
    $setContentPos(axis, value){
        let s = this.$fs
        if(s && (s.pressed || s.moving || s.dragging || s.x.flicking || s.y.flicking)) this.$moveReason = MOVE_MOUSE
        this.$internalMove++
        try {
            if(axis === 'x') this.contentX = value
            else this.contentY = value
        } finally {
            this.$internalMove--
        }
    }

    $highlightRange(){
        let mode = this.$v[ListView.HIGHLIGHTRANGEMODE]
        return { mode, strict: mode === STRICTLY_ENFORCE_RANGE, start: this.$v[ListView.PREFERREDHIGHLIGHTBEGIN], end: this.$v[ListView.PREFERREDHIGHLIGHTEND] }
    }

    // QQuickListViewPrivate::positionAt: a visible entry's position, else estimated from the first visible entry (the
    // current item counted at its size) or after the last one
    $positionAt(index){
        let entry = this.$visibleEntry(index)
        if(entry) return entry.pos
        let visible = this.$visible
        if(!visible.length) return 0
        let spacing = this.$v[ListView.SPACING]
        let average = this.$average
        if(index < this.$visibleIndex){
            let count = this.$visibleIndex - index
            let cs = 0
            let current = index === this.$v[ListView.CURRENTINDEX] ? this.$entries.get(index) : null
            if(current){
                cs = this.$entrySize(current) + spacing
                count--
            }
            return visible[0].pos - count * (average + spacing) - cs
        }
        let last = visible[visible.length - 1]
        return this.$entryEnd(last) + spacing + (index - last.index - 1) * (average + spacing)
    }

    // the position of the item itself, after its section label
    $itemPositionAt(index){
        let entry = this.$visibleEntry(index)
        return this.$positionAt(index) + (entry && entry.section ? this.$sizeOf(entry.section) : 0)
    }

    // QQuickListViewPrivate::endPositionAt
    $endPositionAt(index){
        let entry = this.$visibleEntry(index)
        if(entry) return this.$entryEnd(entry)
        let visible = this.$visible
        if(!visible.length) return 0
        let spacing = this.$v[ListView.SPACING]
        let average = this.$average
        if(index < this.$visibleIndex) return visible[0].pos - (this.$visibleIndex - index - 1) * (average + spacing) - spacing
        let last = visible[visible.length - 1]
        return this.$entryEnd(last) + (index - last.index - 1) * (average + spacing)
    }

    // QQuickListViewPrivate::snapItemAt: the visible entry whose middle (with half the spacing) is nearest pos; past the
    // visible entries the index the average size gives (snapPosAt)
    $snapIndexAt(pos){
        let spacing = this.$v[ListView.SPACING]
        let highlight = this.$highlight
        let snap = -1, previousSize = 0
        for(let entry of this.$visible){
            let top = entry.pos, size = this.$entrySize(entry)
            if(highlight && top >= pos && top + size <= pos + this.$sizeOf(highlight)) return entry.index
            if(top + (size + spacing) / 2 >= pos && top - (previousSize + spacing) / 2 < pos) snap = entry.index
            previousSize = this.$sizeOf(entry.item)
        }
        if(snap >= 0) return snap
        let count = this.$v[ListView.COUNT]
        if(!count) return -1
        let step = this.$average + spacing
        let index = step > 0 ? Math.round((pos - this.$originPosition()) / step) : 0
        return Math.max(0, Math.min(count - 1, index))
    }

    // QQuickListViewPrivate::fixup with snapMode: the start of the view goes to the start of the nearest item
    // (with SnapOneItem at most one item away from where the press found it); the header snaps when half shown
    $fixupTarget(axis, target, state){
        let along = (axis === 'y') === this.$vertical()
        let snap = this.$v[ListView.SNAPMODE]
        if(!along || snap === NO_SNAP || !this.$v[ListView.COUNT]) return target
        let range = this.$highlightRange()
        let flowPos = this.$reversed() ? -target - this.$viewSize() : target
        let tempPos = flowPos
        if(snap === SNAP_ONE_ITEM && state){
            let pressFlow = this.$reversed() ? -state.pressPos - this.$viewSize() : state.pressPos
            let dist = flowPos - pressFlow
            let average = this.$averageSize()
            if(dist > 0 && dist < average / 2) tempPos += average / 2
            else if(dist < 0 && dist > -average / 2) tempPos -= average / 2
        }
        let header = this.$header
        let pos
        if(header && tempPos < 0 && tempPos + range.start < -this.$headerSize() / 2) pos = -this.$headerSize()
        else {
            let index = this.$snapIndexAt(tempPos + range.start)
            pos = index < 0 ? 0 : this.$positionAt(index) - range.start
        }
        let content = this.$reversed() ? -pos - this.$viewSize() : pos
        let b = this.$flickBounds(axis)
        return Math.max(Math.min(b.min, b.max), Math.min(Math.max(b.min, b.max), content))
    }

    // QQuickListViewPrivate::flick with snapMode: the flick stops on an item; SnapOneItem moves one item
    $flickTarget(axis, target, velocity, state){
        let along = (axis === 'y') === this.$vertical()
        let snap = this.$v[ListView.SNAPMODE]
        if(!along || snap === NO_SNAP || !this.$v[ListView.COUNT]) return target
        if(snap === SNAP_ONE_ITEM){
            let pressFlow = this.$reversed() ? -state.pressPos - this.$viewSize() : state.pressPos
            let index = this.$snapIndexAt(pressFlow + this.$highlightRange().start)
            let forward = (velocity > 0) !== this.$reversed()
            index = Math.max(0, Math.min(this.$v[ListView.COUNT] - 1, index + (forward ? 1 : -1)))
            let pos = this.$positionAt(index) - this.$highlightRange().start
            let content = this.$reversed() ? -pos - this.$viewSize() : pos
            let b = this.$flickBounds(axis)
            return Math.max(b.min, Math.min(b.max, content))
        }
        return this.$fixupTarget(axis, target, null)
    }

    // ---- the API ----

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

    // QQuickItemView::positionViewAtBeginning / End: QQuickItemViewPrivate::positionViewAtIndex(-1, Beginning) and
    // (count, End)
    positionViewAtBeginning(){
        this.$positionViewAt(-1, ListView.Beginning)
    }

    positionViewAtEnd(){
        this.$positionViewAt(this.$v[ListView.COUNT], ListView.End)
    }

    // QQuickItemView::positionViewAtIndex
    positionViewAtIndex(index, mode = ListView.Beginning){
        let count = this.$v[ListView.COUNT]
        if(count <= 0 || index < 0 || index >= count) return
        this.$positionViewAt(index, mode)
    }

    // the flow positions the view may take (QQuickItemViewPrivate::calculatedMinExtent, calculatedMaxExtent)
    $flowBounds(){
        let b = this.$flickBounds(this.$vertical() ? 'y' : 'x')
        if(!this.$reversed()) return { min: b.min, max: b.max }
        let viewSize = this.$viewSize()
        return { min: -b.max - viewSize, max: -b.min - viewSize }
    }

    // QQuickItemViewPrivate::positionViewAtIndex: an index not built goes to its estimated position, where the view
    // starts again (changedVisibleIndex) and fills; then the mode places its item, within the extents
    $positionViewAt(index, mode){
        if(!(this.$f & engine.COMPLETED) || !this.$hasItems()) return
        this.$moveReason = MOVE_OTHER
        let count = this.$v[ListView.COUNT]
        if(count <= 0 || mode < ListView.Beginning || mode > ListView.SnapPosition) return
        this.$fill()
        let idx = Math.max(Math.min(index, count - 1), 0)
        let viewSize = this.$viewSize()
        let pos = this.$position()
        let entry = this.$visibleEntry(idx)
        // the extent of the view before it moves (calculatedMaxExtent, taken first)
        let maxExtent = this.$flowBounds().max
        if(!entry){
            let itemPos = this.$positionAt(idx)
            this.$visiblePos = itemPos
            this.$visibleIndex = idx
            // the entries built stay while the view fills, so a row built again keeps its item
            let old = this.$visible
            this.$visible = []
            this.$setPosition(Math.min(itemPos, maxExtent))
            this.$fill()
            for(let e of old) if(this.$visible.indexOf(e) < 0) this.$releaseEntry(e)
            entry = this.$visibleEntry(idx)
        }
        if(entry){
            let stickyHeader = !!this.$header && this.$v[ListView.HEADERPOSITIONING] !== INLINE_HEADER
            let stickyFooter = !!this.$footer && this.$v[ListView.FOOTERPOSITIONING] !== INLINE_HEADER
            let headerSize = stickyHeader ? this.$headerSize() : 0
            let footerSize = stickyFooter ? this.$footerSize() : 0
            let itemPos = entry.pos, size = this.$entrySize(entry), end = itemPos + size
            switch(mode){
                case ListView.Beginning:
                    pos = itemPos
                    if(this.$header && (index < 0 || stickyHeader)) pos -= this.$headerSize()
                    break
                case ListView.Center: pos = itemPos - (viewSize - size) / 2; break
                case ListView.End:
                    pos = itemPos - viewSize + size
                    if(this.$footer && (index >= count || stickyFooter)) pos += this.$footerSize()
                    break
                case ListView.Visible:
                    if(itemPos > pos + viewSize - footerSize) pos = end - viewSize + footerSize
                    else if(end <= pos - headerSize) pos = itemPos - headerSize
                    break
                case ListView.Contain:
                    if(end >= pos + viewSize + footerSize) pos = itemPos - viewSize + size + footerSize
                    if(itemPos - headerSize < pos) pos = itemPos - headerSize
                    break
                case ListView.SnapPosition: pos = itemPos - this.$v[ListView.PREFERREDHIGHLIGHTBEGIN] - headerSize; break
            }
            pos = Math.max(Math.min(pos, maxExtent), this.$flowBounds().min)
            this.$setPosition(pos)
            this.$fill()
            this.$updateHighlight()
        }
    }

    // QQuickItemView::forceLayout: layout() now (the visible items one after the other, refill), not at the next polish
    forceLayout(){
        if(!(this.$f & engine.COMPLETED)) return
        this.$forceLayout = true
        this.$polish()
    }

    $frame(){ this.$fill() }

    // QQuickListView::incrementCurrentIndex / decrementCurrentIndex: keyNavigationWraps goes round
    incrementCurrentIndex(){
        let count = this.$v[ListView.COUNT]
        let current = this.$v[ListView.CURRENTINDEX]
        if(count && (current < count - 1 || this.$v[ListView.KEYNAVIGATIONWRAPS])) this.currentIndex = current < count - 1 ? current + 1 : 0
    }

    decrementCurrentIndex(){
        let count = this.$v[ListView.COUNT]
        let current = this.$v[ListView.CURRENTINDEX]
        if(count && (current > 0 || this.$v[ListView.KEYNAVIGATIONWRAPS])) this.currentIndex = current > 0 ? current - 1 : count - 1
    }

    // QQuickListView::keyPressEvent: the arrows along the flow move the current index (reversed with BottomToTop),
    // when keyNavigationEnabled (which follows interactive until it is set)
    $keyEvent(event, kind){
        if(kind !== 'pressed' || !this.$v[ListView.COUNT]) return 0
        let enabled = this.$keyNavigationSet ? this.$v[ListView.KEYNAVIGATIONENABLED] : this.$v[ListView.INTERACTIVE]
        if(!enabled) return 0
        let vertical = this.$vertical(), reversed = this.$reversed()
        let key = event.key
        let back = vertical ? (reversed ? QtEnums.Key_Down : QtEnums.Key_Up) : (reversed ? QtEnums.Key_Right : QtEnums.Key_Left)
        let forward = vertical ? (reversed ? QtEnums.Key_Up : QtEnums.Key_Down) : (reversed ? QtEnums.Key_Left : QtEnums.Key_Right)
        let wrap = this.$v[ListView.KEYNAVIGATIONWRAPS]
        let current = this.$v[ListView.CURRENTINDEX]
        if(key === back){
            if(current > 0 || (wrap && !event.isAutoRepeat)){
                this.decrementCurrentIndex()
                return 1
            }
            return wrap ? 1 : 0
        }
        if(key === forward){
            if(current < this.$v[ListView.COUNT] - 1 || (wrap && !event.isAutoRepeat)){
                this.incrementCurrentIndex()
                return 1
            }
            return wrap ? 1 : 0
        }
        return 0
    }

    // updateViewport: the size along the flow; across it the content item follows the view (Flickable contentWidth -1)
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

    $averageSize(){ return this.$average }

    // QQuickListViewPrivate::updateAverage: the rounded mean size of the visible entries; none, it stays
    $updateAverage(){
        let visible = this.$visible
        if(!visible.length) return
        let sum = 0
        for(let entry of visible) sum += this.$entrySize(entry)
        this.$average = Math.round(sum / visible.length)
    }

    $componentComplete(){
        super.$componentComplete()
        this.$sync('complete')
    }

    // ---- header, footer, highlight (QQuickItemViewPrivate::createComponentItem: z 1, the highlight z 0) ----

    $componentItem(component, z){
        if(!component) return null
        let item = component.createObject(this.$content, null, null)
        if(!item) return null
        if(!item.$v[Item.Z]) item.z = z
        let attached = attachedOf(item)
        attached.$write(ListViewAttached.VIEW, this)
        return item
    }

    $updateHeaderFooter(){
        let headerComponent = this.$v[ListView.HEADER]
        if(this.$header && this.$header.$component !== headerComponent){
            this.$header.$releaseLater()
            this.$header = null
        }
        if(!this.$header && headerComponent){
            this.$header = this.$componentItem(headerComponent, 1)
            if(this.$header){
                this.$header.$component = headerComponent
                engine.connect(this.$header, this.$vertical() ? Item.SIGNAL_heightChanged : Item.SIGNAL_widthChanged, () => this.$forceLayoutPolish())
            }
            this.$write(ListView.HEADERITEM, this.$header)
        }
        let footerComponent = this.$v[ListView.FOOTER]
        if(this.$footer && this.$footer.$component !== footerComponent){
            this.$footer.$releaseLater()
            this.$footer = null
        }
        if(!this.$footer && footerComponent){
            this.$footer = this.$componentItem(footerComponent, 1)
            if(this.$footer){
                this.$footer.$component = footerComponent
                engine.connect(this.$footer, this.$vertical() ? Item.SIGNAL_heightChanged : Item.SIGNAL_widthChanged, () => this.$forceLayoutPolish())
            }
            this.$write(ListView.FOOTERITEM, this.$footer)
        }
        if(!headerComponent) this.$write(ListView.HEADERITEM, null)
        if(!footerComponent) this.$write(ListView.FOOTERITEM, null)
    }

    // QQuickListViewPrivate::updateHeader / updateFooter: an inline header before the first row (where the view
    // estimates it), an inline footer after the last; overlay and pull back ones follow the view
    $placeHeaderFooter(){
        let position = this.$position()
        let viewSize = this.$viewSize()
        let visible = this.$visible.length > 0
        if(this.$header){
            let mode = this.$v[ListView.HEADERPOSITIONING]
            let size = this.$headerSize()
            let current = this.$header.$flowPos === undefined ? -size : this.$header.$flowPos
            let pos = current
            if(mode === OVERLAY_HEADER) pos = position
            else if(!visible) pos = -size
            else if(mode === PULL_BACK_HEADER){
                let clamped = Math.max(this.$originPosition() - size, Math.min(current, this.$lastPosition() - viewSize))
                pos = Math.max(position - size, Math.min(clamped, position))
            } else {
                let start = this.$originPosition()
                if(this.$visibleIndex === 0 || position <= start || current > start - size) pos = start - size
            }
            this.$header.$flowPos = pos
            this.$placeFlow(this.$header, pos)
        }
        if(this.$footer){
            let mode = this.$v[ListView.FOOTERPOSITIONING]
            let size = this.$footerSize()
            let current = this.$footer.$flowPos === undefined ? this.$lastPosition() : this.$footer.$flowPos
            let pos = current
            if(mode === OVERLAY_HEADER) pos = position + viewSize - size
            else if(!visible) pos = this.$visiblePos
            else if(mode === PULL_BACK_HEADER){
                let viewEnd = position + viewSize
                let clamped = Math.max(this.$originPosition() - size + viewSize, Math.min(current, this.$lastPosition()))
                pos = Math.max(viewEnd - size, Math.min(clamped, viewEnd))
            } else {
                let end = this.$lastPosition()
                let lastIndex = this.$visible[this.$visible.length - 1].index
                // QQuickListViewPrivate::updateFooter compares with contentY + height along either orientation
                if(lastIndex === this.$v[ListView.COUNT] - 1 || end <= position + this.$v[Item.HEIGHT] || current < end) pos = end
            }
            this.$footer.$flowPos = pos
            this.$placeFlow(this.$footer, pos)
        }
    }

    // QQuickListViewPrivate::createHighlight / updateHighlight: the highlight item follows the current item, moved
    // by a SmoothedAnimation (highlightMoveVelocity, highlightMoveDuration)
    $updateHighlight(){
        let component = this.$v[ListView.HIGHLIGHT]
        let current = this.$v[ListView.CURRENTITEM]
        if(this.$highlight && (this.$highlight.$component !== component || !current)){
            this.$stopHighlightAnimations()
            this.$highlight.$releaseLater()
            this.$highlight = null
            this.$write(ListView.HIGHLIGHTITEM, null)
        }
        if(!current) return
        let vertical = this.$vertical()
        let follows = this.$v[ListView.HIGHLIGHTFOLLOWSCURRENTITEM]
        if(!this.$highlight){
            // QQuickItemViewPrivate::createHighlightItem: without a highlight component an empty item (createDefault)
            let highlight = component ? this.$componentItem(component, 0) : new Item(this.$content)
            if(!highlight) return
            highlight.$component = component
            this.$highlight = highlight
            // QQuickItemViewPrivate::itemGeometryChanged: the tracked item (the highlight) moving moves the view
            for(let signal of [Item.SIGNAL_xChanged, Item.SIGNAL_yChanged, Item.SIGNAL_widthChanged, Item.SIGNAL_heightChanged]){
                engine.connect(highlight, signal, () => { if(this.$highlight === highlight) this.$trackedPositionChanged() })
            }
            // QQuickListViewPrivate::createHighlight: following the current item, it takes its size along the flow
            // and its position at once
            if(follows){
                if(vertical){
                    highlight.height = current.$v[Item.HEIGHT]
                    highlight.y = current.$v[Item.Y]
                } else {
                    highlight.width = current.$v[Item.WIDTH]
                    highlight.x = current.$v[Item.X]
                }
            }
            this.$write(ListView.HIGHLIGHTITEM, highlight)
        }
        // QQuickListViewPrivate::updateHighlight: not while a StrictlyEnforceRange view is pressed; then updateTrackedItem
        let s = this.$fs
        if(!follows || (this.$v[ListView.HIGHLIGHTRANGEMODE] === STRICTLY_ENFORCE_RANGE && s && s.pressed)){
            this.$trackedPositionChanged()
            return
        }
        let highlight = this.$highlight
        // across the flow an empty highlight takes the current item's size at once
        if(vertical && !highlight.$v[Item.WIDTH]) highlight.width = current.$v[Item.WIDTH]
        if(!vertical && !highlight.$v[Item.HEIGHT]) highlight.height = current.$v[Item.HEIGHT]
        // the position by highlightMove*, the width and the height by highlightResize* (three QSmoothedAnimations)
        let move = [this.$v[ListView.HIGHLIGHTMOVEVELOCITY], this.$v[ListView.HIGHLIGHTMOVEDURATION]]
        let resize = [this.$v[ListView.HIGHLIGHTRESIZEVELOCITY], this.$v[ListView.HIGHLIGHTRESIZEDURATION]]
        this.$animateHighlight('pos', vertical ? 'y' : 'x', current.$v[vertical ? Item.Y : Item.X], move)
        this.$animateHighlight('width', 'width', current.$v[Item.WIDTH], resize)
        this.$animateHighlight('height', 'height', current.$v[Item.HEIGHT], resize)
        this.$trackedPositionChanged()
    }

    // QQuickListViewPrivate::resetHighlightPosition: the highlight at the current item at once
    $resetHighlightPosition(){
        let highlight = this.$highlight, current = this.$v[ListView.CURRENTITEM]
        if(!highlight || !current || !this.$v[ListView.HIGHLIGHTFOLLOWSCURRENTITEM]) return
        this.$stopHighlightAnimations()
        let vertical = this.$vertical()
        highlight.$write(vertical ? Item.Y : Item.X, current.$v[vertical ? Item.Y : Item.X])
    }

    // the flow position and size of an item (FxListItemSG::position, size)
    $flowPosOf(item){
        let vertical = this.$vertical()
        let pos = item.$v[vertical ? Item.Y : Item.X]
        return this.$reversed() ? -pos - this.$sizeOf(item) : pos
    }

    // QQuickItemView::trackedPositionChanged: after a new current index (SetIndex) the view follows the tracked item,
    // the highlight: into the highlight range when there is one, else so the highlight and the current item show
    $trackedPositionChanged(){
        let highlight = this.$highlight
        let entry = this.$currentEntry
        if(!highlight || !entry || this.$entries.get(entry.index) !== entry) return
        if(this.$moveReason !== MOVE_SET_INDEX) return
        let range = this.$highlightRange()
        let haveRange = range.mode !== NO_HIGHLIGHT_RANGE && range.start <= range.end
        if(!this.$v[ListView.HIGHLIGHTFOLLOWSCURRENTITEM] && range.mode === NO_HIGHLIGHT_RANGE) return
        let size = this.$viewSize()
        let trackedPos = this.$flowPosOf(highlight)
        let trackedSize = this.$sizeOf(highlight)
        let viewPos = this.$position()
        let pos = viewPos
        if(haveRange){
            if(trackedPos > pos + range.end - trackedSize) pos = trackedPos - range.end + trackedSize
            if(trackedPos < pos + range.start) pos = trackedPos - range.start
            if(range.mode !== STRICTLY_ENFORCE_RANGE){
                let bounds = this.$flowBounds()
                if(pos > bounds.max) pos = bounds.max
                if(pos < bounds.min) pos = bounds.min
            }
        } else {
            let trackedEndPos = trackedPos + trackedSize
            let sectionSize = entry.section ? this.$sizeOf(entry.section) : 0
            // the section label of the current item shows too
            trackedPos -= sectionSize
            trackedSize += sectionSize
            let toItemPos = entry.pos, toItemEndPos = this.$entryEnd(entry)
            let index = entry.index
            // showHeaderForIndex / showFooterForIndex: an inline header before the first row, footer after the last
            if(this.$header && this.$v[ListView.HEADERPOSITIONING] === INLINE_HEADER && index === 0){
                let startOffset = this.$headerSize()
                trackedPos -= startOffset
                trackedEndPos -= startOffset
                toItemPos -= startOffset
                toItemEndPos -= startOffset
            } else if(this.$footer && this.$v[ListView.FOOTERPOSITIONING] === INLINE_HEADER && index === this.$v[ListView.COUNT] - 1){
                let endOffset = this.$footerSize()
                trackedPos += endOffset
                trackedEndPos += endOffset
                toItemPos += endOffset
                toItemEndPos += endOffset
            }
            if(trackedEndPos >= viewPos + size && toItemEndPos >= viewPos + size){
                if(trackedEndPos <= toItemEndPos){
                    pos = trackedEndPos - size
                    if(trackedSize > size) pos = trackedPos
                } else {
                    pos = toItemEndPos - size
                    if(this.$entrySize(entry) > size) pos = toItemPos
                }
            }
            if(trackedPos < pos && toItemPos < pos) pos = Math.max(trackedPos, toItemPos)
        }
        if(pos !== viewPos) this.$setPosition(pos)
    }

    $animateHighlight(key, property, to, [velocity, duration]){
        let animations = this.$highlightAnimations || (this.$highlightAnimations = {})
        let animation = animations[key]
        let highlight = this.$highlight
        if(animation && animation.running && animation.to === to && animation.target === highlight) return
        if(highlight[property] === to){
            if(animation && animation.running) animation.stop()
            return
        }
        if(!animation) animation = animations[key] = new SmoothedAnimation(null)
        if(animation.target !== highlight) animation.target = highlight
        animation.property = property
        animation.velocity = velocity
        animation.duration = duration
        animation.to = to
        animation.restart()
    }

    // QQuickListView::viewportMoved with StrictlyEnforceRange: unless the view moves for a new current index, the
    // highlight stays in the highlight range of the view, and the item at the highlight (snapItemAt) becomes current
    $strictViewportMoved(){
        if(this.$inViewportMoved || this.$moveReason === MOVE_SET_INDEX || !(this.$f & engine.COMPLETED)) return
        let highlight = this.$highlight, range = this.$highlightRange()
        if(!range.strict || !highlight || !this.$v[ListView.COUNT] || this.$reversed()) return
        this.$inViewportMoved = true
        try {
            let viewPos = this.$position()
            let pos = this.$flowPosOf(highlight), size = this.$sizeOf(highlight)
            if(pos > viewPos + range.end - size) pos = viewPos + range.end - size
            if(pos < viewPos + range.start) pos = viewPos + range.start
            if(pos !== this.$flowPosOf(highlight)){
                this.$stopHighlightAnimations()
                highlight.$write(this.$vertical() ? Item.Y : Item.X, pos)
            }
            let index = this.$snapIndexAt(pos)
            if(index >= 0 && index !== this.$v[ListView.CURRENTINDEX] && this.$entries.get(index)){
                // updateCurrent: not a SetIndex move
                this.$viewMovedCurrent = true
                try { this.$write(ListView.CURRENTINDEX, index) } finally { this.$viewMovedCurrent = false }
            }
        } finally {
            this.$inViewportMoved = false
        }
    }

    $stopHighlightAnimations(){
        let animations = this.$highlightAnimations
        if(!animations) return
        for(let key in animations) animations[key].stop()
    }

    // ---- sections (QQuickListViewPrivate::sectionAt, updateInlineSection, updateSections) ----

    $sectionAt(index, rows){
        let section = this.$v[ListView.SECTION]
        let property = section ? section.$v[ViewSection.PROPERTY] : ''
        if(!property) return ''
        let row = rows.row(index)
        let value = row && typeof row === 'object' ? row[property] : undefined
        if(value === undefined){
            let data = rows.data(index)
            value = data && typeof data === 'object' ? data[property] : undefined
        }
        let text = value === undefined || value === null ? '' : String(value)
        return section.$v[ViewSection.CRITERIA] === FIRST_CHARACTER ? text.slice(0, 1).toUpperCase() : text
    }

    $updateEntrySection(index, entry, rows){
        let section = this.$v[ListView.SECTION]
        let property = section ? section.$v[ViewSection.PROPERTY] : ''
        let attached = attachedOf(entry.item)
        if(!property){
            if(entry.section){
                entry.section.$releaseLater()
                entry.section = null
            }
            return
        }
        let current = this.$sectionAt(index, rows)
        let previous = index > 0 ? this.$sectionAt(index - 1, rows) : ''
        let next = index < rows.count - 1 ? this.$sectionAt(index + 1, rows) : ''
        attached.$write(ListViewAttached.SECTION, current)
        attached.$write(ListViewAttached.PREVIOUSSECTION, previous)
        attached.$write(ListViewAttached.NEXTSECTION, next)
        let delegate = section.$v[ViewSection.DELEGATE]
        let needed = !!delegate && (index === 0 || previous !== current)
        if(entry.section && (!needed || entry.section.$sectionText !== current)){
            entry.section.$releaseLater()
            entry.section = null
        }
        if(needed && !entry.section){
            let ctx = new ModelContext({ section: current }, -1, current)
            ctx.$view = this
            let item = delegate.createObject(this.$content, null, ctx)
            if(item){
                if(!item.$v[Item.Z]) item.z = 2
                attachedOf(item).$write(ListViewAttached.VIEW, this)
                item.$sectionText = current
                entry.section = item
            }
        }
    }

    // ---- delegates ----

    // QQuickItemViewPrivate::regenerate: everything released, the viewport for no item, the view at the start, refill,
    // then the current item
    // reason: 'complete' (QQuickItemView::componentComplete), 'model' (setModel), else a regeneration (a new delegate...)
    $sync(reason){
        let model = this.$v[ListView.MODEL]
        if(model !== this.$model){
            if(this.$model && this.$model.$removeView) this.$model.$removeView(this)
            this.$model = model
            if(model && model.$addView) model.$addView(this)
        }
        // QQuickItemViewPrivate::clear releases NotReusable
        for(let index of Array.from(this.$entries.keys())) this.$release(index, false)
        this.$drainPool()
        this.$visible = []
        this.$visibleIndex = 0
        this.$visiblePos = 0
        this.$currentEntry = null
        let count = modelRows(this.$model).count
        this.$write(ListView.COUNT, count)
        this.$updateHeaderFooter()
        this.$placeHeaderFooter()
        this.$updateViewport()
        // QQuickItemViewPrivate::contentStartOffset: before the header and the margin at the start of the flow
        this.$setPosition(-this.$headerSize() - this.$startMargin())
        this.$fill(false)
        // QQuickItemView::componentComplete: refill(), then updateCurrent(0) unless the index was cleared;
        // QQuickItemView::setModel: setCurrentIndex(count > 0 ? 0 : -1) (a model without a delegate has no rows);
        // QQuickItemViewPrivate::regenerate: updateCurrent(currentIndex)
        let index = this.$v[ListView.CURRENTINDEX]
        let defaultIndex = false
        if(reason === 'model') index = count > 0 && this.$v[ListView.DELEGATE] ? 0 : -1
        else if(reason === 'complete' && count > 0 && index < 0 && !this.$currentIndexCleared){
            index = 0
            defaultIndex = true
        }
        if(index >= count) index = count - 1
        let range = this.$highlightRange()
        let strictStart = defaultIndex && range.strict
        this.$inSync = true
        // (the first item of a strict view becomes current without a SetIndex move)
        this.$viewMovedCurrent = strictStart
        try { if(index !== this.$v[ListView.CURRENTINDEX]) this.$write(ListView.CURRENTINDEX, index) } finally { this.$inSync = false; this.$viewMovedCurrent = false }
        if(strictStart && this.$visible.length){
            // a view made with its component and the first item current: the delegates come after componentComplete
            // (incubated), the view stays at its start and the item at the highlight range becomes current (checked)
            this.$moveReason = MOVE_OTHER
            this.$ensureCurrentItem()
            this.$resetHighlightPosition()
            this.$strictViewportMoved()
        } else {
            // QQuickItemView::modelUpdated(reset) / componentComplete: SetIndex for the regeneration, the highlight put
            // at the current item at once (resetHighlightPosition) and tracked
            this.$moveReason = MOVE_SET_INDEX
            this.$ensureCurrentItem()
            this.$resetHighlightPosition()
            this.$trackedPositionChanged()
        }
        this.$moveReason = MOVE_OTHER
        // QQuickItemView::setModel / componentComplete end with updateViewport, also when refill laid nothing out
        // (a view of no size): lastPosition estimates the rows with averageSize - the size of the current item when it
        // is the only one (initializeCurrentItem)
        this.$updateViewport()
    }

    // QQuickItemViewPrivate::updateCurrent, QQuickListViewPrivate::initializeCurrentItem: the current item is made even
    // outside the visible entries, at the position the view estimates for it; with no visible entry the average size
    // is its size
    $ensureCurrentItem(){
        let index = this.$v[ListView.CURRENTINDEX]
        if(index < 0 || this.$entries.has(index) || !this.$hasItems() || !(this.$f & engine.COMPLETED)) return this.$updateCurrentItem()
        let rows = modelRows(this.$model)
        if(index >= rows.count) return this.$updateCurrentItem()
        let entry = this.$create(index, rows)
        if(entry){
            let visible = this.$visible
            let spacing = this.$v[ListView.SPACING]
            if(index === this.$visibleIndex - 1 && visible.length) this.$setEntryPos(entry, visible[0].pos - this.$entrySize(entry) - spacing)
            else this.$setEntryPos(entry, this.$positionAt(index))
            entry.size = this.$entrySize(entry)
            if(!visible.length) this.$average = entry.size
        }
        this.$updateCurrentItem()
    }

    // QQmlDelegateModel::object: the entry of an index; the one the view holds (the current item), else the oldest
    // pooled item with its index and every role updated, then reused(), else a new delegate. Its section label is made
    // with it (QQuickListViewPrivate::newViewItem, initializeViewItem)
    $create(index, rows){
        let entry = this.$entries.get(index)
        if(entry) return entry
        if(this.$pool.length){
            entry = this.$pool.shift()
            let ctx = entry.ctx
            ctx.$row = rows.row(index)
            ctx.$write(ModelContext.INDEX, index)
            ctx.$write(ModelContext.MODELDATA, rows.data(index))
            ctx.$rowChanged(null)
            entry.index = index
            this.$entries.set(index, entry)
            entry.item.$culled = false
            engine.markDirty(entry.item, VISIBILITY)
            attachedOf(entry.item).$write(ListViewAttached.ISCURRENTITEM, index === this.$v[ListView.CURRENTINDEX])
            if(this.$currentAttached === entry.item && index !== this.$v[ListView.CURRENTINDEX]) this.$currentAttached = null
            let attached = entry.item.$v[Item.LISTVIEW]
            if(attached) engine.emitArgs(attached, ListViewAttached.SIGNAL_reused, [])
        } else if(rows.object){
            // an ObjectModel: its item itself (QQmlObjectModel::object), shown in the view and kept when it is released
            let item = rows.object(index)
            if(!item) return null
            let ctx = new ModelContext(null, index, item)
            ctx.$view = this
            if(item.$parent !== this.$content) item.$setParent(this.$content)
            item.$culled = false
            engine.markDirty(item, VISIBILITY)
            attachedOf(item).$write(ListViewAttached.VIEW, this)
            entry = { item, ctx, section: null, index, pos: 0, object: true }
            this.$entries.set(index, entry)
        } else {
            let delegate = this.$v[ListView.DELEGATE]
            let ctx = new ModelContext(rows.row(index), index, rows.data(index))
            ctx.$view = this
            let item = delegate.createObject(this.$content, null, ctx)
            if(!item) return null
            // QQuickItemView::initItem: a delegate at z 0 goes to z 1, above the highlight
            if(!item.$v[Item.Z]) item.z = 1
            attachedOf(item).$write(ListViewAttached.VIEW, this)
            entry = { item, ctx, section: null, index, pos: 0 }
            this.$entries.set(index, entry)
        }
        this.$updateEntrySection(index, entry, rows)
        return entry
    }

    // QQuickItemViewPrivate::releaseItem: with reuseItems the item goes to the pool, culled, then pooled()
    $release(index, reusable = true){
        let entry = this.$entries.get(index)
        if(!entry) return
        this.$entries.delete(index)
        if(this.$currentEntry === entry) this.$currentEntry = null
        if(entry.section){
            entry.section.$releaseLater()
            entry.section = null
        }
        // an item of an ObjectModel is the model's: hidden, not deleted (QQmlObjectModel::release)
        if(entry.object){
            entry.item.$culled = true
            engine.markDirty(entry.item, VISIBILITY)
            return
        }
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

    // items to show: a delegate, or the items of an ObjectModel
    $hasItems(){ return !!this.$v[ListView.DELEGATE] || !!(this.$model && this.$model.$isObjectModel) }

    // a visible entry the view lets go of; the current item stays (the view holds it as currentItem)
    $releaseEntry(entry){
        if(entry.index === this.$v[ListView.CURRENTINDEX] && this.$entries.get(entry.index) === entry) return
        if(this.$entries.get(entry.index) === entry) this.$release(entry.index)
    }

    // QQuickItemViewPrivate::releaseVisibleItems
    $releaseVisible(){
        let visible = this.$visible
        this.$visible = []
        for(let entry of visible) this.$releaseEntry(entry)
    }

    // QQmlDelegateModel::drainReusableItemsPool(0)
    $drainPool(){
        let pool = this.$pool
        if(!pool.length) return
        this.$pool = []
        for(let entry of pool) entry.item.$releaseLater()
    }

    // QQuickItemViewPrivate::refill(from, to): the visible range; the cacheBuffer range on a later pass (bufferPause
    // of 16 ms, then polish), or at once when the visible pass added nothing; then the entries past the buffer go.
    // A null-size view without entries does not fill at all
    $fill(withBuffer = false){
        if(this.$filling || !(this.$f & engine.COMPLETED)) return
        if(!this.$hasItems()){
            this.$placeHeaderFooter()
            return
        }
        if(this.$v[Item.WIDTH] === 0 && this.$v[Item.HEIGHT] === 0 && !this.$visible.length) return
        this.$filling = true
        try {
            let rows = modelRows(this.$model)
            if(!rows.count){
                this.$placeHeaderFooter()
                this.$updateViewport()
                return
            }
            let from = this.$position()
            let to = from + Math.max(0, this.$viewSize())
            let buffer = this.$v[ListView.CACHEBUFFER]
            let bufferFrom = from - buffer, bufferTo = to + buffer
            let added = this.$addVisible(rows, from, to, bufferFrom, bufferTo, false)
            if(buffer > 0){
                if(added) this.$scheduleBuffer()
                else added = this.$addVisible(rows, bufferFrom, bufferTo, bufferFrom, bufferTo, true)
            }
            let removed = this.$removeNonVisible(bufferFrom, bufferTo)
            if(added || removed) this.$visibleItemsChanged()
            this.$placeHeaderFooter()
            this.$updateViewport()
            this.$cull()
            this.$updateCurrentItem()
            this.$updateCurrentSection()
        } finally {
            this.$filling = false
        }
    }

    // QQuickListViewPrivate::addVisibleItems: after the last entry while the fill range goes on, before the first while
    // it starts before. A jump of more than a page past the entries releases them and starts at the index the average
    // size gives
    $addVisible(rows, fillFrom, fillTo, bufferFrom, bufferTo, doBuffer){
        let spacing = this.$v[ListView.SPACING]
        let average = this.$average
        let count = rows.count
        let visible = this.$visible
        let itemEnd = this.$visiblePos
        if(visible.length){
            this.$visiblePos = visible[0].pos
            itemEnd = this.$entryEnd(visible[visible.length - 1]) + spacing
        }
        let last = visible.length ? visible[visible.length - 1].index : -1
        let index = last >= 0 ? last + 1 : this.$visibleIndex
        if(last >= 0 && average + spacing > 0 && (bufferFrom > itemEnd + average + spacing || bufferTo < this.$visiblePos - average - spacing)){
            let jump = Math.trunc((fillFrom - itemEnd) / (average + spacing))
            let target = Math.max(0, Math.min(index + jump, count))
            jump = target - index
            if(jump){
                this.$releaseVisible()
                index = target
                this.$visibleIndex = index
                this.$visiblePos = itemEnd + jump * (average + spacing)
                itemEnd = this.$visiblePos
            }
        }
        let changed = false
        let pos = itemEnd
        while(index < count && pos <= fillTo){
            let entry = this.$create(index, rows)
            if(!entry) break
            this.$setEntryPos(entry, pos)
            this.$setCulled(entry, doBuffer)
            entry.size = this.$entrySize(entry)
            pos += entry.size + spacing
            this.$visible.push(entry)
            index++
            changed = true
        }
        while(this.$visibleIndex > 0 && this.$visibleIndex <= count && this.$visiblePos > fillFrom){
            let entry = this.$create(this.$visibleIndex - 1, rows)
            if(!entry) break
            this.$visibleIndex--
            entry.size = this.$entrySize(entry)
            this.$visiblePos -= entry.size + spacing
            this.$setEntryPos(entry, this.$visiblePos)
            this.$setCulled(entry, doBuffer)
            this.$visible.unshift(entry)
            changed = true
        }
        return changed
    }

    // QQuickListViewPrivate::removeNonVisibleItems: from the start the entries ending before the buffer (a zero-sized
    // one only with a sized one after it, or refill would make it and release it again), from the end the entries
    // starting after it; one stays
    $removeNonVisible(bufferFrom, bufferTo){
        let visible = this.$visible
        let changed = false
        let i = 0
        while(visible.length > 1 && i < visible.length && this.$entryEnd(visible[i]) < bufferFrom){
            let entry = visible[i]
            if(this.$entrySize(entry) > 0){
                for(;;){
                    this.$visibleIndex++
                    visible.splice(i, 1)
                    this.$releaseEntry(entry)
                    if(i === 0) break
                    entry = visible[--i]
                }
                changed = true
            } else i++
        }
        while(visible.length > 1 && visible[visible.length - 1].pos > bufferTo){
            this.$releaseEntry(visible.pop())
            changed = true
        }
        return changed
    }

    // QQuickListViewPrivate::visibleItemsChanged: the average of the visible entries, the current item at its
    // estimated position when it is not one of them
    $visibleItemsChanged(){
        if(this.$visible.length) this.$visiblePos = this.$visible[0].pos
        this.$updateAverage()
        let current = this.$currentOutside()
        if(current) this.$setEntryPos(current, this.$positionAt(current.index))
    }

    // QQuickListViewPrivate::layoutVisibleItems: the entries one after the other from the first one's position, the
    // average of their sizes, the current item outside them at its estimated position. A first entry that changed its
    // size while it ended before the view keeps its end there (QQuickListViewPrivate::itemGeometryChanged)
    $layoutVisible(fromIndex = 0){
        let visible = this.$visible
        if(!visible.length) return
        let spacing = this.$v[ListView.SPACING]
        let first = visible[0]
        let firstSize = this.$entrySize(first)
        if(first.size !== undefined && first.size !== firstSize){
            let oldEnd = first.pos + first.size
            let contentPos = this.$vertical() ? this.$v[ListView.CONTENTY] : this.$v[ListView.CONTENTX]
            if(!this.$reversed() && oldEnd < contentPos) first.pos -= firstSize - first.size
            else if(this.$reversed() && oldEnd > contentPos) first.pos += firstSize - first.size
        }
        first.size = firstSize
        this.$setEntryPos(first, first.pos)
        let sum = firstSize
        let pos = first.pos + firstSize + spacing
        for(let i = 1; i < visible.length; i++){
            let entry = visible[i]
            if(entry.index >= fromIndex) this.$setEntryPos(entry, pos)
            let size = this.$entrySize(entry)
            entry.size = size
            pos += size + spacing
            sum += size
        }
        this.$average = Math.round(sum / visible.length)
        let current = this.$currentOutside()
        if(current) this.$setEntryPos(current, this.$positionAt(current.index))
    }

    // QQuickListView::viewportMoved: the entries out of the view are culled, kept but not painted
    $cull(){
        let from = this.$position()
        let to = from + this.$viewSize()
        for(let entry of this.$visible) this.$setCulled(entry, this.$entryEnd(entry) < from || entry.pos > to)
        let current = this.$currentOutside()
        if(current) this.$setCulled(current, this.$entryEnd(current) < from || current.pos > to)
    }

    $setCulled(entry, culled){
        for(let item of entry.section ? [entry.section, entry.item] : [entry.item]){
            if(item.$culled === culled) continue
            item.$culled = culled
            engine.markDirty(item, VISIBILITY)
        }
    }

    // QQuickItemViewPrivate::updateViewport: the size along the flow, the rows from the first to the last estimated
    $updateViewport(){
        let count = this.$v[ListView.COUNT]
        let size = count || this.$visible.length ? this.$lastPosition() - this.$originPosition() : 0
        this.$write(this.$vertical() ? ListView.CONTENTHEIGHT : ListView.CONTENTWIDTH, size + this.$headerSize() + this.$footerSize())
        this.$syncContentItem()
        this.$updateOrigin()
    }

    // the margin where the flow starts: topMargin, bottomMargin of a flow from the bottom; leftMargin, rightMargin of
    // a flow from the right
    $startMargin(){
        let m = require('./Flickable').margins(this, this.$vertical() ? 'y' : 'x')
        return this.$reversed() ? m.end : m.start
    }

    // QQuickItemView::originX / originY (of the content, not the margins), atXBeginning ... (QQuickFlickable::updateBeginningEnd)
    $updateOrigin(){
        let vertical = this.$vertical()
        let axis = vertical ? 'y' : 'x'
        let b = this.$contentBounds(axis)
        let origin = b.min
        if(this.$reversed() && this.$contentSize() < this.$viewSize()) origin = -this.$lastPosition() - this.$footerSize()
        this.$write(vertical ? ListView.ORIGINY : ListView.ORIGINX, origin)
        this.$write(vertical ? ListView.ORIGINX : ListView.ORIGINY, 0)
        this.$updateBeginningEnd()
    }

    $updateBeginningEnd(){
        for(let axis of ['x', 'y']){
            let b = this.$flickBounds(axis)
            let pos = this.$contentPos(axis)
            this.$write(axis === 'x' ? ListView.ATXBEGINNING : ListView.ATYBEGINNING, pos <= b.min)
            this.$write(axis === 'x' ? ListView.ATXEND : ListView.ATYEND, pos >= b.max)
        }
        this.$updateVisibleArea()
    }

    // QQuickListView::currentSection: the section of the item at the start of the view
    $updateCurrentSection(){
        let section = this.$v[ListView.SECTION]
        if(!section || !section.$v[ViewSection.PROPERTY]) return this.$write(ListView.CURRENTSECTION, '')
        let index = this.$snapIndexAt(Math.max(0, this.$position()))
        this.$write(ListView.CURRENTSECTION, index < 0 ? '' : this.$sectionAt(index, modelRows(this.$model)))
    }

    $scheduleBuffer(){
        if(this.$bufferTimer) return
        this.$bufferTimer = setTimeout(() => {
            this.$bufferTimer = null
            this.$fillBuffer = true
            engine.queuePolish(this)
        }, 16)
    }

    // QQuickItemViewPrivate::forceLayoutPolish: the next polish lays the view out
    $forceLayoutPolish(){
        this.$forceLayout = true
        engine.queuePolish(this)
    }

    // QQuickItemView::updatePolish -> layout(): after a change of the model, the geometry or a size, the visible entries
    // one after the other, refill, and the view put back within its extents unless it is moved (fixupPosition); after
    // a move of the view only refill (QQuickItemView::viewportMoved)
    $polish(){
        let withBuffer = !!this.$fillBuffer
        this.$fillBuffer = false
        if(!this.$forceLayout || !(this.$f & engine.COMPLETED)){
            this.$fill(withBuffer)
            return
        }
        this.$forceLayout = false
        this.$layoutVisible()
        this.$fill(withBuffer)
        let s = this.$fs
        if(!s || !(s.pressed || s.moving || s.x.flicking || s.y.flicking)){
            fixup(this, this.$vertical() ? 'y' : 'x')
            this.$fill(withBuffer)
        }
    }

    // QQuickItemView::updateCurrent: the new current item takes the focus, then currentIndexChanged, then currentItemChanged
    $focusCurrentItem(){
        let entry = this.$entries.get(this.$v[ListView.CURRENTINDEX])
        if(entry && entry.item !== this.$v[ListView.CURRENTITEM]) entry.item.$setFocus(true)
    }

    $updateCurrentItem(){
        this.$focusCurrentItem()
        let index = this.$v[ListView.CURRENTINDEX]
        let entry = this.$entries.get(index) || null
        let item = entry ? entry.item : null
        // QQuickItemViewPrivate::updateCurrent releases the old current item; one of the visible entries stays
        let old = this.$currentEntry
        this.$currentEntry = entry
        if(old && old !== entry && this.$entries.get(old.index) === old && !this.$visibleEntry(old.index)) this.$release(old.index)
        // ListView.isCurrentItem: the item that was current and the one that is
        let previous = this.$currentAttached
        if(previous !== item){
            if(previous && !(previous.$f & engine.DESTROYED)) attachedOf(previous).$write(ListViewAttached.ISCURRENTITEM, false)
            if(item) attachedOf(item).$write(ListViewAttached.ISCURRENTITEM, true)
            this.$currentAttached = item
        }
        this.$write(ListView.CURRENTITEM, item)
        this.$updateHighlight()
    }

    $write(id, value){
        let old = this.$v[id]
        if(id === ListView.FLICKABLEDIRECTION && !this.$settingDirection) this.$flickDirectionSet = true
        if(id === ListView.KEYNAVIGATIONENABLED) this.$keyNavigationSet = true
        // QQuickItemView::setCurrentIndex: currentIndexCleared by -1
        if(id === ListView.CURRENTINDEX && !this.$inSync && !this.$viewMovedCurrent) this.$currentIndexCleared = value === -1
        super.$write(id, value)
        // QQuickItemView::setCurrentIndex: moveReason SetIndex (not for a change of the model), updateCurrent
        if(id === ListView.CURRENTINDEX && this.$v[id] !== old){
            if(!this.$modelUpdating && !this.$viewMovedCurrent && this.$f & engine.COMPLETED) this.$moveReason = MOVE_SET_INDEX
            this.$ensureCurrentItem()
        }
        // QQuickListView::geometryChange drains the pool, setReuseItems(false) as well
        else if((id === Item.WIDTH || id === Item.HEIGHT || id === ListView.REUSEITEMS) && this.$v[id] !== old && this.$pool.length){
            if(id !== ListView.REUSEITEMS || !this.$v[id]) this.$drainPool()
        }
        // QQuickListView::geometryChange: BottomToTop keeps its place from the bottom edge
        if(id === Item.HEIGHT && this.$vertical() && this.$reversed() && old !== this.$v[id] && (this.$f & engine.COMPLETED)){
            this.contentY = this.$v[ListView.CONTENTY] - (this.$v[id] - old)
        }
    }

    // QQuickItemViewPrivate::applyModelChanges, applyRemovalChange, QQuickListViewPrivate::applyInsertionChange,
    // repositionFirstItem: the entries after a change take their new index; rows inserted or removed before the visible
    // entries leave them where they are (the estimate of the rows before them changes, so the origin does); rows
    // inserted among them are made at their place; entries removed above the view give their place to the ones after,
    // so the content in the view does not jump. The view lays out with the next polish
    $modelChanged(kind, index, count, roles = null, to = -1){
        if(kind === 'move' || kind === 'insert' || kind === 'remove'){
            this.$modelUpdating = true
            try {
                if(kind === 'move') this.$modelMoved(index, to, count)
                else this.$modelInsertedOrRemoved(kind, index, count)
                // QQuickItemViewPrivate::applyModelChanges -> updateCurrent: the current item exists at once (also when
                // its row was removed and the index stays), the highlight stays and moves to it
                this.$ensureCurrentItem()
            } finally {
                this.$modelUpdating = false
            }
            this.$forceLayoutPolish()
        } else if(kind === 'change'){
            let rows = modelRows(this.$model)
            for(let i = index; i < index + count; i++){
                let entry = this.$entries.get(i)
                if(!entry) continue
                entry.ctx.$row = rows.row(i)
                entry.ctx.$write(ModelContext.MODELDATA, rows.data(i))
                entry.ctx.$rowChanged(roles)
            }
            if(this.$v[ListView.SECTION] && this.$v[ListView.SECTION].$v[ViewSection.PROPERTY]) this.$forceLayoutPolish()
        } else {
            this.$sync()
        }
    }

    // insert / remove (see $modelChanged)
    $modelInsertedOrRemoved(kind, index, count){
        let visible = this.$visible
        let spacing = this.$v[ListView.SPACING]
        let viewPos = this.$position()
        let firstInView = visible.find(entry => this.$entryEnd(entry) > viewPos) || visible[0] || null
        let firstInViewPos = firstInView ? firstInView.pos : 0
        let prevFirst = visible[0] || null
        let prevFirstPos = prevFirst ? prevFirst.pos : 0
        let shifted = new Map()
        let removed = []
        for(let [i, entry] of this.$entries){
            if(kind === 'remove' && i >= index && i < index + count){
                removed.push(entry)
                continue
            }
            let to = i >= index ? (kind === 'insert' ? i + count : i - count) : i
            if(to !== i){
                entry.ctx.$write(ModelContext.INDEX, to)
                entry.index = to
            }
            shifted.set(to, entry)
        }
        this.$entries = shifted
        let total = modelRows(this.$model).count
        if(kind === 'remove'){
            // QQuickItemViewPrivate::removeItem: the sizes removed before the first entry in view (not counting the
            // first visible entry, QQuickListViewPrivate::updateSizeChangesBeforeVisiblePos) and after it
            let sizeBefore = 0, sizeAfter = 0
            for(let entry of removed){
                let at = visible.indexOf(entry)
                if(at >= 0){
                    if(entry.pos < firstInViewPos){
                        if(entry !== prevFirst) sizeBefore += this.$entrySize(entry)
                    } else sizeAfter += this.$entrySize(entry)
                    visible.splice(at, 1)
                }
                if(this.$currentEntry === entry) this.$currentEntry = null
                if(entry.section) entry.section.$releaseLater()
                entry.item.$releaseLater()
            }
            // updateVisibleIndex
            this.$visibleIndex = visible.length ? visible[0].index : Math.min(this.$visibleIndex, Math.max(0, total))
            if(visible.length && prevFirst){
                if(prevFirst === firstInView && prevFirst !== visible[0]) visible[0].pos = prevFirstPos
                else if(visible[0].pos > firstInViewPos) visible[0].pos -= sizeAfter
                else if(visible[0].pos < firstInViewPos) visible[0].pos += sizeBefore
            }
        } else {
            let at = index - this.$visibleIndex
            if(visible.length && at >= 0 && at <= visible.length && (at < visible.length || index === visible[visible.length - 1].index + 1)){
                let pos = at < visible.length ? visible[at].pos : this.$entryEnd(visible[visible.length - 1]) + spacing
                if(firstInView && pos < firstInViewPos){
                    // above the view: the entries before the insertion go, the view keeps its rows where they are
                    for(let entry of visible.splice(0, at)) this.$releaseEntry(entry)
                } else {
                    // in or after the view: the new rows are made at their place while they reach the buffer
                    let lastVisiblePos = viewPos + this.$viewSize() + this.$v[ListView.CACHEBUFFER]
                    let rows = modelRows(this.$model)
                    let made = 0
                    for(; made < count && pos <= lastVisiblePos; made++){
                        let entry = this.$create(index + made, rows)
                        if(!entry) break
                        this.$setEntryPos(entry, pos)
                        entry.size = this.$entrySize(entry)
                        visible.splice(at + made, 0, entry)
                        pos += entry.size + spacing
                    }
                    // the entries after rows not made are no longer in a row with them
                    if(made < count) for(let entry of visible.splice(at + made)) this.$releaseEntry(entry)
                }
                this.$visibleIndex = visible.length ? visible[0].index : index
            } else if(index < this.$visibleIndex){
                this.$visibleIndex += count
            }
        }
        this.$write(ListView.COUNT, total)
        let current = this.$v[ListView.CURRENTINDEX]
        if(kind === 'insert' && current >= index && current >= 0 && total > count) this.$write(ListView.CURRENTINDEX, current + count)
        else if(kind === 'remove' && current >= index + count) this.$write(ListView.CURRENTINDEX, current - count)
        else if(kind === 'remove' && current >= index) this.$write(ListView.CURRENTINDEX, Math.min(index, total - 1))
        if(total > 0 && this.$v[ListView.CURRENTINDEX] < 0) this.$write(ListView.CURRENTINDEX, 0)
    }

    // QQmlChangeSet move: a removal and an insertion with move keys; the delegates of the moved rows are taken out of
    // the visible ones and put back where the rows land when that is in the view (the removed items of the change set
    // are the moved ones: their item is reused), else released. The rows in view keep their place as for a removal
    // and an insertion (repositionFirstItem); the current index follows its row, the view does not move to it
    $modelMoved(from, to, count){
        let visible = this.$visible
        let spacing = this.$v[ListView.SPACING]
        let viewPos = this.$position()
        let firstInView = visible.find(entry => this.$entryEnd(entry) > viewPos) || visible[0] || null
        let firstInViewPos = firstInView ? firstInView.pos : 0
        let prevFirst = visible[0] || null
        let prevFirstPos = prevFirst ? prevFirst.pos : 0
        let map = (i) => {
            if(i >= from && i < from + count) return to + (i - from)
            let j = i < from ? i : i - count
            return j >= to ? j + count : j
        }
        let shifted = new Map()
        for(let [i, entry] of this.$entries){
            let k = map(i)
            if(k !== i){
                entry.ctx.$write(ModelContext.INDEX, k)
                entry.index = k
            }
            shifted.set(k, entry)
        }
        this.$entries = shifted
        let isMoved = (entry) => entry.index >= to && entry.index < to + count
        // the removal: the moved entries leave the visible ones (kept in $entries for the insertion)
        let sizeBefore = 0, sizeAfter = 0
        for(let i = visible.length - 1; i >= 0; i--){
            let entry = visible[i]
            if(!isMoved(entry)) continue
            if(entry.pos < firstInViewPos){
                if(entry !== prevFirst) sizeBefore += this.$entrySize(entry)
            } else sizeAfter += this.$entrySize(entry)
            visible.splice(i, 1)
        }
        if(visible.length){
            this.$visibleIndex = visible[0].index
            if(prevFirst && prevFirst === firstInView && prevFirst !== visible[0]) visible[0].pos = prevFirstPos
            else if(visible[0].pos > firstInViewPos) visible[0].pos -= sizeAfter
            else if(visible[0].pos < firstInViewPos) visible[0].pos += sizeBefore
            this.$layoutVisible()
        }
        // the insertion at to, found as applyInsertionChange does after the removal: by the indexes the rows have
        // between the two (the final index of a row after to, less count); the moved entries put back among the
        // visible ones while they reach the buffer
        let between = (entry) => entry.index >= to ? entry.index - count : entry.index
        let lastVisiblePos = viewPos + this.$viewSize() + this.$v[ListView.CACHEBUFFER]
        let at = -1
        if(visible.length){
            let first = between(visible[0]), last = visible[visible.length - 1]
            if(to >= first && to <= between(last)) at = to - first
            else if(to === between(last) + 1 && this.$entryEnd(last) <= lastVisiblePos) at = visible.length
        }
        if(at >= 0){
            let pos = at < visible.length ? visible[at].pos : this.$entryEnd(visible[visible.length - 1]) + spacing
            if(firstInView && pos < firstInViewPos){
                for(let entry of visible.splice(0, at)) this.$releaseEntry(entry)
            } else {
                let rows = modelRows(this.$model)
                let made = 0
                for(; made < count && pos <= lastVisiblePos; made++){
                    let entry = this.$create(to + made, rows)
                    if(!entry) break
                    this.$setEntryPos(entry, pos)
                    entry.size = this.$entrySize(entry)
                    visible.splice(at + made, 0, entry)
                    pos += entry.size + spacing
                }
                if(made < count) for(let entry of visible.splice(at + made)) this.$releaseEntry(entry)
            }
        }
        if(visible.length) this.$visibleIndex = visible[0].index
        // the moved entries not put back go, the current item stays
        for(let [i, entry] of Array.from(this.$entries)){
            if(isMoved(entry) && visible.indexOf(entry) < 0) this.$releaseEntry(entry)
        }
        let current = this.$v[ListView.CURRENTINDEX]
        if(current >= 0) this.$write(ListView.CURRENTINDEX, map(current))
        // the sections around the moved rows change
        if(this.$v[ListView.SECTION] && this.$v[ListView.SECTION].$v[ViewSection.PROPERTY]){
            let rows = modelRows(this.$model)
            for(let entry of visible) this.$updateEntrySection(entry.index, entry, rows)
        }
    }

    $wheel(x, y, angleDelta, pixelDelta){
        if(!this.$v[ListView.INTERACTIVE]) return false
        return wheelScroll(this, angleDelta, pixelDelta)
    }

    $disposeNative(){
        if(this.$model && this.$model.$removeView) this.$model.$removeView(this)
        if(this.$bufferTimer) clearTimeout(this.$bufferTimer)
        this.$stopHighlightAnimations()
        super.$disposeNative()
    }
}
installFlick(ListView)

const resync = (view) => { if(view.$f & engine.COMPLETED) view.$sync() }
const modelSet = (view, value, old) => { if(!sameModel(value, old) && view.$f & engine.COMPLETED) view.$sync('model') }
// QQuickItemViewPrivate::forceLayoutPolish (setSpacing, setCacheBuffer, setHighlightRangeMode ...)
const refill = (view) => view.$forceLayoutPolish()
const scrolled = (view) => {
    view.$content.$write(Item.X, -view.$v[ListView.CONTENTX])
    view.$content.$write(Item.Y, -view.$v[ListView.CONTENTY])
    view.$updateBeginningEnd()
    engine.queuePolish(view)
    view.$strictViewportMoved()
}
// QQuickListView::setOrientation: the flick direction follows the orientation unless it was set
const orientationChanged = (view) => {
    if(!view.$flickDirectionSet){
        view.$settingDirection = true
        view.$write(ListView.FLICKABLEDIRECTION, view.$vertical() ? 2 : 1)
        view.$settingDirection = false
    }
    resync(view)
}

ListView.prototype.$isFocusScope = true

defineType(ListView, {
    props: Object.assign({}, FLICK_PROPS, {
        model: { kind: 'var', effect: modelSet },
        delegate: { kind: 'component', effect: resync },
        count: { kind: 'int', readonly: true },
        currentIndex: { kind: 'int', initial: -1, effect: (view) => view.$focusCurrentItem() },
        currentItem: { kind: 'var', initial: null, readonly: true },
        orientation: { kind: 'int', initial: VERTICAL, effect: orientationChanged },
        spacing: { kind: 'real', effect: refill },
        cacheBuffer: { kind: 'real', initial: 320, effect: refill },
        contentX: { kind: 'real', effect: scrolled, set(value){ externalMove(this, 'x'); this.$assign(ListView.CONTENTX, value) } },
        contentY: { kind: 'real', effect: scrolled, set(value){ externalMove(this, 'y'); this.$assign(ListView.CONTENTY, value) } },
        // -1 across the flow, as a Flickable's (QQuickItemView sets the size along it)
        contentWidth: { kind: 'real', initial: -1 },
        contentHeight: { kind: 'real', initial: -1 },
        keyNavigationEnabled: { kind: 'bool', initial: true },
        keyNavigationWraps: 'bool',
        highlightFollowsCurrentItem: { kind: 'bool', initial: true },
        reuseItems: 'bool',
        snapMode: 'int',
        flickableDirection: { kind: 'int', initial: 2 },
        highlightRangeMode: { kind: 'int', effect: refill },
        preferredHighlightBegin: { kind: 'real', effect: refill },
        preferredHighlightEnd: { kind: 'real', effect: refill },
        highlightMoveDuration: { kind: 'int', initial: -1 },
        highlightMoveVelocity: { kind: 'real', initial: 400 },
        highlightResizeDuration: { kind: 'int', initial: -1 },
        highlightResizeVelocity: { kind: 'real', initial: 400 },
        highlight: { kind: 'component', effect: (view) => view.$updateHighlight() },
        highlightItem: { kind: 'var', readonly: true },
        header: { kind: 'component', effect: resync },
        footer: { kind: 'component', effect: resync },
        headerItem: { kind: 'var', readonly: true },
        footerItem: { kind: 'var', readonly: true },
        headerPositioning: { kind: 'int', effect: refill },
        footerPositioning: { kind: 'int', effect: refill },
        section: { group: ViewSection },
        currentSection: { kind: 'string', readonly: true },
        verticalLayoutDirection: { kind: 'int', effect: resync },
        layoutDirection: { kind: 'int', effect: resync },
        atYBeginning: { kind: 'bool', initial: true, readonly: true },
        atYEnd: { kind: 'bool', initial: true, readonly: true },
        atXBeginning: { kind: 'bool', initial: true, readonly: true },
        atXEnd: { kind: 'bool', initial: true, readonly: true },
        originX: { kind: 'real', readonly: true },
        originY: { kind: 'real', readonly: true },
    }),
    signals: FLICK_SIGNALS,
})

Object.assign(ListView, FLICK_ENUMS, ENUMS)

// size changes of the view refill it
{
    let base = ListView.prototype.$write
    ListView.prototype.$write = function(id, value){
        base.call(this, id, value)
        if(id === Item.WIDTH || id === Item.HEIGHT){
            this.$syncContentItem()
            this.$forceLayoutPolish()
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

module.exports = { ListView, ListViewAttached, ViewSection }
