const { TableView } = require('./TableView')
const { QVar, QReal, QBool } = require('../utils/properties')

class VerticalHeaderView extends TableView {
    static defaultProperties = {
        syncView: { type: QVar, value: null, changed: '$syncViewChanged' },
        textRole: { type: QVar, value: 'display' },
        resizableRows: { type: QBool, value: true },
        movableRows: { type: QBool, value: false },
    }

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)
        this.$syncedTV = null
    }

    $syncViewChanged() {
        let sv = this.getPropertyValue('syncView')
        if (this.$syncedTV && this.$syncedTV !== sv) {
            if (this.$syncedTV.getSignal && this.$syncedTV.getSignal('layoutChanged')) {
                this.$syncedTV.getSignal('layoutChanged').disconnect(this, this.$onSyncLayout)
            }
        }
        this.$syncedTV = sv
        if (sv) {
            if (sv.getSignal && sv.getSignal('layoutChanged')) {
                sv.getSignal('layoutChanged').connect(this, this.$onSyncLayout)
            }
            this.$syncFromView()
        }
    }

    $onSyncLayout() {
        this.$syncFromView()
    }

    $syncFromView() {
        let sv = this.$syncedTV
        if (!sv) return

        // Sync row heights
        let rowCount = sv.getPropertyValue('rows')
        for (let r = 0; r < rowCount; r++) {
            let h = sv.rowHeight(r)
            if (h > 0) this.setRowHeight(r, h)
        }

        // Sync row spacing
        this.getProperty('rowSpacing').reset(sv.getPropertyValue('rowSpacing'))

        // Sync contentY for vertical scrolling
        this.getProperty('contentY').reset(sv.getPropertyValue('contentY'))

        if (this.$ready) this.$rebuildTable()
    }

    _getModelRowCount() {
        let model = this.getPropertyValue('model')
        if (model) {
            if (Array.isArray(model)) return model.length
            if (typeof model === 'number') return model
        }
        let sv = this.getPropertyValue('syncView')
        if (sv) return sv.getPropertyValue('rows')
        return 0
    }

    _getModelColumnCount() {
        return 1 // Vertical header is always 1 column
    }

    _getCellData(row, col) {
        let display = ''
        let model = this.getPropertyValue('model')
        let textRole = this.getPropertyValue('textRole') || 'display'

        if (model) {
            if (Array.isArray(model)) {
                let item = model[row]
                if (item && typeof item === 'object') {
                    display = item[textRole] !== undefined ? item[textRole] : item.display !== undefined ? item.display : String(row)
                } else if (item !== undefined) {
                    display = String(item)
                }
            }
        } else {
            // Default: row numbers (1-based, matching Qt behavior)
            display = String(row + 1)
        }

        return {
            row: row,
            column: 0,
            display: display,
            model: { display: display },
            index: row,
        }
    }

    $contentYChanged() {
        super.$contentYChanged()
        let sv = this.getPropertyValue('syncView')
        if (sv && sv.getPropertyValue('contentY') !== this.getPropertyValue('contentY')) {
            sv.getProperty('contentY').reset(this.getPropertyValue('contentY'))
        }
    }
}

module.exports = { VerticalHeaderView }
