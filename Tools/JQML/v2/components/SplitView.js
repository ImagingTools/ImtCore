const { Item } = require('./Item')
const { QVar, QReal, QBool } = require('../utils/properties')

class SplitView extends Item {
    static Horizontal = 1
    static Vertical = 2

    static defaultProperties = {
        orientation: { type: QReal, value: SplitView.Horizontal, changed: '$orientationChanged' },
        handle: { type: QVar, value: null },
        resizing: { type: QBool, value: false },
    }

    static defaultSignals = {
    }

    constructor(parent, exCtx, exModel) {
        super(parent, exCtx, exModel)
        this.$exCtx = exCtx
        this.$splitItems = []
        this.$handles = []
        this.$dragging = false
        this.$dragIndex = -1
        this.$dragStart = 0
        this.$dragOrigin = 0

        this.setStyle({
            overflow: 'hidden',
            position: 'relative',
        })
    }

    $complete() {
        super.$complete()
        this._collectSplitItems()
        this._buildHandles()
        this._layout()
    }

    _collectSplitItems() {
        let children = this.getProperty('children').get()
        this.$splitItems = []
        for (let i = 0; i < children.length; i++) {
            let ch = children[i]
            if (ch instanceof Item && ch !== this.getPropertyValue('handle')) {
                this.$splitItems.push(ch)
            }
        }
    }

    _buildHandles() {
        // Remove old handles
        for (let i = 0; i < this.$handles.length; i++) {
            if (this.$handles[i].parentNode) this.$handles[i].parentNode.removeChild(this.$handles[i])
        }
        this.$handles = []

        let count = this.$splitItems.length
        if (count < 2) return

        let isHorizontal = this.getPropertyValue('orientation') === SplitView.Horizontal
        let self = this

        for (let i = 0; i < count - 1; i++) {
            let handle = document.createElement('div')
            handle.style.cssText =
                'position:absolute;z-index:10;' +
                (isHorizontal
                    ? 'width:6px;cursor:col-resize;top:0;bottom:0;'
                    : 'height:6px;cursor:row-resize;left:0;right:0;')
            handle.style.background = '#3c3c3c'

            let handleDelegate = this.getPropertyValue('handle')
            if (handleDelegate) {
                handle.style.background = 'transparent'
            }

            this.$dom.appendChild(handle)
            this.$handles.push(handle)

            ;(function(idx) {
                handle.addEventListener('mousedown', function(e) {
                    e.preventDefault()
                    e.stopPropagation()
                    self.$dragging = true
                    self.$dragIndex = idx
                    self.$dragStart = isHorizontal ? e.clientX : e.clientY
                    let item = self.$splitItems[idx]
                    self.$dragOrigin = isHorizontal
                        ? item.getPropertyValue('width')
                        : item.getPropertyValue('height')
                    self.getProperty('resizing').reset(true)

                    function onMove(e2) {
                        if (!self.$dragging) return
                        let delta = (isHorizontal ? e2.clientX : e2.clientY) - self.$dragStart
                        self._applyDrag(idx, delta)
                    }
                    function onUp() {
                        self.$dragging = false
                        self.$dragIndex = -1
                        self.getProperty('resizing').reset(false)
                        window.removeEventListener('mousemove', onMove)
                        window.removeEventListener('mouseup', onUp)
                    }
                    window.addEventListener('mousemove', onMove)
                    window.addEventListener('mouseup', onUp)
                })
            })(i)
        }
    }

    _applyDrag(handleIndex, delta) {
        let isHorizontal = this.getPropertyValue('orientation') === SplitView.Horizontal
        let itemA = this.$splitItems[handleIndex]
        let itemB = this.$splitItems[handleIndex + 1]

        let sizeA = this.$dragOrigin + delta
        let prop = isHorizontal ? 'width' : 'height'

        // Get min/max from attached properties
        let minA = itemA.$splitMin || 0
        let maxA = itemA.$splitMax || Infinity
        let minB = itemB.$splitMin || 0
        let maxB = itemB.$splitMax || Infinity

        let totalSize = isHorizontal
            ? itemA.getPropertyValue('width') + itemB.getPropertyValue('width')
            : itemA.getPropertyValue('height') + itemB.getPropertyValue('height')

        let sizeB = totalSize - sizeA

        // Clamp
        if (sizeA < minA) { sizeA = minA; sizeB = totalSize - sizeA }
        if (sizeA > maxA) { sizeA = maxA; sizeB = totalSize - sizeA }
        if (sizeB < minB) { sizeB = totalSize - minB; sizeA = totalSize - sizeB }
        if (sizeB > maxB) { sizeB = totalSize - maxB; sizeA = totalSize - sizeB }

        // Final clamp
        if (sizeA < 0) sizeA = 0
        if (sizeB < 0) sizeB = 0

        itemA.getProperty(prop).reset(sizeA)
        itemB.getProperty(prop).reset(sizeB)

        this._layout()
    }

    _layout() {
        let isHorizontal = this.getPropertyValue('orientation') === SplitView.Horizontal
        let totalSize = isHorizontal ? this.getPropertyValue('width') : this.getPropertyValue('height')
        let crossSize = isHorizontal ? this.getPropertyValue('height') : this.getPropertyValue('width')
        let handleSize = 6
        let count = this.$splitItems.length
        if (count === 0) return

        let handleTotal = (count - 1) * handleSize
        let availableSize = totalSize - handleTotal

        // Determine sizes for each item
        let sizes = []
        let unsetCount = 0
        let usedSize = 0

        for (let i = 0; i < count; i++) {
            let item = this.$splitItems[i]
            let pref = isHorizontal
                ? (item.$splitPreferred !== undefined ? item.$splitPreferred : item.getPropertyValue('width'))
                : (item.$splitPreferred !== undefined ? item.$splitPreferred : item.getPropertyValue('height'))

            if (pref > 0) {
                sizes.push(pref)
                usedSize += pref
            } else {
                sizes.push(-1) // unset
                unsetCount++
            }
        }

        // Distribute remaining space equally to unset items
        if (unsetCount > 0) {
            let remaining = availableSize - usedSize
            let perItem = Math.max(0, remaining / unsetCount)
            for (let i = 0; i < sizes.length; i++) {
                if (sizes[i] < 0) sizes[i] = perItem
            }
        }

        // Scale if total doesn't match available
        let totalUsed = 0
        for (let i = 0; i < sizes.length; i++) totalUsed += sizes[i]
        if (totalUsed > 0 && Math.abs(totalUsed - availableSize) > 1) {
            let scale = availableSize / totalUsed
            for (let i = 0; i < sizes.length; i++) sizes[i] = Math.round(sizes[i] * scale)
        }

        // Position items and handles
        let pos = 0
        for (let i = 0; i < count; i++) {
            let item = this.$splitItems[i]
            let size = sizes[i]

            if (isHorizontal) {
                item.getProperty('x').reset(pos)
                item.getProperty('y').reset(0)
                item.getProperty('width').reset(size)
                item.getProperty('height').reset(crossSize)
            } else {
                item.getProperty('x').reset(0)
                item.getProperty('y').reset(pos)
                item.getProperty('width').reset(crossSize)
                item.getProperty('height').reset(size)
            }
            item.setStyle({ position: 'absolute' })

            pos += size

            // Position handle
            if (i < count - 1 && this.$handles[i]) {
                if (isHorizontal) {
                    this.$handles[i].style.left = pos + 'px'
                    this.$handles[i].style.top = '0'
                    this.$handles[i].style.height = crossSize + 'px'
                } else {
                    this.$handles[i].style.top = pos + 'px'
                    this.$handles[i].style.left = '0'
                    this.$handles[i].style.width = crossSize + 'px'
                }
                pos += handleSize
            }
        }
    }

    $orientationChanged() {
        this._buildHandles()
        this._layout()
    }

    $widthChanged() {
        super.$widthChanged()
        if (this.$splitItems.length > 0) this._layout()
    }

    $heightChanged() {
        super.$heightChanged()
        if (this.$splitItems.length > 0) this._layout()
    }

    addChild(child) {
        super.addChild(child)
        if (this.$completed) {
            this._collectSplitItems()
            this._buildHandles()
            this._layout()
        }
    }

    // ── Attached property helpers ──
    // Called from QML as: SplitView.preferredWidth: 200
    // In JS usage: item.$splitPreferred = 200, item.$splitMin = 50, item.$splitMax = 400

    static setPreferredWidth(item, value) { item.$splitPreferred = value }
    static setPreferredHeight(item, value) { item.$splitPreferred = value }
    static setMinimumWidth(item, value) { item.$splitMin = value }
    static setMinimumHeight(item, value) { item.$splitMin = value }
    static setMaximumWidth(item, value) { item.$splitMax = value }
    static setMaximumHeight(item, value) { item.$splitMax = value }
    static setFillWidth(item, value) { item.$splitFill = value }
    static setFillHeight(item, value) { item.$splitFill = value }
}

module.exports.SplitView = SplitView
