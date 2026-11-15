const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { ListModel } = require('../QtQml/Models/ListModel')
const { ModelContext, modelRows, sameModel } = require('../QtQml/Models/ModelContext')
const { Item } = require('./Item')

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
                    this.$items.push(item)
                    this.$contexts.push(ctx)
                }
            }
        }
        this.$write(GridView.COUNT, this.$items.length)
        this.$layout()
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
    }

    // QQuickItemView::updatePolish: a new cell size or size of the view is laid out once, with the frame
    $polish(){
        this.$layout()
    }

    positionViewAtBeginning(){ this.contentY = 0; this.contentX = 0 }
    positionViewAtEnd(){ this.contentY = Math.max(0, this.$v[GridView.CONTENTHEIGHT] - this.$v[Item.HEIGHT]) }
    forceLayout(){ this.$layout() }

    $wheel(x, y, angleDelta, pixelDelta){
        if(!this.$v[GridView.INTERACTIVE]) return false
        return require('./Flickable').wheelScroll(this, GridView.CONTENTX, GridView.CONTENTY, this.$v[GridView.CONTENTWIDTH], this.$v[GridView.CONTENTHEIGHT], angleDelta, pixelDelta)
    }

    $write(id, value){
        super.$write(id, value)
        if((id === Item.WIDTH || id === Item.HEIGHT) && this.$items) engine.queuePolish(this)
    }

    $disposeNative(){
        if(this.$model instanceof ListModel) this.$model.$removeView(this)
        super.$disposeNative()
    }
}

const regrid = (view) => { if(view.$f & engine.COMPLETED) view.$sync() }
const modelSet = (view, value, old) => { if(!sameModel(value, old)) regrid(view) }
const relayout = (view) => { if(view.$items) engine.queuePolish(view) }
const scrolled = (view) => {
    view.$content.$write(Item.X, -view.$v[GridView.CONTENTX])
    view.$content.$write(Item.Y, -view.$v[GridView.CONTENTY])
}
GridView.prototype.$isFocusScope = true

defineType(GridView, {
    props: {
        model: { kind: 'var', effect: modelSet },
        delegate: { kind: 'component', effect: regrid },
        count: { kind: 'int', readonly: true },
        cellWidth: { kind: 'real', initial: 100, effect: relayout },
        cellHeight: { kind: 'real', initial: 100, effect: relayout },
        flow: { kind: 'int', effect: relayout },
        currentIndex: { kind: 'int', initial: -1 },
        currentItem: { kind: 'var', initial: null },
        contentX: { kind: 'real', effect: scrolled },
        contentY: { kind: 'real', effect: scrolled },
        contentWidth: 'real',
        contentHeight: 'real',
        interactive: { kind: 'bool', initial: true },
        boundsBehavior: { kind: 'int', initial: 3 },
        moving: 'bool',
        flicking: 'bool',
        dragging: 'bool',
        atYBeginning: { kind: 'bool', initial: true },
        atYEnd: { kind: 'bool', initial: true },
        atXBeginning: { kind: 'bool', initial: true },
        atXEnd: { kind: 'bool', initial: true },
        originX: 'real',
        originY: 'real',
        snapMode: 'int',
        cacheBuffer: { kind: 'int', initial: 320 },
        keyNavigationEnabled: { kind: 'bool', initial: true },
        highlight: 'component',
        highlightFollowsCurrentItem: { kind: 'bool', initial: true },
        reuseItems: 'bool',
    },
    signals: ['movementStarted', 'movementEnded', 'flickStarted', 'flickEnded', 'dragStarted', 'dragEnded'],
})
Object.assign(GridView, { FlowLeftToRight: LEFT_TO_RIGHT, FlowTopToBottom: TOP_TO_BOTTOM, NoSnap: 0, SnapToRow: 1, SnapOneRow: 2 })

module.exports = { GridView }
