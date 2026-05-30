const QtObject = require("../QtObject")
const Var = require("../Var")
const Signal = require("../Signal")

class ItemSelectionModel extends QtObject {
    // Selection flags
    static NoUpdate        = 0x0000
    static Clear           = 0x0001
    static Select          = 0x0002
    static Deselect        = 0x0004
    static Toggle          = 0x0008
    static Current         = 0x0010
    static Rows            = 0x0020
    static Columns         = 0x0040
    static SelectCurrent   = ItemSelectionModel.Select | ItemSelectionModel.Current
    static ToggleCurrent   = ItemSelectionModel.Toggle | ItemSelectionModel.Current
    static ClearAndSelect  = ItemSelectionModel.Clear  | ItemSelectionModel.Select

    static meta = Object.assign({}, QtObject.meta, {
        model:          { type: Var, value: undefined },
        currentIndex:   { type: Var, value: undefined },

        modelChanged:        { type: Signal, args: [] },
        currentIndexChanged: { type: Signal, args: ['current', 'previous'] },
        selectionChanged:    { type: Signal, args: ['selected', 'deselected'] },
    })

    static create(parent, ...args) {
        let obj = super.create(parent, ...args)
        obj.__selection = {}   // key "row,col" -> true
        obj.__currentIndex = undefined
        return obj
    }

    // --- helpers ---

    __key(index) {
        if (!index) return null
        return (index.row !== undefined ? index.row : -1) + ',' +
               (index.column !== undefined ? index.column : -1)
    }

    // --- public API ---

    setCurrentIndex(index, command) {
        let previous = this.__currentIndex
        this.__currentIndex = index
        this.currentIndex = index

        if (command !== undefined && command !== ItemSelectionModel.NoUpdate) {
            this.select(index, command)
        }
        this.currentIndexChanged(index, previous)
    }

    /**
     * @param {object|object[]} indexOrRange - {row,column} or array of them
     * @param {number} command
     */
    select(indexOrRange, command) {
        if (command === undefined) command = ItemSelectionModel.ClearAndSelect

        let indices = Array.isArray(indexOrRange) ? indexOrRange : [indexOrRange]

        let selected   = []
        let deselected = []

        if (command & ItemSelectionModel.Clear) {
            for (let k in this.__selection) {
                let [r, c] = k.split(',').map(Number)
                deselected.push({ row: r, column: c })
            }
            this.__selection = {}
        }

        for (let idx of indices) {
            if (!idx) continue
            let rows    = (command & ItemSelectionModel.Rows)    ? this.__allColumnsForRow(idx.row)    : [idx]
            let columns = (command & ItemSelectionModel.Columns) ? this.__allRowsForColumn(idx.column) : rows

            let targets = (command & ItemSelectionModel.Rows) ? rows
                        : (command & ItemSelectionModel.Columns) ? columns
                        : [idx]

            for (let t of targets) {
                let k = this.__key(t)
                if (!k) continue
                if (command & ItemSelectionModel.Toggle) {
                    if (this.__selection[k]) {
                        delete this.__selection[k]
                        deselected.push(t)
                    } else {
                        this.__selection[k] = true
                        selected.push(t)
                    }
                } else if (command & ItemSelectionModel.Deselect) {
                    if (this.__selection[k]) {
                        delete this.__selection[k]
                        deselected.push(t)
                    }
                } else if (command & ItemSelectionModel.Select) {
                    if (!this.__selection[k]) {
                        this.__selection[k] = true
                        selected.push(t)
                    }
                }
            }
        }

        if (command & ItemSelectionModel.Current) {
            let last = indices[indices.length - 1]
            if (last) this.__currentIndex = last
        }

        if (selected.length > 0 || deselected.length > 0) {
            this.selectionChanged(selected, deselected)
        }
    }

    isSelected(index) {
        return !!this.__selection[this.__key(index)]
    }

    isRowSelected(row) {
        for (let k in this.__selection) {
            if (k.startsWith(row + ',')) return true
        }
        return false
    }

    isColumnSelected(col) {
        for (let k in this.__selection) {
            if (k.endsWith(',' + col)) return true
        }
        return false
    }

    selectedIndexes() {
        return Object.keys(this.__selection).map(k => {
            let [r, c] = k.split(',').map(Number)
            return { row: r, column: c }
        })
    }

    clearSelection() {
        let deselected = this.selectedIndexes()
        this.__selection = {}
        if (deselected.length) this.selectionChanged([], deselected)
    }

    clearCurrentIndex() {
        let prev = this.__currentIndex
        this.__currentIndex = undefined
        this.currentIndex = undefined
        this.currentIndexChanged(undefined, prev)
    }

    reset() {
        this.clearSelection()
        this.clearCurrentIndex()
    }

    hasSelection() {
        return Object.keys(this.__selection).length > 0
    }

    // --- internal helpers ---

    __allColumnsForRow(row) {
        let model = this.model
        let colCount = 1
        if (model) {
            if (typeof model.columnCount === 'function') colCount = model.columnCount()
            else if (model.columns !== undefined) colCount = model.columns
        }
        let result = []
        for (let c = 0; c < colCount; c++) result.push({ row: row, column: c })
        return result
    }

    __allRowsForColumn(col) {
        let model = this.model
        let rowCount = 1
        if (model) {
            if (typeof model.rowCount === 'function') rowCount = model.rowCount()
            else if (model.rows !== undefined) rowCount = model.rows
            else if (model.count !== undefined) rowCount = model.count
        }
        let result = []
        for (let r = 0; r < rowCount; r++) result.push({ row: r, column: col })
        return result
    }
}

module.exports = ItemSelectionModel
