const { QtObject } = require('./QtObject')
const { QVar, QBool, QReal } = require('../utils/properties')

class ItemSelectionModel extends QtObject {
    static NoUpdate = 0x0000
    static Clear = 0x0001
    static Select = 0x0002
    static Deselect = 0x0004
    static Toggle = 0x0008
    static Current = 0x0010
    static Rows = 0x0020
    static Columns = 0x0040
    static SelectCurrent = 0x0012 // Select | Current
    static ToggleCurrent = 0x0018 // Toggle | Current
    static ClearAndSelect = 0x0003 // Clear | Select

    static defaultProperties = {
        hasSelection: { type: QBool, value: false },
        currentIndex: { type: QVar, value: null },
        model: { type: QVar, value: null },
    }

    static defaultSignals = {
        selectionChanged: { params: ['selected', 'deselected'] },
        currentChanged: { params: ['current', 'previous'] },
    }

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)
        this.$selection = {} // key: "row,col" -> {row, column}
        this.$currentRow = -1
        this.$currentColumn = -1
    }

    _key(row, col) {
        return row + ',' + col
    }

    _selectionSize() {
        return Object.keys(this.$selection).length
    }

    setCurrentIndex(modelIndex, command) {
        let previous = this.getPropertyValue('currentIndex')
        let prevRow = this.$currentRow
        let prevCol = this.$currentColumn

        if (modelIndex) {
            this.$currentRow = modelIndex.row !== undefined ? modelIndex.row : -1
            this.$currentColumn = modelIndex.column !== undefined ? modelIndex.column : -1
        } else {
            this.$currentRow = -1
            this.$currentColumn = -1
        }

        this.getProperty('currentIndex').reset(modelIndex)

        if (command !== undefined && command !== ItemSelectionModel.NoUpdate) {
            this._applyCommand(this.$currentRow, this.$currentColumn, command)
        }

        if (this.$signals.currentChanged) {
            this.$signals.currentChanged(modelIndex, previous)
        }
    }

    select(modelIndex, command) {
        let row = modelIndex ? (modelIndex.row !== undefined ? modelIndex.row : -1) : -1
        let col = modelIndex ? (modelIndex.column !== undefined ? modelIndex.column : -1) : -1

        if (command & ItemSelectionModel.Current) {
            this.setCurrentIndex(modelIndex, ItemSelectionModel.NoUpdate)
        }

        this._applyCommand(row, col, command)
    }

    _applyCommand(row, col, command) {
        let oldSelected = Object.keys(this.$selection)

        if (command & ItemSelectionModel.Clear) {
            this.$selection = {}
        }

        if (command & ItemSelectionModel.Select) {
            if (command & ItemSelectionModel.Rows) {
                this.$selection[this._key(row, -1)] = { row: row, column: -1 }
            } else if (command & ItemSelectionModel.Columns) {
                this.$selection[this._key(-1, col)] = { row: -1, column: col }
            } else {
                this.$selection[this._key(row, col)] = { row: row, column: col }
            }
        }

        if (command & ItemSelectionModel.Deselect) {
            if (command & ItemSelectionModel.Rows) {
                for (let key in this.$selection) {
                    if (this.$selection[key].row === row) delete this.$selection[key]
                }
            } else if (command & ItemSelectionModel.Columns) {
                for (let key in this.$selection) {
                    if (this.$selection[key].column === col) delete this.$selection[key]
                }
            } else {
                delete this.$selection[this._key(row, col)]
            }
        }

        if (command & ItemSelectionModel.Toggle) {
            let key = this._key(row, col)
            if (key in this.$selection) {
                delete this.$selection[key]
            } else {
                this.$selection[key] = { row: row, column: col }
            }
        }

        this.getProperty('hasSelection').reset(this._selectionSize() > 0)

        if (this.$signals.selectionChanged) {
            this.$signals.selectionChanged()
        }
    }

    isSelected(modelIndex) {
        if (!modelIndex) return false
        let row = modelIndex.row !== undefined ? modelIndex.row : -1
        let col = modelIndex.column !== undefined ? modelIndex.column : -1

        // Check direct cell selection
        if (this._key(row, col) in this.$selection) return true
        // Check row selection
        if (this._key(row, -1) in this.$selection) return true
        // Check column selection
        if (this._key(-1, col) in this.$selection) return true

        return false
    }

    isRowSelected(row) {
        if (this._key(row, -1) in this.$selection) return true
        // Also check if all cells in that row are selected (we don't know column count here)
        for (let key in this.$selection) {
            if (this.$selection[key].row === row) return true
        }
        return false
    }

    isColumnSelected(col) {
        if (this._key(-1, col) in this.$selection) return true
        for (let key in this.$selection) {
            if (this.$selection[key].column === col) return true
        }
        return false
    }

    selectedIndexes() {
        let result = []
        for (let key in this.$selection) {
            result.push(this.$selection[key])
        }
        return result
    }

    clearSelection() {
        this.$selection = {}
        this.getProperty('hasSelection').reset(false)
        if (this.$signals.selectionChanged) this.$signals.selectionChanged()
    }

    clearCurrentIndex() {
        let previous = this.getPropertyValue('currentIndex')
        this.$currentRow = -1
        this.$currentColumn = -1
        this.getProperty('currentIndex').reset(null)
        if (this.$signals.currentChanged) this.$signals.currentChanged(null, previous)
    }

    reset() {
        this.clearSelection()
        this.clearCurrentIndex()
    }
}

module.exports.ItemSelectionModel = ItemSelectionModel
