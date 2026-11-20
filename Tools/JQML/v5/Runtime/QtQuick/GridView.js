const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { ListModel } = require('../QtQml/Models/ListModel')
const { ModelContext, modelRows, sameModel, moveEntries, movedIndex } = require('../QtQml/Models/ModelContext')
const { Item } = require('./Item')
const { installFlick, flickDirection, wheelScroll, externalMove, FLICK_PROPS, FLICK_SIGNALS, FLICK_ENUMS } = require('./Flickable')
const QtEnums = require('../Qt/enums')
const { SmoothedAnimation } = require('./SmoothedAnimation')

// QQuickItemViewPrivate::MovementReason: the view follows the highlight only after a new current index
const MOVE_OTHER = 0, MOVE_SET_INDEX = 1, MOVE_MOUSE = 2

// ---- GridView: every delegate exists, laid out in cells ---------------------------------------

const LEFT_TO_RIGHT = 0, TOP_TO_BOTTOM = 1

class GridView extends Item {
    constructor(parent = null){
        super(parent)
        this.$content = new Item(this)
        this.$items = []
        this.$contexts = []
        this.$model = null
        this.$highlight = null
        this.$highlightAnimations = null
        this.$moveReason = MOVE_OTHER
        this.$internalMove = 0
        this.$v[Item.CLIP] = false
        engine.scheduleComponentComplete(this)
    }

    get contentItem(){ return this.$content }

    itemAtIndex(index){ return this.$items[index] || null }

    $componentComplete(){
        super.$componentComplete()
        this.$sync()
    }

    // a delegate for row index of rows, put at index
    $createAt(index, rows){
        let ctx = new ModelContext(rows.row(index), index, rows.data(index))
        ctx.$view = this
        let item = this.$v[GridView.DELEGATE].createObject(this.$content, null, ctx)
        if(!item) return null
        if(!item.$v[Item.Z]) item.z = 1
        this.$items.splice(index, 0, item)
        this.$contexts.splice(index, 0, ctx)
        return item
    }

    $reindex(){
        for(let i = 0; i < this.$contexts.length; i++) this.$contexts[i].$write(ModelContext.INDEX, i)
    }

    $sync(){
        let model = this.$v[GridView.MODEL]
        if(model !== this.$model){
            if(this.$model instanceof ListModel) this.$model.$removeView(this)
            this.$model = model
            if(model instanceof ListModel) model.$addView(this)
        }
        for(let item of this.$items) item.$releaseLater()
        this.$items = []
        this.$contexts = []
        let delegate = this.$v[GridView.DELEGATE]
        if(delegate){
            let rows = modelRows(this.$model)
            for(let i = 0; i < rows.count; i++){
                let ctx = new ModelContext(rows.row(i), i, rows.data(i))
                ctx.$view = this
                let item = delegate.createObject(this.$content, null, ctx)
                if(item){
                    if(!item.$v[Item.Z]) item.z = 1
                    this.$items.push(item)
                    this.$contexts.push(ctx)
                }
            }
        }
        this.$write(GridView.COUNT, this.$items.length)
        this.$layout()
        if(this.$items.length && this.$v[GridView.CURRENTINDEX] < 0) this.$write(GridView.CURRENTINDEX, 0)
        if(this.$v[GridView.CURRENTINDEX] >= this.$items.length) this.$write(GridView.CURRENTINDEX, this.$items.length - 1)
        this.$moveReason = MOVE_SET_INDEX
        this.$updateCurrent()
        this.$resetHighlightPosition()
        this.$trackedPositionChanged()
        this.$moveReason = MOVE_OTHER
    }

    // QQuickItemView applyModelChanges: the delegates of the rows that stay are kept, inserted rows get new ones,
    // moved rows keep theirs (QQmlChangeSet move); the current index follows its row and the view does not move to it
    $modelChanged(kind, index, count, roles = null, to = -1){
        if((kind === 'insert' || kind === 'remove' || kind === 'move') && this.$v[GridView.DELEGATE] && this.$f & engine.COMPLETED){
            let current = this.$v[GridView.CURRENTINDEX]
            let next = current
            if(kind === 'insert'){
                let rows = modelRows(this.$model)
                for(let i = 0; i < count; i++) this.$createAt(index + i, rows)
                if(current >= index) next = current + count
            } else if(kind === 'remove'){
                let removed = this.$items.splice(index, count)
                this.$contexts.splice(index, count)
                for(let item of removed) item.$releaseLater()
                if(current >= index + count) next = current - count
                else if(current >= index) next = Math.min(index, this.$items.length - 1)
            } else {
                moveEntries([this.$items, this.$contexts], index, to, count)
                if(current >= 0) next = movedIndex(current, index, to, count)
            }
            this.$reindex()
            this.$write(GridView.COUNT, this.$items.length)
            this.$layout()
            if(this.$items.length && next < 0) next = 0
            this.$modelUpdating = true
            try {
                if(next !== current) this.$write(GridView.CURRENTINDEX, next)
                else this.$updateCurrent()
            } finally {
                this.$modelUpdating = false
            }
            return
        }
        if(kind === 'change'){
            let rows = modelRows(this.$model)
            for(let i = index; i < index + count; i++){
                let ctx = this.$contexts[i]
                if(!ctx) continue
                ctx.$row = rows.row(i)
                ctx.$write(ModelContext.MODELDATA, rows.data(i))
                ctx.$rowChanged(roles)
            }
            return
        }
        this.$sync()
    }

    // ---- the Flickable side (QQuickFlickable bounds of the content; QQuickGridView flicks along its flow) ----

    $flickBounds(axis){
        let content = axis === 'x' ? this.$v[GridView.CONTENTWIDTH] : this.$v[GridView.CONTENTHEIGHT]
        let size = axis === 'x' ? this.$v[Item.WIDTH] : this.$v[Item.HEIGHT]
        return { min: 0, max: Math.max(0, content - size) }
    }

    $flickAxis(axis){
        let content = axis === 'x' ? this.$v[GridView.CONTENTWIDTH] : this.$v[GridView.CONTENTHEIGHT]
        return flickDirection(this.$v[GridView.FLICKABLEDIRECTION], content, axis === 'x' ? this.$v[Item.WIDTH] : this.$v[Item.HEIGHT], axis)
    }

    $contentPos(axis){ return this.$v[axis === 'x' ? GridView.CONTENTX : GridView.CONTENTY] }

    // the flick engine moves the view: QQuickItemView::viewportMoved takes it for a move of the mouse while flicking
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

    // QQuickItemView::setContentX / setContentY: positioning the view by hand overrides the movement state
    $assign(id, value){
        if((id === GridView.CONTENTX || id === GridView.CONTENTY) && !this.$internalMove) this.$moveReason = MOVE_OTHER
        super.$assign(id, value)
    }

    $alongFlow(axis){ return (axis === 'y') === (this.$v[GridView.FLOW] === LEFT_TO_RIGHT) }

    // QQuickGridViewPrivate::fixup with snapMode: the start of the view on a row (SnapOneRow: one row from the press)
    $fixupTarget(axis, target, state){
        let snap = this.$v[GridView.SNAPMODE]
        if(!snap || !this.$alongFlow(axis)) return target
        let row = axis === 'y' ? this.$v[GridView.CELLHEIGHT] : this.$v[GridView.CELLWIDTH]
        let b = this.$flickBounds(axis)
        let index = Math.round(target / row)
        if(snap === 2 && state){
            let from = Math.round(state.pressPos / row)
            index = Math.max(from - 1, Math.min(from + 1, index))
        }
        return Math.max(b.min, Math.min(b.max, index * row))
    }

    $flickTarget(axis, target, velocity, state){
        let snap = this.$v[GridView.SNAPMODE]
        if(!snap || !this.$alongFlow(axis)) return target
        if(snap === 2){
            let row = axis === 'y' ? this.$v[GridView.CELLHEIGHT] : this.$v[GridView.CELLWIDTH]
            let b = this.$flickBounds(axis)
            let from = Math.round(state.pressPos / row)
            return Math.max(b.min, Math.min(b.max, (from + (velocity > 0 ? 1 : -1)) * row))
        }
        return this.$fixupTarget(axis, target, null)
    }

    $columns(){
        let flow = this.$v[GridView.FLOW]
        let cw = this.$v[GridView.CELLWIDTH], ch = this.$v[GridView.CELLHEIGHT]
        return flow === LEFT_TO_RIGHT ? Math.max(1, Math.floor(this.$v[Item.WIDTH] / cw) || 1) : Math.max(1, Math.floor(this.$v[Item.HEIGHT] / ch) || 1)
    }

    $setCurrent(index){ this.currentIndex = index }

    // QQuickGridView::moveCurrentIndexUp, Down, Left, Right (keyNavigationWraps goes round)
    moveCurrentIndexUp(){
        let count = this.$v[GridView.COUNT], current = this.$v[GridView.CURRENTINDEX], wrap = this.$v[GridView.KEYNAVIGATIONWRAPS]
        if(!count) return
        let step = this.$v[GridView.FLOW] === LEFT_TO_RIGHT ? this.$columns() : 1
        if(current >= step || wrap){
            let index = current - step
            this.$setCurrent(index >= 0 && index < count ? index : count - 1)
        }
    }

    moveCurrentIndexDown(){
        let count = this.$v[GridView.COUNT], current = this.$v[GridView.CURRENTINDEX], wrap = this.$v[GridView.KEYNAVIGATIONWRAPS]
        if(!count) return
        let step = this.$v[GridView.FLOW] === LEFT_TO_RIGHT ? this.$columns() : 1
        if(current < count - step || wrap){
            let index = current + step
            this.$setCurrent(index >= 0 && index < count ? index : 0)
        }
    }

    moveCurrentIndexLeft(){
        let count = this.$v[GridView.COUNT], current = this.$v[GridView.CURRENTINDEX], wrap = this.$v[GridView.KEYNAVIGATIONWRAPS]
        if(!count) return
        let step = this.$v[GridView.FLOW] === LEFT_TO_RIGHT ? 1 : this.$columns()
        if(current >= step || (step === 1 && current > 0) || wrap){
            let index = current - step
            this.$setCurrent(index >= 0 && index < count ? index : count - 1)
        }
    }

    moveCurrentIndexRight(){
        let count = this.$v[GridView.COUNT], current = this.$v[GridView.CURRENTINDEX], wrap = this.$v[GridView.KEYNAVIGATIONWRAPS]
        if(!count) return
        let step = this.$v[GridView.FLOW] === LEFT_TO_RIGHT ? 1 : this.$columns()
        if(current < count - step || wrap){
            let index = current + step
            this.$setCurrent(index >= 0 && index < count ? index : 0)
        }
    }

    // QQuickGridView::keyPressEvent
    $keyEvent(event, kind){
        if(kind !== 'pressed' || !this.$v[GridView.COUNT]) return 0
        let enabled = this.$keyNavigationSet ? this.$v[GridView.KEYNAVIGATIONENABLED] : this.$v[GridView.INTERACTIVE]
        if(!enabled) return 0
        let old = this.$v[GridView.CURRENTINDEX]
        switch(event.key){
            case QtEnums.Key_Up: this.moveCurrentIndexUp(); break
            case QtEnums.Key_Down: this.moveCurrentIndexDown(); break
            case QtEnums.Key_Left: this.moveCurrentIndexLeft(); break
            case QtEnums.Key_Right: this.moveCurrentIndexRight(); break
            default: return 0
        }
        return old !== this.$v[GridView.CURRENTINDEX] || this.$v[GridView.KEYNAVIGATIONWRAPS] ? 1 : 0
    }

    // QQuickItemViewPrivate::updateCurrent: the new current item takes the focus, then the highlight
    $updateCurrent(){
        let index = this.$v[GridView.CURRENTINDEX]
        let item = this.$items[index] || null
        if(item && item !== this.$v[GridView.CURRENTITEM]) item.$setFocus(true)
        this.$write(GridView.CURRENTITEM, item)
        this.$updateHighlight()
    }

    // QQuickGridViewPrivate::createHighlight / updateHighlight: the highlight (an empty item without a highlight
    // component) takes the current item's size and moves to it by two SmoothedAnimations of highlightMoveDuration;
    // then updateTrackedItem
    $updateHighlight(){
        let component = this.$v[GridView.HIGHLIGHT]
        let current = this.$v[GridView.CURRENTITEM]
        if(this.$highlight && (this.$highlight.$component !== component || !current)){
            this.$stopHighlightAnimations()
            this.$highlight.$releaseLater()
            this.$highlight = null
            this.$write(GridView.HIGHLIGHTITEM, null)
        }
        if(!current) return
        let follows = this.$v[GridView.HIGHLIGHTFOLLOWSCURRENTITEM]
        if(!this.$highlight){
            let highlight = component ? component.createObject(this.$content, null, null) : new Item(this.$content)
            if(!highlight) return
            highlight.$component = component
            this.$highlight = highlight
            for(let signal of [Item.SIGNAL_xChanged, Item.SIGNAL_yChanged, Item.SIGNAL_widthChanged, Item.SIGNAL_heightChanged]){
                engine.connect(highlight, signal, () => { if(this.$highlight === highlight) this.$trackedPositionChanged() })
            }
            if(follows) this.$resetHighlightPosition()
            this.$write(GridView.HIGHLIGHTITEM, highlight)
        }
        if(follows){
            let h = this.$highlight
            h.width = current.$v[Item.WIDTH]
            h.height = current.$v[Item.HEIGHT]
            this.$animateHighlight('x', current.$v[Item.X])
            this.$animateHighlight('y', current.$v[Item.Y])
        }
        this.$trackedPositionChanged()
    }

    $animateHighlight(property, to){
        let animations = this.$highlightAnimations || (this.$highlightAnimations = {})
        let animation = animations[property]
        let highlight = this.$highlight
        if(animation && animation.running && animation.to === to && animation.target === highlight) return
        if(highlight[property] === to){
            if(animation && animation.running) animation.stop()
            return
        }
        if(!animation) animation = animations[property] = new SmoothedAnimation(null)
        if(animation.target !== highlight) animation.target = highlight
        animation.property = property
        animation.duration = this.$v[GridView.HIGHLIGHTMOVEDURATION]
        animation.to = to
        animation.restart()
    }

    $stopHighlightAnimations(){
        let animations = this.$highlightAnimations
        if(animations) for(let key in animations) animations[key].stop()
    }

    // QQuickGridViewPrivate::resetHighlightPosition
    $resetHighlightPosition(){
        let highlight = this.$highlight, current = this.$v[GridView.CURRENTITEM]
        if(!highlight || !current) return
        this.$stopHighlightAnimations()
        highlight.$write(Item.X, current.$v[Item.X])
        highlight.$write(Item.Y, current.$v[Item.Y])
    }

    // QQuickItemView::trackedPositionChanged: after a new current index the view follows the highlight along the flow,
    // so the highlight and the current item show (rows of cellHeight with LeftToRight, columns of cellWidth else)
    $trackedPositionChanged(){
        let highlight = this.$highlight
        let current = this.$v[GridView.CURRENTITEM]
        if(!highlight || !current || this.$moveReason !== MOVE_SET_INDEX || !this.$v[GridView.HIGHLIGHTFOLLOWSCURRENTITEM]) return
        let axis = this.$v[GridView.FLOW] === LEFT_TO_RIGHT ? 'y' : 'x'
        let id = axis === 'y' ? Item.Y : Item.X
        let cell = axis === 'y' ? this.$v[GridView.CELLHEIGHT] : this.$v[GridView.CELLWIDTH]
        let size = axis === 'y' ? this.$v[Item.HEIGHT] : this.$v[Item.WIDTH]
        let trackedPos = highlight.$v[id], trackedEndPos = trackedPos + cell, trackedSize = cell
        let toItemPos = current.$v[id], toItemEndPos = toItemPos + cell
        let viewPos = this.$contentPos(axis)
        let pos = viewPos
        if(trackedEndPos >= viewPos + size && toItemEndPos >= viewPos + size){
            if(trackedEndPos <= toItemEndPos){
                pos = trackedEndPos - size
                if(trackedSize > size) pos = trackedPos
            } else {
                pos = toItemEndPos - size
                if(cell > size) pos = toItemPos
            }
        }
        if(trackedPos < pos && toItemPos < pos) pos = Math.max(trackedPos, toItemPos)
        if(pos === viewPos) return
        this.$internalMove++
        try {
            if(axis === 'x') this.contentX = pos
            else this.contentY = pos
        } finally {
            this.$internalMove--
        }
    }

    $updateBeginningEnd(){
        for(let axis of ['x', 'y']){
            let b = this.$flickBounds(axis)
            let pos = this.$contentPos(axis)
            this.$write(axis === 'x' ? GridView.ATXBEGINNING : GridView.ATYBEGINNING, pos <= b.min)
            this.$write(axis === 'x' ? GridView.ATXEND : GridView.ATYEND, pos >= b.max)
        }
    }

    $layout(){
        let cw = this.$v[GridView.CELLWIDTH], ch = this.$v[GridView.CELLHEIGHT]
        let flow = this.$v[GridView.FLOW]
        let perLine = flow === LEFT_TO_RIGHT
            ? Math.max(1, Math.floor(this.$v[Item.WIDTH] / cw) || 1)
            : Math.max(1, Math.floor(this.$v[Item.HEIGHT] / ch) || 1)
        this.$items.forEach((item, i) => {
            let line = Math.floor(i / perLine), pos = i % perLine
            item.$place(Item.X, flow === LEFT_TO_RIGHT ? pos * cw : line * cw)
            item.$place(Item.Y, flow === LEFT_TO_RIGHT ? line * ch : pos * ch)
        })
        // QQuickGridViewPrivate::endPosition: where the last item really is, plus a cell
        let last = this.$items[this.$items.length - 1]
        this.$write(GridView.CONTENTWIDTH, flow === LEFT_TO_RIGHT ? perLine * cw : last ? last.$v[Item.X] + cw : 0)
        this.$write(GridView.CONTENTHEIGHT, flow === LEFT_TO_RIGHT ? (last ? last.$v[Item.Y] + ch : 0) : perLine * ch)
        this.$content.$write(Item.WIDTH, flow === LEFT_TO_RIGHT ? this.$v[Item.WIDTH] : this.$v[GridView.CONTENTWIDTH])
        this.$content.$write(Item.HEIGHT, flow === LEFT_TO_RIGHT ? this.$v[GridView.CONTENTHEIGHT] : this.$v[Item.HEIGHT])
        this.$updateBeginningEnd()
        this.$updateHighlight()
    }

    // QQuickItemView::updatePolish: a new cell size or size of the view is laid out once, with the frame
    $polish(){
        this.$layout()
    }

    positionViewAtBeginning(){ this.contentY = 0; this.contentX = 0 }
    positionViewAtEnd(){
        let axis = this.$v[GridView.FLOW] === LEFT_TO_RIGHT ? 'y' : 'x'
        this.$setContentPos(axis, this.$flickBounds(axis).max)
    }
    forceLayout(){ this.$layout() }

    $wheel(x, y, angleDelta, pixelDelta){
        if(!this.$v[GridView.INTERACTIVE]) return false
        return wheelScroll(this, angleDelta, pixelDelta)
    }

    $write(id, value){
        let old = this.$v[id]
        if(id === GridView.CURRENTINDEX && value !== old && this.$items && !this.$modelUpdating && this.$f & engine.COMPLETED) this.$moveReason = MOVE_SET_INDEX
        if(id === GridView.KEYNAVIGATIONENABLED) this.$keyNavigationSet = true
        if(id === GridView.FLICKABLEDIRECTION && !this.$settingDirection) this.$flickDirectionSet = true
        super.$write(id, value)
        if((id === Item.WIDTH || id === Item.HEIGHT) && this.$items) engine.queuePolish(this)
        if(id === GridView.CURRENTINDEX && old !== this.$v[id] && this.$items) this.$updateCurrent()
    }

    $disposeNative(){
        if(this.$model instanceof ListModel) this.$model.$removeView(this)
        this.$stopHighlightAnimations()
        super.$disposeNative()
    }
}
installFlick(GridView)

const regrid = (view) => { if(view.$f & engine.COMPLETED) view.$sync() }
const modelSet = (view, value, old) => { if(!sameModel(value, old)) regrid(view) }
const relayout = (view) => { if(view.$items) engine.queuePolish(view) }
const scrolled = (view) => {
    view.$content.$write(Item.X, -view.$v[GridView.CONTENTX])
    view.$content.$write(Item.Y, -view.$v[GridView.CONTENTY])
    view.$updateBeginningEnd()
}
// QQuickGridView::setFlow: it flicks along the flow unless the direction was set
const flowChanged = (view) => {
    if(!view.$flickDirectionSet){
        view.$settingDirection = true
        view.$write(GridView.FLICKABLEDIRECTION, view.$v[GridView.FLOW] === LEFT_TO_RIGHT ? 2 : 1)
        view.$settingDirection = false
    }
    relayout(view)
}
GridView.prototype.$isFocusScope = true

defineType(GridView, {
    props: Object.assign({}, FLICK_PROPS, {
        model: { kind: 'var', effect: modelSet },
        delegate: { kind: 'component', effect: regrid },
        count: { kind: 'int', readonly: true },
        cellWidth: { kind: 'real', initial: 100, effect: relayout },
        cellHeight: { kind: 'real', initial: 100, effect: relayout },
        flow: { kind: 'int', effect: flowChanged },
        currentIndex: { kind: 'int', initial: -1 },
        currentItem: { kind: 'var', initial: null, readonly: true },
        contentX: { kind: 'real', effect: scrolled, set(value){ externalMove(this, 'x'); this.$assign(GridView.CONTENTX, value) } },
        contentY: { kind: 'real', effect: scrolled, set(value){ externalMove(this, 'y'); this.$assign(GridView.CONTENTY, value) } },
        contentWidth: 'real',
        contentHeight: 'real',
        flickableDirection: { kind: 'int', initial: 2 },
        keyNavigationWraps: 'bool',
        highlightItem: { kind: 'var', readonly: true },
        atYBeginning: { kind: 'bool', initial: true, readonly: true },
        atYEnd: { kind: 'bool', initial: true, readonly: true },
        atXBeginning: { kind: 'bool', initial: true, readonly: true },
        atXEnd: { kind: 'bool', initial: true, readonly: true },
        originX: { kind: 'real', readonly: true },
        originY: { kind: 'real', readonly: true },
        snapMode: 'int',
        cacheBuffer: { kind: 'int', initial: 320 },
        keyNavigationEnabled: { kind: 'bool', initial: true },
        highlight: { kind: 'component', effect: (view) => { if(view.$items) view.$updateHighlight() } },
        highlightFollowsCurrentItem: { kind: 'bool', initial: true },
        highlightMoveDuration: { kind: 'int', initial: 150 },
        reuseItems: 'bool',
    }),
    signals: FLICK_SIGNALS,
})
Object.assign(GridView, FLICK_ENUMS, { FlowLeftToRight: LEFT_TO_RIGHT, FlowTopToBottom: TOP_TO_BOTTOM, NoSnap: 0, SnapToRow: 1, SnapOneRow: 2 })

module.exports = { GridView }
