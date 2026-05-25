const Control = require("./Control")
const Real = require("../../QtQml/Real")
const Int = require("../../QtQml/Int")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class ScrollBar extends Control {
    // Orientation constants
    static Horizontal = 1
    static Vertical   = 2

    // Policy constants
    static AsNeeded    = 0
    static AlwaysOff   = 1
    static AlwaysOn    = 2

    static meta = Object.assign({}, Control.meta, {
        active:       { type: Var,  value: false },
        interactive:  { type: Var,  value: true  },
        minimumSize:  { type: Real, value: 0.05  },
        orientation:  { type: Int,  value: ScrollBar.Vertical },
        policy:       { type: Int,  value: ScrollBar.AsNeeded },
        position:     { type: Real, value: 0 },
        pressed:      { type: Var,  value: false },
        size:         { type: Real, value: 0 },
        snapMode:     { type: Int,  value: 0 },
        stepSize:     { type: Real, value: 0 },
        visualPosition: { type: Real, value: 0 },
        visualSize:     { type: Real, value: 0 },

        activeChanged:         { type: Signal, args: [] },
        interactiveChanged:    { type: Signal, args: [] },
        minimumSizeChanged:    { type: Signal, args: [] },
        orientationChanged:    { type: Signal, args: [] },
        policyChanged:         { type: Signal, args: [] },
        positionChanged:       { type: Signal, args: [] },
        pressedChanged:        { type: Signal, args: [] },
        sizeChanged:           { type: Signal, args: [] },
        snapModeChanged:       { type: Signal, args: [] },
        stepSizeChanged:       { type: Signal, args: [] },
        visualPositionChanged: { type: Signal, args: [] },
        visualSizeChanged:     { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}) {
        let obj = super.create(parent, properties)
        let dom = obj.__getDOM()
        dom.classList.add('ScrollBar')

        let isV = obj.orientation !== ScrollBar.Horizontal

        // Track (outer container)
        obj.__setDOMStyle({
            position:         'absolute',
            background:       'transparent',
            zIndex:           '10',
            boxSizing:        'border-box',
            cursor:           'default',
            userSelect:       'none',
            ...(isV
                ? { top: '0', right: '0', bottom: '0', width: '8px' }
                : { left: '0', right: '0', bottom: '0', height: '8px' }
            ),
        })

        // Thumb
        let thumb = document.createElement('div')
        thumb.style.cssText = 'position:absolute;border-radius:4px;background:rgba(0,0,0,0.35);transition:background 0.15s;'
        dom.appendChild(thumb)
        obj.__thumbDOM = thumb

        // ── Drag logic ────────────────────────────────────────────────────────
        let dragStart    = 0
        let dragOrigin   = 0
        let self         = obj.__self || obj

        thumb.addEventListener('mousedown', (e) => {
            e.preventDefault()
            e.stopPropagation()
            if (!self.interactive) return
            self.pressed = true
            dragStart  = isV ? e.clientY : e.clientX
            dragOrigin = self.position

            const onMove = (e2) => {
                let trackSize = isV ? dom.getBoundingClientRect().height : dom.getBoundingClientRect().width
                if (trackSize <= 0) return
                let delta  = ((isV ? e2.clientY : e2.clientX) - dragStart) / trackSize
                let newPos = Math.max(0, Math.min(1 - self.size, dragOrigin + delta))
                self.position = newPos
            }
            const onUp = () => {
                self.pressed = false
                window.removeEventListener('mousemove', onMove)
                window.removeEventListener('mouseup',   onUp)
            }
            window.addEventListener('mousemove', onMove)
            window.addEventListener('mouseup',   onUp)
        })

        // Click on track (outside thumb) — jump
        dom.addEventListener('click', (e) => {
            if (!self.interactive || e.target === thumb) return
            let rect     = dom.getBoundingClientRect()
            let trackLen = isV ? rect.height : rect.width
            let click    = isV ? (e.clientY - rect.top) : (e.clientX - rect.left)
            let newPos   = Math.max(0, Math.min(1 - self.size, click / trackLen - self.size / 2))
            self.position = newPos
        })

        // Hover: show/hide
        dom.addEventListener('mouseenter', () => {
            thumb.style.background = 'rgba(0,0,0,0.5)'
        })
        dom.addEventListener('mouseleave', () => {
            if (!self.pressed)
                thumb.style.background = 'rgba(0,0,0,0.35)'
        })

        // Update thumb initially
        obj.__updateThumb()

        return obj
    }

    // ── Property change handlers ──────────────────────────────────────────────

    SLOT_positionChanged(oldValue, newValue) {
        this.visualPosition = newValue
        this.__updateThumb()
    }

    SLOT_sizeChanged(oldValue, newValue) {
        this.visualSize = newValue
        this.__updateThumb()
        this.__applyPolicy()
    }

    SLOT_orientationChanged(oldValue, newValue) {
        this.__applyOrientation()
        this.__updateThumb()
    }

    SLOT_policyChanged(oldValue, newValue) {
        this.__applyPolicy()
    }

    SLOT_widthChanged(oldValue, newValue) {
        this.__updateThumb()
    }

    SLOT_heightChanged(oldValue, newValue) {
        this.__updateThumb()
    }

    // ── Internal helpers ──────────────────────────────────────────────────────

    __isVertical() {
        return this.orientation !== ScrollBar.Horizontal
    }

    __applyOrientation() {
        let isV = this.__isVertical()
        this.__setDOMStyle({
            top:    isV ? '0'   : '',
            right:  isV ? '0'   : '',
            bottom: isV ? '0'   : '0',
            left:   isV ? ''    : '0',
            width:  isV ? '8px' : '',
            height: isV ? ''    : '8px',
        })
    }

    __applyPolicy() {
        let policy   = this.policy
        let thumbSize = this.size
        if (policy === ScrollBar.AlwaysOff) {
            this.__setDOMStyle({ display: 'none' })
        } else if (policy === ScrollBar.AlwaysOn) {
            this.__setDOMStyle({ display: '' })
        } else {
            // AsNeeded: hide when size >= 1 (content fits entirely)
            this.__setDOMStyle({ display: thumbSize >= 1 || thumbSize <= 0 ? 'none' : '' })
        }
    }

    __updateThumb() {
        let thumb = this.__thumbDOM
        if (!thumb) return

        let isV      = this.__isVertical()
        let trackLen = isV ? (this.height || 0) : (this.width || 0)
        let pos      = Math.max(0, Math.min(1, this.position || 0))
        let sz       = Math.max(this.minimumSize || 0.05, Math.min(1, this.size || 0))
        let thumbLen = Math.max(16, sz * trackLen)
        let thumbPos = pos * (trackLen - thumbLen)

        if (isV) {
            thumb.style.top    = thumbPos + 'px'
            thumb.style.height = thumbLen + 'px'
            thumb.style.left   = '1px'
            thumb.style.right  = '1px'
            thumb.style.width  = ''
        } else {
            thumb.style.left   = thumbPos + 'px'
            thumb.style.width  = thumbLen + 'px'
            thumb.style.top    = '1px'
            thumb.style.bottom = '1px'
            thumb.style.height = ''
        }

        this.__applyPolicy()
    }

    // ── Public API ────────────────────────────────────────────────────────────

    increase() {
        let step = this.stepSize || 0.1
        this.position = Math.min(1 - this.size, this.position + step)
    }

    decrease() {
        let step = this.stepSize || 0.1
        this.position = Math.max(0, this.position - step)
    }
}



module.exports = ScrollBar
