const QtObject = require("../QtObject")
const Var = require("../Var")
const Int = require("../Int")
const Signal = require("../Signal")
const TableModelColumn = require("./TableModelColumn")

class TableModel extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        rows:        { type: Var, value: undefined },
        count:       { type: Int, value: 0 },
        columnCount: { type: Int, value: 0 },

        rowsChanged:        { type: Signal, args: [] },
        countChanged:       { type: Signal, args: [] },
        columnCountChanged: { type: Signal, args: [] },
        // internal model data changed
        dataChanged: { type: Signal, args: ['topLeft', 'bottomRight', 'roles'] },
    })

    static create(parent, ...args) {
        let obj = super.create(parent, ...args)
        obj.__columns = []   // TableModelColumn children
        obj.__data = []      // array of row plain objects
        return obj
    }

    __addChild(child) {
        super.__addChild(child)
        if (child instanceof TableModelColumn) {
            this.__columns.push(child)
            this.columnCount = this.__columns.length
        }
    }

    __complete() {
        super.__complete()
        if (this.rows && Array.isArray(this.rows)) {
            this.__setRows(this.rows)
        }
    }

    SLOT_rowsChanged(oldValue, newValue) {
        if (newValue && Array.isArray(newValue)) {
            this.__setRows(newValue)
        }
    }

    __setRows(rows) {
        this.__data = rows.map((r, i) => {
            if (typeof r === 'object' && r !== null) return Object.assign({ index: i }, r)
            return { index: i, display: r }
        })
        this.count = this.__data.length
        this.dataChanged(0, this.__data.length, 'reset')
    }

    rowCount() { return this.count }
    getColumnCount() { return this.__columns.length || 1 }

    getColumnName(colIndex) {
        if (colIndex >= 0 && colIndex < this.__columns.length) {
            return this.__columns[colIndex].display
        }
        return undefined
    }

    getData(row, colIndex) {
        if (!this.__data || row < 0 || row >= this.__data.length) return undefined
        let item = this.__data[row]
        if (colIndex !== undefined && this.__columns.length > 0) {
            let colName = this.getColumnName(colIndex)
            if (colName && item) return item[colName]
        }
        return item
    }

    setModelData(row, colIndex, value) {
        if (!this.__data || row < 0 || row >= this.__data.length) return false
        let item = this.__data[row]
        if (colIndex !== undefined && this.__columns.length > 0) {
            let colName = this.getColumnName(colIndex)
            if (colName && item) {
                item[colName] = value
                this.dataChanged(row, row + 1, 'update')
                return true
            }
        }
        return false
    }

    appendRow(row) {
        let idx = this.__data.length
        let obj = typeof row === 'object' && row !== null
            ? Object.assign({ index: idx }, row)
            : { index: idx, display: row }
        this.__data.push(obj)
        this.count = this.__data.length
        this.dataChanged(idx, idx + 1, 'append')
    }

    removeRow(index, count = 1) {
        this.__data.splice(index, count)
        for (let i = index; i < this.__data.length; i++) this.__data[i].index = i
        this.count = this.__data.length
        this.dataChanged(index, index + count, 'remove')
    }

    clear() {
        let len = this.__data.length
        this.__data = []
        this.count = 0
        this.dataChanged(0, len, 'remove')
    }

    get(index) {
        return (index >= 0 && index < this.__data.length) ? this.__data[index] : undefined
    }
}

module.exports = TableModel
