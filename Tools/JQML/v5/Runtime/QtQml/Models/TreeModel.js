// TreeModel of Qt.labs.qmlmodels (qqmltreemodel.cpp, qqmltreerow.cpp; Qt 6.10+): rows of JavaScript objects whose
// children are the array under their "rows" key, the columns and roles from TableModelColumns as in TableModel. A model
// index of the tree points at its row (the internal pointer).
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { AbstractItemModel, INVALID } = require('./ItemModel')
const { AbstractColumnModel, toVariant } = require('./TableModel')

const ROWS = 'rows'

// QQmlTreeRow: its fields (a QVariantMap: sorted keys) and its children
class TreeRow {
    constructor(data, parent = null){
        this.parent = parent
        this.dataMap = {}
        this.children = []
        if(data !== undefined) this.unpack(data)
    }
    // QQmlTreeRow::unpackVariantMap: "rows" holding an array makes children, any other key is a field
    unpack(map){
        let pending = [[this, map]]
        while(pending.length){
            let [row, value] = pending.pop()
            let source = toVariant(value) || {}
            for(let key of Object.keys(source)){
                let v = source[key]
                if(key === ROWS && Array.isArray(v)){
                    for(let childValue of v){
                        let child = new TreeRow(undefined, row)
                        pending.push([child, childValue])
                        row.children.push(child)
                    }
                } else row.dataMap[key] = v
            }
            let sorted = {}
            for(let key of Object.keys(row.dataMap).sort()) sorted[key] = row.dataMap[key]
            row.dataMap = sorted
        }
    }
    setData(map){
        this.dataMap = {}
        this.unpack(map)
    }
    // QQmlTreeRow::setField: a field the row has
    setField(key, value){ if(Object.prototype.hasOwnProperty.call(this.dataMap, key)) this.dataMap[key] = value }
    data(key){ return key === undefined ? Object.assign({}, this.dataMap) : this.dataMap[key] }
    // the row as a QVariantMap: its fields, and "rows" with the children when it has any
    toVariant(){
        let map = Object.assign({}, this.dataMap)
        if(this.children.length) map[ROWS] = this.children.map(child => child.toVariant())
        let sorted = {}
        for(let key of Object.keys(map).sort()) sorted[key] = map[key]
        return sorted
    }
}

class TreeModel extends AbstractColumnModel {
    constructor(parent = null){
        super(parent)
        this.$treeRows = []
        this.$initialRows = []
    }

    $rowCount(parent = INVALID){
        if(!parent || !parent.valid) return this.$treeRows.length
        return parent.internalPointer.children.length
    }
    $columnCount(parent = INVALID){ return this.$columnCountValue }

    // index(row, column, parent) or index([row path], column)
    index(row, column, parent = INVALID){
        if(Array.isArray(row)) return this.$indexOfPath(row, column)
        if(!parent || !parent.valid){
            if(row < 0 || row >= this.$treeRows.length) return INVALID
            return this.createIndex(row, column, this.$treeRows[row])
        }
        let treeRow = parent.internalPointer
        if(row < 0 || row >= treeRow.children.length) return INVALID
        return this.createIndex(row, column, treeRow.children[row])
    }

    // QQmlTreeModel::index(treeIndex, column): the row at the path of row numbers from the top
    $indexOfPath(path, column){
        let rows = this.$treeRows, row = null
        for(let i of path){
            row = rows[i]
            if(!row){
                this.$warn('TreeModel::index: could not find any node at the specified index: ' + JSON.stringify(path))
                return INVALID
            }
            rows = row.children
        }
        if(!row) return INVALID
        return this.createIndex(path[path.length - 1], column, row)
    }

    parent(child){
        if(!child || !child.valid) return INVALID
        let parentRow = child.internalPointer.parent
        if(!parentRow) return INVALID
        let grandparent = parentRow.parent
        let siblings = grandparent ? grandparent.children : this.$treeRows
        let at = siblings.indexOf(parentRow)
        return at < 0 ? INVALID : this.createIndex(at, 0, parentRow)
    }

    $setRows(rows){
        if(!Array.isArray(rows)){
            this.$warn('setRows(): the type of "rows" is ' + (rows === null ? 'null' : typeof rows) + ' but an array is expected')
            return
        }
        if(!this.$completed){
            this.$initialRows = toVariant(rows)
            return
        }
        this.$setRowsPrivate(toVariant(rows))
    }

    $setRowsPrivate(list){
        if(!this.$columns().length){
            this.$warn('No TableModelColumns were set; model will be empty')
            return
        }
        let first = !this.$columnMetadata.length
        if(!first){
            for(let row of list) if(!this.$validateTreeRow('TreeModel::setRows', row, 'setRows')) return
        }
        this.beginResetModel()
        let wasEmpty = !this.$treeRows.length
        this.$treeRows = []
        for(let row of list){
            if(row && typeof row === 'object' && !Array.isArray(row)) this.$treeRows.push(new TreeRow(row))
            else this.$warn('Cannot create tree row as the row does not contain key-value pairs')
        }
        if(first && (this.$treeRows.length || this.$initialRows.length)) this.$fetchColumnMetadata()
        this.endResetModel()
        if(!wasEmpty || this.$treeRows.length) engine.emitArgs(this, TreeModel.SIGNAL_rowsChanged, [])
    }

    $firstRow(){
        if(!this.$treeRows.length){
            this.$warn('TreeModel::firstRow(): model is empty')
            return {}
        }
        return this.$treeRows[0].data()
    }
    $setInitialRows(){ this.$setRowsPrivate(this.$initialRows) }

    $dataPrivate(index, roleName){
        let property = this.$columnMetadata[index.column].get(roleName).name
        return toVariant(index.internalPointer.data(property))
    }
    $setDataPrivate(index, property, value){ index.internalPointer.setField(property, toVariant(value)) }

    // QQmlTreeModel::validateNewRow: the row and every row under it
    $validateTreeRow(functionName, row, operation = 'other'){
        let pending = [[row, operation]]
        while(pending.length){
            let [r, op] = pending.pop()
            if(r && typeof r === 'object' && Array.isArray(r[ROWS])) for(let child of r[ROWS]) pending.push([child, 'other'])
            if(!this.$validateNewRow(functionName, r, op)) return false
        }
        return true
    }

    // appendRow(parent, row) / appendRow(row)
    appendRow(parent, row){
        if(row === undefined){
            row = parent
            parent = INVALID
        }
        if(!this.$validateTreeRow('TreeModel::appendRow', row)) return
        if(parent && parent.valid){
            let parentRow = parent.internalPointer
            let child = new TreeRow(row, parentRow)
            this.beginInsertRows(parent, parentRow.children.length, parentRow.children.length)
            parentRow.children.push(child)
            if(!this.$columnMetadata.length) this.$fetchColumnMetadata()
            this.endInsertRows()
        } else {
            if(!row || typeof row !== 'object' || Array.isArray(row)){
                this.$warn('Cannot create tree row as the row does not contain key-value pairs')
                return
            }
            this.beginInsertRows(INVALID, this.$treeRows.length, this.$treeRows.length)
            this.$treeRows.push(new TreeRow(row))
            if(!this.$columnMetadata.length) this.$fetchColumnMetadata()
            this.endInsertRows()
        }
        engine.emitArgs(this, TreeModel.SIGNAL_rowsChanged, [])
    }

    clear(){
        if(!this.$treeRows.length) return
        this.beginResetModel()
        this.$treeRows = []
        this.endResetModel()
        engine.emitArgs(this, TreeModel.SIGNAL_rowsChanged, [])
    }

    getRow(index){
        if(index && index.valid) return index.internalPointer.toVariant()
        this.$warn('getRow: could not find any node at the specified index')
        return undefined
    }

    // insertRow(rowIndex, parent, row) / insertRow(rowIndex, row)
    insertRow(rowIndex, parent, row){
        if(row === undefined){
            row = parent
            parent = INVALID
        }
        if(rowIndex < 0){
            this.$warn('insertRow(): rowIndex cannot be negative')
            return
        }
        if(rowIndex > this.$rowCount(parent)){
            this.$warn('insertRow(): rowIndex ' + rowIndex + ' is greater than rowCount() of ' + this.$rowCount(parent))
            return
        }
        if(!this.$validateTreeRow('insertRow()', row)) return
        this.beginInsertRows(parent, rowIndex, rowIndex)
        if(parent && parent.valid){
            let parentRow = parent.internalPointer
            parentRow.children.splice(rowIndex, 0, new TreeRow(row, parentRow))
        } else this.$treeRows.splice(rowIndex, 0, new TreeRow(row))
        if(!this.$columnMetadata.length) this.$fetchColumnMetadata()
        this.endInsertRows()
        engine.emitArgs(this, TreeModel.SIGNAL_rowsChanged, [])
    }

    removeRow(index){
        if(!index || !index.valid){
            this.$warn('TreeModel::removeRow could not find any node at the specified index')
            return
        }
        let parent = index.parent
        this.beginRemoveRows(parent, index.row, index.row)
        if(parent.valid) parent.internalPointer.children.splice(index.row, 1)
        else this.$treeRows.splice(index.row, 1)
        this.endRemoveRows()
        engine.emitArgs(this, TreeModel.SIGNAL_rowsChanged, [])
    }

    setRow(index, rowData){
        if(!index || !index.valid){
            this.$warn('TreeModel::setRow: invalid modelIndex')
            return
        }
        if(rowData && Array.isArray(rowData[ROWS])){
            this.$warn('TreeModel::setRow: child rows are not allowed')
            return
        }
        if(!this.$validateTreeRow('TreeModel::setRow', rowData)) return
        let row = index.internalPointer
        // QQmlTreeRow::setData keeps the children it has: they are not in the new data
        let children = row.children
        row.setData(rowData)
        row.children = children
        engine.emitArgs(this, AbstractItemModel.SIGNAL_dataChanged, [this.createIndex(index.row, 0, row), this.createIndex(index.row, this.$columnCountValue - 1, row), []])
        engine.emitArgs(this, TreeModel.SIGNAL_rowsChanged, [])
    }
}
defineType(TreeModel, {
    props: {
        rows: { kind: 'var', effect: (model, value) => model.$setRows(value), get(){
            engine.track(this, TreeModel.SIGNAL_rowsChanged)
            return this.$treeRows.map(row => row.toVariant())
        } },
    },
})
TreeModel.$defaultProperty = 'columns'

module.exports = { TreeModel, TreeRow }
