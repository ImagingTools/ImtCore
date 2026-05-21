const { QtObject } = require('./QtObject')
const { QReal, QData, QVar, QModelData } = require('../utils/properties')

class TableModelColumn extends QtObject {
    static defaultProperties = {
        display: { type: QVar, value: undefined },
        edit: { type: QVar, value: undefined },
        decoration: { type: QVar, value: undefined },
        toolTip: { type: QVar, value: undefined },
        statusTip: { type: QVar, value: undefined },
        whatsThis: { type: QVar, value: undefined },
        font: { type: QVar, value: undefined },
        textAlignment: { type: QVar, value: undefined },
        background: { type: QVar, value: undefined },
        foreground: { type: QVar, value: undefined },
        checkState: { type: QVar, value: undefined },
        accessibleText: { type: QVar, value: undefined },
        accessibleDescription: { type: QVar, value: undefined },
        sizeHint: { type: QVar, value: undefined },
    }

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)
    }
}

class TableModel extends QtObject {
    static defaultProperties = {
        rows: { type: QVar, value: undefined, changed: '$rowsChanged' },
        count: { type: QReal, value: 0 },
        data: { type: QData, value: undefined, changed: '$dataChanged' },
        columnCount: { type: QReal, value: 0 },
    }

    static defaultSignals = {
        $transaction: { params: ['sender', 'changeset'] },
    }

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)
        this.getProperty('data').value = []
        this.$columns = []
    }

    addResource(resource) {
        super.addResource(resource)
        if (resource instanceof TableModelColumn) {
            this.$columns.push(resource)
            this.getProperty('columnCount').reset(this.$columns.length)
        }
    }

    $complete() {
        super.$complete()
        let rows = this.getPropertyValue('rows')
        if (rows && Array.isArray(rows)) {
            this.$setRows(rows)
        }
    }

    $rowsChanged() {
        let rows = this.getPropertyValue('rows')
        if (rows && Array.isArray(rows)) {
            this.$setRows(rows)
        }
    }

    $setRows(rows) {
        let data = []
        for (let i = 0; i < rows.length; i++) {
            data.push(new QModelData(this, rows[i], i))
        }
        this.getProperty('data').value = data
        this.getProperty('count').reset(data.length)
        this.$emitDataChanged(0, data.length, 'append')
    }

    $dataChanged(topLeft, bottomRight, roles) {
        if (roles) {
            if (TransactionController.add(this)) {
                this.$changeset = this.$changeset || []
                this.$changeset.push([topLeft, bottomRight, roles])
            } else {
                if (this.$signals.$transaction) this.$signals.$transaction(this, [[topLeft, bottomRight, roles]])
            }
        }
    }

    $emitDataChanged(topLeft, bottomRight, roles) {
        this.getStatement('data').getNotify()(topLeft, bottomRight, roles)
    }

    rowCount() {
        return this.getPropertyValue('count')
    }

    columnCount() {
        return this.$columns.length
    }

    getColumnName(columnIndex) {
        if (columnIndex >= 0 && columnIndex < this.$columns.length) {
            return this.$columns[columnIndex].getPropertyValue('display')
        }
        return undefined
    }

    getData(row, columnIndex) {
        let d = this.getProperty('data').get()
        if (!d || row < 0 || row >= d.length) return undefined
        let item = d[row]
        if (columnIndex !== undefined && this.$columns.length > 0) {
            let colName = this.getColumnName(columnIndex)
            if (colName && item) return item[colName]
        }
        return item
    }

    setModelData(row, columnIndex, value) {
        let d = this.getProperty('data').get()
        if (!d || row < 0 || row >= d.length) return false
        let item = d[row]
        if (columnIndex !== undefined && this.$columns.length > 0) {
            let colName = this.getColumnName(columnIndex)
            if (colName && item) {
                if (item instanceof QModelData) {
                    item.$lock = true
                    item[colName] = value
                    item.$lock = false
                } else {
                    item[colName] = value
                }
                this.$emitDataChanged(row, row + 1, 'update')
                return true
            }
        }
        return false
    }

    appendRow(row) {
        let d = this.getProperty('data').get()
        d.push(new QModelData(this, row, d.length))
        this.getProperty('count').reset(d.length)
        this.$emitDataChanged(d.length - 1, d.length, 'append')
    }

    removeRow(index, count = 1) {
        let d = this.getProperty('data').get()
        d.splice(index, count)
        for (let i = index; i < d.length; i++) {
            d[i].index = i
        }
        this.getProperty('count').reset(d.length)
        this.$emitDataChanged(index, index + count, 'remove')
    }

    clear() {
        let d = this.getProperty('data').get()
        let len = d.length
        this.getProperty('data').value = []
        this.getProperty('count').reset(0)
        this.$emitDataChanged(0, len, 'remove')
    }

    get(index) {
        let d = this.getProperty('data').get()
        return index >= 0 && index < d.length ? d[index] : undefined
    }
}

module.exports.TableModel = TableModel
module.exports.TableModelColumn = TableModelColumn
