// Item models as Qt has them (qabstractitemmodel.cpp, Qt 6.12): QModelIndex, QPersistentModelIndex and
// QAbstractItemModel with its signals and the persistent indexes it keeps up to date; QAbstractTableModel and
// QAbstractListModel; and the adaptors of QQmlAdaptorModel, which give a number, an array or a ListModel the same face,
// one column wide, for the views that read every model as an item model (TableView, TreeView).
const engine = require('../../Core/engine')
const { QtObject } = require('../../Core/QtObject')
const { defineType } = require('../../Core/type')
const { ListModel } = require('./ListModel')
const QtEnums = require('../../Qt/enums')

// ---- QModelIndex ------------------------------------------------------------------------------------------------------

// A value: row, column, the model's internal id and the model. Invalid when the model is null or row / column < 0.
class QModelIndex {
    constructor(row = -1, column = -1, internalId = 0, model = null){
        this.row = row
        this.column = column
        this.$id = internalId
        this.model = model
    }
    get valid(){ return this.row >= 0 && this.column >= 0 && this.model !== null }
    // QQmlModelIndexValueType: parent is a property
    get parent(){ return this.model ? this.model.parent(this) : INVALID }
    get internalId(){ return typeof this.$id === 'number' ? this.$id : 0 }
    get internalPointer(){ return this.$id }
    isValid(){ return this.valid }
    sibling(row, column){ return this.model ? this.model.sibling(row, column, this) : INVALID }
    siblingAtRow(row){ return this.sibling(row, this.column) }
    siblingAtColumn(column){ return this.sibling(this.row, column) }
    data(role = QtEnums.DisplayRole){ return this.model ? this.model.data(this, role) : undefined }
    flags(){ return this.model ? this.model.flags(this) : 0 }
    // QDebug of a QModelIndex (the pointers of Qt are not ours: 0x0)
    toString(){
        return this.valid ? 'QModelIndex(' + this.row + ',' + this.column + ',0x0,' + modelName(this.model) + ')' : 'QModelIndex()'
    }
}

const INVALID = Object.freeze(new QModelIndex())

function modelName(model){
    let name = model && model.constructor && model.constructor.$qmlName || model && model.constructor && model.constructor.name || 'QObject'
    return name + '(0x0)'
}

// QModelIndex::operator==: row, column, internal id and model
function sameIndex(a, b){
    if(a === b) return true
    if(!a || !b) return false
    if(!a.valid && !b.valid) return true
    return a.row === b.row && a.column === b.column && a.$id === b.$id && a.model === b.model
}

// QModelIndex::operator<: row, column, internal id, model
function indexLess(a, b){
    if(a.row !== b.row) return a.row < b.row
    if(a.column !== b.column) return a.column < b.column
    return idKey(a.$id) < idKey(b.$id)
}
const idKey = (id) => (typeof id === 'number' ? id : 0)

// ---- the persistent indexes of a model: weakly held, a QPersistentModelIndex nobody keeps drops out (as its destructor
// would take it off the model in Qt) --------------------------------------------------------------------------------------
class PersistentSet {
    constructor(){ this.$refs = new Map() }    // QPersistentModelIndex -> its WeakRef (strong in an engine without WeakRef)
    add(p){ if(!this.$has(p)) this.$refs.set(this.$key(p), typeof WeakRef === 'function' ? new WeakRef(p) : { deref: () => p }) }
    delete(p){ this.$refs.delete(this.$key(p)) }
    clear(){ this.$refs.clear() }
    $key(p){ return p.$key || (p.$key = ++PersistentSet.$next) }
    $has(p){ return p.$key !== undefined && this.$refs.has(p.$key) }
    get size(){ return this.$refs.size }
    // a snapshot of the live ones: a change may take indexes off while they are walked
    *[Symbol.iterator](){
        let live = []
        for(let [key, ref] of this.$refs){
            let p = ref.deref()
            if(p) live.push(p)
            else this.$refs.delete(key)
        }
        yield* live
    }
}
PersistentSet.$next = 0

// ---- QPersistentModelIndex: follows its item while the model changes ---------------------------------------------------
class QPersistentModelIndex {
    constructor(index){
        this.$index = index && index.valid ? index : INVALID
        if(this.$index.valid) this.$index.model.$persistent.add(this)
    }
    get index(){ return this.$index }
    get row(){ return this.$index.row }
    get column(){ return this.$index.column }
    get model(){ return this.$index.model }
    get valid(){ return this.$index.valid }
    get parent(){ return this.$index.parent }
    isValid(){ return this.$index.valid }
    // stops following (the destructor of a QPersistentModelIndex)
    release(){
        if(this.$index.valid) this.$index.model.$persistent.delete(this)
        this.$index = INVALID
    }
    toString(){ return String(this.$index) }
}

// ---- QAbstractItemModel ------------------------------------------------------------------------------------------------

const DEFAULT_ROLE_NAMES = [[QtEnums.DisplayRole, 'display'], [QtEnums.DecorationRole, 'decoration'], [QtEnums.EditRole, 'edit'],
    [QtEnums.ToolTipRole, 'toolTip'], [QtEnums.StatusTipRole, 'statusTip'], [QtEnums.WhatsThisRole, 'whatsThis']]

class AbstractItemModel extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$persistent = new PersistentSet()
        this.$moved = []
        this.$invalidated = []
        this.$resetting = 0
    }

    // ---- the interface (Q_INVOKABLE in Qt) ----
    rowCount(parent = INVALID){ return 0 }
    columnCount(parent = INVALID){ return 0 }
    // what the views and the selection model call: rowCount / columnCount of QAbstractItemModel; a model whose QML face
    // has them as properties (TableModel.rowCount) gives its numbers here
    $rowCount(parent = INVALID){ return this.rowCount(parent) }
    $columnCount(parent = INVALID){ return this.columnCount(parent) }
    index(row, column, parent = INVALID){ return INVALID }
    parent(child){ return INVALID }
    hasIndex(row, column, parent = INVALID){
        if(row < 0 || column < 0) return false
        return row < this.$rowCount(parent) && column < this.$columnCount(parent)
    }
    hasChildren(parent = INVALID){
        if(parent && parent.valid && parent.model !== this) return false
        return this.$rowCount(parent) > 0 && this.$columnCount(parent) > 0
    }
    sibling(row, column, index){
        return row === index.row && column === index.column ? index : this.index(row, column, this.parent(index))
    }
    data(index, role = QtEnums.DisplayRole){ return undefined }
    setData(index, value, role = QtEnums.EditRole){ return false }
    headerData(section, orientation, role = QtEnums.DisplayRole){
        // QAbstractItemModel::headerData: the section number from 1
        return role === QtEnums.DisplayRole ? section + 1 : undefined
    }
    // Qt::ItemIsSelectable | Qt::ItemIsEnabled for a valid index
    flags(index){ return index && index.valid ? (QtEnums.ItemIsSelectable | QtEnums.ItemIsEnabled) : 0 }
    canFetchMore(parent){ return false }
    fetchMore(parent){}
    // QAbstractItemModel::roleNames (not invokable in QML): the role numbers and their names
    roleNames(){ return new Map(DEFAULT_ROLE_NAMES) }

    // the number of a role given by name, -1 when the model has none
    $roleOf(name){
        let names = this.$roleNameCache || (this.$roleNameCache = this.roleNames())
        for(let [role, roleName] of names) if(roleName === name) return role
        return -1
    }

    createIndex(row, column, internalId = 0){ return new QModelIndex(row, column, internalId, this) }

    // ---- the protected begin / end pairs, with the persistent indexes ----

    beginInsertRows(parent, first, last){
        this.$aboutToInsert(parent, first, 'row')
        engine.emitArgs(this, AbstractItemModel.SIGNAL_rowsAboutToBeInserted, [parent, first, last])
        this.$changeOp = ['insert', 'row', parent, first, last]
    }
    endInsertRows(){
        let [, , parent, first, last] = this.$changeOp
        this.$changeOp = null
        this.$inserted(parent, first, last, 'row')
        engine.emitArgs(this, AbstractItemModel.SIGNAL_rowsInserted, [parent, first, last])
    }
    beginRemoveRows(parent, first, last){
        engine.emitArgs(this, AbstractItemModel.SIGNAL_rowsAboutToBeRemoved, [parent, first, last])
        this.$aboutToRemove(parent, first, last, 'row')
        this.$changeOp = ['remove', 'row', parent, first, last]
    }
    endRemoveRows(){
        let [, , parent, first, last] = this.$changeOp
        this.$changeOp = null
        this.$removed(parent, first, last, 'row')
        engine.emitArgs(this, AbstractItemModel.SIGNAL_rowsRemoved, [parent, first, last])
    }
    // QAbstractItemModel::beginMoveRows: false (and nothing happens) for a move onto itself or into its own range
    beginMoveRows(sourceParent, first, last, destinationParent, destination){
        if(!allowMove(sourceParent, first, last, destinationParent, destination)) return false
        this.$aboutToMove(sourceParent, first, last, destinationParent, destination, 'row')
        engine.emitArgs(this, AbstractItemModel.SIGNAL_rowsAboutToBeMoved, [sourceParent, first, last, destinationParent, destination])
        this.$changeOp = ['move', 'row', sourceParent, first, last, destinationParent, destination]
        return true
    }
    endMoveRows(){
        let [, , sourceParent, first, last, destinationParent, destination] = this.$changeOp
        this.$changeOp = null
        this.$movedItems(sourceParent, first, last, destinationParent, destination, 'row')
        engine.emitArgs(this, AbstractItemModel.SIGNAL_rowsMoved, [sourceParent, first, last, destinationParent, destination])
    }
    beginInsertColumns(parent, first, last){
        this.$aboutToInsert(parent, first, 'column')
        engine.emitArgs(this, AbstractItemModel.SIGNAL_columnsAboutToBeInserted, [parent, first, last])
        this.$changeOp = ['insert', 'column', parent, first, last]
    }
    endInsertColumns(){
        let [, , parent, first, last] = this.$changeOp
        this.$changeOp = null
        this.$inserted(parent, first, last, 'column')
        engine.emitArgs(this, AbstractItemModel.SIGNAL_columnsInserted, [parent, first, last])
    }
    beginRemoveColumns(parent, first, last){
        engine.emitArgs(this, AbstractItemModel.SIGNAL_columnsAboutToBeRemoved, [parent, first, last])
        this.$aboutToRemove(parent, first, last, 'column')
        this.$changeOp = ['remove', 'column', parent, first, last]
    }
    endRemoveColumns(){
        let [, , parent, first, last] = this.$changeOp
        this.$changeOp = null
        this.$removed(parent, first, last, 'column')
        engine.emitArgs(this, AbstractItemModel.SIGNAL_columnsRemoved, [parent, first, last])
    }
    beginMoveColumns(sourceParent, first, last, destinationParent, destination){
        if(!allowMove(sourceParent, first, last, destinationParent, destination)) return false
        this.$aboutToMove(sourceParent, first, last, destinationParent, destination, 'column')
        engine.emitArgs(this, AbstractItemModel.SIGNAL_columnsAboutToBeMoved, [sourceParent, first, last, destinationParent, destination])
        this.$changeOp = ['move', 'column', sourceParent, first, last, destinationParent, destination]
        return true
    }
    endMoveColumns(){
        let [, , sourceParent, first, last, destinationParent, destination] = this.$changeOp
        this.$changeOp = null
        this.$movedItems(sourceParent, first, last, destinationParent, destination, 'column')
        engine.emitArgs(this, AbstractItemModel.SIGNAL_columnsMoved, [sourceParent, first, last, destinationParent, destination])
    }
    beginResetModel(){
        this.$resetting++
        engine.emitArgs(this, AbstractItemModel.SIGNAL_modelAboutToBeReset, [])
    }
    // QAbstractItemModel::endResetModel: every persistent index becomes invalid
    endResetModel(){
        this.$resetting--
        this.$roleNameCache = null
        for(let p of this.$persistent) p.$index = INVALID
        this.$persistent.clear()
        engine.emitArgs(this, AbstractItemModel.SIGNAL_modelReset, [])
    }
    // the model keeps its persistent indexes current itself between these (changePersistentIndex)
    beginLayoutChange(parents = [], hint = 0){
        engine.emitArgs(this, AbstractItemModel.SIGNAL_layoutAboutToBeChanged, [parents, hint])
    }
    endLayoutChange(parents = [], hint = 0){
        engine.emitArgs(this, AbstractItemModel.SIGNAL_layoutChanged, [parents, hint])
    }
    changePersistentIndex(from, to){
        for(let p of this.$persistent){
            if(sameIndex(p.$index, from)){
                if(to && to.valid) p.$index = to
                else {
                    p.$index = INVALID
                    this.$persistent.delete(p)
                }
            }
        }
    }
    persistentIndexList(){
        return Array.from(this.$persistent, p => p.$index)
    }

    // ---- QAbstractItemModelPrivate: the persistent indexes through the changes ----

    $axis(index, axis){ return axis === 'row' ? index.row : index.column }
    $count(parent, axis){ return axis === 'row' ? this.$rowCount(parent) : this.$columnCount(parent) }
    $reindex(p, row, column, parent){
        p.$index = this.index(row, column, parent)
        if(!p.$index.valid){
            p.$index = INVALID
            this.$persistent.delete(p)
        }
    }

    $aboutToInsert(parent, first, axis){
        let moved = []
        if(first < this.$count(parent, axis)){
            for(let p of this.$persistent){
                let index = p.$index
                if(this.$axis(index, axis) >= first && index.valid && sameIndex(index.parent, parent)) moved.push(p)
            }
        }
        this.$moved.push(moved)
    }

    $inserted(parent, first, last, axis){
        let count = last - first + 1
        for(let p of this.$moved.pop()){
            let old = p.$index
            if(axis === 'row') this.$reindex(p, old.row + count, old.column, parent)
            else this.$reindex(p, old.row, old.column + count, parent)
        }
    }

    $aboutToRemove(parent, first, last, axis){
        let moved = [], invalidated = []
        for(let p of this.$persistent){
            let levelChanged = false
            for(let current = p.$index; current.valid;){
                let currentParent = current.parent
                if(sameIndex(currentParent, parent)){
                    let at = this.$axis(current, axis)
                    if(!levelChanged && at > last) moved.push(p)
                    else if(at <= last && at >= first) invalidated.push(p)
                    break
                }
                current = currentParent
                levelChanged = true
            }
        }
        this.$moved.push(moved)
        this.$invalidated.push(invalidated)
    }

    $removed(parent, first, last, axis){
        let count = last - first + 1
        for(let p of this.$moved.pop()){
            let old = p.$index
            if(axis === 'row') this.$reindex(p, old.row - count, old.column, parent)
            else this.$reindex(p, old.row, old.column - count, parent)
        }
        for(let p of this.$invalidated.pop()){
            p.$index = INVALID
            this.$persistent.delete(p)
        }
    }

    $aboutToMove(srcParent, srcFirst, srcLast, destinationParent, destinationChild, axis){
        let explicit = [], inSource = [], inDestination = []
        let sameParent = sameIndex(srcParent, destinationParent)
        let movingUp = srcFirst > destinationChild
        for(let p of this.$persistent){
            let index = p.$index
            let parent = index.parent
            let isSource = sameIndex(parent, srcParent), isDestination = sameIndex(parent, destinationParent)
            let at = this.$axis(index, axis)
            if(!index.valid || !(isSource || isDestination)) continue
            if(!sameParent && isDestination){
                if(at >= destinationChild) inDestination.push(p)
                continue
            }
            if(sameParent && movingUp && at < destinationChild) continue
            if(sameParent && !movingUp && at < srcFirst) continue
            if(!sameParent && at < srcFirst) continue
            if(sameParent && at > srcLast && at >= destinationChild) continue
            if(at <= srcLast && at >= srcFirst) explicit.push(p)
            else inSource.push(p)
        }
        this.$moved.push(explicit, inSource, inDestination)
    }

    $movedItems(sourceParent, sourceFirst, sourceLast, destinationParent, destinationChild, axis){
        let inDestination = this.$moved.pop(), inSource = this.$moved.pop(), explicit = this.$moved.pop()
        let sameParent = sameIndex(sourceParent, destinationParent)
        let movingUp = sourceFirst > destinationChild
        let explicitChange = !sameParent || movingUp ? destinationChild - sourceFirst : destinationChild - sourceLast - 1
        let sourceChange = !sameParent || !movingUp ? -(sourceLast - sourceFirst + 1) : sourceLast - sourceFirst + 1
        let destinationChange = sourceLast - sourceFirst + 1
        let move = (list, change, parent) => {
            for(let p of list){
                let row = p.$index.row, column = p.$index.column
                if(axis === 'row') row += change
                else column += change
                this.$reindex(p, row, column, parent)
            }
        }
        move(explicit, explicitChange, destinationParent)
        move(inSource, sourceChange, sourceParent)
        move(inDestination, destinationChange, destinationParent)
    }
}

// QAbstractItemModelPrivate::allowMove
function allowMove(srcParent, start, end, destinationParent, destinationStart){
    if(destinationParent && sameIndex(srcParent, destinationParent) && destinationStart >= start && destinationStart <= end + 1) return false
    return true
}

defineType(AbstractItemModel, {
    props: {},
    signals: [
        'dataChanged(topLeft,bottomRight,roles)', 'headerDataChanged(orientation,first,last)',
        'layoutAboutToBeChanged(parents,hint)', 'layoutChanged(parents,hint)',
        'rowsAboutToBeInserted(parent,first,last)', 'rowsInserted(parent,first,last)',
        'rowsAboutToBeRemoved(parent,first,last)', 'rowsRemoved(parent,first,last)',
        'columnsAboutToBeInserted(parent,first,last)', 'columnsInserted(parent,first,last)',
        'columnsAboutToBeRemoved(parent,first,last)', 'columnsRemoved(parent,first,last)',
        'modelAboutToBeReset', 'modelReset',
        'rowsAboutToBeMoved(sourceParent,sourceStart,sourceEnd,destinationParent,destinationRow)',
        'rowsMoved(sourceParent,sourceStart,sourceEnd,destinationParent,destinationRow)',
        'columnsAboutToBeMoved(sourceParent,sourceStart,sourceEnd,destinationParent,destinationColumn)',
        'columnsMoved(sourceParent,sourceStart,sourceEnd,destinationParent,destinationColumn)',
    ],
})
Object.assign(AbstractItemModel, { NoLayoutChangeHint: 0, VerticalSortHint: 1, HorizontalSortHint: 2 })

// QAbstractTableModel: no children, the index of any cell in range
class AbstractTableModel extends AbstractItemModel {
    index(row, column, parent = INVALID){ return this.hasIndex(row, column, parent) ? this.createIndex(row, column) : INVALID }
    parent(child){ return INVALID }
    hasChildren(parent = INVALID){
        if(parent && parent.valid) return false
        return this.$rowCount(parent) > 0 && this.$columnCount(parent) > 0
    }
    sibling(row, column, index){ return this.index(row, column) }
}

// QAbstractListModel: one column
class AbstractListModel extends AbstractTableModel {
    columnCount(parent = INVALID){ return parent && parent.valid ? 0 : 1 }
}

// ---- QQmlAdaptorModel: a number, an array or a ListModel read as a list model ------------------------------------------

const MODELDATA = 0

// A number n: n rows, modelData the row number. An array: its elements as modelData. They have no change signals: a new
// value of the view's model is a new adaptor.
class ListAccessorModel extends AbstractListModel {
    constructor(value){
        super(null)
        this.$value = value
    }
    rowCount(parent = INVALID){
        if(parent && parent.valid) return 0
        let v = this.$value
        if(typeof v === 'number') return Math.max(0, Math.floor(v))
        if(v && typeof v.length === 'number') return v.length
        return v === undefined || v === null ? 0 : 1
    }
    data(index, role = MODELDATA){
        if(!index.valid || role !== MODELDATA) return undefined
        let v = this.$value
        if(typeof v === 'number') return index.row
        if(v && typeof v.length === 'number') return v[index.row]
        return v
    }
    roleNames(){ return new Map([[MODELDATA, 'modelData']]) }
    $rowObject(row){
        let v = this.$value
        let element = typeof v === 'number' ? null : v && typeof v.length === 'number' ? v[row] : v
        return element && typeof element === 'object' ? element : null
    }
}

// A ListModel (QQmlListModel is a QAbstractListModel): its roles, numbered in the order they were made; its changes as
// the signals of an item model
class ListModelItemModel extends AbstractListModel {
    constructor(list){
        super(null)
        this.$list = list
        list.$addView(this)
    }
    rowCount(parent = INVALID){ return parent && parent.valid ? 0 : this.$list.$rows.length }
    roleNames(){
        let names = new Map()
        let i = 0
        for(let name of this.$list.$roles.keys()) names.set(i++, name)
        return names
    }
    data(index, role = 0){
        if(!index.valid) return undefined
        let row = this.$list.$rows[index.row]
        let name = typeof role === 'string' ? role : this.roleNames().get(role)
        return row && name !== undefined ? row[name] : undefined
    }
    setData(index, value, role = 0){
        if(!index.valid) return false
        let name = typeof role === 'string' ? role : this.roleNames().get(role)
        if(name === undefined) return false
        this.$list.setProperty(index.row, name, value)
        return true
    }
    $rowObject(row){ return this.$list.$rows[row] || null }
    // ListModel tells its views after the change: the begin / end pairs run around nothing
    $modelChanged(kind, index, count, roles, to){
        this.$roleNameCache = null
        const ROOT = INVALID
        if(kind === 'insert'){
            this.$fakeCount = this.$list.$rows.length - count
            this.beginInsertRows(ROOT, index, index + count - 1)
            this.$fakeCount = undefined
            this.endInsertRows()
        } else if(kind === 'remove'){
            // the indexes of the removed rows were found from the rows before the change
            this.$fakeCount = this.$list.$rows.length + count
            this.beginRemoveRows(ROOT, index, index + count - 1)
            this.$fakeCount = undefined
            this.endRemoveRows()
        } else if(kind === 'move'){
            // ListModel::move(from, to, n): to is where the rows start afterwards; Qt's destination is before the move
            let destination = to > index ? to + count : to
            if(this.beginMoveRows(ROOT, index, index + count - 1, ROOT, destination)) this.endMoveRows()
        } else if(kind === 'change'){
            let roleIds = roles ? roles.map(name => this.$roleOf(name)).filter(r => r >= 0) : []
            engine.emitArgs(this, AbstractItemModel.SIGNAL_dataChanged, [this.index(index, 0), this.index(index + count - 1, 0), roleIds])
        } else {
            this.beginResetModel()
            this.endResetModel()
        }
    }
    hasIndex(row, column, parent = INVALID){
        if(row < 0 || column < 0 || (parent && parent.valid)) return false
        let rows = this.$fakeCount !== undefined ? this.$fakeCount : this.$list.$rows.length
        return row < rows && column < 1
    }
    index(row, column, parent = INVALID){ return this.hasIndex(row, column, parent) ? this.createIndex(row, column) : INVALID }
    $count(parent, axis){ return axis === 'row' && this.$fakeCount !== undefined ? this.$fakeCount : super.$count(parent, axis) }
    $release(){ this.$list.$removeView(this) }
}

// the item model of a value given to a view's model property (QQmlAdaptorModel::setModel)
function adaptModel(value){
    if(value instanceof AbstractItemModel) return { model: value, owned: false }
    // one item model a ListModel: a TableView and the ItemSelectionModel of it see the same indexes
    if(value instanceof ListModel) return { model: value.$itemModel || (value.$itemModel = new ListModelItemModel(value)), owned: false }
    if(value === undefined || value === null) return { model: null, owned: false }
    return { model: new ListAccessorModel(value), owned: true }
}

function releaseAdaptor(adaptor){
    if(adaptor && adaptor.owned && adaptor.model && adaptor.model.$release) adaptor.model.$release()
}

module.exports = {
    QModelIndex, QPersistentModelIndex, INVALID, sameIndex, indexLess, AbstractItemModel, AbstractTableModel, AbstractListModel,
    ListAccessorModel, ListModelItemModel, adaptModel, releaseAdaptor, DEFAULT_ROLE_NAMES,
}
