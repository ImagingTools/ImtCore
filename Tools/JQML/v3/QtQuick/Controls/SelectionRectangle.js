const Item = require("../Item")
const Bool = require("../../QtQml/Bool")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

/**
 * SelectionRectangle — rubber-band selection over a TableView / target item.
 * Draws a dashed rectangle overlay while the user drags, then calls
 * selectionModel.select() for the covered cells.
 */
class SelectionRectangle extends Item {
    static SelectionDisabled = 0
    static Drag = 1
    static PressAndHold = 2
    static Auto = 3

    static meta = Object.assign({}, Item.meta, {
        target:        { type: Var, value: undefined },
        selectionMode: { type: Var, value: SelectionRectangle.Drag },
        active:        { type: Bool, value: false },
        dragging:      { type: Bool, value: false },

        targetChanged:        { type: Signal, args: [] },
        selectionModeChanged: { type: Signal, args: [] },
        activeChanged:        { type: Signal, args: [] },
        draggingChanged:      { type: Signal, args: [] },
    })

    static create(parent, ...args) {
        let obj = super.create(parent, ...args)
        obj.__DOM.style.display = 'none'
        obj.__rubberBandDOM = null
        obj.__startX = 0
        obj.__startY = 0
        return obj
    }

    SLOT_targetChanged(oldValue, newValue) {
        this.__uninstallHandlers(oldValue)
        if (newValue) this.__installHandlers(newValue)
    }

    __installHandlers(target) {
        let dom = target.__DOM || (typeof target.__getDOM === 'function' ? target.__getDOM() : null)
        if (!dom) return

        this.__targetDOM = dom

        let self = this
        this.__onMouseDown = function(e) {
            if (e.button !== 0) return
            let rect = dom.getBoundingClientRect()
            self.__startX = e.clientX - rect.left
            self.__startY = e.clientY - rect.top
            self.active = true
            self.dragging = true
            self.__createRubberBand(dom)
            self.__updateRubberBand(self.__startX, self.__startY, 0, 0)
            document.addEventListener('mousemove', self.__onMouseMove, true)
            document.addEventListener('mouseup', self.__onMouseUp, true)
        }

        this.__onMouseMove = function(e) {
            if (!self.dragging) return
            let rect = dom.getBoundingClientRect()
            let curX = e.clientX - rect.left
            let curY = e.clientY - rect.top
            let x = Math.min(self.__startX, curX)
            let y = Math.min(self.__startY, curY)
            let w = Math.abs(curX - self.__startX)
            let h = Math.abs(curY - self.__startY)
            self.__updateRubberBand(x, y, w, h)
        }

        this.__onMouseUp = function(e) {
            if (!self.dragging) return
            let rect = dom.getBoundingClientRect()
            let curX = e.clientX - rect.left
            let curY = e.clientY - rect.top
            let x1 = Math.min(self.__startX, curX)
            let y1 = Math.min(self.__startY, curY)
            let x2 = Math.max(self.__startX, curX)
            let y2 = Math.max(self.__startY, curY)
            self.__destroyRubberBand()
            self.dragging = false
            self.active = false
            self.__selectRange(x1, y1, x2, y2)
            document.removeEventListener('mousemove', self.__onMouseMove, true)
            document.removeEventListener('mouseup', self.__onMouseUp, true)
        }

        dom.addEventListener('mousedown', this.__onMouseDown)
    }

    __uninstallHandlers(target) {
        if (!target) return
        let dom = target.__DOM || (typeof target.__getDOM === 'function' ? target.__getDOM() : null)
        if (dom && this.__onMouseDown) {
            dom.removeEventListener('mousedown', this.__onMouseDown)
        }
        if (this.__onMouseMove) document.removeEventListener('mousemove', this.__onMouseMove, true)
        if (this.__onMouseUp)   document.removeEventListener('mouseup',   this.__onMouseUp,   true)
    }

    __createRubberBand(container) {
        this.__destroyRubberBand()
        let rb = document.createElement('div')
        rb.style.cssText =
            'position:absolute;box-sizing:border-box;pointer-events:none;' +
            'border:2px dashed #0078d4;background:rgba(0,120,212,0.1);z-index:9999;'
        container.style.position = container.style.position || 'relative'
        container.appendChild(rb)
        this.__rubberBandDOM = rb
    }

    __updateRubberBand(x, y, w, h) {
        let rb = this.__rubberBandDOM
        if (!rb) return
        rb.style.left   = x + 'px'
        rb.style.top    = y + 'px'
        rb.style.width  = w + 'px'
        rb.style.height = h + 'px'
    }

    __destroyRubberBand() {
        if (this.__rubberBandDOM) {
            if (this.__rubberBandDOM.parentNode)
                this.__rubberBandDOM.parentNode.removeChild(this.__rubberBandDOM)
            this.__rubberBandDOM = null
        }
    }

    __selectRange(x1, y1, x2, y2) {
        let tv = this.target
        if (!tv) return
        let sm = tv.selectionModel
        if (!sm) return

        // Use TableView API to convert content coordinates to cells
        let contentX = tv.contentX || 0
        let contentY = tv.contentY || 0
        let cx1 = x1 + contentX, cy1 = y1 + contentY
        let cx2 = x2 + contentX, cy2 = y2 + contentY

        if (typeof tv.cellAtPosition === 'function') {
            let tl = tv.cellAtPosition(cx1, cy1)
            let br = tv.cellAtPosition(cx2, cy2)
            if (tl && br && tl.row >= 0 && br.row >= 0) {
                sm.clearSelection && sm.clearSelection()
                let ItemSelectionModel = require('../../QtQml/Models/ItemSelectionModel')
                for (let r = tl.row; r <= br.row; r++) {
                    for (let c = tl.column; c <= br.column; c++) {
                        sm.select({ row: r, column: c }, ItemSelectionModel.Select)
                    }
                }
            }
        }
    }
}

module.exports = SelectionRectangle
