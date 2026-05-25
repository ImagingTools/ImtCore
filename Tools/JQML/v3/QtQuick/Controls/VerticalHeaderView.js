const TableView = require("./TableView")
const Var = require("../../QtQml/Var")
const Bool = require("../../QtQml/Bool")
const Signal = require("../../QtQml/Signal")

class VerticalHeaderView extends TableView {
    static meta = Object.assign({}, TableView.meta, {
        syncView:      { type: Var,  value: undefined },
        textRole:      { type: Var,  value: 'display' },
        resizableRows: { type: Bool, value: true },
        movableRows:   { type: Bool, value: false },

        syncViewChanged:      { type: Signal, args: [] },
        textRoleChanged:      { type: Signal, args: [] },
        resizableRowsChanged: { type: Signal, args: [] },
        movableRowsChanged:   { type: Signal, args: [] },
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
        let rowCount = sv.rows
        for (let r = 0; r < rowCount; r++) {
            let h = sv.rowHeight(r)
            if (h > 0) this.setRowHeight(r, h)
        }
        this.rowSpacing = sv.rowSpacing
        this.contentY = sv.contentY
        if (this.__tvReady) this.__rebuildTable()
    }

    _getModelColumnCount() { return 1 }

    _getModelRowCount() {
        let model = this.model
        if (model) {
            if (Array.isArray(model)) return model.length
            if (typeof model === 'number') return model
        }
        let sv = this.syncView
        if (sv) return sv.rows
        return 0
    }

    _getCellData(row, col) {
        let display  = ''
        let model    = this.model
        let textRole = this.textRole || 'display'

        if (model) {
            if (Array.isArray(model)) {
                let item = model[row]
                if (item && typeof item === 'object') display = item[textRole] !== undefined ? item[textRole] : item.display !== undefined ? item.display : String(row)
                else if (item !== undefined) display = String(item)
            }
        } else {
            display = String(row + 1)  // default: 1-based row numbers
        }

        return { row, column: 0, display, model: { display }, index: row }
    }

    SLOT_contentYChanged(o, n) {
        super.SLOT_contentYChanged(o, n)
        let sv = this.syncView
        if (sv && sv.contentY !== this.contentY) sv.contentY = this.contentY
    }
}

module.exports = VerticalHeaderView
