const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { ListModel } = require('../QtQml/Models/ListModel')
const { ModelContext, modelRows, sameModel } = require('../QtQml/Models/ModelContext')
const { Item } = require('./Item')
const { installFlick, flickDirection, wheelScroll, FLICK_PROPS, FLICK_SIGNALS, FLICK_ENUMS } = require('./Flickable')
const QtEnums = require('../Qt/enums')

// ---- GridView: every delegate exists, laid out in cells ---------------------------------------

const LEFT_TO_RIGHT = 0, TOP_TO_BOTTOM = 1

class GridView extends Item {
    constructor(parent = null){
        super(parent)
        this.$content = new Item(this)
        this.$items = []
        this.$contexts = []
        this.$model = null
        this.$v[Item.CLIP] = false
        engine.scheduleComponentComplete(this)
    }

    get contentItem(){ return this.$content }

    itemAtIndex(index){ return this.$items[index] || null }

    $componentComplete(){
        super.$componentComplete()
        this.$sync()
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
        this.$updateCurrent()
    }

    $modelChanged(kind, index, count, roles = null){
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

    $setContentPos(axis, value){
        if(axis === 'x') this.contentX = value
        else this.contentY = value
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

    $updateCurrent(){
        let index = this.$v[GridView.CURRENTINDEX]
        let item = this.$items[index] || null
        if(item && item !== this.$v[GridView.CURRENTITEM]) item.$setFocus(true)
        this.$write(GridView.CURRENTITEM, item)
        this.$updateHighlight()
        // QQuickItemView::trackedPositionChanged: the current item comes into the view
        if(item && (this.$f & engine.COMPLETED) && !(this.$fs && (this.$fs.pressed || this.$fs.moving))){
            for(let axis of ['x', 'y']){
                if(!this.$alongFlow(axis)) continue
                let start = item.$v[axis === 'x' ? Item.X : Item.Y]
                let size = axis === 'x' ? this.$v[GridView.CELLWIDTH] : this.$v[GridView.CELLHEIGHT]
                let viewSize = axis === 'x' ? this.$v[Item.WIDTH] : this.$v[Item.HEIGHT]
                let pos = this.$contentPos(axis)
                if(start + size > pos + viewSize) pos = start + size - viewSize
                if(start < pos) pos = start
                let b = this.$flickBounds(axis)
                this.$setContentPos(axis, Math.max(b.min, Math.min(b.max, pos)))
            }
        }
    }

    $updateHighlight(){
        let component = this.$v[GridView.HIGHLIGHT]
        let current = this.$v[GridView.CURRENTITEM]
        if(this.$highlight && (this.$highlight.$component !== component || !current)){
            this.$highlight.$releaseLater()
            this.$highlight = null
            this.$write(GridView.HIGHLIGHTITEM, null)
        }
        if(!component || !current) return
        if(!this.$highlight){
            let item = component.createObject(this.$content, null, null)
            if(!item) return
            item.$component = component
            this.$highlight = item
            this.$write(GridView.HIGHLIGHTITEM, item)
        }
        if(!this.$v[GridView.HIGHLIGHTFOLLOWSCURRENTITEM]) return
        let h = this.$highlight
        h.x = current.$v[Item.X]
        h.y = current.$v[Item.Y]
        if(!h.$v[Item.WIDTH]) h.width = this.$v[GridView.CELLWIDTH]
        if(!h.$v[Item.HEIGHT]) h.height = this.$v[GridView.CELLHEIGHT]
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
        if(id === GridView.KEYNAVIGATIONENABLED) this.$keyNavigationSet = true
        if(id === GridView.FLICKABLEDIRECTION && !this.$settingDirection) this.$flickDirectionSet = true
        super.$write(id, value)
        if((id === Item.WIDTH || id === Item.HEIGHT) && this.$items) engine.queuePolish(this)
        if(id === GridView.CURRENTINDEX && old !== this.$v[id] && this.$items) this.$updateCurrent()
    }

    $disposeNative(){
        if(this.$model instanceof ListModel) this.$model.$removeView(this)
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
        contentX: { kind: 'real', effect: scrolled },
        contentY: { kind: 'real', effect: scrolled },
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
        reuseItems: 'bool',
    }),
    signals: FLICK_SIGNALS,
})
Object.assign(GridView, FLICK_ENUMS, { FlowLeftToRight: LEFT_TO_RIGHT, FlowTopToBottom: TOP_TO_BOTTOM, NoSnap: 0, SnapToRow: 1, SnapOneRow: 2 })

module.exports = { GridView }
