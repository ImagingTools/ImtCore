const { Item } = require('./Item')
const { QVar, QBool } = require('../utils/properties')
const { ItemSelectionModel } = require('./ItemSelectionModel')

class SelectionRectangle extends Item {
    static defaultProperties = {
        target: { type: QVar, value: null, changed: '$targetChanged' },
        active: { type: QBool, value: false },
        dragging: { type: QBool, value: false },
    }

    static defaultSignals = {
        selectionStarted: { params: [] },
        selectionChanged: { params: [] },
        selectionFinished: { params: [] },
    }

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)
        this.$startX = 0
        this.$startY = 0
        this.$rectDom = null

        this.setStyle({
            display: 'none',
            pointerEvents: 'none',
        })
    }

    $targetChanged() {
        let target = this.getPropertyValue('target')
        if (target && target.$dom) {
            this._installHandlers(target)
        }
    }

    _installHandlers(tableView) {
        if (this.$handlersInstalled) return
        this.$handlersInstalled = true

        let dom = tableView.$dom
        let self = this

        // Create visual rectangle
        this.$rectDom = document.createElement('div')
        this.$rectDom.style.cssText = 'position:absolute;border:1px dashed #0078d4;background:rgba(0,120,212,0.1);pointer-events:none;display:none;z-index:100;'
        dom.appendChild(this.$rectDom)

        dom.addEventListener('mousedown', (e) => {
            if (e.button !== 0) return
            if (!e.shiftKey && !e.ctrlKey) return // Only activate with modifier

            let rect = dom.getBoundingClientRect()
            self.$startX = e.clientX - rect.left + tableView.getPropertyValue('contentX')
            self.$startY = e.clientY - rect.top + tableView.getPropertyValue('contentY')
            self.getProperty('dragging').reset(true)
            self.getProperty('active').reset(true)
            self.$rectDom.style.display = 'block'

            if (self.$signals.selectionStarted) self.$signals.selectionStarted()

            e.preventDefault()
        })

        dom.addEventListener('mousemove', (e) => {
            if (!self.getPropertyValue('dragging')) return

            let rect = dom.getBoundingClientRect()
            let curX = e.clientX - rect.left + tableView.getPropertyValue('contentX')
            let curY = e.clientY - rect.top + tableView.getPropertyValue('contentY')

            let x1 = Math.min(self.$startX, curX)
            let y1 = Math.min(self.$startY, curY)
            let x2 = Math.max(self.$startX, curX)
            let y2 = Math.max(self.$startY, curY)

            self.$rectDom.style.left = (x1 - tableView.getPropertyValue('contentX')) + 'px'
            self.$rectDom.style.top = (y1 - tableView.getPropertyValue('contentY')) + 'px'
            self.$rectDom.style.width = (x2 - x1) + 'px'
            self.$rectDom.style.height = (y2 - y1) + 'px'

            // Select cells within rectangle
            self._selectRange(tableView, x1, y1, x2, y2)

            if (self.$signals.selectionChanged) self.$signals.selectionChanged()
        })

        let finish = (e) => {
            if (!self.getPropertyValue('dragging')) return
            self.getProperty('dragging').reset(false)
            self.$rectDom.style.display = 'none'
            if (self.$signals.selectionFinished) self.$signals.selectionFinished()
        }

        dom.addEventListener('mouseup', finish)
        dom.addEventListener('mouseleave', finish)
    }

    _selectRange(tableView, x1, y1, x2, y2) {
        let sm = tableView.getPropertyValue('selectionModel')
        if (!sm || !(sm instanceof ItemSelectionModel)) return

        sm.clearSelection()

        let rowCount = tableView.getPropertyValue('rows')
        let colCount = tableView.getPropertyValue('columns')

        for (let r = 0; r < rowCount; r++) {
            let ry = tableView.$rowPositions[r]
            let rh = tableView.$resolvedRowHeights[r]
            if (ry + rh < y1 || ry > y2) continue

            for (let c = 0; c < colCount; c++) {
                let cx = tableView.$colPositions[c]
                let cw = tableView.$resolvedColumnWidths[c]
                if (cx + cw < x1 || cx > x2) continue

                let behavior = tableView.getPropertyValue('selectionBehavior')
                let cmd = ItemSelectionModel.Select
                if (behavior === tableView.constructor.SelectRows) cmd |= ItemSelectionModel.Rows
                else if (behavior === tableView.constructor.SelectColumns) cmd |= ItemSelectionModel.Columns

                sm.select({ row: r, column: c }, cmd)
            }
        }
    }

    destroy() {
        if (this.$rectDom && this.$rectDom.parentNode) {
            this.$rectDom.parentNode.removeChild(this.$rectDom)
        }
        super.destroy()
    }
}

module.exports.SelectionRectangle = SelectionRectangle
