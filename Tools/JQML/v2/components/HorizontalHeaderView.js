const { TableView } = require('./TableView')
const { QVar, QReal, QBool, QString } = require('../utils/properties')
const { Item } = require('./Item')

class HorizontalHeaderView extends TableView {
    static defaultProperties = {
        syncView: { type: QVar, value: null, changed: '$syncViewChanged' },
        textRole: { type: QVar, value: 'display' },
        resizableColumns: { type: QBool, value: true },
        movableColumns: { type: QBool, value: false },
    }

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)
        this.$syncedTV = null
    }

    $syncViewChanged() {
        let sv = this.getPropertyValue('syncView')
        if (this.$syncedTV && this.$syncedTV !== sv) {
            // Disconnect old sync
            if (this.$syncedTV.getSignal && this.$syncedTV.getSignal('layoutChanged')) {
                this.$syncedTV.getSignal('layoutChanged').disconnect(this, this.$onSyncLayout)
            }
        }
        this.$syncedTV = sv
        if (sv) {
            // Connect to layout changes
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

        // Sync column widths
        let colCount = sv.getPropertyValue('columns')
        for (let c = 0; c < colCount; c++) {
            let w = sv.columnWidth(c)
            if (w > 0) this.setColumnWidth(c, w)
        }

        // Sync column spacing
        this.getProperty('columnSpacing').reset(sv.getPropertyValue('columnSpacing'))

        // Sync contentX for horizontal scrolling
        this.getProperty('contentX').reset(sv.getPropertyValue('contentX'))

        // Rebuild if needed
        if (this.$ready) this.$rebuildTable()
    }

    // Override to derive model from syncView if not explicitly set
    _getModelRowCount() {
        return 1 // Header is always 1 row
    }

    _getModelColumnCount() {
        let model = this.getPropertyValue('model')
        if (model) {
            if (Array.isArray(model)) return model.length
            if (typeof model === 'number') return model
        }
        // Derive from syncView
        let sv = this.getPropertyValue('syncView')
        if (sv) return sv.getPropertyValue('columns')
        return 0
    }

    _getCellData(row, col) {
        let display = ''
        let model = this.getPropertyValue('model')
        let textRole = this.getPropertyValue('textRole') || 'display'

        if (model) {
            if (Array.isArray(model)) {
                let item = model[col]
                if (item && typeof item === 'object') {
                    display = item[textRole] !== undefined ? item[textRole] : item.display !== undefined ? item.display : String(col)
                } else if (item !== undefined) {
                    display = String(item)
                }
            }
        } else {
            // Derive header text from syncView's model columns
            let sv = this.getPropertyValue('syncView')
            if (sv) {
                let svModel = sv.getPropertyValue('model')
                if (svModel && svModel.$columns && svModel.$columns[col]) {
                    // TableModel with TableModelColumns
                    let colDef = svModel.$columns[col]
                    display = colDef.getPropertyValue ? colDef.getPropertyValue('display') : String(col)
                } else if (svModel && Array.isArray(svModel)) {
                    // Array model — use object keys
                    if (svModel.length > 0 && typeof svModel[0] === 'object') {
                        let keys = Object.keys(svModel[0]).filter(k => !k.startsWith('$'))
                        display = keys[col] || String(col)
                    }
                }
            }
            if (!display) display = String(col)
        }

        return {
            row: 0,
            column: col,
            display: display,
            model: { display: display },
            index: col,
        }
    }

    // Sync contentX with syncView
    $contentXChanged() {
        super.$contentXChanged()
        let sv = this.getPropertyValue('syncView')
        if (sv && sv.getPropertyValue('contentX') !== this.getPropertyValue('contentX')) {
            sv.getProperty('contentX').reset(this.getPropertyValue('contentX'))
        }
    }
}

module.exports = { HorizontalHeaderView }
