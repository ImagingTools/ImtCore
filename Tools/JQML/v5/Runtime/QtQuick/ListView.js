const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { Item } = require('./Item')
const { ListModel } = require('../QtQml/Models/ListModel')
const { ModelContext, modelRows, sameModel } = require('../QtQml/Models/ModelContext')
const { GroupObject } = require('../Core/QtObject')
const { installFlick, flickDirection, flickState, wheelScroll, FLICK_PROPS, FLICK_SIGNALS, FLICK_ENUMS } = require('./Flickable')
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
        this.$content.$childGeometryChanged = () => engine.queuePolish(this)
        this.$entries = new Map()     // index -> { item, ctx, section }
        this.$sizes = []              // the size along the flow of an index: its section label and its item
        this.$model = null
        this.$filling = false
        this.$pool = []               // released { item, ctx } kept for reuseItems
        this.$header = null
        this.$footer = null
        this.$highlight = null
        this.$highlightAnimations = null
        this.$itemsSize = 0
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
        if(this.$vertical()) this.contentY = content
        else this.contentX = content
    }

    $placeFlow(item, pos){
        let size = this.$sizeOf(item)
        item.$place(this.$vertical() ? Item.Y : Item.X, this.$reversed() ? -pos - size : pos)
    }

    // the flow end of the last item (QQuickListViewPrivate::lastPosition)
    $lastPosition(){ return this.$itemsSize }

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
            maxExtent = viewSize
            if(count && range.strict){
                maxExtent = -(0 - highlightStart)
                if(highlightEnd !== highlightStart) maxExtent = Math.max(maxExtent, -(0 - highlightEnd))
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
            minExtent = header
            if(range.strict && count){
                minExtent += range.start
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

    $setContentPos(axis, value){
        if(axis === 'x') this.contentX = value
        else this.contentY = value
    }

    $highlightRange(){
        let mode = this.$v[ListView.HIGHLIGHTRANGEMODE]
        return { mode, strict: mode === STRICTLY_ENFORCE_RANGE, start: this.$v[ListView.PREFERREDHIGHLIGHTBEGIN], end: this.$v[ListView.PREFERREDHIGHLIGHTEND] }
    }

    // the flow position where the item (its section label first) and the item itself start
    $positionAt(index){
        let spacing = this.$v[ListView.SPACING]
        let pos = 0
        let average = this.$averageSize()
        for(let i = 0; i < index; i++) pos += (this.$sizes[i] === undefined ? average : this.$sizes[i]) + spacing
        return pos
    }

    $itemPositionAt(index){
        let entry = this.$entries.get(index)
        return this.$positionAt(index) + (entry && entry.section ? this.$sizeOf(entry.section) : 0)
    }

    $endPositionAt(index){
        let size = this.$sizes[index] === undefined ? this.$averageSize() : this.$sizes[index]
        return this.$positionAt(index) + size
    }

    // QQuickListViewPrivate::snapItemAt: the item whose start is nearest pos
    $snapIndexAt(pos){
        let count = this.$v[ListView.COUNT]
        let spacing = this.$v[ListView.SPACING]
        let average = this.$averageSize()
        let start = 0, previousSize = 0
        for(let i = 0; i < count; i++){
            let size = this.$sizes[i] === undefined ? average : this.$sizes[i]
            if(start + size / 2 >= pos && start - previousSize / 2 <= pos) return i
            start += size + spacing
            previousSize = size
        }
        return count ? count - 1 : -1
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

    // QQuickItemView::positionViewAtBeginning / End: the bounds of the flow
    positionViewAtBeginning(){
        this.$fill()
        let axis = this.$vertical() ? 'y' : 'x'
        let b = this.$flickBounds(axis)
        this.$setContentPos(axis, this.$reversed() ? b.max : b.min)
    }

    positionViewAtEnd(){
        this.$fill()
        let axis = this.$vertical() ? 'y' : 'x'
        let b = this.$flickBounds(axis)
        this.$setContentPos(axis, this.$reversed() ? b.min : b.max)
    }

    // QQuickItemViewPrivate::positionViewAtIndex: the mode decides the new position, bounded by the extents
    positionViewAtIndex(index, mode = ListView.Beginning){
        if(this.$v[ListView.COUNT] <= 0) return
        this.$fill()
        this.$moveToIndex(index, mode)
    }

    // the position for the mode, from the items built and the average size of the others; no refill (the view
    // refills when its position changes, as QQuickItemView::viewportMoved)
    $moveToIndex(index, mode){
        let count = this.$v[ListView.COUNT]
        if(count <= 0) return
        let idx = Math.max(Math.min(index, count - 1), 0)
        let itemPos = this.$itemPositionAt(idx)
        let entry = this.$entries.get(idx)
        let itemSize = entry ? this.$sizeOf(entry.item) : this.$averageSize()
        let itemEnd = itemPos + itemSize
        let viewSize = this.$viewSize()
        let pos = this.$position()
        switch(mode){
            case ListView.Beginning: pos = itemPos; if(idx === 0 && this.$header) pos -= this.$headerSize(); break
            case ListView.Center: pos = itemPos - (viewSize - itemSize) / 2; break
            case ListView.End: pos = itemPos - viewSize + itemSize; if(idx === count - 1 && this.$footer) pos += this.$footerSize(); break
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
        this.$setFlowPositionBounded(pos)
    }

    $setFlowPositionBounded(pos){
        let axis = this.$vertical() ? 'y' : 'x'
        let b = this.$flickBounds(axis)
        let content = this.$reversed() ? -pos - this.$viewSize() : pos
        this.$setContentPos(axis, Math.max(b.min, Math.min(b.max, content)))
    }

    forceLayout(){ this.$fill() }

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

    // QQuickListViewPrivate::updateAverage: the rounded mean size of the items built; none built, it stays
    $updateAverage(){
        let sum = 0, count = 0
        for(let index of this.$entries.keys()){
            let size = this.$sizes[index]
            if(size !== undefined){ sum += size; count++ }
        }
        if(count) this.$average = Math.round(sum / count)
    }

    $componentComplete(){
        super.$componentComplete()
        this.$sync()
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
                engine.connect(this.$header, this.$vertical() ? Item.SIGNAL_heightChanged : Item.SIGNAL_widthChanged, () => engine.queuePolish(this))
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
                engine.connect(this.$footer, this.$vertical() ? Item.SIGNAL_heightChanged : Item.SIGNAL_widthChanged, () => engine.queuePolish(this))
            }
            this.$write(ListView.FOOTERITEM, this.$footer)
        }
        if(!headerComponent) this.$write(ListView.HEADERITEM, null)
        if(!footerComponent) this.$write(ListView.FOOTERITEM, null)
    }

    // QQuickListViewPrivate::updateHeader / updateFooter
    $placeHeaderFooter(){
        let position = this.$position()
        if(this.$header){
            let mode = this.$v[ListView.HEADERPOSITIONING]
            let size = this.$headerSize()
            let pos = -size
            if(mode === OVERLAY_HEADER) pos = position
            else if(mode === PULL_BACK_HEADER){
                let current = this.$header.$flowPos === undefined ? -size : this.$header.$flowPos
                let clamped = Math.max(-size, Math.min(current, this.$lastPosition() - this.$viewSize()))
                pos = Math.max(position - size, Math.min(clamped, position))
            }
            this.$header.$flowPos = pos
            this.$placeFlow(this.$header, pos)
        }
        if(this.$footer){
            let mode = this.$v[ListView.FOOTERPOSITIONING]
            let size = this.$footerSize()
            let pos = this.$lastPosition()
            if(mode === OVERLAY_HEADER) pos = position + this.$viewSize() - size
            else if(mode === PULL_BACK_HEADER){
                let viewEnd = position + this.$viewSize()
                let current = this.$footer.$flowPos === undefined ? pos : this.$footer.$flowPos
                let clamped = Math.max(-this.$headerSize() - size + this.$viewSize(), Math.min(current, this.$lastPosition()))
                pos = Math.max(viewEnd - size, Math.min(clamped, viewEnd))
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
        if(!component || !current) return
        let vertical = this.$vertical()
        let follows = this.$v[ListView.HIGHLIGHTFOLLOWSCURRENTITEM]
        if(!this.$highlight){
            let highlight = this.$componentItem(component, 0)
            if(!highlight) return
            highlight.$component = component
            this.$highlight = highlight
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
        if(!follows) return
        // QQuickListViewPrivate::updateHighlight: not while a StrictlyEnforceRange view is pressed
        let s = this.$fs
        if(this.$v[ListView.HIGHLIGHTRANGEMODE] === STRICTLY_ENFORCE_RANGE && s && s.pressed) return
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
        this.$itemsSize = 0
        let count = modelRows(this.$model).count
        this.$write(ListView.COUNT, count)
        this.$updateHeaderFooter()
        // QQuickItemViewPrivate::regenerate: updateViewport, then setPosition(contentStartOffset)
        this.$fill(false)
        this.$setPosition(-this.$headerSize())
        this.$fill(false)
        // QQuickItemView::componentComplete: refill(), then updateCurrent(0) unless the index was cleared
        if(count > 0 && this.$v[ListView.CURRENTINDEX] < 0) this.$write(ListView.CURRENTINDEX, 0)
        if(this.$v[ListView.CURRENTINDEX] >= count) this.$write(ListView.CURRENTINDEX, count - 1)
        this.$ensureCurrentItem()
        this.$trackCurrent()
        // QQuickItemView::setModel / componentComplete end with updateViewport, also when refill laid nothing out
        // (a view of no size): lastPosition estimates the rows with averageSize - the size of the current item when
        // it is the only one (initializeCurrentItem)
        if(this.$v[Item.WIDTH] === 0 && this.$v[Item.HEIGHT] === 0 && this.$v[ListView.DELEGATE]){
            let spacing = this.$v[ListView.SPACING]
            this.$itemsSize = count ? count * this.$average + (count - 1) * spacing : 0
            this.$write(this.$vertical() ? ListView.CONTENTHEIGHT : ListView.CONTENTWIDTH, this.$itemsSize + this.$headerSize() + this.$footerSize())
        }
    }

    // updateCurrent() creates the current item even when it is outside the filled range
    $ensureCurrentItem(){
        let index = this.$v[ListView.CURRENTINDEX]
        if(index < 0 || this.$entries.has(index) || !this.$v[ListView.DELEGATE]) return this.$updateCurrentItem()
        let rows = modelRows(this.$model)
        if(index >= rows.count) return this.$updateCurrentItem()
        let alone = this.$entries.size === 0
        let item = this.$create(index, rows)
        if(item){
            let entry = this.$entries.get(index)
            this.$updateEntrySection(index, entry, rows)
            // QQuickListViewPrivate::initializeCurrentItem: with no visible items the average is the current item's size
            let size = this.$layoutEntry(index, entry, this.$positionAt(index))
            if(alone) this.$average = size
        }
        this.$updateCurrentItem()
    }

    $layoutEntry(index, entry, pos){
        let size = this.$sizeOf(entry.item)
        let sectionSize = 0
        if(entry.section){
            sectionSize = this.$sizeOf(entry.section)
            this.$placeFlow(entry.section, pos)
        }
        this.$sizes[index] = sectionSize + size
        this.$placeFlow(entry.item, pos + sectionSize)
        return sectionSize + size
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
            attachedOf(entry.item).$write(ListViewAttached.ISCURRENTITEM, index === this.$v[ListView.CURRENTINDEX])
            if(this.$currentAttached === entry.item && index !== this.$v[ListView.CURRENTINDEX]) this.$currentAttached = null
            let attached = entry.item.$v[Item.LISTVIEW]
            if(attached) engine.emitArgs(attached, ListViewAttached.SIGNAL_reused, [])
            return entry.item
        }
        let delegate = this.$v[ListView.DELEGATE]
        let ctx = new ModelContext(rows.row(index), index, rows.data(index))
        ctx.$view = this
        let item = delegate.createObject(this.$content, null, ctx)
        if(!item) return null
        // QQuickItemView::initItem: a delegate at z 0 goes to z 1, above the highlight
        if(!item.$v[Item.Z]) item.z = 1
        attachedOf(item).$write(ListViewAttached.VIEW, this)
        this.$entries.set(index, { item, ctx, section: null })
        return item
    }

    // QQuickItemViewPrivate::releaseItem: with reuseItems the item goes to the pool, culled, then pooled()
    $release(index, reusable = true){
        let entry = this.$entries.get(index)
        if(!entry) return
        this.$entries.delete(index)
        if(entry.section){
            entry.section.$releaseLater()
            entry.section = null
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
        if(!delegate){
            this.$placeHeaderFooter()
            return
        }
        if(this.$v[Item.WIDTH] === 0 && this.$v[Item.HEIGHT] === 0 && this.$entries.size === 0) return
        this.$filling = true
        let added = false
        try {
            let rows = modelRows(this.$model)
            let count = rows.count
            let vertical = this.$vertical()
            let spacing = this.$v[ListView.SPACING]
            let cache = this.$v[ListView.CACHEBUFFER]
            let position = this.$position()
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
                    this.$updateEntrySection(i, entry, rows)
                    size = this.$layoutEntry(i, entry, pos)
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
            // QQuickItemViewPrivate::refill: visibleItemsChanged (updateAverage), then updateViewport
            this.$updateAverage()
            let estimated = pos + (count - known) * (this.$averageSize() + spacing)
            this.$itemsSize = Math.max(0, count ? estimated - spacing : 0)
            // QQuickItemViewPrivate::updateViewport: the items, the header and the footer
            let contentSize = this.$itemsSize + this.$headerSize() + this.$footerSize()
            this.$write(vertical ? ListView.CONTENTHEIGHT : ListView.CONTENTWIDTH, contentSize)
            this.$syncContentItem()
            this.$placeHeaderFooter()
            this.$updateOrigin()
            this.$updateCurrentItem()
            this.$updateCurrentSection()
        } finally {
            this.$filling = false
        }
        if(!withBuffer && this.$v[ListView.CACHEBUFFER] > 0){
            if(added) this.$scheduleBuffer()
            else this.$fill(true)
        }
    }

    // QQuickItemView::originX / originY, atXBeginning ... (QQuickFlickable::updateBeginningEnd)
    $updateOrigin(){
        let vertical = this.$vertical()
        let axis = vertical ? 'y' : 'x'
        let b = this.$flickBounds(axis)
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
    }

    // QQuickListView::currentSection: the section of the item at the start of the view
    $updateCurrentSection(){
        let section = this.$v[ListView.SECTION]
        if(!section || !section.$v[ViewSection.PROPERTY]) return this.$write(ListView.CURRENTSECTION, '')
        let index = this.$snapIndexAt(Math.max(0, this.$position()))
        this.$write(ListView.CURRENTSECTION, index < 0 ? '' : this.$sectionAt(index, modelRows(this.$model)))
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
        let index = this.$v[ListView.CURRENTINDEX]
        let entry = this.$entries.get(index)
        let item = entry ? entry.item : null
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

    // QQuickItemView::trackedPositionChanged: a new current index brings its item into the view (into the highlight
    // range when one is applied); not while the user moves the view. It only moves the view, it does not refill:
    // a contentHeight handler sees the new items after the bindings of the change settle (the PopupDecorator
    // of the filters sizes itself in onContentHeightChanged)
    $trackCurrent(){
        let index = this.$v[ListView.CURRENTINDEX]
        if(index < 0 || !(this.$f & engine.COMPLETED)) return
        let s = this.$fs
        if(s && (s.pressed || s.moving)) return
        let range = this.$highlightRange()
        if(range.mode !== NO_HIGHLIGHT_RANGE){
            this.$setFlowPositionBounded(this.$itemPositionAt(index) - range.start)
            return
        }
        this.$moveToIndex(index, ListView.Contain)
    }

    $write(id, value){
        let old = this.$v[id]
        if(id === ListView.FLICKABLEDIRECTION && !this.$settingDirection) this.$flickDirectionSet = true
        if(id === ListView.KEYNAVIGATIONENABLED) this.$keyNavigationSet = true
        super.$write(id, value)
        if(id === ListView.CURRENTINDEX && this.$v[id] !== old){
            this.$updateCurrentItem()
            this.$trackCurrent()
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

    $modelChanged(kind, index, count, roles = null){
        if(kind === 'insert' || kind === 'remove'){
            let shifted = new Map()
            for(let [i, entry] of this.$entries){
                if(kind === 'remove' && i >= index && i < index + count){
                    if(entry.section) entry.section.$releaseLater()
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
            if(this.$v[ListView.SECTION] && this.$v[ListView.SECTION].$v[ViewSection.PROPERTY]) engine.queuePolish(this)
        } else {
            this.$sync()
        }
    }

    $wheel(x, y, angleDelta, pixelDelta){
        if(!this.$v[ListView.INTERACTIVE]) return false
        return wheelScroll(this, angleDelta, pixelDelta)
    }

    $disposeNative(){
        if(this.$model instanceof ListModel) this.$model.$removeView(this)
        if(this.$bufferTimer) clearTimeout(this.$bufferTimer)
        this.$stopHighlightAnimations()
        super.$disposeNative()
    }
}
installFlick(ListView)

const resync = (view) => { if(view.$f & engine.COMPLETED) view.$sync() }
const modelSet = (view, value, old) => { if(!sameModel(value, old)) resync(view) }
const refill = (view) => engine.queuePolish(view)
const scrolled = (view) => {
    view.$content.$write(Item.X, -view.$v[ListView.CONTENTX])
    view.$content.$write(Item.Y, -view.$v[ListView.CONTENTY])
    view.$updateBeginningEnd()
    engine.queuePolish(view)
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
        contentX: { kind: 'real', effect: scrolled },
        contentY: { kind: 'real', effect: scrolled },
        contentWidth: 'real',
        contentHeight: 'real',
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

module.exports = { ListView, ListViewAttached, ViewSection }
