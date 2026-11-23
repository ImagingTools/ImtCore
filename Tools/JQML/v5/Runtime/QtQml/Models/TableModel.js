// TableModel and TableModelColumn of Qt.labs.qmlmodels (qqmlabstractcolumnmodel.cpp, qqmltablemodel.cpp,
// qqmltablemodelcolumn.cpp of Qt 6.12). The columns and the roles of each come from the TableModelColumns and the first
// row once the model is complete; the rows are JavaScript objects, copied in as QVariant copies them.
const engine = require('../../Core/engine')
const { QtObject } = require('../../Core/QtObject')
const { defineType } = require('../../Core/type')
const { AbstractItemModel, INVALID } = require('./ItemModel')
const QtEnums = require('../../Qt/enums')

// QQmlTableModelColumn::supportedRoleNames
const SUPPORTED_ROLES = [
    [QtEnums.DisplayRole, 'display'], [QtEnums.DecorationRole, 'decoration'], [QtEnums.EditRole, 'edit'],
    [QtEnums.ToolTipRole, 'toolTip'], [QtEnums.StatusTipRole, 'statusTip'], [QtEnums.WhatsThisRole, 'whatsThis'],
    [QtEnums.FontRole, 'font'], [QtEnums.TextAlignmentRole, 'textAlignment'], [QtEnums.BackgroundRole, 'background'],
    [QtEnums.ForegroundRole, 'foreground'], [QtEnums.CheckStateRole, 'checkState'],
    [QtEnums.AccessibleTextRole, 'accessibleText'], [QtEnums.AccessibleDescriptionRole, 'accessibleDescription'],
    [QtEnums.SizeHintRole, 'sizeHint'],
]

// ---- TableModelColumn: for each role the name of a property of the row, or a function of the index ------------------------
class TableModelColumn extends QtObject {
    getterAtRole(roleName){
        let id = TableModelColumn[roleName.toUpperCase()]
        return id === undefined ? undefined : this.$v[id]
    }
    $getters(){
        let getters = new Map()
        for(let [, name] of SUPPORTED_ROLES){
            let value = this.getterAtRole(name)
            if(value !== undefined && value !== null) getters.set(name, value)
        }
        return getters
    }
}
defineType(TableModelColumn, { props: Object.fromEntries(SUPPORTED_ROLES.map(([, name]) => [name, 'var'])) })

// QVariant of a JavaScript value: arrays copied, objects copied as a QVariantMap (a QMap: its keys sorted), other values as
// they are (QJSValue::toVariant)
function toVariant(value){
    if(Array.isArray(value)) return value.map(toVariant)
    if(value && typeof value === 'object' && Object.getPrototypeOf(value) === Object.prototype){
        let copy = {}
        for(let key of Object.keys(value).sort()){
            let v = value[key]
            if(typeof v !== 'function') copy[key] = toVariant(v)
        }
        return copy
    }
    return value
}

const isMap = (v) => !!v && typeof v === 'object' && !Array.isArray(v) && Object.getPrototypeOf(v) === Object.prototype
const typeOf = (v) => (v === undefined ? 'UnknownType' : v === null ? 'std::nullptr_t' : Array.isArray(v) ? 'QVariantList'
    : typeof v === 'object' ? 'QVariantMap' : typeof v === 'string' ? 'QString' : typeof v === 'boolean' ? 'bool'
    : typeof v === 'number' ? (Number.isInteger(v) ? 'int' : 'double') : 'QJSValue')
// a JS number is an int or a double in a QVariant; both convert into each other
const sameType = (a, b) => a === b || ((a === 'int' || a === 'double') && (b === 'int' || b === 'double'))

// QVariant::canConvert / convert, as far as a JavaScript value goes
function convertible(value, type){
    let from = typeOf(value)
    if(sameType(from, type)) return true
    if(type === 'QString') return from !== 'QVariantMap' && from !== 'QVariantList'
    if(type === 'int' || type === 'double') return from === 'bool' || (from === 'QString' && value.trim() !== '' && !Number.isNaN(Number(value)))
    if(type === 'bool') return from === 'int' || from === 'double' || from === 'QString'
    return false
}

const quotedList = (keys) => 'QList(' + keys.map(k => '"' + k + '"').join(', ') + ')'

// ---- QQmlAbstractColumnModel ----------------------------------------------------------------------------------------------
class AbstractColumnModel extends AbstractItemModel {
    constructor(parent = null){
        super(parent)
        this.$completed = false
        this.$columnCountValue = 0
        this.$columnMetadata = []
        this.$roleNamesMap = new Map()
        engine.scheduleComponentComplete(this)
    }

    // the columns: the TableModelColumns declared in the model, or the list assigned to columns
    $columns(){
        let list = this.$v[this.constructor.COLUMNS]
        if(Array.isArray(list)) return list
        return (this.$children || []).filter(child => child instanceof TableModelColumn)
    }

    roleNames(){ return new Map(this.$roleNamesMap) }

    $warn(message){ console.warn(this.constructor.$qmlName + ': ' + message) }

    // data(index, role): role by number, or by name (QQmlAbstractColumnModel::data(index, QString))
    data(index, role = QtEnums.DisplayRole){
        if(typeof role === 'string'){
            let id = this.$roleOf(role)
            return id >= 0 ? this.data(index, id) : undefined
        }
        if(!index || !index.valid){
            this.$warn('data(): invalid QModelIndex')
            return undefined
        }
        let row = index.row, column = index.column
        if(row < 0 || row >= this.$rowCount(this.parent(index))){
            this.$warn('data(): invalid row specified in QModelIndex')
            return undefined
        }
        if(column < 0 || column >= this.$columnCount(this.parent(index))){
            this.$warn('data(): invalid column specified in QModelIndex')
            return undefined
        }
        let metadata = this.$columnMetadata[column]
        let roleName = this.$roleNamesMap.get(role) || ''
        if(!metadata || !metadata.has(roleName)){
            this.$warn('data(): no role named ' + roleName + ' at column index ' + column + '. The available roles for that column are: '
                + quotedList(metadata ? Array.from(metadata.keys()) : []))
            return undefined
        }
        let roleData = metadata.get(roleName)
        if(roleData.string) return this.$dataPrivate(index, roleName)
        let getter = this.$columns()[column].getterAtRole(roleName)
        return toVariant(getter(index))
    }

    setData(index, value, role = QtEnums.DisplayRole){
        if(typeof role === 'string'){
            let id = this.$roleOf(role)
            return id >= 0 ? this.setData(index, value, id) : false
        }
        let row = index.row, column = index.column
        if(row < 0 || row >= this.$rowCount(this.parent(index))) return false
        if(column < 0 || column >= this.$columnCount(this.parent(index))) return false
        let roleName = this.$roleNamesMap.get(role) || ''
        let metadata = this.$columnMetadata[index.column]
        if(!metadata || !metadata.has(roleName)){
            this.$warn('setData(): no role named "' + roleName + '" at column index ' + column + '. The available roles for that column are: '
                + quotedList(metadata ? Array.from(metadata.keys()) : []))
            return false
        }
        let roleData = metadata.get(roleName)
        if(!sameType(typeOf(value), roleData.type) && !convertible(value, roleData.type)){
            this.$warn('setData(): the value ' + value + ' set at row ' + row + ' column ' + column + ' with role ' + roleName
                + ' cannot be converted to ' + roleData.type)
            return false
        }
        if(!roleData.string){
            this.$warn('setData(): manipulation of complex row structures is not supported')
            return false
        }
        this.$setDataPrivate(index, roleData.name, value)
        engine.emitArgs(this, AbstractItemModel.SIGNAL_dataChanged, [index, index, [role]])
        engine.emitArgs(this, this.constructor.SIGNAL_rowsChanged, [])
        return true
    }

    // Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable
    flags(index){ return QtEnums.ItemIsSelectable | QtEnums.ItemIsEnabled | QtEnums.ItemIsEditable }

    $componentComplete(){
        if(super.$componentComplete) super.$componentComplete()
        this.$completed = true
        this.$columnCountValue = this.$columns().length
        if(this.$columnCountValue > 0) engine.emitArgs(this, this.constructor.SIGNAL_columnCountChanged, [])
        this.$setInitialRows()
    }

    // QQmlAbstractColumnModel::fetchColumnRoleData: from the first row, a property name or a function of the index
    $fetchColumnRoleData(roleName, column, columnIndex){
        let getter = column.getterAtRole(roleName)
        if(getter === undefined || getter === null) return null
        let row = this.$firstRow()
        if(typeof getter === 'string'){
            if(!isMap(row)){
                this.$warn('"expected row for role ' + roleName + ' of TableModelColumn at index ' + columnIndex
                    + ' to be a simple object, but it\'s ' + typeOf(row) + ' instead: ' + JSON.stringify(row) + '"')
                return null
            }
            let property = row[getter]
            return { string: true, name: getter, type: typeOf(property) }
        }
        if(typeof getter === 'function'){
            let cell = toVariant(getter(this.index(0, columnIndex)))
            return { string: false, name: '', type: typeOf(cell) }
        }
        this.$warn('TableModelColumn role for column at index ' + columnIndex + ' must be either a string or a function; actual type is: ' + String(getter))
        return null
    }

    $fetchColumnMetadata(){
        let columns = this.$columns()
        this.$columnMetadata = []
        for(let columnIndex = 0; columnIndex < columns.length; columnIndex++){
            let metadata = new Map()
            for(let [role, name] of SUPPORTED_ROLES){
                let roleData = this.$fetchColumnRoleData(name, columns[columnIndex], columnIndex)
                if(!roleData || roleData.type === 'UnknownType') continue
                metadata.set(name, roleData)
                this.$roleNamesMap.set(role, name)
            }
            this.$columnMetadata[columnIndex] = metadata
        }
        this.$roleNameCache = null
    }

    // QQmlAbstractColumnModel::validateNewRow
    $validateNewRow(functionName, row, operation = 'other'){
        if(!this.$columnMetadata.length) return true
        if(operation !== 'setRows' && !isMap(row) && !Array.isArray(row)){
            if(!row || typeof row !== 'object'){
                this.$warn(functionName + ': expected "row" argument to be an object or array, but got:\n' + String(row))
                return false
            }
        }
        if(!isMap(row)){
            this.$warn(functionName + ': row manipulation functions do not support complex rows')
            return false
        }
        if(Object.keys(row).length < this.$columnCountValue){
            this.$warn(functionName + ': expected ' + this.$columnCountValue + ' columns, but only got ' + Object.keys(row).length)
            return false
        }
        let columns = this.$columns()
        for(let columnIndex = 0; columnIndex < columns.length; columnIndex++){
            let metadata = this.$columnMetadata[columnIndex]
            for(let roleName of columns[columnIndex].$getters().keys()){
                let roleData = metadata.get(roleName)
                if(!roleData || !roleData.string) continue
                if(!(roleData.name in row)){
                    this.$warn(functionName + ': expected a property named "' + roleData.name + '" in row')
                    return false
                }
                let value = row[roleData.name]
                if(!sameType(typeOf(value), roleData.type) && !convertible(value, roleData.type)){
                    this.$warn(functionName + ': expected the property named "' + roleData.name + '" to be of type "' + roleData.type
                        + '", but got "' + typeOf(value) + '" instead')
                    return false
                }
            }
        }
        return true
    }
}
defineType(AbstractColumnModel, {
    props: {
        columns: 'var',
        columnCount: { kind: 'int', readonly: true, get(){
            engine.track(this, AbstractColumnModel.SIGNAL_columnCountChanged)
            return this.$columnCountValue
        } },
    },
})

// ---- TableModel -----------------------------------------------------------------------------------------------------------
class TableModel extends AbstractColumnModel {
    constructor(parent = null){
        super(parent)
        this.$rowsData = []
        this.$rowCountValue = 0
    }

    $rowCount(parent = INVALID){ return parent && parent.valid ? 0 : this.$rowCountValue }
    $columnCount(parent = INVALID){ return this.$columnCountValue }

    index(row, column, parent = INVALID){
        return row >= 0 && row < this.$rowCountValue && column >= 0 && column < this.$columnCountValue && !(parent && parent.valid)
            ? this.createIndex(row, column) : INVALID
    }
    parent(child){ return INVALID }

    $setRows(rows){
        if(!Array.isArray(rows)){
            this.$warn('setRows(): the type of "rows" is ' + (rows === null ? 'null' : typeof rows) + ' but an array is expected')
            return
        }
        let list = toVariant(rows)
        if(JSON.stringify(list) === JSON.stringify(this.$rowsData) && list.length === this.$rowsData.length) return
        if(!this.$completed){
            this.$rowsData = list
            return
        }
        this.$setRowsPrivate(list)
    }

    $setRowsPrivate(list){
        if(!this.$columns().length){
            this.$warn('No TableModelColumns were set; model will be empty')
            return
        }
        let first = !this.$columnMetadata.length
        if(!first){
            for(let row of list) if(!this.$validateNewRow('setRows()', row, 'setRows')) return
        }
        let oldRowCount = this.$rowCountValue
        this.beginResetModel()
        this.$rowsData = list
        this.$rowCountValue = list.length
        if(first && list.length) this.$fetchColumnMetadata()
        this.endResetModel()
        engine.emitArgs(this, this.constructor.SIGNAL_rowsChanged, [])
        if(this.$rowCountValue !== oldRowCount) engine.emitArgs(this, TableModel.SIGNAL_rowCountChanged, [])
    }

    $firstRow(){ return this.$rowsData[0] }
    $setInitialRows(){ this.$setRowsPrivate(this.$rowsData) }

    $dataPrivate(index, roleName){
        let property = this.$columnMetadata[index.column].get(roleName).name
        let row = this.$rowsData[index.row]
        return isMap(row) ? toVariant(row[property]) : undefined
    }
    $setDataPrivate(index, property, value){
        let row = Object.assign({}, this.$rowsData[index.row])
        row[property] = toVariant(value)
        this.$rowsData[index.row] = row
    }

    $validateRowIndex(functionName, argumentName, rowIndex, needsExisting){
        if(rowIndex < 0){
            this.$warn(functionName + ': "' + argumentName + '" cannot be negative')
            return false
        }
        if(needsExisting ? rowIndex >= this.$rowCountValue : rowIndex > this.$rowCountValue){
            this.$warn(functionName + ': "' + argumentName + '" ' + rowIndex + (needsExisting ? ' is greater than or equal to' : ' is greater than')
                + ' rowCount() of ' + this.$rowCountValue)
            return false
        }
        return true
    }

    appendRow(row){
        if(!this.$validateNewRow('appendRow()', row, 'append')) return
        this.$doInsert(this.$rowCountValue, row)
    }

    clear(){
        if(!this.$rowsData.length) return
        this.beginResetModel()
        this.$rowsData = []
        this.$rowCountValue = 0
        this.endResetModel()
        engine.emitArgs(this, TableModel.SIGNAL_rowCountChanged, [])
        engine.emitArgs(this, this.constructor.SIGNAL_rowsChanged, [])
    }

    getRow(rowIndex){
        if(!this.$validateRowIndex('getRow()', 'rowIndex', rowIndex, true)) return undefined
        return toVariant(this.$rowsData[rowIndex])
    }

    insertRow(rowIndex, row){
        if(!this.$validateNewRow('insertRow()', row) || !this.$validateRowIndex('insertRow()', 'rowIndex', rowIndex, false)) return
        this.$doInsert(rowIndex, row)
    }

    $doInsert(rowIndex, row){
        this.beginInsertRows(INVALID, rowIndex, rowIndex)
        this.$rowsData.splice(rowIndex, 0, toVariant(row))
        this.$rowCountValue++
        if(!this.$columnMetadata.length) this.$fetchColumnMetadata()
        this.endInsertRows()
        engine.emitArgs(this, TableModel.SIGNAL_rowCountChanged, [])
        engine.emitArgs(this, this.constructor.SIGNAL_rowsChanged, [])
    }

    moveRow(fromRowIndex, toRowIndex, rows = 1){
        if(fromRowIndex === toRowIndex){
            this.$warn('moveRow(): "fromRowIndex" cannot be equal to "toRowIndex"')
            return
        }
        if(rows <= 0){
            this.$warn('moveRow(): "rows" is less than or equal to 0')
            return
        }
        if(!this.$validateRowIndex('moveRow()', 'fromRowIndex', fromRowIndex, true)) return
        if(!this.$validateRowIndex('moveRow()', 'toRowIndex', toRowIndex, true)) return
        if(fromRowIndex + rows > this.$rowCountValue){
            this.$warn('moveRow(): "fromRowIndex" (' + fromRowIndex + ') + "rows" (' + rows + ') = ' + (fromRowIndex + rows)
                + ', which is greater than rowCount() of ' + this.$rowCountValue)
            return
        }
        if(toRowIndex + rows > this.$rowCountValue){
            this.$warn('moveRow(): "toRowIndex" (' + toRowIndex + ') + "rows" (' + rows + ') = ' + (toRowIndex + rows)
                + ', which is greater than rowCount() of ' + this.$rowCountValue)
            return
        }
        this.beginMoveRows(INVALID, fromRowIndex, fromRowIndex + rows - 1, INVALID, toRowIndex > fromRowIndex ? toRowIndex + rows : toRowIndex)
        let moved = this.$rowsData.splice(fromRowIndex, rows)
        this.$rowsData.splice(toRowIndex, 0, ...moved)
        this.endMoveRows()
        engine.emitArgs(this, this.constructor.SIGNAL_rowsChanged, [])
    }

    removeRow(rowIndex, rows = 1){
        if(!this.$validateRowIndex('removeRow()', 'rowIndex', rowIndex, true)) return
        if(rows <= 0){
            this.$warn('removeRow(): "rows" is less than or equal to zero')
            return
        }
        if(rowIndex + rows - 1 >= this.$rowCountValue){
            this.$warn('removeRow(): "rows" ' + rows + ' exceeds available rowCount() of ' + this.$rowCountValue + ' when removing from "rowIndex" ' + rowIndex)
            return
        }
        this.beginRemoveRows(INVALID, rowIndex, rowIndex + rows - 1)
        this.$rowsData.splice(rowIndex, rows)
        this.$rowCountValue -= rows
        this.endRemoveRows()
        engine.emitArgs(this, TableModel.SIGNAL_rowCountChanged, [])
        engine.emitArgs(this, this.constructor.SIGNAL_rowsChanged, [])
    }

    setRow(rowIndex, row){
        if(!this.$validateNewRow('setRow()', row) || !this.$validateRowIndex('setRow()', 'rowIndex', rowIndex, false)) return
        if(rowIndex !== this.$rowCountValue){
            this.$rowsData[rowIndex] = toVariant(row)
            engine.emitArgs(this, AbstractItemModel.SIGNAL_dataChanged, [this.createIndex(rowIndex, 0), this.createIndex(rowIndex, this.$columnCountValue - 1), []])
            engine.emitArgs(this, this.constructor.SIGNAL_rowsChanged, [])
        } else {
            this.$doInsert(rowIndex, row)
        }
    }
}
defineType(TableModel, {
    props: {
        // the rows read back as the model has them (a copy, as QVariant gives it), written through setRows
        rows: { kind: 'var', effect: (model, value) => model.$setRows(value), get(){
            engine.track(this, TableModel.SIGNAL_rowsChanged)
            return toVariant(this.$rowsData)
        } },
        rowCount: { kind: 'int', readonly: true, get(){
            engine.track(this, TableModel.SIGNAL_rowCountChanged)
            return this.$rowCountValue
        } },
    },
})
TableModel.$defaultProperty = 'columns'

module.exports = { TableModel, TableModelColumn, AbstractColumnModel, toVariant, SUPPORTED_ROLES }
