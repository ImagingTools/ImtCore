const TableView = require("./TableView")
const Var = require("../../QtQml/Var")
const Bool = require("../../QtQml/Bool")
const Signal = require("../../QtQml/Signal")

class HorizontalHeaderView extends TableView {
    static meta = Object.assign({}, TableView.meta, {
        syncView:         { type: Var,  value: undefined },
        textRole:         { type: Var,  value: 'display' },
        resizableColumns: { type: Bool, value: true },
        movableColumns:   { type: Bool, value: false },

        syncViewChanged:         { type: Signal, args: [] },
        textRoleChanged:         { type: Signal, args: [] },
        resizableColumnsChanged: { type: Signal, args: [] },
        movableColumnsChanged:   { type: Signal, args: [] },
    })

    static create(parent, ...args) {
        let obj = super.create(parent, ...args)
        obj.__syncedTV = null
        return obj
    }

    SLOT_syncViewChanged(oldValue, newValue) {
        if (this.__syncedTV && this.__syncedTV !== newValue) {
            try { this.__syncedTV.layoutChanged.disconnect(this.__onSyncLayout) } catch(e) {}
        }
        this.__syncedTV = newValue
        if (newValue) {
            this.__onSyncLayout = () => this.__syncFromView()
            newValue.layoutChanged.connect(this.__onSyncLayout)
            this.__syncFromView()
        }
    }

    __syncFromView() {
        let sv = this.__syncedTV
        if (!sv) return
        let colCount = sv.columns
        for (let c = 0; c < colCount; c++) {
            let w = sv.columnWidth(c)
            if (w > 0) this.setColumnWidth(c, w)
        }
        this.columnSpacing = sv.columnSpacing
        this.contentX = sv.contentX
        if (this.__tvReady) this.__rebuildTable()
    }

    _getModelRowCount() { return 1 }

    _getModelColumnCount() {
        let model = this.model
        if (model) {
            if (Array.isArray(model)) return model.length
            if (typeof model === 'number') return model
        }
        let sv = this.syncView
        if (sv) return sv.columns
        return 0
    }

    _getCellData(row, col) {
        let display  = ''
        let model    = this.model
        let textRole = this.textRole || 'display'

        if (model) {
            if (Array.isArray(model)) {
                let item = model[col]
                if (item && typeof item === 'object') display = item[textRole] !== undefined ? item[textRole] : item.display !== undefined ? item.display : String(col)
                else if (item !== undefined) display = String(item)
            }
        } else {
            let sv = this.syncView
            if (sv) {
                let svModel = sv.model
                if (svModel && svModel.__columns && svModel.__columns[col]) {
                    let colDef = svModel.__columns[col]
                    display = colDef.display !== undefined ? colDef.display : String(col)
                } else if (Array.isArray(svModel)) {
                    if (svModel.length > 0 && typeof svModel[0] === 'object') {
                        let keys = Object.keys(svModel[0]).filter(k => !k.startsWith('$'))
                        display = keys[col] || String(col)
                    }
                }
            }
            if (!display) display = String(col)
        }

        return { row: 0, column: col, display, model: { display }, index: col }
    }

    SLOT_contentXChanged(o, n) {
        super.SLOT_contentXChanged(o, n)
        let sv = this.syncView
        if (sv && sv.contentX !== this.contentX) sv.contentX = this.contentX
    }
}

module.exports = HorizontalHeaderView
