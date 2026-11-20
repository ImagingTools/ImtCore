// TableView (qquicktableview.cpp and qqmltableinstancemodel.cpp of Qt 6.12). The table loads the cells that fill the
// viewport edge by edge (loadedColumns x loadedRows), lays each column out at the width of its widest loaded cell (or the
// width a provider or setColumnWidth gives) and each row likewise, estimates the size of what is not loaded from the
// average of what is, and rebuilds in the states of QQuickTableViewPrivate::RebuildState when the model, the delegate or
// the geometry change. Delegates are pooled and reused; a delegate made for a cell gets the roles of the cell, row,
// column, index, model, modelData and the required current / selected / editing of the selection model.
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { GroupObject } = require('../Core/QtObject')
const { QPoint, QPointF, QRectF } = require('../Core/ValueTypes')
const { Item } = require('./Item')
const { Flickable, clock, DRAG_THRESHOLD } = require('./Flickable')
const { ModelContext } = require('../QtQml/Models/ModelContext')
const { AbstractItemModel, QModelIndex, QPersistentModelIndex, INVALID, sameIndex, adaptModel, releaseAdaptor } = require('../QtQml/Models/ItemModel')
const { ItemSelectionModel, QItemSelection, QItemSelectionRange } = require('../QtQml/Models/ItemSelectionModel')
const { RoleCell } = require('../QtQml/Models/ListModel')
const { resolveDelegate } = require('../QtQml/Models/DelegateChooser')
const QtEnums = require('../Qt/enums')

const VISIBILITY = require('./Item').bits.VISIBILITY

const K_DEFAULT_ROW_HEIGHT = 50, K_DEFAULT_COLUMN_WIDTH = 50
const EDGE_NOT_SET = -2, EDGE_AT_END = -3
const TOP = QtEnums.TopEdge, LEFT = QtEnums.LeftEdge, RIGHT = QtEnums.RightEdge, BOTTOM = QtEnums.BottomEdge
const ALL_EDGES = [LEFT, RIGHT, TOP, BOTTOM]
const edgeIndex = (edge) => (edge === TOP ? 0 : edge === LEFT ? 1 : edge === RIGHT ? 2 : 3)

// QQuickTableViewPrivate::RebuildState
const RS = { Begin: 0, LoadInitialTable: 1, VerifyTable: 2, LayoutTable: 3, CancelOvershoot: 4, UpdateContentSize: 5,
    PreloadColumns: 6, PreloadRows: 7, MovePreloadedItemsToPool: 8, Done: 9 }
// QQuickTableViewPrivate::RebuildOption
const RO = { None: 0, All: 0x1, LayoutOnly: 0x2, ViewportOnly: 0x4, CalculateNewTopLeftRow: 0x8, CalculateNewTopLeftColumn: 0x10,
    CalculateNewContentWidth: 0x20, CalculateNewContentHeight: 0x40, PositionViewAtRow: 0x80, PositionViewAtColumn: 0x100 }

// QQuickTableView enums
const ENUMS = {
    AlignLeft: QtEnums.AlignLeft, AlignRight: QtEnums.AlignRight, AlignHCenter: QtEnums.AlignHCenter,
    AlignTop: QtEnums.AlignTop, AlignBottom: QtEnums.AlignBottom, AlignVCenter: QtEnums.AlignVCenter,
    AlignCenter: QtEnums.AlignVCenter | QtEnums.AlignHCenter, Visible: 0x01000, Contain: 0x02000,
    SelectionDisabled: 0, SelectCells: 1, SelectRows: 2, SelectColumns: 3,
    SingleSelection: 0, ContiguousSelection: 1, ExtendedSelection: 2,
    NoEditTriggers: 0x0, SingleTapped: 0x1, DoubleTapped: 0x2, SelectedTapped: 0x4, EditKeyPressed: 0x8, AnyKeyPressed: 0x10,
}

const fuzzyIsNull = (d) => Math.abs(d) <= 0.000000000001
const fuzzyCompare = (a, b) => Math.abs(a - b) * 1000000000000 <= Math.min(Math.abs(a), Math.abs(b))
const bound = (min, v, max) => Math.max(min, Math.min(v, max))
const cellValid = (cell) => cell.x !== -1 && cell.y !== -1

// ---- QMinimalFlatSet<int>: the loaded columns / rows, kept sorted --------------------------------------------------------
class SortedSet {
    constructor(){ this.a = [] }
    insert(v){
        let a = this.a, lo = 0, hi = a.length
        while(lo < hi){ let mid = (lo + hi) >> 1; if(a[mid] < v) lo = mid + 1; else hi = mid }
        if(a[lo] !== v) a.splice(lo, 0, v)
    }
    remove(v){ let i = this.a.indexOf(v); if(i >= 0) this.a.splice(i, 1) }
    contains(v){ return this.a.indexOf(v) >= 0 }
    clear(){ this.a = [] }
    get count(){ return this.a.length }
    get first(){ return this.a[0] }
    get last(){ return this.a[this.a.length - 1] }
    values(){ return this.a.slice() }
    isEmpty(){ return this.a.length === 0 }
    [Symbol.iterator](){ return this.a.slice()[Symbol.iterator]() }
}

// ---- TableView attached object (QQuickTableViewAttached) -------------------------------------------------------------------
class TableViewAttached extends GroupObject {}
defineType(TableViewAttached, {
    props: {
        view: { kind: 'var', readonly: true },
        editDelegate: 'component',
    },
    signals: ['pooled', 'reused', 'commit'],
})
Object.assign(TableViewAttached.prototype, ENUMS)

{
    let prop = Item.$type.propByName.get('TableView')
    prop.group = TableViewAttached
    let id = prop.id
    Object.defineProperty(Item.prototype, 'TableView', {
        get(){ return this.$v[id] || (this.$v[id] = new TableViewAttached(this)) },
        configurable: true,
    })
}
const attachedOf = (item) => (item ? item.TableView : null)
// qmlAttachedPropertiesObject(object, false): the attached object if the item has one already
const existingAttached = (item) => (item ? item.$v[Item.$type.propByName.get('TableView').id] || null : null)

// ---- the context of a cell: its roles, row, column, index (QQmlDelegateModelItem of a QQmlTableInstanceModel) -------------
class CellContext extends ModelContext {
    constructor(instanceModel, flat, row, column){
        super(null, flat, undefined)
        this.$im = instanceModel
        this.$cellRow = row
        this.$cellColumn = column
        this.$view = instanceModel.view
        this.$tableRequired = null
        this.$row = cellRow(this)
        this.$v[ModelContext.MODELDATA] = this.$modelData()
    }

    $index(){
        let model = this.$im.model
        return model ? model.index(this.$cellRow, this.$cellColumn) : INVALID
    }

    // a role by name: row, column, index and modelData of the delegate model item, else data() of the item model
    $roleValue(name){
        if(name === 'row') return this.$cellRow
        if(name === 'column') return this.$cellColumn
        if(name === 'index') return this.$v[ModelContext.INDEX]
        let model = this.$im.model
        if(!model) return undefined
        if(name === 'modelData') return this.$modelData()
        let role = model.$roleOf(name)
        if(role < 0) return undefined
        return model.data(this.$index(), role)
    }

    $hasRole(name){
        if(name === 'row' || name === 'column' || name === 'index') return true
        let model = this.$im.model
        if(!model) return false
        if(name === 'modelData') return true
        return model.$roleOf(name) >= 0
    }

    // modelData: the only role of a model with one role (the element of an array, the number of a count)
    $modelData(){
        let model = this.$im.model
        if(!model) return undefined
        let names = model.$roleNameCache || (model.$roleNameCache = model.roleNames())
        if(names.size !== 1) return undefined
        let [role] = names.keys()
        return model.data(this.$index(), role)
    }

    $role(name){
        if(!this.$cells) this.$cells = new Map()
        let cell = this.$cells.get(name)
        if(!cell) this.$cells.set(name, cell = new RoleCell())
        engine.track(cell, RoleCell.SIGNAL_changed)
        return this.$roleValue(name)
    }

    // `model.display = value` writes the item model (QQmlDelegateModel::Qt5ReadWrite)
    get model(){
        if(!this.$model){
            let ctx = this
            this.$model = new Proxy({}, {
                get(target, key){
                    if(typeof key !== 'string') return undefined
                    if(key === 'index') return ctx.index
                    if(key === 'modelData') return ctx.$role('modelData')
                    return ctx.$role(key)
                },
                set(target, key, value){
                    let model = ctx.$im.model
                    if(model){
                        let role = key === 'modelData' ? 0 : model.$roleOf(key)
                        if(role >= 0) model.setData(ctx.$index(), value, role)
                    }
                    return true
                },
                has(target, key){ return typeof key === 'string' && ctx.$hasRole(key) },
            })
        }
        return this.$model
    }

    // QQmlDelegateModelItem::setModelIndex: the cell moved to another index; every role is new
    $setModelIndex(flat, row, column){
        this.$cellRow = row
        this.$cellColumn = column
        this.$write(ModelContext.INDEX, flat)
        this.$write(ModelContext.MODELDATA, this.$modelData())
        this.$rowChanged(null)
    }

    // required properties of TableView (tableView, current, selected, editing, containsDrag): set when the delegate
    // declares them (QQuickTableViewPrivate::setRequiredProperty)
    $setTableRequired(name, value){
        let item = this.$requiredItem || this.$tableItem
        if(!item || item.$f & engine.DESTROYED) return
        let names = item.constructor && item.constructor.$qmlRequired
        if(!names || names.indexOf(name) < 0) return
        if(item[name] !== value) item[name] = value
    }

    $attachRequired(item){
        this.$tableItem = item
        super.$attachRequired(item)
        // QQmlTableInstanceModelIncubationTask::setInitialState: initItem, then updateItemProperties(init)
        if(this.$im.onInit) this.$im.onInit(this, item)
    }
}

// the row object of a cell context: the names it has and their values (the lookup of a role in a delegate)
function cellRow(ctx){
    return new Proxy({}, {
        has(target, key){ return typeof key === 'string' && ctx.$hasRole(key) },
        get(target, key){ return typeof key === 'string' ? ctx.$roleValue(key) : undefined },
        set(target, key, value){ ctx.model[key] = value; return true },
    })
}

// ---- QQmlTableInstanceModel: the delegates of the cells, the released items and the reuse pool ------------------------------
class TableInstanceModel {
    constructor(view, onInit){
        this.view = view
        this.onInit = onInit
        this.items = new Map()          // flat index -> entry { item, ctx, delegate, persistent }
        this.released = []
        this.pool = []                  // { entry, poolTime }
        this.adaptor = null
        this.model = null
        this.modelValue = undefined
        this.delegate = null
        this.links = []
    }

    rows(){ return this.model ? this.model.$rowCount() : 0 }
    columns(){ return this.model ? this.model.$columnCount() : 0 }
    count(){ return this.rows() * this.columns() }
    // QQmlAdaptorModel::rowAt / columnAt: the flat index runs down the rows, column by column
    rowAt(flat){ let rows = this.rows(); return rows ? flat % rows : 0 }
    columnAt(flat){ let rows = this.rows(); return rows ? Math.floor(flat / rows) : 0 }

    setModel(value){
        if(this.modelValue === value && this.model) return
        this.drainReusableItemsPool(0)
        for(let [object, signal, fn] of this.links) engine.disconnect(object, signal, fn)
        this.links = []
        releaseAdaptor(this.adaptor)
        this.modelValue = value
        this.adaptor = adaptModel(value)
        this.model = this.adaptor.model
        if(this.model){
            let link = (signal, fn) => {
                engine.connect(this.model, signal, fn)
                this.links.push([this.model, signal, fn])
            }
            link(AbstractItemModel.SIGNAL_dataChanged, (tl, br, roles) => this.$dataChanged(tl, br, roles))
        }
    }

    $resolveDelegate(flat){
        let row = this.rowAt(flat), column = this.columnAt(flat)
        let model = this.model
        let index = model ? model.index(row, column) : INVALID
        let value = (name) => {
            if(!model) return undefined
            if(name === 'modelData'){
                let names = model.roleNames()
                return names.size === 1 ? model.data(index, [...names.keys()][0]) : undefined
            }
            let role = model.$roleOf(name)
            return role >= 0 ? model.data(index, role) : undefined
        }
        return resolveDelegate(this.delegate, value, row, column)
    }

    // QQmlTableInstanceModel::object: the item of a flat index, from the released items, the pool, or made
    object(flat){
        let entry = this.items.get(flat)
        if(entry) return entry.item
        let delegate = this.$resolveDelegate(flat)
        if(!delegate) return null
        let row = this.rowAt(flat), column = this.columnAt(flat)
        let modelIndex = this.model ? this.model.index(row, column) : INVALID
        if(modelIndex.valid){
            let at = this.released.findIndex(e => e.persistent && sameIndex(e.persistent.index, modelIndex))
            if(at >= 0 && this.released[at].delegate === delegate){
                entry = this.released.splice(at, 1)[0]
                this.items.set(flat, entry)
                entry.flat = flat
                entry.ctx.$setModelIndex(flat, row, column)
                this.view.$updateItemProperties(flat, entry.item, false)
                return entry.item
            }
        }
        let poolAt = this.pool.findIndex(p => p.entry.delegate === delegate)
        if(poolAt >= 0){
            entry = this.pool.splice(poolAt, 1)[0].entry
            this.items.set(flat, entry)
            entry.flat = flat
            entry.persistent = new QPersistentModelIndex(modelIndex)
            entry.ctx.$setModelIndex(flat, row, column)
            this.view.$updateItemProperties(flat, entry.item, false)
            this.view.$itemReused(flat, entry.item)
            return entry.item
        }
        let ctx = new CellContext(this, flat, row, column)
        entry = { item: null, ctx, delegate, persistent: null, flat }
        this.items.set(flat, entry)
        let item = delegate.createObject(this.view.$content, null, ctx)
        if(!item){
            this.items.delete(flat)
            console.warn('Error incubating delegate')
            return null
        }
        entry.item = item
        entry.persistent = new QPersistentModelIndex(modelIndex)
        item.$tableEntry = entry
        return item
    }

    entryOf(item){ return item && item.$tableEntry || null }

    // QQmlTableInstanceModel::release: a reusable item waits among the released ones until commitReleasedItems
    release(item, reusable){
        let entry = this.entryOf(item)
        if(!entry) return 'destroyed'
        if(this.items.get(entry.flat) === entry) this.items.delete(entry.flat)
        if(reusable){
            this.released.push(entry)
            return 'pooled'
        }
        this.$destroy(entry)
        return 'destroyed'
    }

    commitReleasedItems(){
        let released = this.released
        this.released = []
        for(let entry of released){
            if(entry.persistent) entry.persistent.release()
            entry.persistent = null
            this.pool.push({ entry, poolTime: 0 })
            this.view.$itemPooled(entry.flat, entry.item)
        }
    }

    // QQmlReusableDelegateModelItemsPool::drain: what waited maxPoolTime rounds goes; the others wait one round more
    drainReusableItemsPool(maxPoolTime){
        if(maxPoolTime === 0){
            let removed = this.pool
            this.pool = []
            for(let p of removed) this.$destroy(p.entry)
            return
        }
        let keep = [], removed = []
        for(let p of this.pool){
            if(p.poolTime < maxPoolTime){
                p.poolTime++
                keep.push(p)
            } else removed.push(p)
        }
        this.pool = keep
        for(let p of removed) this.$destroy(p.entry)
    }

    dispose(item){
        let entry = this.entryOf(item)
        if(!entry) return
        if(this.items.get(entry.flat) === entry) this.items.delete(entry.flat)
        this.$destroy(entry)
    }

    $destroy(entry){
        if(entry.persistent) entry.persistent.release()
        entry.persistent = null
        if(entry.item && !(entry.item.$f & engine.DESTROYED)){
            entry.item.$tableEntry = null
            entry.item.$releaseLater()
        }
    }

    // QQmlTableInstanceModel::dataChangedCallback: the roles of the cells that show the changed items
    $dataChanged(topLeft, bottomRight, roles){
        let model = this.model
        let names = roles && roles.length ? roles.map(r => model.roleNames().get(r)).filter(n => n !== undefined) : null
        for(let entry of this.items.values()){
            let p = entry.persistent && entry.persistent.index
            if(!p || !p.valid) continue
            if(p.row < topLeft.row || p.row > bottomRight.row || p.column < topLeft.column || p.column > bottomRight.column) continue
            if(names && names.length === 0) continue
            entry.ctx.$write(ModelContext.MODELDATA, entry.ctx.$modelData())
            entry.ctx.$rowChanged(names)
        }
    }

    releaseAll(){
        for(let entry of Array.from(this.items.values())) this.$destroy(entry)
        this.items.clear()
        for(let entry of this.released) this.$destroy(entry)
        this.released = []
        this.drainReusableItemsPool(0)
    }
}

// ---- the animation of positionViewAt (QQuickPropertyAnimation of contentX / contentY, OutQuart) ----------------------------
class PositionAnimation {
    constructor(view, axis){
        this.view = view
        this.axis = axis
        this.running = false
        this.to = 0
        this.duration = 250
        this.token = 0
    }
    isRunning(){ return this.running }
    restart(){
        this.stop()
        this.from = this.view.$v[this.axis === 'x' ? Flickable.CONTENTX : Flickable.CONTENTY]
        this.start = clock.now()
        this.running = true
        let token = ++this.token
        let step = () => {
            if(token !== this.token || this.view.$f & engine.DESTROYED) return
            let t = Math.min(1, (clock.now() - this.start) / this.duration)
            let eased = 1 - Math.pow(1 - t, 4)
            engine.batch(() => this.view.$setContentPos(this.axis, this.from + (this.to - this.from) * eased))
            if(t < 1) clock.frame(step)
            else this.running = false
        }
        clock.frame(step)
    }
    stop(){
        this.running = false
        this.token++
    }
    // QAbstractAnimation::complete
    complete(){
        if(!this.running) return
        this.stop()
        engine.batch(() => this.view.$setContentPos(this.axis, this.to))
    }
}

// ---- TableView ----------------------------------------------------------------------------------------------------------
class TableView extends Flickable {
    constructor(parent = null){
        super(parent)
        this.$loadedItems = new Map()                     // flat index -> fx { item, cell: {x, y}, index }
        this.$loadedColumns = new SortedSet()
        this.$loadedRows = new SortedSet()
        this.$outer = new QRectF(0, 0, 0, 0)               // loadedTableOuterRect
        this.$inner = new QRectF(0, 0, 0, 0)               // loadedTableInnerRect
        this.$origin = { x: 0, y: 0 }
        this.$endExtent = { w: 0, h: 0 }
        this.$viewportRect = new QRectF(0, 0, -1, -1)
        this.$tableSize = { w: 0, h: 0 }
        this.$rebuildState = RS.Done
        this.$rebuildOptions = RO.All
        this.$scheduledRebuildOptions = RO.All
        this.$loadRequest = { active: false, edge: 0, edgeIndex: 0, cells: [], current: 0, startPos: { x: 0, y: 0 } }
        this.$cellSpacing = { w: 0, h: 0 }
        this.$layoutWarningIssued = false
        this.$polishing = false
        this.$syncVertically = false
        this.$syncHorizontally = false
        this.$inSetLocalViewportPos = false
        this.$inSyncViewportPosRecursive = false
        this.$inUpdateContentSize = false
        this.$cachedNextVisibleEdgeIndex = [0, 1, 2, 3].map(() => ({ start: EDGE_NOT_SET, end: EDGE_NOT_SET }))
        this.$cachedColumnWidth = { start: EDGE_NOT_SET, size: 0 }
        this.$cachedRowHeight = { start: EDGE_NOT_SET, size: 0 }
        this.$explicitContentWidth = null
        this.$explicitContentHeight = null
        this.$averageEdgeSize = { w: 0, h: 0 }
        this.$syncView = null
        this.$syncChildren = []
        this.$selectionModel = null
        this.$selectionLinks = []
        this.$selectionFlag = ItemSelectionModel.NoUpdate
        this.$selectionStartCell = { x: -1, y: -1 }
        this.$selectionEndCell = { x: -1, y: -1 }
        this.$existingSelection = new QItemSelection()
        this.$inSelectionModelUpdate = false
        this.$warnNoSelectionModel = true
        this.$assignedPositionViewAtRowAfterRebuild = 0
        this.$assignedPositionViewAtColumnAfterRebuild = 0
        this.$positionViewAtRowAfterRebuild = 0
        this.$positionViewAtColumnAfterRebuild = 0
        this.$positionViewAtRowOffset = 0
        this.$positionViewAtColumnOffset = 0
        this.$positionViewAtRowSubRect = null
        this.$positionViewAtColumnSubRect = null
        this.$positionViewAtRowAlignment = QtEnums.AlignTop
        this.$positionViewAtColumnAlignment = QtEnums.AlignLeft
        this.$positionXAnimation = new PositionAnimation(this, 'x')
        this.$positionYAnimation = new PositionAnimation(this, 'y')
        this.$edgesBeforeRebuild = null
        this.$tableSizeBeforeRebuild = { w: 0, h: 0 }
        this.$currentRow = -1
        this.$currentColumn = -1
        this.$explicitColumnWidths = new Map()
        this.$explicitRowHeights = new Map()
        this.$editModel = null
        this.$editItem = null
        this.$editIndex = new QPersistentModelIndex(INVALID)
        this.$hover = { row: -1, column: -1 }
        this.$resize = { state: 'listening', row: -1, column: -1, startX: 0, startY: 0, startWidth: 0, startHeight: 0, pressX: 0, pressY: 0 }
        this.$tap = { pressed: false, x: 0, y: 0, lastTapTime: -1, lastX: 0, lastY: 0, modifiers: 0 }
        this.$horizontalVisualIndices = []
        this.$verticalVisualIndices = []
        this.$horizontalLogicalIndices = []
        this.$verticalLogicalIndices = []
        this.$sectionMoving = false
        this.$isTransposed = false
        this.$needsModelSynchronization = false
        this.$assignedModel = 0
        this.$tableModel = new TableInstanceModel(this, (ctx, item) => this.$initNewItem(ctx, item))
        this.$editModelHolder = null
        // QQuickTableViewPrivate::init: a focus scope that takes the focus by Tab
        this.$isFocusScope = true
        this.$v[Item.ACTIVEFOCUSONTAB] = true
        engine.scheduleComponentComplete(this)
        engine.scheduleFinalize(this)
    }

    // ---- small helpers ----
    get $model(){ return this.$tableModel.model }
    $topRow(){ return this.$loadedRows.first }
    $bottomRow(){ return this.$loadedRows.last }
    $leftColumn(){ return this.$loadedColumns.first }
    $rightColumn(){ return this.$loadedColumns.last }
    $isComplete(){ return !!(this.$f & engine.COMPLETED) }
    $width(){ return this.$v[Item.WIDTH] }
    $height(){ return this.$v[Item.HEIGHT] }
    $contentX(){ return this.$v[Flickable.CONTENTX] }
    $contentY(){ return this.$v[Flickable.CONTENTY] }
    $margin(name){ return this.$v[TableView[name.toUpperCase()]] || 0 }

    // the flat index of a cell: down the rows column by column; a transposed table (a horizontal header of a list) runs
    // along the columns row by row
    $modelIndexAtCell(cell){
        return this.$isTransposed ? cell.y * this.$tableSize.w + cell.x : cell.x * this.$tableSize.h + cell.y
    }
    $cellAtModelIndex(flat){
        if(this.$isTransposed){
            let columns = this.$tableSize.w
            return columns ? { x: flat % columns, y: Math.floor(flat / columns) } : { x: 0, y: 0 }
        }
        let rows = this.$tableSize.h
        return rows ? { x: Math.floor(flat / rows), y: flat % rows } : { x: 0, y: 0 }
    }
    $modelIndexToCellIndex(index, visual = true){
        let cell = this.cellAtIndex(index)
        if(!cellValid(cell)) return -1
        return this.$modelIndexAtCell(visual ? cell : { x: index.column, y: index.row })
    }
    $loadedTableItem(cell){ return this.$loadedItems.get(this.$modelIndexAtCell(cell)) }

    $geometry(fx){ let v = fx.item.$v; return new QRectF(v[Item.X], v[Item.Y], v[Item.WIDTH], v[Item.HEIGHT]) }
    $setGeometry(fx, x, y, w, h){
        let item = fx.item
        item.x = x
        item.y = y
        item.width = w
        item.height = h
    }
    // QQuickItemViewFxItem::setVisible: a hidden item is culled (it keeps its visible property)
    $setVisible(fx, visible){
        if(fx.item.$culled === !visible) return
        fx.item.$culled = !visible
        engine.markDirty(fx.item, VISIBILITY)
    }

    // ---- extents (QQuickTableView::minXExtent ...): $flickBounds is -minExtent .. -maxExtent ----
    $flickBounds(axis){
        // the Flickable constructor asks before the fields of the table exist
        if(!this.$origin) return super.$flickBounds(axis)
        if(axis === 'x'){
            let start = this.$margin('leftMargin'), end = this.$margin('rightMargin')
            let content = this.$v[Flickable.CONTENTWIDTH] < 0 ? this.$width() : this.$v[Flickable.CONTENTWIDTH]
            let minExtent = start - this.$origin.x
            let maxExtent = Math.min(minExtent, this.$width() - content - end) - this.$endExtent.w
            return { min: -minExtent, max: Math.max(-minExtent, -maxExtent) }
        }
        let start = this.$margin('topMargin'), end = this.$margin('bottomMargin')
        let content = this.$v[Flickable.CONTENTHEIGHT] < 0 ? this.$height() : this.$v[Flickable.CONTENTHEIGHT]
        let minExtent = start - this.$origin.y
        let maxExtent = Math.min(minExtent, this.$height() - content - end) - this.$endExtent.h
        return { min: -minExtent, max: Math.max(-minExtent, -maxExtent) }
    }
    minXExtent(){ return -this.$flickBounds('x').min }
    maxXExtent(){ return -this.$flickBounds('x').max }
    minYExtent(){ return -this.$flickBounds('y').min }
    maxYExtent(){ return -this.$flickBounds('y').max }
    get horizontalOvershoot(){
        let b = this.$flickBounds('x'), x = this.$contentX()
        return x < b.min ? x - b.min : x > b.max ? x - b.max : 0
    }
    get verticalOvershoot(){
        let b = this.$flickBounds('y'), y = this.$contentY()
        return y < b.min ? y - b.min : y > b.max ? y - b.max : 0
    }
    $isMoving(){ return !!this.$v[Flickable.MOVING] }

    // QQuickFlickable::setContentWidth from TableView itself (not an explicit content width)
    $setContentWidth(w){
        this.$internalContentSize = true
        try { this.$write(Flickable.CONTENTWIDTH, w) } finally { this.$internalContentSize = false }
    }
    $setContentHeight(h){
        this.$internalContentSize = true
        try { this.$write(Flickable.CONTENTHEIGHT, h) } finally { this.$internalContentSize = false }
    }

    $write(id, value){
        let old = this.$v[id]
        if((id === Flickable.CONTENTWIDTH || id === Flickable.CONTENTHEIGHT) && !this.$internalContentSize && this.$tableModel){
            // QQuickTableView::setContentWidth: an explicit content width
            if(id === Flickable.CONTENTWIDTH) this.$explicitContentWidth = value
            else this.$explicitContentHeight = value
        }
        super.$write(id, value)
        if(!this.$tableModel) return
        if((id === Flickable.CONTENTX || id === Flickable.CONTENTY) && old !== this.$v[id]) this.$viewportMoved()
        else if((id === Item.WIDTH || id === Item.HEIGHT) && old !== this.$v[id]) this.$geometryChange()
    }

    // ---- nextVisibleEdgeIndex ----
    $nextVisibleEdgeIndexAroundLoadedTable(edge){
        let start = edge === LEFT ? this.$leftColumn() - 1 : edge === RIGHT ? this.$rightColumn() + 1 : edge === TOP ? this.$topRow() - 1 : this.$bottomRow() + 1
        return this.$nextVisibleEdgeIndex(edge, start)
    }

    $nextVisibleEdgeIndex(edge, startIndex){
        let cache = this.$cachedNextVisibleEdgeIndex[edgeIndex(edge)]
        if(cache.start !== EDGE_NOT_SET){
            let contains
            if(cache.end === EDGE_AT_END) contains = (edge === LEFT || edge === TOP) ? startIndex <= cache.start : startIndex >= cache.start
            else contains = startIndex >= Math.min(cache.start, cache.end) && startIndex <= Math.max(cache.start, cache.end)
            if(contains) return cache.end
        }
        let found
        let test = startIndex
        if(edge === LEFT){
            for(;;){ if(test < 0){ found = EDGE_AT_END; break } if(!this.$isColumnHidden(test)){ found = test; break } test-- }
        } else if(edge === RIGHT){
            for(;;){ if(test > this.$tableSize.w - 1){ found = EDGE_AT_END; break } if(!this.$isColumnHidden(test)){ found = test; break } test++ }
        } else if(edge === TOP){
            for(;;){ if(test < 0){ found = EDGE_AT_END; break } if(!this.$isRowHidden(test)){ found = test; break } test-- }
        } else {
            for(;;){ if(test > this.$tableSize.h - 1){ found = EDGE_AT_END; break } if(!this.$isRowHidden(test)){ found = test; break } test++ }
        }
        cache.start = startIndex
        cache.end = found
        return found
    }
    $atTableEnd(edge, startIndex){
        return (startIndex === undefined ? this.$nextVisibleEdgeIndexAroundLoadedTable(edge) : this.$nextVisibleEdgeIndex(edge, startIndex)) === EDGE_AT_END
    }

    $clearEdgeSizeCache(){
        this.$cachedColumnWidth.start = EDGE_NOT_SET
        this.$cachedRowHeight.start = EDGE_NOT_SET
        for(let c of this.$cachedNextVisibleEdgeIndex) c.start = EDGE_NOT_SET
    }

    // ---- content size and extents ----
    $updateContentWidth(){
        if(this.$syncHorizontally){
            this.$inUpdateContentSize = true
            try { this.$setContentWidth(this.$syncView.$v[Flickable.CONTENTWIDTH]) } finally { this.$inUpdateContentSize = false }
            return
        }
        if(this.$explicitContentWidth !== null) return
        this.$inUpdateContentSize = true
        try {
            if(!this.$loadedItems.size){
                let tm = this.$tableModel
                this.$setContentWidth(tm.model && tm.count() > 0 && tm.delegate ? K_DEFAULT_COLUMN_WIDTH : 0)
                return
            }
            let next = this.$nextVisibleEdgeIndexAroundLoadedTable(RIGHT)
            let remaining = next === EDGE_AT_END ? 0 : this.$tableSize.w - next
            this.$setContentWidth(this.$outer.right + remaining * this.$averageEdgeSize.w + remaining * this.$cellSpacing.w)
        } finally { this.$inUpdateContentSize = false }
    }

    $updateContentHeight(){
        if(this.$syncVertically){
            this.$inUpdateContentSize = true
            try { this.$setContentHeight(this.$syncView.$v[Flickable.CONTENTHEIGHT]) } finally { this.$inUpdateContentSize = false }
            return
        }
        if(this.$explicitContentHeight !== null) return
        this.$inUpdateContentSize = true
        try {
            if(!this.$loadedItems.size){
                let tm = this.$tableModel
                this.$setContentHeight(tm.model && tm.count() > 0 && tm.delegate ? K_DEFAULT_ROW_HEIGHT : 0)
                return
            }
            let next = this.$nextVisibleEdgeIndexAroundLoadedTable(BOTTOM)
            let remaining = next === EDGE_AT_END ? 0 : this.$tableSize.h - next
            this.$setContentHeight(this.$outer.bottom + remaining * this.$averageEdgeSize.h + remaining * this.$cellSpacing.h)
        } finally { this.$inUpdateContentSize = false }
    }

    $moveOuter(dx, dy){
        this.$outer.x += dx; this.$outer.y += dy
        this.$inner.x += dx; this.$inner.y += dy
    }

    $updateExtents(){
        let movedH = false, movedV = false
        let nextLeft = this.$nextVisibleEdgeIndexAroundLoadedTable(LEFT)
        let nextRight = this.$nextVisibleEdgeIndexAroundLoadedTable(RIGHT)
        let nextTop = this.$nextVisibleEdgeIndexAroundLoadedTable(TOP)
        let nextBottom = this.$nextVisibleEdgeIndexAroundLoadedTable(BOTTOM)
        let prevOrigin = { x: this.$origin.x, y: this.$origin.y }, prevEnd = { w: this.$endExtent.w, h: this.$endExtent.h }
        let outer = this.$outer, vp = this.$viewportRect, sp = this.$cellSpacing, avg = this.$averageEdgeSize
        let contentWidth = this.$v[Flickable.CONTENTWIDTH], contentHeight = this.$v[Flickable.CONTENTHEIGHT]
        if(this.$syncHorizontally){
            this.$origin.x = this.$syncView.$origin.x
            this.$endExtent.w = this.$syncView.$endExtent.w
        } else if(nextLeft === EDGE_AT_END){
            if(outer.left > vp.left && outer.left > this.$origin.x){
                this.$moveOuter(-(outer.left - this.$origin.x), 0)
                movedH = true
            }
            this.$origin.x = this.$outer.left
        } else if(outer.left <= this.$origin.x + sp.w){
            let remaining = nextLeft + 1
            this.$origin.x = outer.left - (remaining * avg.w + remaining * sp.w)
        } else if(nextRight === EDGE_AT_END){
            if(outer.right < vp.right){
                let w = Math.min(vp.right, contentWidth + this.$endExtent.w)
                if(outer.right < w){
                    this.$moveOuter(-(outer.right - w), 0)
                    movedH = true
                }
            }
            this.$endExtent.w = this.$outer.right - contentWidth
        } else if(outer.right >= contentWidth + this.$endExtent.w - sp.w){
            let remaining = this.$tableSize.w - nextRight
            this.$endExtent.w = (outer.right - contentWidth) + remaining * avg.w + remaining * sp.w
        }
        if(this.$syncVertically){
            this.$origin.y = this.$syncView.$origin.y
            this.$endExtent.h = this.$syncView.$endExtent.h
        } else if(nextTop === EDGE_AT_END){
            if(this.$outer.top > vp.top && this.$outer.top > this.$origin.y){
                this.$moveOuter(0, -(this.$outer.top - this.$origin.y))
                movedV = true
            }
            this.$origin.y = this.$outer.top
        } else if(this.$outer.top <= this.$origin.y + sp.h){
            let remaining = nextTop + 1
            this.$origin.y = this.$outer.top - (remaining * avg.h + remaining * sp.h)
        } else if(nextBottom === EDGE_AT_END){
            if(this.$outer.bottom < vp.bottom){
                let h = Math.min(vp.bottom, contentHeight + this.$endExtent.h)
                if(this.$outer.bottom < h){
                    this.$moveOuter(0, -(this.$outer.bottom - h))
                    movedV = true
                }
            }
            this.$endExtent.h = this.$outer.bottom - contentHeight
        } else if(this.$outer.bottom >= contentHeight + this.$endExtent.h - sp.h){
            let remaining = this.$tableSize.h - nextBottom
            this.$endExtent.h = (this.$outer.bottom - contentHeight) + remaining * avg.h + remaining * sp.h
        }
        if(movedH || movedV){
            this.$relayoutTableItems()
            for(let child of this.$syncChildren){
                child.$scheduledRebuildOptions |= RO.ViewportOnly
                if(movedH) child.$scheduledRebuildOptions |= RO.CalculateNewTopLeftColumn
                if(movedV) child.$scheduledRebuildOptions |= RO.CalculateNewTopLeftRow
            }
        }
        if(prevOrigin.x !== this.$origin.x || prevOrigin.y !== this.$origin.y || prevEnd.w !== this.$endExtent.w || prevEnd.h !== this.$endExtent.h){
            this.$updateBeginningEnd()
            if(!this.$isMoving()) this.returnToBounds()
        }
    }

    // QQuickFlickablePrivate::updateBeginningEnd and the origin
    $updateBeginningEnd(){
        this.$write(Flickable.ORIGINX, this.$origin.x)
        this.$write(Flickable.ORIGINY, this.$origin.y)
        this.$updateBounds()
    }

    $updateAverageColumnWidth(){
        if(this.$explicitContentWidth !== null){
            this.$averageEdgeSize.w = (this.$explicitContentWidth - (this.$tableSize.w - 1) * this.$cellSpacing.w) / this.$tableSize.w
        } else {
            this.$averageEdgeSize.w = (this.$outer.width - (this.$loadedColumns.count - 1) * this.$cellSpacing.w) / this.$loadedColumns.count
        }
    }
    $updateAverageRowHeight(){
        if(this.$explicitContentHeight !== null){
            this.$averageEdgeSize.h = (this.$explicitContentHeight - (this.$tableSize.h - 1) * this.$cellSpacing.h) / this.$tableSize.h
        } else {
            this.$averageEdgeSize.h = (this.$outer.height - (this.$loadedRows.count - 1) * this.$cellSpacing.h) / this.$loadedRows.count
        }
    }

    $syncLoadedTableRectFromLoadedTable(){
        let tl = this.$geometry(this.$loadedTableItem({ x: this.$leftColumn(), y: this.$topRow() }))
        let br = this.$geometry(this.$loadedTableItem({ x: this.$rightColumn(), y: this.$bottomRow() }))
        this.$outer = new QRectF(tl.x, tl.y, br.right - tl.x, br.bottom - tl.y)
        this.$inner = new QRectF(tl.right, tl.bottom, br.x - tl.right, br.y - tl.bottom)
    }

    $checkForVisibilityChanges(){
        if(!this.$loadedItems.size) return RO.None
        let options = RO.None
        if(this.$outer.x === this.$origin.x && this.$leftColumn() !== 0){
            options |= RO.ViewportOnly | RO.CalculateNewTopLeftColumn
        } else {
            for(let column = this.$leftColumn(); column <= this.$rightColumn(); column++){
                let was = this.$loadedColumns.contains(column), now = !fuzzyIsNull(this.$getColumnWidth(column))
                if(was === now) continue
                options |= RO.ViewportOnly
                if(column === this.$leftColumn()) options |= RO.CalculateNewTopLeftColumn
                break
            }
        }
        if(this.$outer.y === this.$origin.y && this.$topRow() !== 0){
            options |= RO.ViewportOnly | RO.CalculateNewTopLeftRow
        } else {
            for(let row = this.$topRow(); row <= this.$bottomRow(); row++){
                let was = this.$loadedRows.contains(row), now = !fuzzyIsNull(this.$getRowHeight(row))
                if(was === now) continue
                options |= RO.ViewportOnly
                if(row === this.$topRow()) options |= RO.CalculateNewTopLeftRow
                break
            }
        }
        return options
    }

    $forceLayout(immediate){
        this.$clearEdgeSizeCache()
        let options = RO.None
        let actual = this.$calculateTableSize()
        if(this.$tableSize.w !== actual.w || this.$tableSize.h !== actual.h) options |= RO.ViewportOnly
        options |= RO.LayoutOnly | RO.CalculateNewContentWidth | RO.CalculateNewContentHeight | this.$checkForVisibilityChanges()
        this.$scheduleRebuildTable(options)
        if(immediate){
            let root = this.$rootSyncView()
            if(!root.$updateTableRecursive()){
                console.warn('TableView::forceLayout(): Cannot do an immediate re-layout during an ongoing layout!')
                engine.queuePolish(root)
            }
        }
    }

    $syncLoadedTableFromLoadRequest(){
        let r = this.$loadRequest
        if(r.edge === 0){
            this.$loadedColumns.insert(this.$requestCell(0).x)
            this.$loadedRows.insert(this.$requestCell(0).y)
            return
        }
        if(r.edge === LEFT || r.edge === RIGHT) this.$loadedColumns.insert(this.$requestCell(0).x)
        else this.$loadedRows.insert(this.$requestCell(0).y)
    }

    // TableEdgeLoadRequest::cellAt
    $requestCell(i){
        let r = this.$loadRequest
        return !r.edge || r.edge === LEFT || r.edge === RIGHT ? { x: r.edgeIndex, y: r.cells[i] } : { x: r.cells[i], y: r.edgeIndex }
    }

    // ---- items ----
    $createFxTableItem(cell){
        let modelRow = this.$isTransposed ? this.$logicalColumnIndex(cell.y) : this.$logicalRowIndex(cell.y)
        let modelColumn = this.$isTransposed ? this.$logicalRowIndex(cell.x) : this.$logicalColumnIndex(cell.x)
        let flat = this.$modelIndexAtCell({ x: modelColumn, y: modelRow })
        let object = this.$tableModel.object(flat)
        let own = false
        if(!object){
            console.warn('TableView: failed loading index: ' + flat)
            object = new Item(this.$content)
            object.implicitWidth = K_DEFAULT_COLUMN_WIDTH
            object.implicitHeight = K_DEFAULT_ROW_HEIGHT
            own = true
        } else if(hasActiveAnchors(object)){
            console.warn('TableView: detected anchors on delegate with index: ' + flat + '. Use implicitWidth and implicitHeight instead.')
        }
        let fx = { item: object, own, cell, index: flat }
        this.$setVisible(fx, false)
        return fx
    }

    $releaseLoadedItems(reusable){
        let list = Array.from(this.$loadedItems.values())
        this.$loadedItems.clear()
        for(let fx of list) this.$releaseItem(fx, reusable)
    }

    $releaseItem(fx, reusable){
        let item = fx.item
        if(fx.own){
            item.$releaseLater()
            return
        }
        if(this.$tableModel.release(item, reusable) === 'pooled'){
            this.$setVisible(fx, false)
            // QQuickTableViewPrivate::releaseItem: a pooled item with the focus gives the focus back to the view
            if(item.$v[Item.ACTIVEFOCUS]) this.forceActiveFocus()
        }
    }

    $unloadItem(cell){
        let flat = this.$modelIndexAtCell(cell)
        let fx = this.$loadedItems.get(flat)
        this.$loadedItems.delete(flat)
        if(fx) this.$releaseItem(fx, this.$reusable())
        this.$tableModel.commitReleasedItems()
    }

    $reusable(){ return !!this.$v[TableView.REUSEITEMS] }

    $canLoadTableEdge(edge, rect){
        let o = this.$outer, sp = this.$cellSpacing
        if(edge === LEFT) return o.left > rect.left + sp.w
        if(edge === RIGHT) return o.right < rect.right - sp.w
        if(edge === TOP) return o.top > rect.top + sp.h
        return o.bottom < rect.bottom - sp.h
    }

    $canUnloadTableEdge(edge, rect){
        let i = this.$inner
        let ax = this.$positionXAnimation, ay = this.$positionYAnimation
        if(edge === LEFT){
            if(this.$loadedColumns.count <= 1) return false
            if(ax.isRunning() && ax.to < this.$viewportRect.x) return false
            return i.left <= rect.left
        }
        if(edge === RIGHT){
            if(this.$loadedColumns.count <= 1) return false
            if(ax.isRunning() && ax.to > this.$viewportRect.x) return false
            return i.right >= rect.right
        }
        if(edge === TOP){
            if(this.$loadedRows.count <= 1) return false
            if(ay.isRunning() && ay.to < this.$viewportRect.y) return false
            return i.top <= rect.top
        }
        if(this.$loadedRows.count <= 1) return false
        if(ay.isRunning() && ay.to > this.$viewportRect.y) return false
        return i.bottom >= rect.bottom
    }

    $nextEdgeToLoad(rect){
        for(let edge of ALL_EDGES){
            if(!this.$canLoadTableEdge(edge, rect)) continue
            if(this.$nextVisibleEdgeIndexAroundLoadedTable(edge) === EDGE_AT_END) continue
            return edge
        }
        return 0
    }

    $nextEdgeToUnload(rect){
        for(let edge of ALL_EDGES) if(this.$canUnloadTableEdge(edge, rect)) return edge
        return 0
    }

    $cellWidth(cell){ return this.$loadedTableItem(cell).item.$v[Item.IMPLICITWIDTH] || 0 }
    $cellHeight(cell){ return this.$loadedTableItem(cell).item.$v[Item.IMPLICITHEIGHT] || 0 }

    $sizeHintForColumn(column){
        let w = 0
        for(let row of this.$loadedRows) w = Math.max(w, this.$cellWidth({ x: column, y: row }))
        return w
    }
    $sizeHintForRow(row){
        let h = 0
        for(let column of this.$loadedColumns) h = Math.max(h, this.$cellHeight({ x: column, y: row }))
        return h
    }

    $calculateTableSize(){
        let tm = this.$tableModel
        let size = tm.model ? { w: tm.columns(), h: tm.rows() } : { w: 0, h: 0 }
        return this.$isTransposed ? { w: size.h, h: size.w } : size
    }

    $getColumnLayoutWidth(column){
        let explicit = this.$getColumnWidth(column)
        if(explicit >= 0) return explicit
        if(this.$syncHorizontally && this.$syncView.$loadedColumns.contains(column)) return this.$syncView.$getColumnLayoutWidth(column)
        let w = this.$sizeHintForColumn(column)
        if(Number.isNaN(w) || w <= 0){
            if(!this.$layoutWarningIssued){
                this.$layoutWarningIssued = true
                console.warn("TableView: the delegate's implicitWidth needs to be greater than zero")
            }
            w = K_DEFAULT_COLUMN_WIDTH
        }
        return w
    }

    $getRowLayoutHeight(row){
        let explicit = this.$getRowHeight(row)
        if(explicit >= 0) return explicit
        if(this.$syncVertically && this.$syncView.$loadedRows.contains(row)) return this.$syncView.$getRowLayoutHeight(row)
        let h = this.$sizeHintForRow(row)
        if(Number.isNaN(h) || h <= 0){
            if(!this.$layoutWarningIssued){
                this.$layoutWarningIssued = true
                console.warn("TableView: the delegate's implicitHeight needs to be greater than zero")
            }
            h = K_DEFAULT_ROW_HEIGHT
        }
        return h
    }

    $getEffectiveRowY(row){ return this.$geometry(this.$loadedTableItem({ x: this.$leftColumn(), y: row })).y }
    $getEffectiveRowHeight(row){ return this.$geometry(this.$loadedTableItem({ x: this.$leftColumn(), y: row })).height }
    $getEffectiveColumnX(column){ return this.$geometry(this.$loadedTableItem({ x: column, y: this.$topRow() })).x }
    $getEffectiveColumnWidth(column){ return this.$geometry(this.$loadedTableItem({ x: column, y: this.$topRow() })).width }

    $getColumnWidth(column){
        if(this.$cachedColumnWidth.start === this.$logicalColumnIndex(column)) return this.$cachedColumnWidth.size
        if(this.$syncHorizontally) return this.$syncView.$getColumnWidth(column)
        let provider = this.$v[TableView.COLUMNWIDTHPROVIDER]
        if(provider === undefined || provider === null){
            let explicit = this.explicitColumnWidth(column)
            return explicit >= 0 ? explicit : -1
        }
        let w = -1
        if(typeof provider === 'function'){
            w = Number(provider(column))
            if(Number.isNaN(w) || w < 0) w = -1
        } else if(!this.$layoutWarningIssued){
            this.$layoutWarningIssued = true
            console.warn("TableView: columnWidthProvider doesn't contain a function")
        }
        this.$cachedColumnWidth.start = this.$logicalColumnIndex(column)
        this.$cachedColumnWidth.size = w
        return w
    }

    $getRowHeight(row){
        if(this.$cachedRowHeight.start === this.$logicalRowIndex(row)) return this.$cachedRowHeight.size
        if(this.$syncVertically) return this.$syncView.$getRowHeight(row)
        let provider = this.$v[TableView.ROWHEIGHTPROVIDER]
        if(provider === undefined || provider === null){
            let explicit = this.explicitRowHeight(row)
            return explicit >= 0 ? explicit : -1
        }
        let h = -1
        if(typeof provider === 'function'){
            h = Number(provider(row))
            if(Number.isNaN(h) || h < 0) h = -1
        } else if(!this.$layoutWarningIssued){
            this.$layoutWarningIssued = true
            console.warn("TableView: rowHeightProvider doesn't contain a function")
        }
        this.$cachedRowHeight.start = this.$logicalRowIndex(row)
        this.$cachedRowHeight.size = h
        return h
    }

    $getAlignmentContentX(column, alignment, offset, subRect){
        let x = 0
        let columnX = Math.trunc(this.$getEffectiveColumnX(column))
        let vp = this.$viewportRect
        if(subRect && subRect.width > 0 && subRect.height > 0){
            if(alignment === (QtEnums.AlignLeft | QtEnums.AlignRight)) alignment = subRect.width > this.$width() ? QtEnums.AlignLeft : QtEnums.AlignRight
            if(alignment & QtEnums.AlignLeft) x = columnX + subRect.x + offset
            else if(alignment & QtEnums.AlignRight) x = columnX + subRect.x + subRect.width - vp.width + offset
            else if(alignment & QtEnums.AlignHCenter) x = columnX + subRect.x - (vp.width - subRect.width) / 2 + offset
        } else {
            let columnWidth = Math.trunc(this.$getEffectiveColumnWidth(column))
            if(alignment === (QtEnums.AlignLeft | QtEnums.AlignRight)) alignment = columnWidth > this.$width() ? QtEnums.AlignLeft : QtEnums.AlignRight
            if(alignment & QtEnums.AlignLeft) x = columnX + offset
            else if(alignment & QtEnums.AlignRight) x = columnX + columnWidth - vp.width + offset
            else if(alignment & QtEnums.AlignHCenter) x = columnX - (vp.width - columnWidth) / 2 + offset
        }
        let b = this.$flickBounds('x')
        return bound(b.min, x, b.max)
    }

    $getAlignmentContentY(row, alignment, offset, subRect){
        let y = 0
        let rowY = Math.trunc(this.$getEffectiveRowY(row))
        let vp = this.$viewportRect
        if(subRect && subRect.width > 0 && subRect.height > 0){
            if(alignment === (QtEnums.AlignTop | QtEnums.AlignBottom)) alignment = subRect.height > this.$height() ? QtEnums.AlignTop : QtEnums.AlignBottom
            if(alignment & QtEnums.AlignTop) y = rowY + subRect.y + offset
            else if(alignment & QtEnums.AlignBottom) y = rowY + subRect.y + subRect.height - vp.height + offset
            else if(alignment & QtEnums.AlignVCenter) y = rowY + subRect.y - (vp.height - subRect.height) / 2 + offset
        } else {
            let rowHeight = Math.trunc(this.$getEffectiveRowHeight(row))
            if(alignment === (QtEnums.AlignTop | QtEnums.AlignBottom)) alignment = rowHeight > this.$height() ? QtEnums.AlignTop : QtEnums.AlignBottom
            if(alignment & QtEnums.AlignTop) y = rowY + offset
            else if(alignment & QtEnums.AlignBottom) y = rowY + rowHeight - vp.height + offset
            else if(alignment & QtEnums.AlignVCenter) y = rowY - (vp.height - rowHeight) / 2 + offset
        }
        let b = this.$flickBounds('y')
        return bound(b.min, y, b.max)
    }

    $isColumnHidden(column){ return fuzzyIsNull(this.$getColumnWidth(column)) }
    $isRowHidden(row){ return fuzzyIsNull(this.$getRowHeight(row)) }

    $relayoutTableItems(){
        let vp = this.$viewportRect
        if(vp.width <= 0 || vp.height <= 0) return
        let nextColumnX = this.$outer.x, nextRowY = this.$outer.y
        for(let column of this.$loadedColumns){
            let width = this.$getColumnLayoutWidth(column)
            for(let row of this.$loadedRows){
                let fx = this.$loadedTableItem({ x: column, y: row })
                fx.item.x = nextColumnX
                fx.item.width = width
            }
            if(width > 0) nextColumnX += width + this.$cellSpacing.w
        }
        for(let row of this.$loadedRows){
            let height = this.$getRowLayoutHeight(row)
            for(let column of this.$loadedColumns){
                let fx = this.$loadedTableItem({ x: column, y: row })
                fx.item.y = nextRowY
                fx.item.height = height
            }
            if(height > 0) nextRowY += height + this.$cellSpacing.h
        }
    }

    $layoutVerticalEdge(edge){
        let column, neighbour, columnX, columnWidth
        let columns = this.$loadedColumns.values()
        if(edge === LEFT){
            column = this.$leftColumn()
            neighbour = columns[1]
            columnWidth = this.$getColumnLayoutWidth(column)
            columnX = this.$geometry(this.$loadedTableItem({ x: neighbour, y: this.$topRow() })).left - this.$cellSpacing.w - columnWidth
        } else {
            column = this.$rightColumn()
            neighbour = columns[columns.length - 2]
            columnWidth = this.$getColumnLayoutWidth(column)
            columnX = this.$geometry(this.$loadedTableItem({ x: neighbour, y: this.$topRow() })).right + this.$cellSpacing.w
        }
        for(let row of this.$loadedRows){
            let fx = this.$loadedTableItem({ x: column, y: row })
            let n = this.$geometry(this.$loadedTableItem({ x: neighbour, y: row }))
            this.$setGeometry(fx, columnX, n.y, columnWidth, n.height)
            this.$setVisible(fx, true)
        }
    }

    $layoutHorizontalEdge(edge){
        let row, neighbour
        let rows = this.$loadedRows.values()
        if(edge === TOP){
            row = this.$topRow()
            neighbour = rows[1]
        } else {
            row = this.$bottomRow()
            neighbour = rows[rows.length - 2]
        }
        for(let column of this.$loadedColumns){
            let fx = this.$loadedTableItem({ x: column, y: row })
            let n = this.$geometry(this.$loadedTableItem({ x: column, y: neighbour }))
            fx.item.x = n.x
            fx.item.width = n.width
        }
        let rowY, rowHeight = this.$getRowLayoutHeight(row)
        let n = this.$geometry(this.$loadedTableItem({ x: this.$leftColumn(), y: neighbour }))
        rowY = edge === TOP ? n.top - this.$cellSpacing.h - rowHeight : n.bottom + this.$cellSpacing.h
        for(let column of this.$loadedColumns){
            let fx = this.$loadedTableItem({ x: column, y: row })
            fx.item.y = rowY
            fx.item.height = rowHeight
            this.$setVisible(fx, true)
        }
    }

    $layoutTopLeftItem(){
        let cell = this.$requestCell(0)
        let fx = this.$loadedTableItem(cell)
        let start = this.$loadRequest.startPos
        this.$setGeometry(fx, start.x, start.y, this.$getColumnLayoutWidth(cell.x), this.$getRowLayoutHeight(cell.y))
        this.$setVisible(fx, true)
    }

    $layoutTableEdgeFromLoadRequest(){
        let edge = this.$loadRequest.edge
        if(edge === 0) this.$layoutTopLeftItem()
        else if(edge === LEFT || edge === RIGHT) this.$layoutVerticalEdge(edge)
        else this.$layoutHorizontalEdge(edge)
    }

    $processLoadRequest(){
        let r = this.$loadRequest
        while(r.current < r.cells.length){
            let cell = this.$requestCell(r.current)
            let fx = this.$createFxTableItem(cell)
            this.$loadedItems.set(this.$modelIndexAtCell(cell), fx)
            r.current++
        }
        this.$syncLoadedTableFromLoadRequest()
        this.$layoutTableEdgeFromLoadRequest()
        this.$syncLoadedTableRectFromLoadedTable()
        if(this.$rebuildState === RS.Done){
            this.$updateExtents()
            this.$drainReusePoolAfterLoadRequest()
            let signal = r.edge === LEFT ? TableView.SIGNAL_leftColumnChanged : r.edge === RIGHT ? TableView.SIGNAL_rightColumnChanged
                : r.edge === TOP ? TableView.SIGNAL_topRowChanged : r.edge === BOTTOM ? TableView.SIGNAL_bottomRowChanged : -1
            if(signal >= 0) engine.emitArgs(this, signal, [])
            if(this.$editIndex.valid) this.$updateEditItem()
            engine.emitArgs(this, TableView.SIGNAL_layoutChanged, [])
        }
        r.active = false
    }

    $processRebuildTable(){
        if(this.$rebuildState === RS.Begin){
            this.$tableSizeBeforeRebuild = { w: this.$tableSize.w, h: this.$tableSize.h }
            this.$edgesBeforeRebuild = this.$loadedItems.size ? { left: this.leftColumn, top: this.topRow, right: this.rightColumn, bottom: this.bottomRow }
                : { left: -1, top: -1, right: -1, bottom: -1 }
        }
        this.$moveToNextRebuildState()
        if(this.$rebuildState === RS.LoadInitialTable){
            this.$loadInitialTable()
            this.$moveToNextRebuildState()
        }
        if(this.$rebuildState === RS.VerifyTable){
            if(!this.$loadedItems.size){
                this.$updateContentWidth()
                this.$updateContentHeight()
                this.$rebuildState = RS.Done
            } else this.$moveToNextRebuildState()
        }
        if(this.$rebuildState === RS.LayoutTable){
            this.$layoutAfterLoadingInitialTable()
            this.$loadAndUnloadVisibleEdges()
            this.$moveToNextRebuildState()
        }
        if(this.$rebuildState === RS.CancelOvershoot){
            this.$cancelOvershootAfterLayout()
            this.$loadAndUnloadVisibleEdges()
            this.$moveToNextRebuildState()
        }
        if(this.$rebuildState === RS.UpdateContentSize){
            this.$updateContentSize()
            this.$moveToNextRebuildState()
        }
        let preload = (this.$rebuildOptions & RO.All) && this.$reusable()
        if(this.$rebuildState === RS.PreloadColumns){
            if(preload && !this.$atTableEnd(RIGHT)) this.$loadEdge(RIGHT)
            this.$moveToNextRebuildState()
        }
        if(this.$rebuildState === RS.PreloadRows){
            if(preload && !this.$atTableEnd(BOTTOM)) this.$loadEdge(BOTTOM)
            this.$moveToNextRebuildState()
        }
        if(this.$rebuildState === RS.MovePreloadedItemsToPool){
            for(let edge; (edge = this.$nextEdgeToUnload(this.$viewportRect));) this.$unloadEdge(edge)
            this.$moveToNextRebuildState()
        }
        if(this.$rebuildState === RS.Done){
            if(this.$tableSizeBeforeRebuild.w !== this.$tableSize.w) engine.emitArgs(this, TableView.SIGNAL_columnsChanged, [])
            if(this.$tableSizeBeforeRebuild.h !== this.$tableSize.h) engine.emitArgs(this, TableView.SIGNAL_rowsChanged, [])
            let e = this.$edgesBeforeRebuild
            if(e.left !== this.leftColumn) engine.emitArgs(this, TableView.SIGNAL_leftColumnChanged, [])
            if(e.right !== this.rightColumn) engine.emitArgs(this, TableView.SIGNAL_rightColumnChanged, [])
            if(e.top !== this.topRow) engine.emitArgs(this, TableView.SIGNAL_topRowChanged, [])
            if(e.bottom !== this.bottomRow) engine.emitArgs(this, TableView.SIGNAL_bottomRowChanged, [])
            if(this.$editIndex.valid) this.$updateEditItem()
            this.$updateCurrentRowAndColumn()
            this.$tableModel.commitReleasedItems()
            engine.emitArgs(this, TableView.SIGNAL_layoutChanged, [])
        }
    }

    $moveToNextRebuildState(){
        if(this.$loadRequest.active) return false
        if(this.$rebuildState === RS.Begin && (this.$rebuildOptions & RO.LayoutOnly)) this.$rebuildState = RS.LayoutTable
        else this.$rebuildState++
        return true
    }

    $calculateTopLeft(){
        let cell = { x: 0, y: 0 }, pos = { x: 0, y: 0 }
        if(this.$tableSize.w <= 0 || this.$tableSize.h <= 0) return { cell: { x: EDGE_AT_END, y: EDGE_AT_END }, pos }
        if(this.$syncHorizontally || this.$syncVertically){
            let sv = this.$syncView
            if(!sv.$loadedItems.size) return { cell: { x: 0, y: 0 }, pos }
            let svCell = { x: sv.$leftColumn(), y: sv.$topRow() }
            let svPos = sv.$geometry(sv.$loadedTableItem(svCell))
            if(this.$syncHorizontally){
                cell.x = svCell.x
                pos.x = svPos.x
                if(cell.x >= this.$tableSize.w){ cell.x = EDGE_AT_END; pos.x = EDGE_AT_END }
            }
            if(this.$syncVertically){
                cell.y = svCell.y
                pos.y = svPos.y
                if(cell.y >= this.$tableSize.h){ cell.y = EDGE_AT_END; pos.y = EDGE_AT_END }
            }
            if(this.$syncHorizontally && this.$syncVertically) return { cell, pos }
        }
        let opts = this.$rebuildOptions, avg = this.$averageEdgeSize, sp = this.$cellSpacing
        if(!this.$syncHorizontally){
            if(opts & RO.All){
                cell.x = this.$nextVisibleEdgeIndex(RIGHT, 0)
                if(cell.x === EDGE_AT_END) return { cell, pos }
            } else if(opts & RO.CalculateNewTopLeftColumn){
                let newColumn = Math.trunc(this.$viewportRect.x / (avg.w + sp.w))
                cell.x = bound(0, newColumn, this.$tableSize.w - 1)
                pos.x = cell.x * (avg.w + sp.w)
            } else if(opts & RO.PositionViewAtColumn){
                cell.x = bound(0, this.$positionViewAtColumnAfterRebuild, this.$tableSize.w - 1)
                pos.x = Math.floor(cell.x) * (avg.w + sp.w)
            } else {
                cell.x = bound(0, this.$leftColumn(), this.$tableSize.w - 1)
                pos.x = this.$outer.x
            }
        }
        if(!this.$syncVertically){
            if(opts & RO.All){
                cell.y = this.$nextVisibleEdgeIndex(BOTTOM, 0)
                if(cell.y === EDGE_AT_END) return { cell, pos }
            } else if(opts & RO.CalculateNewTopLeftRow){
                let newRow = Math.trunc(this.$viewportRect.y / (avg.h + sp.h))
                cell.y = bound(0, newRow, this.$tableSize.h - 1)
                pos.y = cell.y * (avg.h + sp.h)
            } else if(opts & RO.PositionViewAtRow){
                cell.y = bound(0, this.$positionViewAtRowAfterRebuild, this.$tableSize.h - 1)
                pos.y = Math.floor(cell.y) * (avg.h + sp.h)
            } else {
                cell.y = bound(0, this.$topRow(), this.$tableSize.h - 1)
                pos.y = this.$outer.y
            }
        }
        return { cell, pos }
    }

    $loadInitialTable(){
        this.$tableSize = this.$calculateTableSize()
        if(this.$positionXAnimation.isRunning()){
            this.$positionXAnimation.stop()
            this.$setLocalViewportX(this.$positionXAnimation.to)
            this.$syncViewportRect()
        }
        if(this.$positionYAnimation.isRunning()){
            this.$positionYAnimation.stop()
            this.$setLocalViewportY(this.$positionYAnimation.to)
            this.$syncViewportRect()
        }
        let { cell: topLeft, pos: topLeftPos } = this.$calculateTopLeft()
        if(this.$loadedItems.size){
            if(this.$rebuildOptions & RO.All) this.$releaseLoadedItems(false)
            else if(this.$rebuildOptions & RO.ViewportOnly) this.$releaseLoadedItems(this.$reusable())
        }
        if(this.$rebuildOptions & RO.All){
            this.$origin = { x: 0, y: 0 }
            this.$endExtent = { w: 0, h: 0 }
            this.$updateBeginningEnd()
        }
        this.$loadedColumns.clear()
        this.$loadedRows.clear()
        this.$outer = new QRectF(0, 0, 0, 0)
        this.$inner = new QRectF(0, 0, 0, 0)
        this.$clearEdgeSizeCache()
        if(this.$syncHorizontally) this.$setLocalViewportX(this.$syncView.$contentX())
        if(this.$syncVertically) this.$setLocalViewportY(this.$syncView.$contentY())
        if(!this.$syncHorizontally && (this.$rebuildOptions & RO.PositionViewAtColumn)) this.$setLocalViewportX(topLeftPos.x)
        if(!this.$syncVertically && (this.$rebuildOptions & RO.PositionViewAtRow)) this.$setLocalViewportY(topLeftPos.y)
        this.$syncViewportRect()
        let tm = this.$tableModel
        if(!tm.model || tm.count() === 0 || !tm.delegate) return
        if(topLeft.x === EDGE_AT_END || topLeft.y === EDGE_AT_END) return
        if(topLeft.x === EDGE_NOT_SET || topLeft.y === EDGE_NOT_SET) return
        if(this.$viewportRect.width <= 0 || this.$viewportRect.height <= 0) return
        let r = this.$loadRequest
        r.active = true
        r.edge = 0
        r.edgeIndex = topLeft.x
        r.cells = [topLeft.y]
        r.current = 0
        r.startPos = topLeftPos
        this.$processLoadRequest()
        this.$loadAndUnloadVisibleEdges()
    }

    $updateContentSize(){
        let allColumns = this.$atTableEnd(LEFT) && this.$atTableEnd(RIGHT)
        if((this.$rebuildOptions & RO.CalculateNewContentWidth) || allColumns){
            this.$updateAverageColumnWidth()
            this.$updateContentWidth()
        }
        let allRows = this.$atTableEnd(TOP) && this.$atTableEnd(BOTTOM)
        if((this.$rebuildOptions & RO.CalculateNewContentHeight) || allRows){
            this.$updateAverageRowHeight()
            this.$updateContentHeight()
        }
        this.$updateExtents()
    }

    $layoutAfterLoadingInitialTable(){
        this.$clearEdgeSizeCache()
        this.$relayoutTableItems()
        this.$syncLoadedTableRectFromLoadedTable()
        this.$updateContentSize()
        this.$adjustViewportXAccordingToAlignment()
        this.$adjustViewportYAccordingToAlignment()
    }

    $adjustViewportXAccordingToAlignment(){
        if(!(this.$rebuildOptions & RO.PositionViewAtColumn)) return
        if(this.$positionViewAtColumnAfterRebuild !== this.$leftColumn()) return
        this.$setLocalViewportX(this.$getAlignmentContentX(this.$positionViewAtColumnAfterRebuild, this.$positionViewAtColumnAlignment,
            this.$positionViewAtColumnOffset, this.$positionViewAtColumnSubRect))
        this.$syncViewportRect()
    }

    $adjustViewportYAccordingToAlignment(){
        if(!(this.$rebuildOptions & RO.PositionViewAtRow)) return
        if(this.$positionViewAtRowAfterRebuild !== this.$topRow()) return
        this.$setLocalViewportY(this.$getAlignmentContentY(this.$positionViewAtRowAfterRebuild, this.$positionViewAtRowAlignment,
            this.$positionViewAtRowOffset, this.$positionViewAtRowSubRect))
        this.$syncViewportRect()
    }

    $cancelOvershootAfterLayout(){
        let vertically = (this.$rebuildOptions & RO.PositionViewAtRow) && !this.$syncVertically
        let horizontally = (this.$rebuildOptions & RO.PositionViewAtColumn) && !this.$syncHorizontally
        if(horizontally && !fuzzyIsNull(this.horizontalOvershoot)){
            let b = this.$flickBounds('x')
            this.$setLocalViewportX(this.horizontalOvershoot < 0 ? b.min : b.max)
            this.$syncViewportRect()
        }
        if(vertically && !fuzzyIsNull(this.verticalOvershoot)){
            let b = this.$flickBounds('y')
            this.$setLocalViewportY(this.verticalOvershoot < 0 ? b.min : b.max)
            this.$syncViewportRect()
        }
    }

    $unloadEdge(edge){
        if(edge === LEFT || edge === RIGHT){
            let column = edge === LEFT ? this.$leftColumn() : this.$rightColumn()
            for(let row of this.$loadedRows) this.$unloadItem({ x: column, y: row })
            this.$loadedColumns.remove(column)
        } else {
            let row = edge === TOP ? this.$topRow() : this.$bottomRow()
            for(let column of this.$loadedColumns) this.$unloadItem({ x: column, y: row })
            this.$loadedRows.remove(row)
        }
        this.$syncLoadedTableRectFromLoadedTable()
        if(this.$rebuildState === RS.Done){
            let signal = edge === LEFT ? TableView.SIGNAL_leftColumnChanged : edge === RIGHT ? TableView.SIGNAL_rightColumnChanged
                : edge === TOP ? TableView.SIGNAL_topRowChanged : TableView.SIGNAL_bottomRowChanged
            engine.emitArgs(this, signal, [])
            engine.emitArgs(this, TableView.SIGNAL_layoutChanged, [])
        }
    }

    $loadEdge(edge){
        let index = this.$nextVisibleEdgeIndexAroundLoadedTable(edge)
        let r = this.$loadRequest
        r.active = true
        r.edge = edge
        r.edgeIndex = index
        r.cells = edge === LEFT || edge === RIGHT ? this.$loadedRows.values() : this.$loadedColumns.values()
        r.current = 0
        this.$processLoadRequest()
    }

    $loadAndUnloadVisibleEdges(){
        if(this.$loadRequest.active || !this.$loadedItems.size) return
        let modified
        do {
            modified = false
            let edge = this.$nextEdgeToUnload(this.$viewportRect)
            if(edge){
                modified = true
                this.$unloadEdge(edge)
            }
            edge = this.$nextEdgeToLoad(this.$viewportRect)
            if(edge){
                modified = true
                this.$loadEdge(edge)
            }
        } while(modified)
    }

    $drainReusePoolAfterLoadRequest(){
        if(!this.$reusable()) return
        if(!fuzzyIsNull(this.verticalOvershoot) || !fuzzyIsNull(this.horizontalOvershoot)) return
        let w = this.$loadedColumns.count, h = this.$loadedRows.count
        let minTime = Math.ceil(w > h ? (w + 1) / h : (h + 1) / w)
        this.$tableModel.drainReusableItemsPool(minTime * 2)
    }

    $scheduleRebuildTable(options){
        if(!this.$isComplete()) return
        this.$scheduledRebuildOptions |= options
        engine.queuePolish(this)
    }

    $rootSyncView(){
        let root = this
        while(root.$syncView) root = root.$syncView
        return root
    }

    // QQuickTableViewPrivate::updatePolish
    $polish(){ this.$rootSyncView().$updateTableRecursive() }
    // QQuickTableView::componentFinalized: the table is built before Component.onCompleted
    $componentFinalized(){ this.$polish() }

    $updateTableRecursive(){
        if(this.$polishing) return false
        if(!this.$updateTable()) return false
        for(let child of this.$syncChildren.slice()){
            let mask = RO.PositionViewAtRow | RO.PositionViewAtColumn | RO.CalculateNewTopLeftRow | RO.CalculateNewTopLeftColumn
            child.$scheduledRebuildOptions |= this.$rebuildOptions & ~mask
            if(!child.$updateTableRecursive()) return false
        }
        this.$rebuildOptions = RO.None
        return true
    }

    $updateTable(){
        this.$polishing = true
        try {
            if(this.$loadRequest.active) return false
            if(this.$rebuildState !== RS.Done){
                this.$processRebuildTable()
                return this.$rebuildState === RS.Done
            }
            this.$syncWithPendingChanges()
            if(this.$rebuildState === RS.Begin){
                this.$processRebuildTable()
                return this.$rebuildState === RS.Done
            }
            if(!this.$loadedItems.size) return !this.$loadRequest.active
            this.$loadAndUnloadVisibleEdges()
            this.$updateEditItem()
            return !this.$loadRequest.active
        } finally {
            this.$polishing = false
        }
    }

    // ---- selection and current ----
    $selectedInSelectionModel(cell){
        let sm = this.$selectionModel
        if(!sm || !sm.$model) return false
        return sm.isSelected(this.$modelIndexOfCell(cell))
    }
    $currentInSelectionModel(cell){
        let sm = this.$selectionModel
        if(!sm || !sm.$model) return false
        return sameIndex(sm.currentIndex, this.$modelIndexOfCell(cell))
    }
    $selectionChangedInSelectionModel(selected, deselected){
        if(!this.$inSelectionModelUpdate) this.$cancelSelectionTracking()
        for(let index of selected.indexes()) this.$setSelectedOnDelegateItem(index, true)
        for(let index of deselected.indexes()) this.$setSelectedOnDelegateItem(index, false)
    }
    $setSelectedOnDelegateItem(index, select){
        if(index.valid && index.model !== this.$selectionSourceModel()){
            console.warn('TableView: Cannot select cells: TableView.selectionModel.model is not compatible with the model displayed in the view')
            return
        }
        let cellIndex = this.$modelIndexToCellIndex(index)
        let fx = this.$loadedItems.get(cellIndex)
        if(!fx) return
        this.$setRequired(fx.item, 'selected', select)
    }
    // QQuickTableViewPrivate::selectionSourceModel: the item model of the model assigned (a ListModel's own)
    $selectionSourceModel(){
        if(!this.$needsModelSynchronization && this.$tableModel.model) return this.$tableModel.model
        return adaptModel(this.$assignedModel).model
    }
    // TableView.model as it reads: the value assigned
    $modelValue(){
        return this.$needsModelSynchronization ? this.$assignedModel : this.$tableModel.modelValue === undefined ? this.$assignedModel : this.$tableModel.modelValue
    }
    // qaim(modelImpl()): the model assigned is an item model (a ListModel is a QAbstractListModel), not an array
    $assignedIsItemModel(){
        let value = this.$modelValue()
        return value instanceof AbstractItemModel || !!(value && value.$rows && value.$addView)
    }
    $updateSelectedOnAllDelegateItems(){
        this.$updateCurrentRowAndColumn()
        for(let [cellIndex, fx] of this.$loadedItems){
            let cell = this.$cellAtModelIndex(cellIndex)
            this.$setRequired(fx.item, 'selected', this.$selectedInSelectionModel(cell))
            this.$setRequired(fx.item, 'current', this.$currentInSelectionModel(cell))
            this.$setRequired(fx.item, 'editing', sameIndex(this.$editIndex.index, this.$modelIndexOfCell(cell)) && this.$editIndex.valid)
        }
    }
    $currentChangedInSelectionModel(current, previous){
        if(current.valid && current.model !== this.$selectionSourceModel()){
            console.warn('TableView: Cannot change current index: TableView.selectionModel.model is not compatible with the model displayed in the view')
            return
        }
        this.$updateCurrentRowAndColumn()
        this.$setCurrentOnDelegateItem(previous, false)
        this.$setCurrentOnDelegateItem(current, true)
    }
    $updateCurrentRowAndColumn(){
        let current = this.$selectionModel ? this.$selectionModel.currentIndex : INVALID
        let cell = this.cellAtIndex(current)
        if(cell.x !== this.$currentColumn){
            this.$currentColumn = cell.x
            engine.emitArgs(this, TableView.SIGNAL_currentColumnChanged, [])
        }
        if(cell.y !== this.$currentRow){
            this.$currentRow = cell.y
            engine.emitArgs(this, TableView.SIGNAL_currentRowChanged, [])
        }
    }
    $setCurrentOnDelegateItem(index, isCurrent){
        let fx = this.$loadedItems.get(this.$modelIndexToCellIndex(index))
        if(fx) this.$setRequired(fx.item, 'current', isCurrent)
    }

    // the required properties of TableView on a delegate (setRequiredProperty after init)
    $setRequired(item, name, value){
        let entry = item && item.$tableEntry
        if(entry) entry.ctx.$setTableRequired(name, value)
        else if(item && item.$editCtx) item.$editCtx.$setTableRequired(name, value)
    }

    // QQuickTableViewPrivate::initItemCallback and updateItemProperties(init): from setInitialState of the cell
    $initNewItem(ctx, item){
        if(this.$creatingEditItem) return
        if(!item.$v[Item.Z]) item.z = 1
        let attached = attachedOf(item)
        attached.$write(TableViewAttached.VIEW, this)
        this.$updateItemProperties(ctx.index, item, true, ctx)
    }

    $updateItemProperties(flat, item, init, ctxArg){
        let ctx = ctxArg || (item.$tableEntry && item.$tableEntry.ctx) || item.$editCtx
        if(!ctx) return
        let cell = this.$cellAtModelIndex(flat)
        let visual = { x: this.$visualColumnIndex(cell.x), y: this.$visualRowIndex(cell.y) }
        ctx.$tableItem = item
        ctx.$setTableRequired('tableView', this)
        ctx.$setTableRequired('current', this.$currentInSelectionModel(visual))
        ctx.$setTableRequired('selected', this.$selectedInSelectionModel(visual))
        ctx.$setTableRequired('editing', false)
        ctx.$setTableRequired('containsDrag', false)
    }

    $itemPooled(flat, item){
        let attached = existingAttached(item)
        if(attached) engine.emitArgs(attached, TableViewAttached.SIGNAL_pooled, [])
    }
    $itemReused(flat, item){
        item.$culled = false
        engine.markDirty(item, VISIBILITY)
        let attached = existingAttached(item)
        if(attached) engine.emitArgs(attached, TableViewAttached.SIGNAL_reused, [])
    }

    // ---- syncing ----
    $syncWithPendingChanges(){
        this.$syncViewportRect()
        this.$syncModel()
        this.$syncDelegate()
        this.$syncSyncView()
        this.$syncPositionView()
        this.$syncRebuildOptions()
    }

    $syncRebuildOptions(){
        if(!this.$scheduledRebuildOptions) return
        this.$rebuildState = RS.Begin
        this.$rebuildOptions = this.$scheduledRebuildOptions
        this.$scheduledRebuildOptions = RO.None
        if(!this.$loadedItems.size) this.$rebuildOptions |= RO.All
        if(this.$rebuildOptions & RO.All){
            this.$rebuildOptions &= ~(RO.ViewportOnly | RO.LayoutOnly)
            this.$rebuildOptions |= RO.CalculateNewContentWidth | RO.CalculateNewContentHeight
        } else if(this.$rebuildOptions & RO.ViewportOnly){
            this.$rebuildOptions &= ~RO.LayoutOnly
        }
        if(this.$rebuildOptions & RO.PositionViewAtRow) this.$rebuildOptions &= ~RO.CalculateNewTopLeftRow
        if(this.$rebuildOptions & RO.PositionViewAtColumn) this.$rebuildOptions &= ~RO.CalculateNewTopLeftColumn
    }

    $syncDelegate(){
        let delegate = this.$v[TableView.DELEGATE] || null
        if(this.$tableModel.delegate === delegate) return
        if(this.$delegateLink){
            engine.disconnect(this.$delegateLink[0], this.$delegateLink[1], this.$delegateLink[2])
            this.$delegateLink = null
        }
        this.$tableModel.delegate = delegate
        // a DelegateChooser whose choices change: the view lays its delegates out again
        const { DelegateChooser } = require('../QtQml/Models/DelegateChooser')
        if(delegate instanceof DelegateChooser){
            let fn = () => this.$scheduleRebuildTable(RO.All)
            engine.connect(delegate, DelegateChooser.SIGNAL_delegateChanged, fn)
            this.$delegateLink = [delegate, DelegateChooser.SIGNAL_delegateChanged, fn]
        }
    }

    $syncModel(){
        if(!this.$needsModelSynchronization && this.$tableModel.model) return
        if(!this.$needsModelSynchronization && this.$tableModel.modelValue === this.$assignedModel) return
        if(this.$tableModel.model){
            this.$disconnectFromModel()
            this.$releaseLoadedItems(false)
        }
        this.$tableModel.setModel(this.$assignedModel)
        this.$needsModelSynchronization = false
        this.$connectToModel()
    }

    $syncSyncView(){
        let assigned = this.$v[TableView.SYNCVIEW] || null
        if(assigned !== this.$syncView){
            if(this.$syncView){
                let i = this.$syncView.$syncChildren.indexOf(this)
                if(i >= 0) this.$syncView.$syncChildren.splice(i, 1)
            }
            if(assigned){
                for(let view = assigned; view; view = view.$syncView){
                    if(view === this){
                        if(!this.$layoutWarningIssued){
                            this.$layoutWarningIssued = true
                            console.warn('TableView: recursive syncView connection detected!')
                        }
                        this.$syncView = null
                        return
                    }
                }
                assigned.$syncChildren.push(this)
                this.$scheduledRebuildOptions |= RO.ViewportOnly
            }
            this.$syncView = assigned
        }
        let direction = this.$v[TableView.SYNCDIRECTION]
        this.$syncHorizontally = !!this.$syncView && !!(direction & QtEnums.Horizontal)
        this.$syncVertically = !!this.$syncView && !!(direction & QtEnums.Vertical)
        let sv = this.$syncView
        if(this.$syncHorizontally){
            this.$inUpdateContentSize = true
            try {
                this.columnSpacing = sv.columnSpacing
                this.leftMargin = sv.leftMargin
                this.rightMargin = sv.rightMargin
                this.$updateContentWidth()
            } finally { this.$inUpdateContentSize = false }
            if(this.$scheduledRebuildOptions & RO.LayoutOnly){
                if(sv.leftColumn !== this.leftColumn || sv.$outer.left !== this.$outer.left){
                    this.$scheduledRebuildOptions |= RO.CalculateNewTopLeftColumn | RO.ViewportOnly
                }
            }
        }
        if(this.$syncVertically){
            this.$inUpdateContentSize = true
            try {
                this.rowSpacing = sv.rowSpacing
                this.topMargin = sv.topMargin
                this.bottomMargin = sv.bottomMargin
                this.$updateContentHeight()
            } finally { this.$inUpdateContentSize = false }
            if(this.$scheduledRebuildOptions & RO.LayoutOnly){
                if(sv.topRow !== this.topRow || sv.$outer.top !== this.$outer.top){
                    this.$scheduledRebuildOptions |= RO.CalculateNewTopLeftRow | RO.ViewportOnly
                }
            }
        }
        if(sv && !this.$loadedItems.size && this.$tableSize.w > 0 && this.$tableSize.h > 0 && sv.$loadedItems.size){
            if(this.$syncHorizontally && sv.$leftColumn() <= this.$tableSize.w - 1) this.$scheduledRebuildOptions |= RO.ViewportOnly
            else if(this.$syncVertically && sv.$topRow() <= this.$tableSize.h - 1) this.$scheduledRebuildOptions |= RO.ViewportOnly
        }
    }

    $syncPositionView(){
        this.$positionViewAtRowAfterRebuild = this.$assignedPositionViewAtRowAfterRebuild
        this.$positionViewAtColumnAfterRebuild = this.$assignedPositionViewAtColumnAfterRebuild
    }

    $connectToModel(){
        let model = this.$tableModel.model
        if(!model) return
        this.$modelLinks = []
        let link = (signal, fn) => {
            engine.connect(model, signal, fn)
            this.$modelLinks.push([model, signal, fn])
        }
        let M = AbstractItemModel
        let root = (parent) => !(parent && parent.valid)
        link(M.SIGNAL_rowsMoved, (parent) => { if(root(parent)) this.$scheduleRebuildTable(RO.ViewportOnly) })
        link(M.SIGNAL_columnsMoved, (parent) => { if(root(parent)) this.$scheduleRebuildTable(RO.ViewportOnly) })
        link(M.SIGNAL_rowsInserted, (parent) => { if(root(parent)) this.$scheduleRebuildTable(RO.ViewportOnly | RO.CalculateNewContentHeight) })
        link(M.SIGNAL_rowsRemoved, (parent) => {
            if(!root(parent)) return
            if(!this.$editIndex.valid && this.$editItem) this.closeEditor()
            this.$scheduleRebuildTable(RO.ViewportOnly | RO.CalculateNewContentHeight)
        })
        link(M.SIGNAL_columnsInserted, (parent) => { if(root(parent)) this.$scheduleRebuildTable(RO.ViewportOnly | RO.CalculateNewContentWidth) })
        link(M.SIGNAL_columnsRemoved, (parent) => {
            if(!root(parent)) return
            if(!this.$editIndex.valid && this.$editItem) this.closeEditor()
            this.$scheduleRebuildTable(RO.ViewportOnly | RO.CalculateNewContentWidth)
        })
        link(M.SIGNAL_modelReset, () => {
            this.closeEditor()
            this.$scheduleRebuildTable(RO.All)
        })
        link(M.SIGNAL_layoutChanged, () => this.$scheduleRebuildTable(RO.ViewportOnly))
        link(M.SIGNAL_dataChanged, (topLeft, bottomRight, roles) => this.$dataChangedCallback(topLeft, bottomRight, roles))
    }

    $disconnectFromModel(){
        for(let [object, signal, fn] of this.$modelLinks || []) engine.disconnect(object, signal, fn)
        this.$modelLinks = []
    }

    // a DelegateChooser picks the delegates again when the role it chooses by changes in the view
    $dataChangedCallback(topLeft, bottomRight, roles){
        const { DelegateChooser } = require('../QtQml/Models/DelegateChooser')
        let chooser = this.$v[TableView.DELEGATE]
        if(!(chooser instanceof DelegateChooser)) return
        if(!this.$loadedItems.size || topLeft.column > this.$rightColumn() || bottomRight.column < this.$leftColumn()
            || topLeft.row > this.$bottomRow() || bottomRight.row < this.$topRow()) return
        if(roles && roles.length){
            let role = topLeft.model.$roleOf(chooser.$v[DelegateChooser.ROLE])
            if(roles.indexOf(role) < 0) return
        }
        this.$scheduleRebuildTable(RO.ViewportOnly)
    }

    // ---- positioning ----
    $positionViewAtRow(row, alignment, offset, subRect){
        let vertical = alignment & (QtEnums.AlignTop | QtEnums.AlignVCenter | QtEnums.AlignBottom)
        if(this.$syncVertically){
            this.$syncView.$positionViewAtRow(row, vertical, offset, subRect)
        } else if(!this.$scrollToRow(row, vertical, offset, subRect)){
            this.$assignedPositionViewAtRowAfterRebuild = row
            this.$positionViewAtRowAlignment = vertical
            this.$positionViewAtRowOffset = offset
            this.$positionViewAtRowSubRect = subRect
            this.$scheduleRebuildTable(RO.ViewportOnly | RO.PositionViewAtRow)
        }
    }

    $positionViewAtColumn(column, alignment, offset, subRect){
        let horizontal = alignment & (QtEnums.AlignLeft | QtEnums.AlignHCenter | QtEnums.AlignRight)
        if(this.$syncHorizontally){
            this.$syncView.$positionViewAtColumn(column, horizontal, offset, subRect)
        } else if(!this.$scrollToColumn(column, horizontal, offset, subRect)){
            this.$assignedPositionViewAtColumnAfterRebuild = column
            this.$positionViewAtColumnAlignment = horizontal
            this.$positionViewAtColumnOffset = offset
            this.$positionViewAtColumnSubRect = subRect
            this.$scheduleRebuildTable(RO.ViewportOnly | RO.PositionViewAtColumn)
        }
    }

    $scrollToRow(row, alignment, offset, subRect){
        if(row < this.$topRow()){
            if(row !== this.$nextVisibleEdgeIndex(TOP, this.$topRow() - 1)) return false
            this.$loadEdge(TOP)
        } else if(row > this.$bottomRow()){
            if(row !== this.$nextVisibleEdgeIndex(BOTTOM, this.$bottomRow() + 1)) return false
            this.$loadEdge(BOTTOM)
        }
        if(!this.$loadedRows.contains(row)) return false
        let y = this.$getAlignmentContentY(row, alignment, offset, subRect)
        if(fuzzyCompare(y, this.$contentY()) || y === this.$contentY()) return true
        if(this.$v[TableView.ANIMATE]){
            let a = this.$positionYAnimation
            a.to = y
            a.duration = bound(700, Math.abs(y - this.$contentY()) * 5, 1500)
            a.restart()
        } else {
            this.$positionYAnimation.stop()
            this.contentY = y
        }
        return true
    }

    $scrollToColumn(column, alignment, offset, subRect){
        if(column < this.$leftColumn()){
            if(column !== this.$nextVisibleEdgeIndex(LEFT, this.$leftColumn() - 1)) return false
            this.$loadEdge(LEFT)
        } else if(column > this.$rightColumn()){
            if(column !== this.$nextVisibleEdgeIndex(RIGHT, this.$rightColumn() + 1)) return false
            this.$loadEdge(RIGHT)
        }
        if(!this.$loadedColumns.contains(column)) return false
        let x = this.$getAlignmentContentX(column, alignment, offset, subRect)
        if(fuzzyCompare(x, this.$contentX()) || x === this.$contentX()) return true
        if(this.$v[TableView.ANIMATE]){
            let a = this.$positionXAnimation
            a.to = x
            a.duration = bound(700, Math.abs(x - this.$contentX()) * 5, 1500)
            a.restart()
        } else {
            this.$positionXAnimation.stop()
            this.contentX = x
        }
        return true
    }

    $scheduleRebuildIfFastFlick(){
        let vp = this.$viewportRect
        // the viewport moved past what is loaded: start from a new top-left cell
        if(!(this.$contentY() < vp.bottom && this.$contentY() + this.$height() > vp.y)){
            this.$scheduledRebuildOptions |= RO.CalculateNewTopLeftRow | RO.ViewportOnly
        }
        if(!(this.$contentX() < vp.right && this.$contentX() + this.$width() > vp.x)){
            this.$scheduledRebuildOptions |= RO.CalculateNewTopLeftColumn | RO.ViewportOnly
        }
    }

    $setLocalViewportX(x){
        this.$inSetLocalViewportPos = true
        try { if(!fuzzyCompare(x, this.$contentX()) && x !== this.$contentX()) this.contentX = x } finally { this.$inSetLocalViewportPos = false }
    }
    $setLocalViewportY(y){
        this.$inSetLocalViewportPos = true
        try { if(!fuzzyCompare(y, this.$contentY()) && y !== this.$contentY()) this.contentY = y } finally { this.$inSetLocalViewportPos = false }
    }

    $syncViewportRect(){
        let w = this.$width(), h = this.$height()
        for(let child of this.$syncChildren){
            if(child.$syncHorizontally) w = Math.max(w, child.$width())
            if(child.$syncVertically) h = Math.max(h, child.$height())
        }
        this.$viewportRect = new QRectF(this.$contentX(), this.$contentY(), w, h)
    }

    $syncViewportPosRecursive(){
        this.$inSyncViewportPosRecursive = true
        try {
            let sv = this.$syncView
            if(sv && !sv.$inSyncViewportPosRecursive){
                if(this.$syncHorizontally) sv.$setLocalViewportX(this.$contentX())
                if(this.$syncVertically) sv.$setLocalViewportY(this.$contentY())
                sv.$syncViewportPosRecursive()
            }
            for(let child of this.$syncChildren){
                if(child.$inSyncViewportPosRecursive) continue
                if(child.$syncHorizontally) child.$setLocalViewportX(this.$contentX())
                if(child.$syncVertically) child.$setLocalViewportY(this.$contentY())
                child.$syncViewportPosRecursive()
            }
        } finally { this.$inSyncViewportPosRecursive = false }
    }

    // QQuickTableView::viewportMoved
    $viewportMoved(){
        if(this.$inSetLocalViewportPos) return
        this.$syncViewportPosRecursive()
        let root = this.$rootSyncView()
        root.$scheduleRebuildIfFastFlick()
        if(!root.$polishQueued){
            if(root.$scheduledRebuildOptions) engine.queuePolish(root)
            else if(!root.$updateTableRecursive()) engine.queuePolish(root)
        }
    }

    // QQuickTableView::geometryChange
    $geometryChange(){
        if(!this.$tableModel) return
        this.$tableModel.drainReusableItemsPool(0)
        this.$forceLayout(false)
    }

    // ---- section mapping (moveColumn / moveRow) ----
    $initializeIndexMapping(){
        let init = (visual, logical, size) => {
            visual.length = 0
            logical.length = 0
            for(let i = 0; i < size; i++){
                visual.push({ index: i, prevIndex: -1 })
                logical.push({ index: i, prevIndex: -1 })
            }
        }
        if(this.$horizontalVisualIndices.length !== this.$tableSize.w || this.$horizontalLogicalIndices.length !== this.$tableSize.w){
            init(this.$horizontalVisualIndices, this.$horizontalLogicalIndices, this.$tableSize.w)
        }
        if(this.$verticalVisualIndices.length !== this.$tableSize.h || this.$verticalLogicalIndices.length !== this.$tableSize.h){
            init(this.$verticalVisualIndices, this.$verticalLogicalIndices, this.$tableSize.h)
        }
    }
    $clearIndexMapping(){
        this.$horizontalLogicalIndices = []
        this.$horizontalVisualIndices = []
        this.$verticalLogicalIndices = []
        this.$verticalVisualIndices = []
    }
    $logicalRowIndex(visual){
        if(this.$syncView) return this.$syncView.$logicalRowIndex(visual)
        if(!this.$verticalLogicalIndices.length || visual < 0) return visual
        return this.$verticalLogicalIndices[visual].index
    }
    $logicalColumnIndex(visual){
        if(this.$syncView) return this.$syncView.$logicalColumnIndex(visual)
        if(!this.$horizontalLogicalIndices.length || visual < 0) return visual
        return this.$horizontalLogicalIndices[visual].index
    }
    $visualRowIndex(logical){
        if(this.$syncView) return this.$syncView.$visualRowIndex(logical)
        if(!this.$verticalVisualIndices.length || logical < 0) return logical
        return this.$verticalVisualIndices[logical].index
    }
    $visualColumnIndex(logical){
        if(this.$syncView) return this.$syncView.$visualColumnIndex(logical)
        if(!this.$horizontalVisualIndices.length || logical < 0) return logical
        return this.$horizontalVisualIndices[logical].index
    }

    $moveSection(source, destination, horizontal){
        let size = horizontal ? this.$tableSize.w : this.$tableSize.h
        if(source < 0 || destination < 0 || source >= size || destination >= size || source === destination) return
        if(this.$sectionMoving) return
        this.$sectionMoving = true
        if(this.$syncView){
            this.$syncView.$moveSection(source, destination, horizontal)
        } else {
            this.$initializeIndexMapping()
            let visualIndices = horizontal ? this.$horizontalVisualIndices : this.$verticalVisualIndices
            let logicalIndices = horizontal ? this.$horizontalLogicalIndices : this.$verticalLogicalIndices
            let logical = logicalIndices[source].index
            let visual = source
            if(destination > source){
                while(visual < destination){
                    let visualData = visualIndices[logicalIndices[visual + 1].index]
                    let logicalData = logicalIndices[visual]
                    visualData.prevIndex = visualData.index
                    visualData.index = visual
                    logicalData.prevIndex = logicalData.index
                    logicalData.index = logicalIndices[visual + 1].index
                    visual++
                }
            } else {
                while(visual > destination){
                    let visualData = visualIndices[logicalIndices[visual - 1].index]
                    let logicalData = logicalIndices[visual]
                    visualData.prevIndex = visualData.index
                    visualData.index = visual
                    logicalData.prevIndex = logicalData.index
                    logicalData.index = logicalIndices[visual - 1].index
                    visual--
                }
            }
            visualIndices[logical].prevIndex = visualIndices[logical].index
            visualIndices[logical].index = destination
            logicalIndices[destination].prevIndex = logicalIndices[destination].index
            logicalIndices[destination].index = logical
            for(let child of this.$syncChildren){
                if(!child.$sectionMoving && ((child.$syncHorizontally && horizontal) || (child.$syncVertically && !horizontal))) child.$moveSection(source, destination, horizontal)
            }
        }
        this.$scheduleRebuildTable(RO.ViewportOnly)
        this.$sectionMoving = false
        let start = Math.min(source, destination), end = Math.max(source, destination)
        let owner = this.$syncView || this
        let logicalData = horizontal ? owner.$horizontalLogicalIndices : owner.$verticalLogicalIndices
        let visualData = horizontal ? owner.$horizontalVisualIndices : owner.$verticalVisualIndices
        for(let index = start; index <= end; index++){
            let prevLogical = logicalData[index].prevIndex
            engine.emitArgs(this, horizontal ? TableView.SIGNAL_columnMoved : TableView.SIGNAL_rowMoved,
                [prevLogical, visualData[prevLogical].prevIndex, visualData[prevLogical].index])
        }
    }

    // QQuickTableViewPrivate::clearSection: the mapping goes; the loop of Qt that would tell the moves walks the vector it
    // just cleared (a reference to it), so no columnMoved / rowMoved is emitted
    $clearSection(horizontal){
        if(this.$syncView){
            this.$syncView.$clearSection(horizontal)
            return
        }
        if(horizontal){ this.$horizontalLogicalIndices = []; this.$horizontalVisualIndices = [] }
        else { this.$verticalLogicalIndices = []; this.$verticalVisualIndices = [] }
        this.$scheduleRebuildTable(RO.ViewportOnly)
    }

    // ---- the public functions (Q_INVOKABLE) ----
    forceLayout(){ this.$forceLayout(true) }

    get leftColumn(){ engine.track(this, TableView.SIGNAL_leftColumnChanged); return this.$loadedItems.size ? this.$leftColumn() : -1 }
    get rightColumn(){ engine.track(this, TableView.SIGNAL_rightColumnChanged); return this.$loadedItems.size ? this.$rightColumn() : -1 }
    get topRow(){ engine.track(this, TableView.SIGNAL_topRowChanged); return this.$loadedItems.size ? this.$topRow() : -1 }
    get bottomRow(){ engine.track(this, TableView.SIGNAL_bottomRowChanged); return this.$loadedItems.size ? this.$bottomRow() : -1 }
    get rows(){ engine.track(this, TableView.SIGNAL_rowsChanged); return this.$tableSize.h }
    get columns(){ engine.track(this, TableView.SIGNAL_columnsChanged); return this.$tableSize.w }
    get currentRow(){ engine.track(this, TableView.SIGNAL_currentRowChanged); return this.$currentRow }
    get currentColumn(){ engine.track(this, TableView.SIGNAL_currentColumnChanged); return this.$currentColumn }

    positionViewAtRow(row, mode, offset = 0, subRect = null){
        if(row < 0 || row >= this.$tableSize.h || this.$loadedRows.isEmpty()) return
        subRect = validRect(subRect)
        let o = this.$outer, i = this.$inner, vp = this.$viewportRect
        let A = QtEnums
        if(mode & (A.AlignTop | A.AlignBottom | A.AlignVCenter)){
            this.$positionViewAtRow(row, mode & (A.AlignTop | A.AlignBottom | A.AlignVCenter), offset, subRect)
        } else if(mode === ENUMS.Contain){
            if(row < this.topRow) this.$positionViewAtRow(row, A.AlignTop, offset, subRect)
            else if(row > this.bottomRow) this.$positionViewAtRow(row, A.AlignTop | A.AlignBottom, offset, subRect)
            else if(row === this.topRow){
                if(!subRect) this.$positionViewAtRow(row, A.AlignTop, offset, subRect)
                else {
                    let top = o.top + subRect.y, bottom = o.top + subRect.y + subRect.height
                    if(top < vp.y) this.$positionViewAtRow(row, A.AlignTop, offset, subRect)
                    else if(bottom > vp.bottom) this.$positionViewAtRow(row, A.AlignTop | A.AlignBottom, offset, subRect)
                }
            } else if(row === this.bottomRow){
                if(!subRect) this.$positionViewAtRow(row, A.AlignTop | A.AlignBottom, offset, subRect)
                else if(i.bottom + subRect.y + subRect.height > vp.bottom) this.$positionViewAtRow(row, A.AlignTop | A.AlignBottom, offset, subRect)
            }
        } else if(mode === ENUMS.Visible){
            if(row < this.topRow) this.$positionViewAtRow(row, A.AlignTop, -offset, subRect)
            else if(row > this.bottomRow) this.$positionViewAtRow(row, A.AlignTop | A.AlignBottom, offset, subRect)
            else if(subRect){
                if(row === this.topRow){
                    let top = o.top + subRect.y, bottom = o.top + subRect.y + subRect.height
                    if(bottom < vp.top) this.$positionViewAtRow(row, A.AlignTop, offset, subRect)
                    else if(top > vp.bottom) this.$positionViewAtRow(row, A.AlignTop | A.AlignBottom, offset, subRect)
                } else if(row === this.bottomRow){
                    if(i.bottom + subRect.y > vp.bottom) this.$positionViewAtRow(row, A.AlignTop | A.AlignBottom, offset, subRect)
                }
            }
        } else {
            console.warn('TableView: Unsupported mode:' + mode)
        }
    }

    positionViewAtColumn(column, mode, offset = 0, subRect = null){
        if(column < 0 || column >= this.$tableSize.w || this.$loadedColumns.isEmpty()) return
        subRect = validRect(subRect)
        let o = this.$outer, i = this.$inner, vp = this.$viewportRect
        let A = QtEnums
        if(mode & (A.AlignLeft | A.AlignRight | A.AlignHCenter)){
            this.$positionViewAtColumn(column, mode & (A.AlignLeft | A.AlignRight | A.AlignHCenter), offset, subRect)
        } else if(mode === ENUMS.Contain){
            if(column < this.leftColumn) this.$positionViewAtColumn(column, A.AlignLeft, offset, subRect)
            else if(column > this.rightColumn) this.$positionViewAtColumn(column, A.AlignLeft | A.AlignRight, offset, subRect)
            else if(column === this.leftColumn){
                if(!subRect) this.$positionViewAtColumn(column, A.AlignLeft, offset, subRect)
                else {
                    let left = o.left + subRect.x, right = o.left + subRect.x + subRect.width
                    if(left < vp.left) this.$positionViewAtColumn(column, A.AlignLeft, offset, subRect)
                    else if(right > vp.right) this.$positionViewAtColumn(column, A.AlignLeft | A.AlignRight, offset, subRect)
                }
            } else if(column === this.rightColumn){
                if(!subRect) this.$positionViewAtColumn(column, A.AlignLeft | A.AlignRight, offset, subRect)
                else if(i.right + subRect.x + subRect.width > vp.right) this.$positionViewAtColumn(column, A.AlignLeft | A.AlignRight, offset, subRect)
            }
        } else if(mode === ENUMS.Visible){
            if(column < this.leftColumn) this.$positionViewAtColumn(column, A.AlignLeft, -offset, subRect)
            else if(column > this.rightColumn) this.$positionViewAtColumn(column, A.AlignLeft | A.AlignRight, offset, subRect)
            else if(subRect){
                if(column === this.leftColumn){
                    let left = o.left + subRect.x, right = o.left + subRect.x + subRect.width
                    if(right < vp.left) this.$positionViewAtColumn(column, A.AlignLeft, offset, subRect)
                    else if(left > vp.right) this.$positionViewAtColumn(column, A.AlignLeft | A.AlignRight, offset, subRect)
                } else if(column === this.rightColumn){
                    if(i.right + subRect.x > vp.right) this.$positionViewAtColumn(column, A.AlignLeft | A.AlignRight, offset, subRect)
                }
            }
        } else {
            console.warn('TableView: Unsupported mode:' + mode)
        }
    }

    // positionViewAtCell(cell, mode, offset, subRect); the deprecated positionViewAtCell(column, row, mode, offset, subRect)
    positionViewAtCell(cell, mode, offset, subRect){
        if(typeof cell === 'number'){
            let column = cell, row = mode
            return this.positionViewAtCell(new QPointF(column, row), offset, subRect, arguments[4])
        }
        let A = QtEnums
        let horizontalMode = mode & ~(A.AlignTop | A.AlignBottom | A.AlignVCenter)
        let verticalMode = mode & ~(A.AlignLeft | A.AlignRight | A.AlignHCenter)
        if(!horizontalMode && !verticalMode){
            console.warn('TableView: Unsupported mode:' + mode)
            return
        }
        offset = offset || { x: 0, y: 0 }
        if(horizontalMode) this.positionViewAtColumn(cell.x, horizontalMode, offset.x, subRect)
        if(verticalMode) this.positionViewAtRow(cell.y, verticalMode, offset.y, subRect)
    }

    positionViewAtIndex(index, mode, offset, subRect){
        let A = QtEnums
        let horizontalMode = mode & ~(A.AlignTop | A.AlignBottom | A.AlignVCenter)
        let verticalMode = mode & ~(A.AlignLeft | A.AlignRight | A.AlignHCenter)
        if(!horizontalMode && !verticalMode){
            console.warn('TableView: Unsupported mode:' + mode)
            return
        }
        offset = offset || { x: 0, y: 0 }
        if(horizontalMode) this.positionViewAtColumn(this.columnAtIndex(index), horizontalMode, offset.x, subRect)
        if(verticalMode) this.positionViewAtRow(this.rowAtIndex(index), verticalMode, offset.y, subRect)
    }

    moveColumn(source, destination){ this.$moveSection(source, destination, true) }
    moveRow(source, destination){ this.$moveSection(source, destination, false) }
    clearColumnReordering(){ this.$clearSection(true) }
    clearRowReordering(){ this.$clearSection(false) }

    // itemAtCell(cell); the deprecated itemAtCell(column, row)
    itemAtCell(cell, row){
        if(typeof cell === 'number') cell = { x: cell, y: row }
        let fx = this.$loadedItems.get(this.$modelIndexAtCell(cell))
        return fx ? fx.item : null
    }
    itemAtIndex(index){
        let fx = this.$loadedItems.get(this.$modelIndexToCellIndex(index))
        return fx ? fx.item : null
    }

    // cellAtPosition(position, includeSpacing) / cellAtPosition(x, y, includeSpacing): content item coordinates
    cellAtPosition(position, y, includeSpacing){
        if(typeof position === 'number'){
            position = { x: position, y }
        } else includeSpacing = y
        includeSpacing = !!includeSpacing
        let o = this.$outer
        if(!(position.x >= o.left && position.x < o.right && position.y >= o.top && position.y < o.bottom)) return new QPoint(-1, -1)
        let hSpace = this.$cellSpacing.w, vSpace = this.$cellSpacing.h
        let columnEnd = o.x, rowEnd = o.y
        let foundColumn = -1, foundRow = -1
        for(let column of this.$loadedColumns){
            columnEnd += this.$getEffectiveColumnWidth(column)
            if(position.x < columnEnd){ foundColumn = column; break }
            columnEnd += hSpace
            if(!includeSpacing && position.x < columnEnd) return new QPoint(-1, -1)
            else if(includeSpacing && position.x < columnEnd - hSpace / 2){ foundColumn = column; break }
        }
        for(let row of this.$loadedRows){
            rowEnd += this.$getEffectiveRowHeight(row)
            if(position.y < rowEnd){ foundRow = row; break }
            rowEnd += vSpace
            if(!includeSpacing && position.y < rowEnd) return new QPoint(-1, -1)
            if(includeSpacing && position.y < rowEnd - vSpace / 2){ foundRow = row; break }
        }
        return new QPoint(foundColumn, foundRow)
    }
    // cellAtPos: a position in the view (deprecated)
    cellAtPos(x, y, includeSpacing){
        if(typeof x === 'object'){ includeSpacing = y; y = x.y; x = x.x }
        return this.cellAtPosition({ x: x + this.$contentX() - this.$content.$v[Item.X] - this.$contentX(), y: y + this.$contentY() - this.$content.$v[Item.Y] - this.$contentY() }, includeSpacing)
    }

    isColumnLoaded(column){
        if(!this.$loadedColumns.contains(column)) return false
        if(this.$rebuildState !== RS.Done && this.$rebuildState < RS.LayoutTable) return false
        return true
    }
    isRowLoaded(row){
        if(!this.$loadedRows.contains(row)) return false
        if(this.$rebuildState !== RS.Done && this.$rebuildState < RS.LayoutTable) return false
        return true
    }
    columnWidth(column){ return this.isColumnLoaded(column) ? this.$getEffectiveColumnWidth(column) : -1 }
    rowHeight(row){ return this.isRowLoaded(row) ? this.$getEffectiveRowHeight(row) : -1 }
    implicitColumnWidth(column){ return this.isColumnLoaded(column) ? this.$sizeHintForColumn(column) : -1 }
    implicitRowHeight(row){ return this.isRowLoaded(row) ? this.$sizeHintForRow(row) : -1 }

    setColumnWidth(column, size){
        if(column < 0){
            console.warn('TableView: column must be greather than, or equal to, zero')
            return
        }
        if(this.$syncHorizontally) return this.$syncView.setColumnWidth(column, size)
        if(this.explicitColumnWidth(column) === size || fuzzyCompare(this.explicitColumnWidth(column), size)) return
        if(size < 0) this.$explicitColumnWidths.delete(this.$logicalColumnIndex(column))
        else this.$explicitColumnWidths.set(this.$logicalColumnIndex(column), size)
        if(!this.$loadedItems.size) return
        let all = this.$atTableEnd(LEFT) && this.$atTableEnd(RIGHT)
        if(column >= this.leftColumn || column <= this.rightColumn || all) this.$forceLayout(false)
    }
    clearColumnWidths(){
        if(this.$syncHorizontally) return this.$syncView.clearColumnWidths()
        if(!this.$explicitColumnWidths.size) return
        this.$explicitColumnWidths.clear()
        this.$forceLayout(false)
    }
    explicitColumnWidth(column){
        if(this.$syncHorizontally) return this.$syncView.explicitColumnWidth(column)
        let w = this.$explicitColumnWidths.get(this.$logicalColumnIndex(column))
        return w === undefined ? -1 : w
    }
    setRowHeight(row, size){
        if(row < 0){
            console.warn('TableView: row must be greather than, or equal to, zero')
            return
        }
        if(this.$syncVertically) return this.$syncView.setRowHeight(row, size)
        if(this.explicitRowHeight(row) === size || fuzzyCompare(this.explicitRowHeight(row), size)) return
        if(size < 0) this.$explicitRowHeights.delete(this.$logicalRowIndex(row))
        else this.$explicitRowHeights.set(this.$logicalRowIndex(row), size)
        if(!this.$loadedItems.size) return
        let all = this.$atTableEnd(TOP) && this.$atTableEnd(BOTTOM)
        if(row >= this.topRow || row <= this.bottomRow || all) this.$forceLayout(false)
    }
    clearRowHeights(){
        if(this.$syncVertically) return this.$syncView.clearRowHeights()
        if(!this.$explicitRowHeights.size) return
        this.$explicitRowHeights.clear()
        this.$forceLayout(false)
    }
    explicitRowHeight(row){
        if(this.$syncVertically) return this.$syncView.explicitRowHeight(row)
        let h = this.$explicitRowHeights.get(this.$logicalRowIndex(row))
        return h === undefined ? -1 : h
    }

    // the model index of a visual cell
    $modelIndexOfCell(cell){
        if(cell.x < 0 || cell.x >= this.$tableSize.w || cell.y < 0 || cell.y >= this.$tableSize.h) return INVALID
        let model = this.$tableModel.model
        if(!model) return INVALID
        return model.index(this.$logicalRowIndex(cell.y), this.$logicalColumnIndex(cell.x))
    }
    // modelIndex(cell); the deprecated modelIndex(row, column) (column first since 6.4.x: the warning of Qt)
    modelIndex(cell, column){
        if(typeof cell === 'number'){
            console.warn('TableView: modelIndex(row, column) is deprecated. Use index(row, column) instead. For more information, see https://doc.qt.io/qt-6/qml-qtquick-tableview-obsolete.html')
            return this.$modelIndexOfCell({ x: column, y: cell })
        }
        return this.$modelIndexOfCell(cell)
    }
    index(row, column){ return this.$modelIndexOfCell({ x: column, y: row }) }
    cellAtIndex(index){
        if(!index || !index.valid || (index.parent && index.parent.valid)) return new QPoint(-1, -1)
        return new QPoint(this.$visualColumnIndex(index.column), this.$visualRowIndex(index.row))
    }
    rowAtIndex(index){ return this.cellAtIndex(index).y }
    columnAtIndex(index){ return this.cellAtIndex(index).x }

    // ---- selection through a selection rectangle and the keys (QQuickSelectable) ----
    $startSelection(pos, modifiers){
        let sm = this.$selectionModel
        if(!sm){
            if(this.$warnNoSelectionModel) console.warn('TableView: Cannot start selection: no SelectionModel assigned!')
            this.$warnNoSelectionModel = false
            return false
        }
        let behavior = this.$v[TableView.SELECTIONBEHAVIOR]
        if(behavior === ENUMS.SelectionDisabled){
            console.warn('TableView: Cannot start selection: TableView.selectionBehavior == TableView.SelectionDisabled')
            return false
        }
        if(this.$resize.state !== 'listening') return false
        let mode = this.$v[TableView.SELECTIONMODE]
        if(mode === ENUMS.SingleSelection || mode === ENUMS.ContiguousSelection || modifiers === QtEnums.NoModifier) this.$clearSelection()
        else this.$existingSelection = sm.selection
        this.$selectionFlag = ItemSelectionModel.Select
        if(modifiers & QtEnums.ControlModifier){
            let start = this.$clampedCellAtPos(pos)
            if(!cellValid(start)) return false
            if(sm.isSelected(this.index(start.y, start.x))) this.$selectionFlag = ItemSelectionModel.Deselect
        }
        this.$selectionStartCell = { x: -1, y: -1 }
        this.$selectionEndCell = { x: -1, y: -1 }
        this.$closeEditorAndCommit()
        return true
    }

    $setSelectionStartPos(pos){
        if(!this.$loadedItems.size) return
        let sm = this.$selectionModel
        if(!sm){
            if(this.$warnNoSelectionModel) console.warn('TableView: Cannot set selection: no SelectionModel assigned!')
            this.$warnNoSelectionModel = false
            return
        }
        if(!sm.$model) return
        if(this.$v[TableView.SELECTIONMODE] === ENUMS.SingleSelection && cellValid(this.$selectionStartCell)) return
        let prev = this.$selectionRect()
        this.$inSelectionModelUpdate = true
        try {
            let clamped
            if(pos.x === -1) clamped = this.cellAtIndex(sm.currentIndex)
            else {
                clamped = this.$clampedCellAtPos(pos)
                if(cellValid(clamped)) this.$setCurrentIndex(clamped)
            }
            if(!cellValid(clamped)) return
            let behavior = this.$v[TableView.SELECTIONBEHAVIOR]
            if(behavior === ENUMS.SelectCells) this.$selectionStartCell = { x: clamped.x, y: clamped.y }
            else if(behavior === ENUMS.SelectRows) this.$selectionStartCell = { x: 0, y: clamped.y }
            else if(behavior === ENUMS.SelectColumns) this.$selectionStartCell = { x: clamped.x, y: 0 }
            else return
            if(!cellValid(this.$selectionEndCell)) return
            this.$updateSelection(prev, this.$selectionRect())
        } finally { this.$inSelectionModelUpdate = false }
    }

    $setSelectionEndPos(pos){
        if(!this.$loadedItems.size) return
        let sm = this.$selectionModel
        if(!sm){
            if(this.$warnNoSelectionModel) console.warn('TableView: Cannot set selection: no SelectionModel assigned!')
            this.$warnNoSelectionModel = false
            return
        }
        if(!sm.$model) return
        let prev = this.$selectionRect()
        let clamped
        if(this.$v[TableView.SELECTIONMODE] === ENUMS.SingleSelection) clamped = this.$selectionStartCell
        else {
            clamped = this.$clampedCellAtPos(pos)
            if(!cellValid(clamped)) return
        }
        this.$inSelectionModelUpdate = true
        try {
            this.$setCurrentIndex(clamped)
            let behavior = this.$v[TableView.SELECTIONBEHAVIOR]
            if(behavior === ENUMS.SelectCells) this.$selectionEndCell = { x: clamped.x, y: clamped.y }
            else if(behavior === ENUMS.SelectRows) this.$selectionEndCell = { x: this.$tableSize.w - 1, y: clamped.y }
            else if(behavior === ENUMS.SelectColumns) this.$selectionEndCell = { x: clamped.x, y: this.$tableSize.h - 1 }
            else return
            if(!cellValid(this.$selectionStartCell)) return
            this.$updateSelection(prev, this.$selectionRect())
        } finally { this.$inSelectionModelUpdate = false }
    }

    $clampedCellAtPos(pos){
        let cell = this.cellAtPosition(pos, true)
        if(cellValid(cell)) return cell
        let o = this.$outer
        if(o.width === 0 || o.height === 0) return new QPoint(-1, -1)
        let x = bound(o.x, pos.x, o.right - 1), y = bound(o.y, pos.y, o.bottom - 1)
        // within the viewport: content coordinates of the view's own area
        let inViewX = bound(0, x - this.$contentX(), this.$viewportRect.width)
        let inViewY = bound(0, y - this.$contentY(), this.$viewportRect.height)
        return this.cellAtPosition({ x: inViewX + this.$contentX(), y: inViewY + this.$contentY() }, true)
    }

    // QRect(start, end - start): x, y, width, height as Qt keeps them
    $selectionRect(){
        let s = this.$selectionStartCell, e = this.$selectionEndCell
        return { x: s.x, y: s.y, w: e.x - s.x, h: e.y - s.y }
    }

    $updateSelection(oldSel, newSel){
        if(oldSel.x === newSel.x && oldSel.y === newSel.y && oldSel.w === newSel.w && oldSel.h === newSel.h) return
        let model = this.$selectionModel.$model
        let normalized = (r) => {
            let x = r.w < 0 ? r.x + r.w : r.x, y = r.h < 0 ? r.y + r.h : r.y
            return { x, y, w: Math.abs(r.w), h: Math.abs(r.h) }
        }
        let o = normalized(oldSel), n = normalized(newSel)
        let owner = this.$syncView || this
        let hasMapping = owner.$horizontalLogicalIndices.length || owner.$verticalLogicalIndices.length
        let select = new QItemSelection(), deselect = new QItemSelection()
        let mergeInto = (selection, start, end) => {
            if(hasMapping){
                for(let mi of new QItemSelection(start, end).indexes()){
                    let logical = model.index(this.$logicalRowIndex(mi.row), this.$logicalColumnIndex(mi.column))
                    selection.merge(new QItemSelection(logical, logical), ItemSelectionModel.Select)
                }
            } else selection.merge(new QItemSelection(start, end), ItemSelectionModel.Select)
        }
        mergeInto(select, model.index(n.y, n.x), model.index(n.y + n.h, n.x + n.w))
        if(o.x < n.x) mergeInto(deselect, model.index(o.y, o.x), model.index(o.y + o.h, n.x - 1))
        else if(o.x + o.w > n.x + n.w) mergeInto(deselect, model.index(o.y, n.x + n.w + 1), model.index(o.y + o.h, o.x + o.w))
        if(o.y < n.y) mergeInto(deselect, model.index(o.y, o.x), model.index(n.y - 1, o.x + o.w))
        else if(o.y + o.h > n.y + n.h) mergeInto(deselect, model.index(n.y + n.h + 1, o.x), model.index(o.y + o.h, o.x + o.w))
        let sm = this.$selectionModel
        if(this.$selectionFlag === ItemSelectionModel.Select){
            deselect.merge(this.$existingSelection, ItemSelectionModel.Deselect)
            sm.select(deselect, ItemSelectionModel.Deselect)
            sm.select(select, ItemSelectionModel.Select)
        } else if(this.$selectionFlag === ItemSelectionModel.Deselect){
            let old = this.$existingSelection.clone()
            old.merge(select, ItemSelectionModel.Deselect)
            sm.select(old, ItemSelectionModel.Select)
            sm.select(select, ItemSelectionModel.Deselect)
        }
    }

    $cancelSelectionTracking(){
        this.$selectionStartCell = { x: -1, y: -1 }
        this.$selectionEndCell = { x: -1, y: -1 }
        this.$existingSelection = new QItemSelection()
        this.$selectionFlag = ItemSelectionModel.NoUpdate
        if(this.$selectableCallback) this.$selectableCallback('cancel')
    }

    $clearSelection(){
        if(!this.$selectionModel) return
        this.$inSelectionModelUpdate = true
        try { this.$selectionModel.clearSelection() } finally { this.$inSelectionModelUpdate = false }
    }

    // the rectangle of the selection in the content item (QQuickTableViewPrivate::selectionRectangle)
    $selectionRectangle(){
        if(this.$loadedColumns.isEmpty() || this.$loadedRows.isEmpty()) return new QRectF(0, 0, 0, 0)
        let tl = { x: this.$selectionStartCell.x, y: this.$selectionStartCell.y }, br = { x: this.$selectionEndCell.x, y: this.$selectionEndCell.y }
        if(br.x < tl.x){ let t = tl.x; tl.x = br.x; br.x = t }
        if(this.$selectionEndCell.y < tl.y){ let t = tl.y; tl.y = br.y; br.y = t }
        let leftCell = { x: tl.x, y: this.$topRow() }, topCell = { x: this.$leftColumn(), y: tl.y }
        let rightCell = { x: br.x, y: this.$topRow() }, bottomCell = { x: this.$leftColumn(), y: br.y }
        let left = 0, top = 0, right = 0, bottom = 0
        let geometry = (cell) => this.$geometry(this.$loadedTableItem(cell))
        if(this.$loadedItems.has(this.$modelIndexAtCell(leftCell))) left = geometry(leftCell).left
        else if(leftCell.x > this.$rightColumn()) left = this.$v[Flickable.CONTENTWIDTH]
        if(this.$loadedItems.has(this.$modelIndexAtCell(topCell))) top = geometry(topCell).top
        else if(topCell.y > this.$bottomRow()) top = this.$v[Flickable.CONTENTHEIGHT]
        if(this.$loadedItems.has(this.$modelIndexAtCell(rightCell))) right = geometry(rightCell).right
        else if(rightCell.x > this.$rightColumn()) right = this.$v[Flickable.CONTENTWIDTH]
        if(this.$loadedItems.has(this.$modelIndexAtCell(bottomCell))) bottom = geometry(bottomCell).bottom
        else if(bottomCell.y > this.$bottomRow()) bottom = this.$v[Flickable.CONTENTHEIGHT]
        return new QRectF(left, top, right - left, bottom - top)
    }

    $setCurrentIndex(cell){
        if(!this.$selectionModel) return
        this.$selectionModel.setCurrentIndex(this.$modelIndexOfCell(cell), ItemSelectionModel.NoUpdate)
    }

    // ---- the keys (QQuickTableView::keyPressEvent) ----
    $keyEvent(event, kind){
        if(kind !== 'pressed') return 0
        if(!this.$v[TableView.KEYNAVIGATIONENABLED]) return 0
        if(this.$tableSize.w <= 0 || this.$tableSize.h <= 0) return 1
        if(this.$editIndex.valid) return 1
        if(this.$setCurrentIndexFromKeyEvent(event)) return 1
        if(this.$editFromKeyEvent(event)) return 1
        return 0
    }

    $setCurrentIndexFromKeyEvent(e){
        let sm = this.$selectionModel
        if(!sm || !sm.$model) return false
        let currentCell = this.cellAtIndex(sm.currentIndex)
        let K = QtEnums
        if(!this.$v[Item.ACTIVEFOCUSONTAB] && (e.key === K.Key_Tab || e.key === K.Key_Backtab)) return false
        if(!cellValid(currentCell)){
            if([K.Key_Up, K.Key_Down, K.Key_Left, K.Key_Right, K.Key_PageUp, K.Key_PageDown, K.Key_Home, K.Key_End, K.Key_Tab, K.Key_Backtab].indexOf(e.key) >= 0){
                if(!this.$loadedRows.isEmpty() && !this.$loadedColumns.isEmpty()){
                    sm.setCurrentIndex(this.index(this.$topRow(), this.$leftColumn()), ItemSelectionModel.NoUpdate)
                    return true
                }
            }
            return false
        }
        let begin = () => {
            let shouldSelect = (e.modifiers & K.ShiftModifier) && e.key !== K.Key_Backtab
            let startNew = this.$selectionRectangle().width <= 0 || this.$selectionRectangle().height <= 0
            if(!shouldSelect){
                this.$clearSelection()
                this.$cancelSelectionTracking()
            } else if(startNew){
                let startIndex = this.$modelIndexToCellIndex(sm.currentIndex)
                let fx = this.$loadedItems.get(startIndex)
                if(fx){
                    let g = this.$geometry(fx)
                    let center = { x: g.x + g.width / 2, y: g.y + g.height / 2 }
                    if(this.$startSelection(center, K.ShiftModifier)){
                        this.$setSelectionStartPos(center)
                        if(this.$selectableCallback) this.$selectableCallback('changed')
                    }
                }
            }
        }
        let end = (cell) => {
            if(this.$selectionFlag !== ItemSelectionModel.NoUpdate){
                if(this.$polishQueued) this.$forceLayout(true)
                let fx = this.$loadedItems.get(this.$modelIndexAtCell(cell))
                if(fx){
                    let g = this.$geometry(fx)
                    this.$setSelectionEndPos({ x: g.x + g.width / 2, y: g.y + g.height / 2 })
                    if(this.$selectableCallback) this.$selectableCallback('changed')
                }
            }
            sm.setCurrentIndex(this.$modelIndexOfCell(cell), ItemSelectionModel.NoUpdate)
        }
        let topMargin = this.$margin('topMargin'), bottomMargin = this.$margin('bottomMargin')
        let leftMargin = this.$margin('leftMargin'), rightMargin = this.$margin('rightMargin')
        switch(e.key){
        case K.Key_Up: {
            begin()
            let next = this.$nextVisibleEdgeIndex(TOP, currentCell.y - 1)
            if(next === EDGE_AT_END) break
            this.positionViewAtRow(next, ENUMS.Contain, this.$atTableEnd(TOP, next - 1) ? -topMargin : 0)
            end({ x: currentCell.x, y: next })
            break }
        case K.Key_Down: {
            begin()
            let next = this.$nextVisibleEdgeIndex(BOTTOM, currentCell.y + 1)
            if(next === EDGE_AT_END) break
            this.positionViewAtRow(next, ENUMS.Contain, this.$atTableEnd(BOTTOM, next + 1) ? bottomMargin : 0)
            end({ x: currentCell.x, y: next })
            break }
        case K.Key_Left: {
            begin()
            let next = this.$nextVisibleEdgeIndex(LEFT, currentCell.x - 1)
            if(next === EDGE_AT_END) break
            this.positionViewAtColumn(next, ENUMS.Contain, this.$atTableEnd(LEFT, next - 1) ? -leftMargin : 0)
            end({ x: next, y: currentCell.y })
            break }
        case K.Key_Right: {
            begin()
            let next = this.$nextVisibleEdgeIndex(RIGHT, currentCell.x + 1)
            if(next === EDGE_AT_END) break
            this.positionViewAtColumn(next, ENUMS.Contain, this.$atTableEnd(RIGHT, next + 1) ? rightMargin : 0)
            end({ x: next, y: currentCell.y })
            break }
        case K.Key_PageDown: {
            let newBottom
            begin()
            if(currentCell.y < this.$bottomRow()){
                newBottom = this.$bottomRow()
                this.positionViewAtRow(newBottom, ENUMS.AlignBottom, 0)
            } else {
                this.positionViewAtRow(this.$bottomRow(), ENUMS.AlignTop, 0)
                this.$positionYAnimation.complete()
                newBottom = this.$topRow() !== this.$bottomRow() ? this.$bottomRow() : this.$bottomRow() + 1
                this.positionViewAtRow(newBottom, ENUMS.AlignTop | ENUMS.AlignBottom, this.$atTableEnd(BOTTOM, newBottom + 1) ? bottomMargin : 0)
                this.$positionYAnimation.complete()
            }
            end({ x: currentCell.x, y: newBottom })
            break }
        case K.Key_PageUp: {
            let newTop
            begin()
            if(currentCell.y > this.$topRow()){
                newTop = this.$topRow()
                this.positionViewAtRow(newTop, ENUMS.AlignTop, 0)
            } else {
                this.positionViewAtRow(this.$topRow(), ENUMS.AlignBottom, 0)
                this.$positionYAnimation.complete()
                newTop = this.$topRow() !== this.$bottomRow() ? this.$topRow() : this.$topRow() - 1
                this.positionViewAtRow(newTop, ENUMS.AlignTop, this.$atTableEnd(TOP, newTop - 1) ? -topMargin : 0)
                this.$positionYAnimation.complete()
            }
            end({ x: currentCell.x, y: newTop })
            break }
        case K.Key_Home: {
            begin()
            let first = this.$nextVisibleEdgeIndex(RIGHT, 0)
            this.positionViewAtColumn(first, ENUMS.AlignLeft, -leftMargin)
            end({ x: first, y: currentCell.y })
            break }
        case K.Key_End: {
            begin()
            let last = this.$nextVisibleEdgeIndex(LEFT, this.$tableSize.w - 1)
            this.positionViewAtColumn(last, ENUMS.AlignRight, rightMargin)
            end({ x: last, y: currentCell.y })
            break }
        case K.Key_Tab: {
            begin()
            let nextRow = currentCell.y
            let nextColumn = this.$nextVisibleEdgeIndex(RIGHT, currentCell.x + 1)
            if(nextColumn === EDGE_AT_END){
                nextRow = this.$nextVisibleEdgeIndex(BOTTOM, currentCell.y + 1)
                if(nextRow === EDGE_AT_END) nextRow = this.$nextVisibleEdgeIndex(BOTTOM, 0)
                nextColumn = this.$nextVisibleEdgeIndex(RIGHT, 0)
                this.positionViewAtRow(nextRow, ENUMS.Contain, this.$atTableEnd(BOTTOM, nextRow + 1) ? bottomMargin : 0)
            }
            let marginX = this.$atTableEnd(RIGHT, nextColumn + 1) ? leftMargin : this.$atTableEnd(LEFT, nextColumn - 1) ? -leftMargin : 0
            this.positionViewAtColumn(nextColumn, ENUMS.Contain, marginX)
            end({ x: nextColumn, y: nextRow })
            break }
        case K.Key_Backtab: {
            begin()
            let nextRow = currentCell.y
            let nextColumn = this.$nextVisibleEdgeIndex(LEFT, currentCell.x - 1)
            if(nextColumn === EDGE_AT_END){
                nextRow = this.$nextVisibleEdgeIndex(TOP, currentCell.y - 1)
                if(nextRow === EDGE_AT_END) nextRow = this.$nextVisibleEdgeIndex(TOP, this.$tableSize.h - 1)
                nextColumn = this.$nextVisibleEdgeIndex(LEFT, this.$tableSize.w - 1)
                this.positionViewAtRow(nextRow, ENUMS.Contain, this.$atTableEnd(TOP, nextRow - 1) ? -topMargin : 0)
            }
            let marginX = this.$atTableEnd(RIGHT, nextColumn + 1) ? leftMargin : this.$atTableEnd(LEFT, nextColumn - 1) ? -leftMargin : 0
            this.positionViewAtColumn(nextColumn, ENUMS.Contain, marginX)
            end({ x: nextColumn, y: nextRow })
            break }
        default:
            return false
        }
        return true
    }

    $editFromKeyEvent(e){
        let triggers = this.$v[TableView.EDITTRIGGERS]
        if(triggers === ENUMS.NoEditTriggers) return false
        let sm = this.$selectionModel
        if(!sm || !sm.$model) return false
        let index = sm.currentIndex
        let cellItem = this.itemAtCell(this.cellAtIndex(index))
        if(!cellItem) return false
        let attached = existingAttached(cellItem)
        if(!attached || !attached.$v[TableViewAttached.EDITDELEGATE]) return false
        let K = QtEnums
        let anyKey = false, editKey = false
        let isMac = typeof navigator !== 'undefined' && /Mac|iPhone|iPad|iPod/.test(String(navigator.platform || ''))
        if(e.key === K.Key_Return || e.key === K.Key_Enter || (e.key === K.Key_F2 && !isMac)){
            anyKey = true
            editKey = true
        } else if([K.Key_Shift, K.Key_Alt, K.Key_Control, K.Key_Meta, K.Key_Tab, K.Key_Backtab].indexOf(e.key) < 0){
            anyKey = true
        }
        let anyAccepted = anyKey && (triggers & ENUMS.AnyKeyPressed)
        let editAccepted = editKey && (triggers & ENUMS.EditKeyPressed)
        if(!(editAccepted || anyAccepted)) return false
        if(!this.$canEdit(index, false)) return true
        this.edit(index)
        // the key that opened the editor types into it (QGuiApplication::sendEvent to the focus object)
        if(this.$editIndex.valid && anyAccepted && !editKey && e.text){
            let keys = require('./KeyDelivery')
            let focus = keys.agent.activeFocusItem
            if(focus && focus.$typeText) focus.$typeText(e.text)
        }
        return true
    }

    // ---- editing ----
    $canEdit(index, warn){
        if(!index || !index.valid){
            if(warn) console.warn('TableView: cannot edit: index is not valid!')
            return false
        }
        let model = this.$selectionSourceModel()
        if(!model || !this.$assignedIsItemModel()){
            if(warn) console.warn('TableView: cannot edit: TableView.model does not inherit QAbstractItemModel!')
            return false
        }
        if(!(model.flags(index) & QtEnums.ItemIsEditable)){
            if(warn) console.warn("TableView: cannot edit: the index flags don't include Qt::ItemIsEditable")
            return false
        }
        let cellItem = this.itemAtCell(this.cellAtIndex(index))
        if(!cellItem){
            if(warn) console.warn('TableView: cannot edit: the cell to edit is not inside the viewport!')
            return false
        }
        let attached = existingAttached(cellItem)
        if(!attached || !attached.$v[TableViewAttached.EDITDELEGATE]){
            if(warn) console.warn('TableView: cannot edit: no TableView.editDelegate set!')
            return false
        }
        return true
    }

    edit(requestedIndex){
        if(!this.$canEdit(requestedIndex, true)) return
        let index = requestedIndex
        if(this.$editIndex.valid && sameIndex(this.$editIndex.index, index)) return
        if(this.$selectionModel) this.$selectionModel.setCurrentIndex(index, ItemSelectionModel.NoUpdate)
        this.$closeEditorAndCommit()
        let cellItem = this.itemAtCell(this.cellAtIndex(index))
        let attached = existingAttached(cellItem)
        let delegate = attached.$v[TableViewAttached.EDITDELEGATE]
        let entry = cellItem.$tableEntry
        let flat = entry ? entry.flat : this.$modelIndexToCellIndex(index)
        let ctx = new CellContext(this.$tableModel, flat, entry ? entry.ctx.$cellRow : index.row, entry ? entry.ctx.$cellColumn : index.column)
        this.$creatingEditItem = true
        let editItem
        try {
            editItem = delegate.createObject(cellItem, null, ctx)
        } finally { this.$creatingEditItem = false }
        if(!editItem){
            this.$editIndex = new QPersistentModelIndex(INVALID)
            this.$editItem = null
            console.warn('TableView: cannot edit: TableView.editDelegate could not be instantiated!')
            return
        }
        if(!(editItem instanceof Item)){
            console.warn('TableView: cannot edit: TableView.editDelegate is not an Item!')
            editItem.$releaseLater()
            return
        }
        editItem.$editCtx = ctx
        this.$editIndex = new QPersistentModelIndex(index)
        this.$editItem = editItem
        ctx.$tableItem = editItem
        this.$updateItemProperties(flat, editItem, true, ctx)
        cellItem.z = 2
        this.$setRequired(cellItem, 'editing', true)
        editItem.forceActiveFocus()
        // QQuickTableView::eventFilter on the focus item of the editor: Enter commits, Escape cancels, Tab moves on
        require('./KeyDelivery').addKeyFilter(this)
        this.$watchEditFocus()
    }

    $watchEditFocus(){
        let editItem = this.$editItem
        if(!editItem) return
        let fn = () => {
            if(this.$editItem !== editItem) return
            engine.queueCall(() => {
                if(this.$editItem !== editItem) return
                let focus = require('./KeyDelivery').agent.activeFocusItem
                for(let it = focus; it; it = it.$parent) if(it === editItem) return
                this.$closeEditorAndCommit()
            })
        }
        engine.connect(editItem, Item.SIGNAL_activeFocusChanged, (old, active) => { if(!active) fn() })
    }

    // QQuickTableView::eventFilter of the editor's focus object
    $filterEditKey(event, kind, target){
        let editItem = this.$editItem
        if(!editItem || kind !== 'pressed') return false
        let inside = false
        for(let it = target; it; it = it.$parent) if(it === editItem){ inside = true; break }
        if(!inside) return false
        let K = QtEnums
        if(event.key === K.Key_Enter || event.key === K.Key_Return){
            this.$closeEditorAndCommit()
            return true
        }
        if(event.key === K.Key_Tab || event.key === K.Key_Backtab){
            if(this.$v[Item.ACTIVEFOCUSONTAB]){
                if(this.$setCurrentIndexFromKeyEvent(event)){
                    let current = this.$selectionModel.currentIndex
                    if(this.$canEdit(current, false)) this.edit(current)
                }
                return true
            }
            return false
        }
        if(event.key === K.Key_Escape){
            this.closeEditor()
            return true
        }
        return false
    }

    $closeEditorAndCommit(){
        if(!this.$editItem) return
        let attached = existingAttached(this.$editItem)
        if(attached) engine.emitArgs(attached, TableViewAttached.SIGNAL_commit, [])
        this.closeEditor()
    }

    closeEditor(){
        let editItem = this.$editItem
        if(!editItem) return
        let cellItem = editItem.$parent
        this.$editItem = null
        editItem.$releaseLater()
        if(cellItem) cellItem.z = 1
        this.$setRequired(cellItem, 'editing', false)
        this.$editIndex = new QPersistentModelIndex(INVALID)
        require('./KeyDelivery').removeKeyFilter(this)
        if(cellItem && editItem.$v[Item.ACTIVEFOCUS]) this.forceActiveFocus()
    }

    $updateEditItem(){
        if(!this.$editItem) return
        let cellItem = this.itemAtCell(this.cellAtIndex(this.$editIndex.index))
        if(!cellItem && this.$editItem.$parent) this.$editItem.$parent.x = -this.$editItem.$v[Item.WIDTH] - 10000
    }

    // ---- the pointer: taps, resizing (QQuickTableViewTapHandler, HoverHandler, ResizeHandler) ----
    $acceptsHover(){ return !!(this.$v[TableView.RESIZABLECOLUMNS] || this.$v[TableView.RESIZABLEROWS]) && this.$v[Item.ENABLED] }
    $hoverEnter(x, y){ this.$hoverMove(x, y) }
    $hoverMove(x, y){
        let pos = { x: x + this.$contentX(), y: y + this.$contentY() }
        let cell = this.cellAtPosition(pos, true)
        let item = this.itemAtCell(cell)
        if(!item){
            this.$hover = { row: -1, column: -1 }
        } else {
            let margin = 5
            let ix = pos.x - item.$v[Item.X], iy = pos.y - item.$v[Item.Y]
            let hoveringRow = iy < margin || iy > item.$v[Item.HEIGHT] - margin
            let hoveringColumn = ix < margin || ix > item.$v[Item.WIDTH] - margin
            this.$hover = {
                row: hoveringRow ? (iy < margin ? cell.y - 1 : cell.y) : -1,
                column: hoveringColumn ? (ix < margin ? cell.x - 1 : cell.x) : -1,
            }
        }
        this.$updateCursor()
    }
    $hoverLeave(){
        this.$hover = { row: -1, column: -1 }
        this.$updateCursor()
    }
    $updateCursor(){
        let row = this.$v[TableView.RESIZABLEROWS] ? this.$hover.row : -1
        let column = this.$v[TableView.RESIZABLECOLUMNS] ? this.$hover.column : -1
        if(this.$resize.state === 'draggingStarted' || this.$resize.state === 'dragging'){
            row = this.$resize.row
            column = this.$resize.column
        }
        let shape = null
        if(row !== -1 || column !== -1) shape = row !== -1 && column !== -1 ? QtEnums.SizeFDiagCursor : row !== -1 ? QtEnums.SplitVCursor : QtEnums.SplitHCursor
        let delivery = require('./PointerDelivery')
        delivery.setOverrideCursor(this, shape)
    }

    // a press at a grid line of a resizable table starts a resize (the resize handler grabs before the flick)
    $filterPointer(kind, receiver, x, y, buttons){
        if(kind === 'press' && this.$startResize(x, y)) return { filter: true, grab: true }
        if(this.$resize.state !== 'listening') return { filter: true, grab: true }
        return super.$filterPointer(kind, receiver, x, y, buttons)
    }

    $startResize(x, y){
        if(!this.$acceptsHover()) return false
        this.$hoverMove(x, y)
        let row = this.$v[TableView.RESIZABLEROWS] ? this.$hover.row : -1
        let column = this.$v[TableView.RESIZABLECOLUMNS] ? this.$hover.column : -1
        if(row === -1 && column === -1) return false
        this.$resize = { state: 'tracking', row, column, startX: 0, startY: 0, startWidth: 0, startHeight: 0, pressX: x, pressY: y }
        return true
    }

    $mousePress(x, y, button, modifiers){
        this.$positionXAnimation.stop()
        this.$positionYAnimation.stop()
        if(this.$startResize(x, y)) return true
        if(button === 1 && this.$v[TableView.POINTERNAVIGATIONENABLED]){
            let t = this.$tap
            t.pressed = true
            t.x = x
            t.y = y
            t.modifiers = modifiers || 0
            t.moved = false
            if(!this.$v[Flickable.INTERACTIVE]) this.$handleTap(x, y, t.modifiers)
        }
        let flickTook = super.$mousePress(x, y, button)
        return flickTook || (button === 1 && this.$v[TableView.POINTERNAVIGATIONENABLED])
    }

    $mouseMove(x, y, buttons){
        let r = this.$resize
        if(r.state !== 'listening'){
            if(r.state === 'tracking'){
                let dx = r.column !== -1 ? x - r.pressX : 0, dy = r.row !== -1 ? y - r.pressY : 0
                if(Math.sqrt(dx * dx + dy * dy) > DRAG_THRESHOLD){
                    r.state = 'draggingStarted'
                    r.startX = x
                    r.startWidth = this.columnWidth(r.column)
                    r.startY = y
                    r.startHeight = this.rowHeight(r.row)
                    this.$updateCursor()
                }
            }
            if(r.state === 'draggingStarted' || r.state === 'dragging'){
                if(r.state === 'draggingStarted') r.state = 'dragging'
                if(r.column !== -1) this.setColumnWidth(r.column, Math.max(0.001, r.startWidth + x - r.startX))
                if(r.row !== -1) this.setRowHeight(r.row, Math.max(0.001, r.startHeight + y - r.startY))
            }
            return
        }
        let t = this.$tap
        if(t.pressed && (Math.abs(x - t.x) > DRAG_THRESHOLD || Math.abs(y - t.y) > DRAG_THRESHOLD)) t.moved = true
        super.$mouseMove(x, y, buttons)
    }

    $mouseRelease(x, y){
        let r = this.$resize
        if(r.state !== 'listening'){
            let wasDragging = r.state === 'dragging' || r.state === 'draggingStarted'
            this.$resize = { state: 'listening', row: -1, column: -1 }
            this.$updateCursor()
            if(!wasDragging) this.$tapAt(x, y, true)
            return
        }
        let t = this.$tap
        let moving = this.$isMoving()
        super.$mouseRelease(x, y)
        if(t.pressed){
            t.pressed = false
            if(!t.moved && !moving && this.$v[Flickable.INTERACTIVE]) this.$tapAt(x, y, false)
        }
    }

    $ungrabMouse(){
        if(this.$resize.state !== 'listening'){
            this.$resize = { state: 'listening', row: -1, column: -1 }
            this.$updateCursor()
        }
        this.$tap.pressed = false
        super.$ungrabMouse()
    }

    // QQuickTapHandler singleTapped / doubleTapped (QStyleHints::mouseDoubleClickInterval, the double click distance)
    $tapAt(x, y, onGridLine){
        let t = this.$tap
        let now = clock.now()
        let double = t.lastTapTime >= 0 && now - t.lastTapTime <= 400 && Math.abs(x - t.lastX) <= 5 && Math.abs(y - t.lastY) <= 5
        t.lastTapTime = double ? -1 : now
        t.lastX = x
        t.lastY = y
        if(double){
            let resizeRow = this.$v[TableView.RESIZABLEROWS] && this.$hover.row !== -1
            let resizeColumn = this.$v[TableView.RESIZABLECOLUMNS] && this.$hover.column !== -1
            if(resizeRow || resizeColumn){
                if(resizeRow) this.setRowHeight(this.$hover.row, -1)
                if(resizeColumn) this.setColumnWidth(this.$hover.column, -1)
            } else if(this.$v[TableView.EDITTRIGGERS] & ENUMS.DoubleTapped){
                let index = this.$modelIndexOfCell(this.cellAtPosition({ x: x + this.$contentX(), y: y + this.$contentY() }))
                if(this.$canEdit(index, false)) this.edit(index)
            }
            if(this.$doubleTapped) this.$doubleTapped(x, y, t.modifiers)
            return
        }
        if(!onGridLine) this.$handleTap(x, y, t.modifiers)
    }

    $handleTap(x, y, modifiers){
        if(this.$v[TableView.KEYNAVIGATIONENABLED]) this.forceActiveFocus()
        if(modifiers !== QtEnums.NoModifier) return
        if(this.$v[TableView.RESIZABLEROWS] && this.$hover.row !== -1) return
        if(this.$v[TableView.RESIZABLECOLUMNS] && this.$hover.column !== -1) return
        if(this.$resize.state !== 'listening') return
        let position = { x: x + this.$contentX(), y: y + this.$contentY() }
        let tapped = this.$modelIndexOfCell(this.cellAtPosition(position))
        let tappedSelected = this.$selectionModel ? this.$selectionModel.isSelected(tapped) : false
        if(this.$canEdit(tapped, false)){
            let triggers = this.$v[TableView.EDITTRIGGERS]
            if(triggers & ENUMS.SingleTapped){
                if(this.$v[TableView.SELECTIONBEHAVIOR] !== ENUMS.SelectionDisabled) this.$clearSelection()
                this.edit(tapped)
                return
            } else if(triggers & ENUMS.SelectedTapped && tappedSelected){
                this.edit(tapped)
                return
            }
        }
        if(this.$v[TableView.POINTERNAVIGATIONENABLED]){
            this.$closeEditorAndCommit()
            if(this.$v[TableView.SELECTIONBEHAVIOR] !== ENUMS.SelectionDisabled){
                this.$clearSelection()
                this.$cancelSelectionTracking()
            }
            let cell = this.cellAtPosition(position)
            if(cellValid(cell)) this.$setCurrentIndex(cell)
        }
    }

    // ---- the properties with effects ----
    $setModelValue(value){
        let model = value
        if(model === this.$assignedModel) return
        this.closeEditor()
        this.$assignedModel = model
        this.$needsModelSynchronization = true
        this.$scheduleRebuildTable(RO.All)
        if(this.$selectionModel) this.$selectionModel.$setModel(this.$selectionSourceModel())
    }

    $setSelectionModelValue(selectionModel){
        if(this.$selectionModel === selectionModel) return
        for(let [object, signal, fn] of this.$selectionLinks) engine.disconnect(object, signal, fn)
        this.$selectionLinks = []
        this.$selectionModel = selectionModel || null
        let sm = this.$selectionModel
        if(sm){
            sm.$setModel(this.$selectionSourceModel())
            let link = (signal, fn) => {
                engine.connect(sm, signal, fn)
                this.$selectionLinks.push([sm, signal, fn])
            }
            link(ItemSelectionModel.SIGNAL_selectionChanged, (selected, deselected) => this.$selectionChangedInSelectionModel(selected, deselected))
            link(ItemSelectionModel.SIGNAL_currentChanged, (current, previous) => this.$currentChangedInSelectionModel(current, previous))
        }
        this.$updateSelectedOnAllDelegateItems()
    }

    $componentComplete(){
        super.$componentComplete()
        // a model or delegate set while the component was made: the first polish (componentFinalized) builds
        this.$scheduledRebuildOptions |= RO.All
    }

    $disposeNative(){
        if(this.$syncView){
            let i = this.$syncView.$syncChildren.indexOf(this)
            if(i >= 0) this.$syncView.$syncChildren.splice(i, 1)
            this.$syncView.$scheduleRebuildTable(RO.ViewportOnly)
        }
        this.$disconnectFromModel()
        for(let [object, signal, fn] of this.$selectionLinks) engine.disconnect(object, signal, fn)
        this.$selectionLinks = []
        this.$positionXAnimation.stop()
        this.$positionYAnimation.stop()
        let delivery = require('./PointerDelivery')
        delivery.setOverrideCursor(this, null)
        require('./KeyDelivery').removeKeyFilter(this)
        releaseAdaptor(this.$tableModel.adaptor)
        super.$disposeNative && super.$disposeNative()
    }
}

// QQuickAnchors::activeDirections: an anchor line, fill or centerIn set
function hasActiveAnchors(item){
    const { Anchors } = require('./Item')
    let anchors = item.$v[Item.ANCHORS]
    if(!anchors) return false
    for(let name of ['fill', 'centerIn', 'left', 'right', 'horizontalCenter', 'top', 'bottom', 'verticalCenter', 'baseline']){
        let v = anchors.$v[Anchors[name.toUpperCase()]]
        if(v !== undefined && v !== null) return true
    }
    return false
}

// a QRectF given to positionViewAt is valid when its width and height are positive
function validRect(rect){
    if(!rect || typeof rect !== 'object') return null
    return rect.width > 0 && rect.height > 0 ? rect : null
}

const rebuildOn = (options) => (view) => view.$scheduleRebuildTable(options)

defineType(TableView, {
    props: {
        model: { kind: 'var', initial: 0, effect: (view, value) => view.$setModelValue(value), get(){
            engine.track(this, TableView.SIGNAL_modelChanged)
            return this.$modelValue()
        } },
        delegate: { kind: 'component', effect: rebuildOn(RO.All) },
        rowSpacing: { kind: 'real', effect: (view, value) => {
            if(!Number.isFinite(value)) return
            view.$cellSpacing.h = value
            view.$scheduleRebuildTable(RO.LayoutOnly | RO.CalculateNewContentHeight)
        } },
        columnSpacing: { kind: 'real', effect: (view, value) => {
            if(!Number.isFinite(value)) return
            view.$cellSpacing.w = value
            view.$scheduleRebuildTable(RO.LayoutOnly | RO.CalculateNewContentWidth)
        } },
        rowHeightProvider: { kind: 'var', effect: rebuildOn(RO.ViewportOnly | RO.CalculateNewContentHeight) },
        columnWidthProvider: { kind: 'var', effect: rebuildOn(RO.ViewportOnly | RO.CalculateNewContentWidth) },
        reuseItems: { kind: 'bool', initial: true, effect: (view, value) => { if(!value) view.$tableModel.drainReusableItemsPool(0) } },
        syncView: { kind: 'var', initial: null, effect: (view) => {
            view.$clearIndexMapping()
            view.$scheduleRebuildTable(RO.ViewportOnly)
        } },
        syncDirection: { kind: 'int', initial: QtEnums.Horizontal | QtEnums.Vertical, effect: (view) => {
            if(view.$v[TableView.SYNCVIEW]) view.$scheduleRebuildTable(RO.ViewportOnly)
        } },
        selectionModel: { kind: 'var', initial: null, effect: (view, value) => view.$setSelectionModelValue(value) },
        animate: { kind: 'bool', initial: true, effect: (view, value) => {
            if(!value){
                view.$positionXAnimation.stop()
                view.$positionYAnimation.stop()
            }
        } },
        keyNavigationEnabled: { kind: 'bool', initial: true },
        pointerNavigationEnabled: { kind: 'bool', initial: true },
        alternatingRows: { kind: 'bool', initial: true },
        selectionBehavior: { kind: 'int', initial: ENUMS.SelectCells },
        selectionMode: { kind: 'int', initial: ENUMS.ExtendedSelection },
        resizableColumns: 'bool',
        resizableRows: 'bool',
        editTriggers: { kind: 'int', initial: ENUMS.DoubleTapped | ENUMS.EditKeyPressed },
        delegateModelAccess: { kind: 'int', initial: 0, effect: rebuildOn(RO.All) },
        // QQuickFlickable margins: the extents grow by them
        leftMargin: { kind: 'real', effect: (view) => view.$updateBounds() },
        rightMargin: { kind: 'real', effect: (view) => view.$updateBounds() },
        topMargin: { kind: 'real', effect: (view) => view.$updateBounds() },
        bottomMargin: { kind: 'real', effect: (view) => view.$updateBounds() },
    },
    signals: ['rowsChanged', 'columnsChanged', 'leftColumnChanged', 'rightColumnChanged', 'topRowChanged', 'bottomRowChanged',
        'currentRowChanged', 'currentColumnChanged', 'layoutChanged', 'rowMoved(logicalIndex,oldVisualIndex,newVisualIndex)',
        'columnMoved(logicalIndex,oldVisualIndex,newVisualIndex)'],
})
Object.assign(TableView, ENUMS)
TableView.$defaultParent = (view) => view.$content

module.exports = { TableView, TableViewAttached, CellContext, TableInstanceModel, SortedSet, RO, RS, EDGE_AT_END, EDGE_NOT_SET, ENUMS, attachedOf, existingAttached }
