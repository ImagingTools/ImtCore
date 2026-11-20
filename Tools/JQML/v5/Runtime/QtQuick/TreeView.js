// TreeView (qquicktreeview.cpp of Qt 6.12) over QQmlTreeModelToTableModel (qqmltreemodeltotablemodel.cpp): the tree
// model is shown as a table of its visible rows, a row's children following it while it is expanded. The table is a
// TableView of that proxy; the selection model, index(), cellAtIndex() and the editor work on the indexes of the tree.
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { QPoint } = require('../Core/ValueTypes')
const { TableView, ENUMS } = require('./TableView')
const { AbstractItemModel, QPersistentModelIndex, INVALID, sameIndex } = require('../QtQml/Models/ItemModel')
const { ItemSelectionModel, QItemSelection } = require('../QtQml/Models/ItemSelectionModel')
const QtEnums = require('../Qt/enums')

// the roles the proxy names in its dataChanged (QQmlTreeModelToTableModel::TreeModelRoles)
const DepthRole = QtEnums.UserRole - 5, ExpandedRole = DepthRole + 1, HasChildrenRole = DepthRole + 2, ModelIndexRole = DepthRole + 4

// ---- QQmlTreeModelToTableModel --------------------------------------------------------------------------------------------
class TreeModelToTableModel extends AbstractItemModel {
    constructor(){
        super(null)
        this.$model = null
        this.$items = []                // { index: QPersistentModelIndex, depth, expanded }
        this.$expanded = []             // the expanded indexes (QSet<QPersistentModelIndex>)
        this.$itemsToExpand = []
        this.$rootIndex = new QPersistentModelIndex(INVALID)
        this.$links = []
        this.$aggregating = 0
        this.$queued = []
        this.$visibleRowsMoved = false
        this.$modelLayoutChanged = false
    }

    get model(){ return this.$model }

    $expandedHas(index){ return this.$expanded.some(p => p.valid && sameIndex(p.index, index)) }
    $expandedAdd(index){ if(!this.$expandedHas(index)) this.$expanded.push(new QPersistentModelIndex(index)) }
    $expandedRemove(index){
        let at = this.$expanded.findIndex(p => p.valid && sameIndex(p.index, index))
        if(at >= 0) this.$expanded.splice(at, 1)[0].release()
    }
    $item(index, depth = 0, expanded = false){ return { index: new QPersistentModelIndex(index), depth, expanded } }

    setModel(model){
        if(this.$model === model) return
        for(let [object, signal, fn] of this.$links) engine.disconnect(object, signal, fn)
        this.$links = []
        this.$clearModelData()
        this.$model = model
        if(this.$rootIndex.valid && this.$rootIndex.model !== model) this.$rootIndex = new QPersistentModelIndex(INVALID)
        if(model){
            let M = AbstractItemModel
            let link = (signal, fn) => {
                engine.connect(model, signal, fn)
                this.$links.push([model, signal, fn])
            }
            link(M.SIGNAL_modelReset, () => { this.$clearModelData(); this.$showModelTopLevelItems() })
            link(M.SIGNAL_dataChanged, (tl, br, roles) => this.$modelDataChanged(tl, br, roles))
            link(M.SIGNAL_layoutAboutToBeChanged, (parents) => this.$modelLayoutAboutToBeChanged(parents))
            link(M.SIGNAL_layoutChanged, (parents) => this.$modelLayoutChanged(parents))
            link(M.SIGNAL_rowsInserted, (p, s, e) => this.$modelRowsInserted(p, s, e))
            link(M.SIGNAL_rowsAboutToBeRemoved, (p, s, e) => this.$modelRowsAboutToBeRemoved(p, s, e))
            link(M.SIGNAL_rowsRemoved, (p) => this.$modelRowsRemoved(p))
            link(M.SIGNAL_rowsAboutToBeMoved, (sp, ss, se, dp, dr) => this.$modelRowsAboutToBeMoved(sp, ss, se, dp, dr))
            link(M.SIGNAL_rowsMoved, (sp, ss, se, dp, dr) => this.$modelRowsMoved(sp, ss, se, dp, dr))
            link(M.SIGNAL_columnsAboutToBeInserted, (p, s, e) => this.beginInsertColumns(INVALID, s, e))
            link(M.SIGNAL_columnsAboutToBeRemoved, (p, s, e) => this.beginRemoveColumns(INVALID, s, e))
            link(M.SIGNAL_columnsInserted, () => { this.endInsertColumns(); this.$items = []; this.$showModelTopLevelItems() })
            link(M.SIGNAL_columnsRemoved, () => { this.endRemoveColumns(); this.$items = []; this.$showModelTopLevelItems() })
            this.$showModelTopLevelItems()
        }
    }

    $clearModelData(){
        this.beginResetModel()
        this.$items = []
        this.$expanded = []
        this.endResetModel()
    }

    get rootIndex(){ return this.$rootIndex.index }
    setRootIndex(index){
        index = index || INVALID
        if(sameIndex(this.$rootIndex.index, index)) return
        if(this.$model) this.$clearModelData()
        this.$rootIndex = new QPersistentModelIndex(index)
        if(this.$model) this.$showModelTopLevelItems()
        if(this.onRootIndexChanged) this.onRootIndexChanged()
    }

    index(row, column, parent = INVALID){ return this.hasIndex(row, column, parent) ? this.createIndex(row, column) : INVALID }
    parent(){ return INVALID }
    roleNames(){ return this.$model ? this.$model.roleNames() : super.roleNames() }
    $roleOf(name){ return this.$model ? this.$model.$roleOf(name) : super.$roleOf(name) }
    get $roleNameCache(){ return this.$model ? (this.$model.$roleNameCache || (this.$model.$roleNameCache = this.$model.roleNames())) : null }
    set $roleNameCache(value){}
    rowCount(){ return this.$model ? this.$items.length : 0 }
    $rowCount(){ return this.$model ? this.$items.length : 0 }
    $columnCount(parent = INVALID){ return this.$model ? this.$model.$columnCount(parent) : 0 }
    columnCount(parent = INVALID){ return this.$columnCount(parent) }
    data(index, role){ return this.$model ? this.$model.data(this.mapToModel(index), role) : undefined }
    setData(index, value, role){ return this.$model ? this.$model.setData(this.mapToModel(index), value, role) : false }
    headerData(section, orientation, role){ return this.$model.headerData(section, orientation, role) }
    flags(index){ return this.$model.flags(this.mapToModel(index)) }

    depthAtRow(row){ return row < 0 || row >= this.$items.length ? 0 : this.$items[row].depth }

    itemIndex(index){
        if(!index || !index.valid || sameIndex(index, this.$rootIndex.index) || !this.$items.length) return -1
        for(let i = 0; i < this.$items.length; i++) if(sameIndex(this.$items[i].index.index, index)) return i
        return -1
    }
    $isVisible(index){ return this.itemIndex(index) !== -1 }
    $childrenVisible(index){
        return (sameIndex(index, this.$rootIndex.index) && this.$items.length > 0) || (this.$expandedHas(index) && this.$isVisible(index))
    }

    mapToModel(index){
        if(typeof index === 'number'){
            let row = index
            return row < 0 || row >= this.$items.length ? INVALID : this.$items[row].index.index
        }
        if(!index || !index.valid) return INVALID
        let row = index.row
        if(row < 0 || row > this.$items.length - 1) return INVALID
        let source = this.$items[row].index.index
        return this.$model.index(source.row, index.column, source.parent)
    }

    mapFromModel(index){
        if(!index || !index.valid) return INVALID
        let parent = index.parent
        for(let i = 0; i < this.$items.length; i++){
            let p = this.$items[i].index.index
            if(p.row === index.row && sameIndex(p.parent, parent)) return this.index(i, index.column)
        }
        return INVALID
    }

    $showModelTopLevelItems(doInsertRows = true){
        if(!this.$model) return
        let root = this.$rootIndex.index
        let count = this.$model.$rowCount(root)
        if(count === 0) return
        this.$showModelChildItems({ index: { index: root }, depth: 0, expanded: true }, 0, count - 1, doInsertRows)
    }

    $showModelChildItems(parentItem, start, end, doInsertRows = true, doExpandPendingRows = true){
        let parentIndex = parentItem.index.index
        let root = this.$rootIndex.index
        let isRoot = !parentIndex.valid || sameIndex(parentIndex, root)
        let rowIdx = !isRoot ? this.itemIndex(parentIndex) + 1 : 0
        if(!isRoot && (rowIdx === 0 || !parentItem.expanded)) return
        if(this.$model.$rowCount(parentIndex) === 0) return
        let insertCount = end - start + 1
        let startIdx
        if(start === 0) startIdx = rowIdx
        else {
            let next = this.$model.index(end + 1, 0, parentIndex)
            if(next.valid) startIdx = this.itemIndex(next)
            else startIdx = this.$lastChildIndex(this.$model.index(start - 1, 0, parentIndex)) + 1
        }
        let rowDepth = rowIdx === 0 ? 0 : parentItem.depth + 1
        if(doInsertRows) this.beginInsertRows(INVALID, startIdx, startIdx + insertCount - 1)
        for(let i = 0; i < insertCount; i++){
            let cmi = this.$model.index(start + i, 0, parentIndex)
            let expanded = this.$expandedHas(cmi)
            let item = this.$item(cmi, rowDepth, expanded)
            this.$items.splice(startIdx + i, 0, item)
            if(expanded) this.$itemsToExpand.push(item)
        }
        if(doInsertRows) this.endInsertRows()
        if(doExpandPendingRows) this.$expandPendingRows(doInsertRows)
    }

    expand(index){
        if(!this.$model || !index || !index.valid || !this.$model.hasChildren(index) || this.$expandedHas(index)) return
        let row = this.itemIndex(index)
        if(row !== -1) this.expandRow(row)
        else this.$expandedAdd(index)
    }

    collapse(index){
        if(!this.$model || !index || !index.valid || !this.$model.hasChildren(index) || !this.$expandedHas(index)) return
        let row = this.itemIndex(index)
        if(row !== -1) this.collapseRow(row)
        else this.$expandedRemove(index)
    }

    isExpanded(row){ return row < 0 || row >= this.$items.length ? false : this.$items[row].expanded }
    hasChildren(row){
        if(typeof row !== 'number') return super.hasChildren(row)
        return row < 0 || row >= this.$items.length ? false : this.$model.hasChildren(this.$items[row].index.index)
    }

    expandRow(n){
        if(!this.$model || this.isExpanded(n)) return
        let item = this.$items[n]
        let index = item.index.index
        if((this.$model.flags(index) & QtEnums.ItemNeverHasChildren) || !this.$model.hasChildren(index)) return
        item.expanded = true
        this.$expandedAdd(index)
        engine.emitArgs(this, AbstractItemModel.SIGNAL_dataChanged, [this.index(n, 0), this.index(n, 0), [ExpandedRole]])
        this.$itemsToExpand.push(item)
        this.$expandPendingRows()
    }

    expandRecursively(row, depth){
        let startDepth = this.depthAtRow(row)
        let help = (index) => {
            let rowToExpand = this.itemIndex(index)
            if(!this.$expandedHas(index)) this.expandRow(rowToExpand)
            if(depth !== -1 && this.depthAtRow(rowToExpand) === startDepth + depth - 1) return
            let count = this.$model.$rowCount(index)
            for(let childRow = 0; childRow < count; childRow++){
                let child = this.$model.index(childRow, 0, index)
                if(this.$model.hasChildren(child)) help(child)
            }
        }
        let index = this.$items[row].index.index
        if(index.valid) help(index)
    }

    $expandPendingRows(doInsertRows = true){
        while(this.$itemsToExpand.length){
            let item = this.$itemsToExpand.shift()
            let count = this.$model.$rowCount(item.index.index)
            if(count === 0) continue
            this.$showModelChildItems(item, 0, count - 1, doInsertRows, false)
        }
    }

    collapseRecursively(row){
        let help = (index) => {
            if(this.$expandedHas(index)){
                let rowToCollapse = this.itemIndex(index)
                if(rowToCollapse !== -1) this.collapseRow(rowToCollapse)
                else this.$expandedRemove(index)
            }
            let count = this.$model.$rowCount(index)
            for(let childRow = 0; childRow < count; childRow++){
                let child = this.$model.index(childRow, 0, index)
                if(this.$model.hasChildren(child)) help(child)
            }
        }
        let index = this.$items[row].index.index
        if(index.valid) help(index)
    }

    collapseRow(n){
        if(!this.$model || !this.isExpanded(n)) return
        this.$aggregating++
        try {
            let item = this.$items[n]
            let index = item.index.index
            item.expanded = false
            this.$expandedRemove(index)
            this.$queueDataChanged(n, n, [ExpandedRole])
            let count = this.$model.$rowCount(index)
            if((this.$model.flags(index) & QtEnums.ItemNeverHasChildren) || !this.$model.hasChildren(index) || count === 0) return
            let last = this.$lastChildIndex(this.$model.index(count - 1, 0, index))
            this.$removeVisibleRows(n + 1, last)
        } finally {
            if(--this.$aggregating === 0) this.$emitQueuedSignals()
        }
    }

    $lastChildIndex(index){
        if(!this.$expandedHas(index)) return this.itemIndex(index)
        let parent = index.parent
        let next = INVALID
        while(parent.valid){
            next = parent.sibling(parent.row + 1, 0)
            if(next.valid) break
            parent = parent.parent
        }
        let first = next.valid ? this.itemIndex(next) : this.$items.length
        return first - 1
    }

    $removeVisibleRows(startIndex, endIndex, doRemoveRows = true){
        if(startIndex < 0 || endIndex < 0 || startIndex > endIndex) return
        if(doRemoveRows) this.beginRemoveRows(INVALID, startIndex, endIndex)
        let removed = this.$items.splice(startIndex, endIndex - startIndex + 1)
        for(let item of removed) item.index.release()
        if(doRemoveRows){
            this.endRemoveRows()
            let last = this.$items.length - 1
            if(startIndex <= last) this.$queueDataChanged(startIndex, last, [ModelIndexRole])
        }
    }

    $modelDataChanged(topLeft, bottomRight, roles){
        let parent = topLeft.parent
        if(parent.valid && !this.$childrenVisible(parent)) return
        let topIndex = this.itemIndex(topLeft.siblingAtColumn(0))
        if(topIndex === -1) return
        for(let i = topLeft.row; i <= bottomRight.row; i++){
            let bottomIndex = topIndex
            while(bottomIndex < this.$items.length){
                let idx = this.$items[bottomIndex].index.index
                if(!sameIndex(idx.parent, parent)){
                    bottomIndex--
                    break
                }
                if(idx.row === bottomRight.row) break
                bottomIndex++
            }
            engine.emitArgs(this, AbstractItemModel.SIGNAL_dataChanged, [this.index(topIndex, topLeft.column), this.index(bottomIndex, bottomRight.column), roles])
            i += bottomIndex - topIndex
            if(i === bottomRight.row) break
            topIndex = bottomIndex + 1
            while(topIndex < this.$items.length && !sameIndex(this.$items[topIndex].index.index.parent, parent)) topIndex++
        }
    }

    $modelLayoutAboutToBeChanged(parents){
        this.$modelLayoutChanged = false
        if(!parents || !parents.length || !(parents[0] && parents[0].valid)){
            this.beginLayoutChange()
            this.$modelLayoutChanged = true
            this.$items = []
            return
        }
        for(let p of parents){
            let index = p.index || p
            if(!this.$expandedHas(index)) continue
            let row = this.itemIndex(index)
            if(row === -1) continue
            let count = this.$model.$rowCount(index)
            if(count === 0) continue
            if(!this.$modelLayoutChanged){
                this.beginLayoutChange()
                this.$modelLayoutChanged = true
            }
            this.$removeVisibleRows(row + 1, this.$lastChildIndex(this.$model.index(count - 1, 0, index)), false)
        }
    }

    $modelLayoutChanged(parents){
        if(!this.$modelLayoutChanged) return
        if(!this.$items.length){
            this.$showModelTopLevelItems(false)
            let columns = this.$model.$columnCount(this.$model.index(0, 0))
            engine.emitArgs(this, AbstractItemModel.SIGNAL_dataChanged, [this.index(0, 0), this.index(this.$items.length - 1, columns - 1), []])
            this.endLayoutChange()
            return
        }
        for(let p of parents || []){
            let index = p.index || p
            if(!this.$expandedHas(index)) continue
            let row = this.itemIndex(index)
            if(row === -1) continue
            let count = this.$model.$rowCount(index)
            if(count === 0) continue
            let lmi = this.$model.index(count - 1, 0, index)
            let columns = this.$model.$columnCount(lmi)
            this.$showModelChildItems(this.$items[row], 0, count - 1, false)
            engine.emitArgs(this, AbstractItemModel.SIGNAL_dataChanged, [this.index(row + 1, 0), this.index(this.$lastChildIndex(lmi), columns - 1), []])
        }
        this.endLayoutChange()
    }

    $modelRowsInserted(parent, start, end){
        let item
        let parentRow = this.itemIndex(parent)
        if(parentRow >= 0){
            this.$queueDataChanged(parentRow, parentRow, [HasChildrenRole])
            item = this.$items[parentRow]
            if(!item.expanded) return
        } else if(sameIndex(parent, this.$rootIndex.index)){
            item = { index: { index: parent }, depth: 0, expanded: true }
        } else return
        this.$showModelChildItems(item, start, end)
    }

    $modelRowsAboutToBeRemoved(parent, start, end){
        this.$aggregating++
        if(sameIndex(parent, this.$rootIndex.index) || this.$childrenVisible(parent)){
            let startIndex = this.itemIndex(this.$model.index(start, 0, parent))
            let emi = this.$model.index(end, 0, parent)
            let endIndex = -1
            if(this.$expandedHas(emi) || !emi.valid){
                let count = this.$model.$rowCount(emi)
                if(count > 0) endIndex = this.$lastChildIndex(this.$model.index(count - 1, 0, emi))
            }
            if(endIndex === -1) endIndex = this.itemIndex(emi)
            this.$removeVisibleRows(startIndex, endIndex)
        }
        for(let r = start; r <= end; r++) this.$expandedRemove(this.$model.index(r, 0, parent))
    }

    $modelRowsRemoved(parent){
        let parentRow = this.itemIndex(parent)
        if(parentRow >= 0) this.$queueDataChanged(parentRow, parentRow, [HasChildrenRole])
        if(--this.$aggregating === 0) this.$emitQueuedSignals()
    }

    $modelRowsAboutToBeMoved(sourceParent, sourceStart, sourceEnd, destinationParent, destinationRow){
        this.$aggregating++
        this.$visibleRowsMoved = false
        if(!this.$childrenVisible(sourceParent)) return
        if(!this.$childrenVisible(destinationParent)){
            this.$modelRowsAboutToBeRemoved(sourceParent, sourceStart, sourceEnd)
            this.$aggregating--
            if(this.$isVisible(destinationParent) && this.$model.$rowCount(destinationParent) === 0){
                let parentRow = this.itemIndex(destinationParent)
                this.$queueDataChanged(parentRow, parentRow, [HasChildrenRole])
            }
            return
        }
        let depthDifference = -1
        if(destinationParent.valid) depthDifference = this.$items[this.itemIndex(destinationParent)].depth
        if(sourceParent.valid) depthDifference -= this.$items[this.itemIndex(sourceParent)].depth
        else depthDifference++
        let startIndex = this.itemIndex(this.$model.index(sourceStart, 0, sourceParent))
        let emi = this.$model.index(sourceEnd, 0, sourceParent)
        let endIndex = -1
        if(this.$expandedHas(emi)){
            let count = this.$model.$rowCount(emi)
            if(count > 0) endIndex = this.$lastChildIndex(this.$model.index(count - 1, 0, emi))
        }
        if(endIndex === -1) endIndex = this.itemIndex(emi)
        let destIndex
        if(destinationRow === this.$model.$rowCount(destinationParent)) destIndex = this.$lastChildIndex(this.$model.index(destinationRow - 1, 0, destinationParent)) + 1
        else destIndex = this.itemIndex(this.$model.index(destinationRow, 0, destinationParent))
        let total = endIndex - startIndex + 1
        this.$visibleRowsMoved = startIndex !== destIndex && this.beginMoveRows(INVALID, startIndex, endIndex, INVALID, destIndex)
        let buffer = this.$items.slice(startIndex, startIndex + total)
        let offset
        if(destIndex > endIndex){
            for(let i = endIndex + 1; i < destIndex; i++) this.$items[i - total] = this.$items[i]
            offset = destIndex - total
        } else {
            for(let i = startIndex - 1; i >= destIndex; i--) this.$items[i + total] = this.$items[i]
            offset = destIndex
        }
        for(let i = 0; i < buffer.length; i++){
            buffer[i].depth += depthDifference
            this.$items[offset + i] = buffer[i]
        }
        let top = Math.min(startIndex, offset)
        let bottom = Math.max(endIndex, offset + total - 1)
        if(!sameIndex(sourceParent, destinationParent)){
            let bottomParent = bottom === endIndex ? sourceParent : destinationParent
            let count = this.$model.$rowCount(bottomParent)
            if(count > 0) bottom = Math.max(bottom, this.$lastChildIndex(this.$model.index(count - 1, 0, bottomParent)))
        }
        this.$queueDataChanged(top, bottom, [ModelIndexRole])
        if(depthDifference !== 0) this.$queueDataChanged(offset, offset + total - 1, [DepthRole])
    }

    $modelRowsMoved(sourceParent, sourceStart, sourceEnd, destinationParent, destinationRow){
        if(!this.$childrenVisible(sourceParent)) this.$modelRowsInserted(destinationParent, destinationRow, destinationRow + sourceEnd - sourceStart)
        else if(!this.$childrenVisible(destinationParent)){
            let parentRow = this.itemIndex(sourceParent)
            if(parentRow >= 0) this.$queueDataChanged(parentRow, parentRow, [HasChildrenRole])
        }
        if(this.$visibleRowsMoved) this.endMoveRows()
        if(this.$isVisible(sourceParent) && this.$model.$rowCount(sourceParent) === 0){
            let parentRow = this.itemIndex(sourceParent)
            this.collapseRow(parentRow)
            this.$queueDataChanged(parentRow, parentRow, [ExpandedRole, HasChildrenRole])
        }
        if(--this.$aggregating === 0) this.$emitQueuedSignals()
    }

    $queueDataChanged(top, bottom, roles){
        if(this.$aggregating) this.$queued.push({ top, bottom, roles: roles.slice() })
        else engine.emitArgs(this, AbstractItemModel.SIGNAL_dataChanged, [this.index(top, 0), this.index(bottom, 0), roles])
    }

    // QQmlTreeModelToTableModel::emitQueuedSignals: overlapping changes merged
    $emitQueuedSignals(){
        let combined = []
        for(let change of this.$queued){
            let merged = false
            for(let c of combined){
                if((change.top <= c.top && change.bottom >= c.top) || (change.top <= c.bottom && change.bottom >= c.bottom)){
                    if(change.top < c.top) c.top = change.top
                    if(change.bottom > c.bottom) c.bottom = change.bottom
                    for(let role of change.roles) if(c.roles.indexOf(role) < 0) c.roles.push(role)
                    merged = true
                    break
                }
            }
            if(!merged) combined.push(change)
        }
        this.$queued = []
        for(let c of combined) engine.emitArgs(this, AbstractItemModel.SIGNAL_dataChanged, [this.index(c.top, 0), this.index(c.bottom, 0), c.roles])
    }
}

// ---- TreeView -----------------------------------------------------------------------------------------------------------
const TREE_COLUMN = 0

class TreeView extends TableView {
    constructor(parent = null){
        super(parent)
        this.$proxy = new TreeModelToTableModel()
        this.$treeModel = null
        this.$v[TableView.SELECTIONBEHAVIOR] = ENUMS.SelectRows
        this.$v[TableView.EDITTRIGGERS] = ENUMS.EditKeyPressed
        // QQuickTableViewPrivate::setModelImpl(the proxy): the table of the tree view is the proxy
        this.$assignedModel = this.$proxy
        this.$needsModelSynchronization = true
        engine.connect(this.$proxy, AbstractItemModel.SIGNAL_dataChanged, (tl, br) => this.$treeDataChanged(tl, br))
        this.$proxy.onRootIndexChanged = () => engine.emitArgs(this, TreeView.SIGNAL_rootIndexChanged, [])
    }

    $modelValue(){ return this.$treeModel }
    $selectionSourceModel(){ return this.$treeModel instanceof AbstractItemModel ? this.$treeModel : null }
    $assignedIsItemModel(){ return this.$treeModel instanceof AbstractItemModel }

    // QQuickTreeViewPrivate::setModelImpl: an item model only
    $setModelValue(value){
        if(value === this.$treeModel) return
        this.closeEditor()
        this.$treeModel = value === undefined ? null : value
        if(value === null || value === undefined) this.$proxy.setModel(null)
        else if(value instanceof AbstractItemModel) this.$proxy.setModel(value)
        else console.warn('TreeView only accepts a model of type QAbstractItemModel')
        this.$scheduleRebuildTable(1)
        if(this.$selectionModel) this.$selectionModel.$setModel(this.$selectionSourceModel())
    }

    // the cells of the proxy, the indexes of the tree
    $modelIndexOfCell(cell){
        let tableIndex = this.$proxy.index(cell.y, cell.x)
        return this.$proxy.mapToModel(tableIndex)
    }
    cellAtIndex(index){
        let tableIndex = this.$proxy.mapFromModel(index)
        return new QPoint(tableIndex.column, tableIndex.row)
    }
    modelIndex(cell, column){
        if(typeof cell === 'number'){
            console.warn('TreeView: modelIndex(row, column) is deprecated. Use index(row, column) instead. For more information, see https://doc.qt.io/qt-6/qml-qtquick-tableview-obsolete.html')
            return this.$modelIndexOfCell({ x: column, y: cell })
        }
        return this.$modelIndexOfCell(cell)
    }

    // QQuickTreeViewPrivate::dataChangedCallback: the tree properties of the cells of the changed rows
    $treeDataChanged(topLeft, bottomRight){
        for(let row = topLeft.row; row <= bottomRight.row; row++){
            for(let column = topLeft.column; column <= bottomRight.column; column++){
                let item = this.itemAtCell({ x: column, y: row })
                if(!item) continue
                this.$updateItemProperties(column * this.$proxy.$rowCount() + row, item, false)
            }
        }
    }

    $updateItemProperties(flat, item, init, ctxArg){
        super.$updateItemProperties(flat, item, init, ctxArg)
        let ctx = ctxArg || (item.$tableEntry && item.$tableEntry.ctx) || item.$editCtx
        if(!ctx) return
        let rows = this.$proxy.$rowCount()
        let column = rows ? Math.floor(flat / rows) : 0, row = rows ? flat % rows : 0
        ctx.$setTableRequired('treeView', this)
        ctx.$setTableRequired('isTreeNode', column === TREE_COLUMN)
        ctx.$setTableRequired('hasChildren', this.$proxy.hasChildren(row))
        ctx.$setTableRequired('expanded', this.isExpanded(row))
        ctx.$setTableRequired('depth', this.$proxy.depthAtRow(row))
    }

    // QQuickTreeViewPrivate::updateSelection: whole rows of the table, as indexes of the tree
    $updateSelection(oldSel, newSel){
        if(oldSel.x === newSel.x && oldSel.y === newSel.y && oldSel.w === newSel.w && oldSel.h === newSel.h) return
        let select = new QItemSelection(), deselect = new QItemSelection()
        let x = newSel.w < 0 ? newSel.x + newSel.w : newSel.x, y = newSel.h < 0 ? newSel.y + newSel.h : newSel.y
        let w = Math.abs(newSel.w), h = Math.abs(newSel.h)
        for(let row = y; row <= y + h; row++){
            select.merge(new QItemSelection(this.index(row, x), this.index(row, x + w)), ItemSelectionModel.Select)
        }
        for(let index of this.$selectionModel.selection.indexes()){
            if(!select.contains(index) && !this.$existingSelection.contains(index)) deselect.merge(new QItemSelection(index, index), ItemSelectionModel.Select)
        }
        let sm = this.$selectionModel
        if(this.$selectionFlag === ItemSelectionModel.Select){
            sm.select(deselect, ItemSelectionModel.Deselect)
            sm.select(select, ItemSelectionModel.Select)
        } else {
            let old = this.$existingSelection.clone()
            old.merge(select, ItemSelectionModel.Deselect)
            sm.select(old, ItemSelectionModel.Select)
            sm.select(select, ItemSelectionModel.Deselect)
        }
    }

    // ---- the tree (Q_INVOKABLE) ----
    // QQuickTreeView::setRootIndex
    $setRootIndex(index){
        this.$proxy.setRootIndex(index)
        this.positionViewAtCell(new QPoint(0, 0), ENUMS.AlignTop | ENUMS.AlignLeft)
    }
    resetRootIndex(){
        this.$proxy.setRootIndex(INVALID)
        this.positionViewAtCell(new QPoint(0, 0), ENUMS.AlignTop | ENUMS.AlignLeft)
    }

    depth(row){ return row < 0 || row >= this.$proxy.$rowCount() ? -1 : this.$proxy.depthAtRow(row) }
    isExpanded(row){ return row < 0 || row >= this.$proxy.$rowCount() ? false : this.$proxy.isExpanded(row) }

    $setExpandedOnRow(row, expanded){
        for(let c = this.leftColumn; c <= this.rightColumn; c++){
            let item = this.itemAtCell({ x: c, y: row })
            if(item) this.$setRequired(item, 'expanded', expanded)
        }
    }

    expand(row){ if(row >= 0) this.expandRecursively(row, 1) }

    expandRecursively(row = -1, depth = -1){
        let proxy = this.$proxy
        if(row >= proxy.$rowCount()) return
        if(row < 0 && row !== -1) return
        if(depth === 0 || depth < -1) return
        let expandRow = (startRow) => {
            proxy.expandRecursively(startRow, depth)
            this.$setExpandedOnRow(startRow, true)
        }
        if(row >= 0){
            if(proxy.isExpanded(row) && depth === 1) return
            expandRow(row)
        } else if(proxy.model){
            for(let r = 0; r < proxy.model.$rowCount(); r++){
                let rootRow = proxy.itemIndex(proxy.model.index(r, 0))
                if(rootRow !== -1) expandRow(rootRow)
            }
        }
        engine.emitArgs(this, TreeView.SIGNAL_expanded, [row, depth])
    }

    expandToIndex(index){
        let proxy = this.$proxy
        if(!index || !index.valid){
            console.warn('TreeView: index is not valid: ' + index)
            return
        }
        if(index.model !== proxy.model){
            console.warn("TreeView: index doesn't belong to correct model: " + index)
            return
        }
        if(this.rowAtIndex(index) !== -1) return
        let depth = 1
        let parent = index.parent
        let row = this.rowAtIndex(parent)
        while(parent.valid){
            if(row !== -1){
                proxy.expandRow(row)
                this.$setExpandedOnRow(row, true)
                break
            }
            proxy.expand(parent)
            parent = parent.parent
            row = this.rowAtIndex(parent)
            depth++
        }
        engine.emitArgs(this, TreeView.SIGNAL_expanded, [row, depth])
    }

    collapse(row){
        let proxy = this.$proxy
        if(row < 0 || row >= proxy.$rowCount() || !proxy.isExpanded(row)) return
        proxy.collapseRow(row)
        this.$setExpandedOnRow(row, false)
        engine.emitArgs(this, TreeView.SIGNAL_collapsed, [row, false])
    }

    collapseRecursively(row = -1){
        let proxy = this.$proxy
        if(row >= proxy.$rowCount()) return
        if(row < 0 && row !== -1) return
        let collapseRow = (startRow) => {
            proxy.collapseRecursively(startRow)
            this.$setExpandedOnRow(startRow, false)
        }
        if(row >= 0) collapseRow(row)
        else if(proxy.model){
            for(let r = 0; r < proxy.model.$rowCount(); r++){
                let rootRow = proxy.itemIndex(proxy.model.index(r, 0))
                if(rootRow !== -1) collapseRow(rootRow)
            }
        }
        engine.emitArgs(this, TreeView.SIGNAL_collapsed, [row, true])
    }

    toggleExpanded(row){
        if(this.isExpanded(row)) this.collapse(row)
        else this.expand(row)
    }

    // QQuickTreeView::keyPressEvent: Left collapses, Right expands the current row; the rest is TableView's
    $keyEvent(event, kind){
        if(kind !== 'pressed') return 0
        if(!this.$v[TableView.KEYNAVIGATIONENABLED] || !this.$selectionModel) return 0
        let row = this.cellAtIndex(this.$selectionModel.currentIndex).y
        if(event.key === QtEnums.Key_Left){
            this.collapse(row)
            return 1
        }
        if(event.key === QtEnums.Key_Right){
            this.expand(row)
            return 1
        }
        return super.$keyEvent(event, kind)
    }

    // the TapHandler of QQuickTreeView: a double tap without modifiers toggles the row, unless it edits
    $doubleTapped(x, y, modifiers){
        if(modifiers) return
        if(!this.$v[TableView.POINTERNAVIGATIONENABLED]) return
        if(this.$v[TableView.EDITTRIGGERS] & ENUMS.DoubleTapped) return
        let row = this.cellAtPosition({ x: x + this.$contentX(), y: y + this.$contentY() }).y
        this.toggleExpanded(row)
    }

    $disposeNative(){
        this.$proxy.setModel(null)
        super.$disposeNative()
    }
}
defineType(TreeView, {
    props: {
        rootIndex: { kind: 'var', effect: (view, value) => view.$setRootIndex(value), get(){
            engine.track(this, TreeView.SIGNAL_rootIndexChanged)
            return this.$proxy.rootIndex
        } },
    },
    signals: ['expanded(row,depth)', 'collapsed(row,recursively)'],
})
Object.assign(TreeView, ENUMS)

module.exports = { TreeView, TreeModelToTableModel }
