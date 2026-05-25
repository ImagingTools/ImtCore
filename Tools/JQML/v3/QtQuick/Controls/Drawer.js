const Popup = require("./Popup")
const Real = require("../../QtQml/Real")
const Int = require("../../QtQml/Int")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class Drawer extends Popup {
    // Edge constants (mirror Qt.Edge)
    static LeftEdge   = 1
    static RightEdge  = 2
    static TopEdge    = 4
    static BottomEdge = 8

    static meta = Object.assign({}, Popup.meta, {
        dragMargin: { type: Real, value: 20 },
        edge:       { type: Int,  value: Drawer.LeftEdge },
        interactive: { type: Var, value: true },
        position:   { type: Real, value: 0 },

        dragMarginChanged:  { type: Signal, args: [] },
        edgeChanged:        { type: Signal, args: [] },
        interactiveChanged: { type: Signal, args: [] },
        positionChanged:    { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}) {
        let obj = super.create(parent, properties)
        let dom = obj.__getDOM()
        dom.classList.add('Drawer')
        dom.style.position  = 'fixed'
        dom.style.zIndex    = '1000'
        dom.style.overflowY = 'auto'
        dom.style.transition = 'transform 0.3s ease'
        obj.__applyEdgeLayout()
        return obj
    }

    // ── Property change handlers ─────────────────────────────────────────────

    SLOT_edgeChanged(oldValue, newValue) {
        this.__applyEdgeLayout()
    }

    SLOT_positionChanged(oldValue, newValue) {
        this.__applyEdgeTransform(newValue)
    }

    SLOT_visibleChanged(oldValue, newValue) {
        super.SLOT_visibleChanged && super.SLOT_visibleChanged(oldValue, newValue)
        if (newValue) {
            this.__applyEdgeLayout()
        }
    }

    // ── Internal helpers ────────────────────────────────────────────────────

    __applyEdgeLayout() {
        let dom = this.__getDOM()
        let edge = this.edge

        // Reset all edge anchors
        dom.style.left   = ''
        dom.style.right  = ''
        dom.style.top    = ''
        dom.style.bottom = ''
        dom.style.width  = ''
        dom.style.height = ''

        switch (edge) {
            case Drawer.LeftEdge:
                dom.style.left   = '0'
                dom.style.top    = '0'
                dom.style.bottom = '0'
                dom.style.width  = (this.width > 0 ? this.width : 280) + 'px'
                dom.style.height = '100%'
                break
            case Drawer.RightEdge:
                dom.style.right  = '0'
                dom.style.top    = '0'
                dom.style.bottom = '0'
                dom.style.width  = (this.width > 0 ? this.width : 280) + 'px'
                dom.style.height = '100%'
                break
            case Drawer.TopEdge:
                dom.style.left   = '0'
                dom.style.right  = '0'
                dom.style.top    = '0'
                dom.style.width  = '100%'
                dom.style.height = (this.height > 0 ? this.height : 200) + 'px'
                break
            case Drawer.BottomEdge:
                dom.style.left   = '0'
                dom.style.right  = '0'
                dom.style.bottom = '0'
                dom.style.width  = '100%'
                dom.style.height = (this.height > 0 ? this.height : 200) + 'px'
                break
        }

        this.__applyEdgeTransform(this.position)
    }

    __applyEdgeTransform(position) {
        let dom    = this.__getDOM()
        let edge   = this.edge
        let offset = (1 - position) * 100

        switch (edge) {
            case Drawer.LeftEdge:
                dom.style.transform = `translateX(-${offset}%)`
                break
            case Drawer.RightEdge:
                dom.style.transform = `translateX(${offset}%)`
                break
            case Drawer.TopEdge:
                dom.style.transform = `translateY(-${offset}%)`
                break
            case Drawer.BottomEdge:
                dom.style.transform = `translateY(${offset}%)`
                break
        }
    }

    // ── Public API ───────────────────────────────────────────────────────────

    open() {
        this.visible  = true
        this.position = 1.0
    }

    close() {
        this.position = 0
        setTimeout(() => {
            this.visible = false
        }, 300)
    }
}



module.exports = Drawer
