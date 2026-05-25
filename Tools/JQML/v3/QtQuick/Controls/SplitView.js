const Item = require("../Item")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Bool = require("../../QtQml/Bool")
const Signal = require("../../QtQml/Signal")

class SplitView extends Item {
    // Orientation constants (mirror Qt.Orientation)
    static Horizontal = 1
    static Vertical   = 2

    static meta = Object.assign({}, Item.meta, {
        handle:      { type: Var },
        orientation: { type: Real, value: SplitView.Horizontal },
        resizing:    { type: Bool, value: false },

        handleChanged:      { type: Signal, args: [] },
        orientationChanged: { type: Signal, args: [] },
        resizingChanged:    { type: Signal, args: [] }
    })

    // Per-instance state (stored directly on the raw object to avoid proxy interception)
    __splitItems  = []
    __handles     = []
    __dragging    = false
    __dragIndex   = -1
    __dragStart   = 0
    __dragOrigin  = 0

    static create(parent = null, properties = {}) {
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('SplitView')
        obj.__setDOMStyle({
            overflow: 'hidden',
            position: 'relative',
        })
        return obj
    }

    // ── Lifecycle ────────────────────────────────────────────────────────────

    __complete() {
        super.__complete && super.__complete()
        this.__collectSplitItems()
        this.__buildHandles()
        this.__layout()
    }

    // ── Property change handlers ─────────────────────────────────────────────

    SLOT_orientationChanged(oldValue, newValue) {
        this.__buildHandles()
        this.__layout()
    }

    SLOT_widthChanged(oldValue, newValue) {
        super.SLOT_widthChanged(oldValue, newValue)
        if (this.__splitItems.length > 0) this.__layout()
    }

    SLOT_heightChanged(oldValue, newValue) {
        super.SLOT_heightChanged(oldValue, newValue)
        if (this.__splitItems.length > 0) this.__layout()
    }

    SLOT_childrenChanged(oldValue, newValue) {
        super.SLOT_childrenChanged && super.SLOT_childrenChanged(oldValue, newValue)
        if (this.__splitItems) {
            this.__collectSplitItems()
            this.__buildHandles()
            this.__layout()
        }
    }

    // ── Internal – item collection ───────────────────────────────────────────

    __collectSplitItems() {
        this.__splitItems = []
        for (let child of this.children) {
            // Skip handle template instances
            if (child === this.handle) continue
            this.__splitItems.push(child)
        }
    }

    // ── Internal – handle DOM elements ──────────────────────────────────────

    __buildHandles() {
        // Remove old handle DOM nodes
        for (let h of this.__handles) {
            if (h.parentNode) h.parentNode.removeChild(h)
        }
        this.__handles = []

        let count = this.__splitItems.length
        if (count < 2) return

        let isHorizontal = this.orientation === SplitView.Horizontal
        let self = this

        for (let i = 0; i < count - 1; i++) {
            let handle = document.createElement('div')
            handle.style.cssText =
                'position:absolute;z-index:10;background:#3c3c3c;' +
                (isHorizontal
                    ? 'width:5px;cursor:col-resize;top:0;bottom:0;'
                    : 'height:5px;cursor:row-resize;left:0;right:0;')

            this.__getDOM().appendChild(handle)
            this.__handles.push(handle)

            ;(function (idx) {
                handle.addEventListener('mousedown', function (e) {
                    e.preventDefault()
                    e.stopPropagation()

                    self.__dragging   = true
                    self.__dragIndex  = idx
                    self.__dragStart  = isHorizontal ? e.clientX : e.clientY

                    let item = self.__splitItems[idx]
                    self.__dragOrigin = isHorizontal ? item.width : item.height
                    self.resizing = true

                    function onMove(e2) {
                        if (!self.__dragging) return
                        let delta = (isHorizontal ? e2.clientX : e2.clientY) - self.__dragStart
                        self.__applyDrag(idx, delta)
                    }

                    function onUp() {
                        self.__dragging  = false
                        self.__dragIndex = -1
                        self.resizing    = false
                        window.removeEventListener('mousemove', onMove)
                        window.removeEventListener('mouseup',   onUp)
                    }

                    window.addEventListener('mousemove', onMove)
                    window.addEventListener('mouseup',   onUp)
                })
            })(i)
        }
    }

    // ── Internal – drag resizing ─────────────────────────────────────────────

    __applyDrag(handleIndex, delta) {
        let isHorizontal = this.orientation === SplitView.Horizontal
        let prop  = isHorizontal ? 'width' : 'height'
        let itemA = this.__splitItems[handleIndex]
        let itemB = this.__splitItems[handleIndex + 1]

        let minA  = itemA.__splitMin  !== undefined ? itemA.__splitMin  : 0
        let maxA  = itemA.__splitMax  !== undefined ? itemA.__splitMax  : Infinity
        let minB  = itemB.__splitMin  !== undefined ? itemB.__splitMin  : 0

        let sizeA = this.__dragOrigin + delta
        let totalSize = (isHorizontal ? itemA.width : itemA.height)
                      + (isHorizontal ? itemB.width : itemB.height)
        let sizeB = totalSize - sizeA

        // Clamp A
        if (sizeA < minA) { sizeA = minA;  sizeB = totalSize - sizeA }
        if (sizeA > maxA) { sizeA = maxA;  sizeB = totalSize - sizeA }
        // Clamp B
        if (sizeB < minB) { sizeB = minB;  sizeA = totalSize - sizeB }

        if (sizeA < 0) sizeA = 0
        if (sizeB < 0) sizeB = 0

        itemA[prop] = sizeA
        itemB[prop] = sizeB

        this.__layout()
    }

    // ── Internal – layout ────────────────────────────────────────────────────

    __layout() {
        let isHorizontal = this.orientation === SplitView.Horizontal
        let totalSize  = isHorizontal ? this.width  : this.height
        let crossSize  = isHorizontal ? this.height : this.width
        let handleSize = 5
        let count = this.__splitItems.length
        if (count === 0) return

        let handleTotal   = (count - 1) * handleSize
        let availableSize = totalSize - handleTotal

        // Resolve sizes
        let sizes     = []
        let unsetCount = 0
        let usedSize  = 0

        for (let i = 0; i < count; i++) {
            let item = this.__splitItems[i]
            let pref = item.__splitPreferred !== undefined
                ? item.__splitPreferred
                : (isHorizontal ? item.width : item.height)

            if (pref > 0) {
                sizes.push(pref)
                usedSize += pref
            } else {
                sizes.push(-1)
                unsetCount++
            }
        }

        if (unsetCount > 0) {
            let remaining = availableSize - usedSize
            let perItem   = Math.max(0, remaining / unsetCount)
            for (let i = 0; i < count; i++) {
                if (sizes[i] < 0) sizes[i] = perItem
            }
        }

        // Scale to fit if needed
        let totalUsed = sizes.reduce((a, b) => a + b, 0)
        if (totalUsed > 0 && Math.abs(totalUsed - availableSize) > 1) {
            let scale = availableSize / totalUsed
            sizes = sizes.map(s => Math.round(s * scale))
        }

        // Position items and handles
        let pos = 0
        for (let i = 0; i < count; i++) {
            let item = this.__splitItems[i]
            let size = sizes[i]

            if (isHorizontal) {
                item.x      = pos
                item.y      = 0
                item.width  = size
                item.height = crossSize
            } else {
                item.x      = 0
                item.y      = pos
                item.width  = crossSize
                item.height = size
            }
            item.__setDOMStyle && item.__setDOMStyle({ position: 'absolute' })

            pos += size

            if (i < count - 1 && this.__handles[i]) {
                let h = this.__handles[i]
                if (isHorizontal) {
                    h.style.left   = pos + 'px'
                    h.style.top    = '0'
                    h.style.height = crossSize + 'px'
                } else {
                    h.style.top    = pos + 'px'
                    h.style.left   = '0'
                    h.style.width  = crossSize + 'px'
                }
                pos += handleSize
            }
        }
    }

    // ── Attached-property helpers ────────────────────────────────────────────
    // Usage in JS:  item.__splitPreferred = 200; item.__splitMin = 50

    static setPreferredWidth(item, value)  { item.__splitPreferred = value }
    static setPreferredHeight(item, value) { item.__splitPreferred = value }
    static setMinimumWidth(item, value)    { item.__splitMin = value }
    static setMinimumHeight(item, value)   { item.__splitMin = value }
    static setMaximumWidth(item, value)    { item.__splitMax = value }
    static setMaximumHeight(item, value)   { item.__splitMax = value }
    static setFillWidth(item, value)       { item.__splitFill = value }
    static setFillHeight(item, value)      { item.__splitFill = value }
}



module.exports = SplitView
