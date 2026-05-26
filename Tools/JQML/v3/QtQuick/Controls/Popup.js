const QtObject = require("../../QtQml/QtObject")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")
const Font = require("../../QtQml/Font")

// ClosePolicy flags (combinable via |)
const CloseOnPressOutside         = 1
const CloseOnPressOutsideParent   = 2
const CloseOnReleaseOutside       = 4
const CloseOnReleaseOutsideParent = 8
const CloseOnEscape               = 16

class Popup extends QtObject {
    // Static ClosePolicy constants
    static NoAutoClose                   = 0
    static CloseOnPressOutside           = CloseOnPressOutside
    static CloseOnPressOutsideParent     = CloseOnPressOutsideParent
    static CloseOnReleaseOutside         = CloseOnReleaseOutside
    static CloseOnReleaseOutsideParent   = CloseOnReleaseOutsideParent
    static CloseOnEscape                 = CloseOnEscape

    static meta = Object.assign({}, QtObject.meta, {
        activeFocus: { type: Var, value: false },
        anchors: { type: Var },
        availableHeight: { type: Real, value: 0 },
        availableWidth: { type: Real, value: 0 },
        background: { type: Var },
        bottomInset: { type: Real, value: 0 },
        bottomMargin: { type: Real, value: -1 },
        bottomPadding: { type: Real, value: 0 },
        clip: { type: Var, value: false },
        closePolicy: { type: Var, value: CloseOnEscape | CloseOnPressOutside },
        contentHeight: { type: Real, value: 0 },
        contentItem: { type: Var },
        contentWidth: { type: Real, value: 0 },
        dim: { type: Var, value: false },
        enabled: { type: Var, value: true },
        focus: { type: Var, value: false },
        font: { type: Font },
        height: { type: Real, value: 0 },
        horizontalPadding: { type: Real, value: 0 },
        implicitHeight: { type: Real, value: 0 },
        implicitWidth: { type: Real, value: 0 },
        leftMargin: { type: Real, value: -1 },
        leftPadding: { type: Real, value: 0 },
        margins: { type: Real, value: -1 },
        mirrored: { type: Var, value: false },
        modal: { type: Var, value: false },
        opacity: { type: Real, value: 1 },
        opened: { type: Var, value: false },
        padding: { type: Real, value: 0 },
        rightMargin: { type: Real, value: -1 },
        rightPadding: { type: Real, value: 0 },
        scale: { type: Real, value: 1 },
        spacing: { type: Real, value: 0 },
        topMargin: { type: Real, value: -1 },
        topPadding: { type: Real, value: 0 },
        verticalPadding: { type: Real, value: 0 },
        visible: { type: Var, value: false },
        width: { type: Real, value: 0 },
        x: { type: Real, value: 0 },
        y: { type: Real, value: 0 },
        z: { type: Real, value: 0 },

        closePolicyChanged: { type: Signal, args: [] },
        dimChanged:         { type: Signal, args: [] },
        enabledChanged:     { type: Signal, args: [] },
        focusChanged:       { type: Signal, args: [] },
        heightChanged:      { type: Signal, args: [] },
        modalChanged:       { type: Signal, args: [] },
        opacityChanged:     { type: Signal, args: [] },
        openedChanged:      { type: Signal, args: [] },
        paddingChanged:     { type: Signal, args: [] },
        spacingChanged:     { type: Signal, args: [] },
        visibleChanged:     { type: Signal, args: [] },
        widthChanged:       { type: Signal, args: [] },
        xChanged:           { type: Signal, args: [] },
        yChanged:           { type: Signal, args: [] },
        zChanged:           { type: Signal, args: [] },

        closed:      { type: Signal, args: [] },
        aboutToHide: { type: Signal, args: [] },
        aboutToShow: { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}) {
        let obj = super.create(parent, properties)

        // Fixed full-screen overlay container (lives on document.body)
        let overlay = document.createElement('div')
        overlay.style.cssText = 'position:fixed;left:0;top:0;right:0;bottom:0;z-index:10000;display:none;pointer-events:none;'
        document.body.appendChild(overlay)
        obj.__overlayDOM = overlay

        // Dim / modal background element
        let dimEl = document.createElement('div')
        dimEl.style.cssText = 'position:absolute;left:0;top:0;right:0;bottom:0;background:transparent;pointer-events:none;display:none;'
        overlay.appendChild(dimEl)
        obj.__dimDOM = dimEl

        // Content box – the actual visible popup panel
        let box = document.createElement('div')
        box.style.cssText = 'position:absolute;display:flex;flex-direction:column;pointer-events:auto;background:#ffffff;border:1px solid #c0c0c0;border-radius:4px;box-shadow:0 2px 8px rgba(0,0,0,0.15);padding:12px;box-sizing:border-box;outline:none;'
        box.setAttribute('tabindex', '-1')
        overlay.appendChild(box)
        obj.__contentDOM = box

        return obj
    }

    // ── __getDOM: Item children connect their DOM inside the content box ──────

    __getDOM() {
        return this.__contentDOM
    }

    // Called by Item.__connectDOM when a child DOM is appended.
    // Reset position so children flow in the flex column naturally.
    __onChildDOM(child, dom) {
        dom.style.position = 'relative'
        dom.style.left = ''
        dom.style.top  = ''
    }

    // ── Property change handlers ──────────────────────────────────────────────

    SLOT_visibleChanged(oldValue, newValue) {
        if (!this.__overlayDOM) return
        let self = this.__self || this

        if (newValue) {
            this.__overlayDOM.style.display = 'block'
            this.__applyPopupGeometry()
            this.__updatePopupOverlay()

            // Outside-click → close
            this.__popupMouseHandler = (e) => {
                if (self.__contentDOM && self.__contentDOM.contains(e.target)) return
                let policy = self.closePolicy
                if (policy & CloseOnPressOutside) self.close()
            }
            document.addEventListener('mousedown', this.__popupMouseHandler, true)

            // Escape key → close
            this.__popupKeyHandler = (e) => {
                if (e.key === 'Escape') {
                    let policy = self.closePolicy
                    if (policy & CloseOnEscape) self.close()
                }
            }
            document.addEventListener('keydown', this.__popupKeyHandler)

            this.opened = true
            if (this.aboutToShow) this.aboutToShow()

            // Clamp to viewport after first render
            requestAnimationFrame(() => { if (self.visible) self.__clampToViewport() })

        } else {
            this.__overlayDOM.style.display = 'none'
            this.__updatePopupOverlay()

            if (this.__popupMouseHandler) {
                document.removeEventListener('mousedown', this.__popupMouseHandler, true)
                this.__popupMouseHandler = null
            }
            if (this.__popupKeyHandler) {
                document.removeEventListener('keydown', this.__popupKeyHandler)
                this.__popupKeyHandler = null
            }

            this.opened = false
            if (this.aboutToHide) this.aboutToHide()
            if (this.closed) this.closed()
        }
    }

    SLOT_xChanged(oldValue, newValue) {
        if (this.__contentDOM) this.__contentDOM.style.left = newValue + 'px'
    }

    SLOT_yChanged(oldValue, newValue) {
        if (this.__contentDOM) this.__contentDOM.style.top = newValue + 'px'
    }

    SLOT_widthChanged(oldValue, newValue) {
        if (this.__contentDOM && newValue > 0) this.__contentDOM.style.width = newValue + 'px'
    }

    SLOT_heightChanged(oldValue, newValue) {
        if (this.__contentDOM && newValue > 0) this.__contentDOM.style.height = newValue + 'px'
    }

    SLOT_modalChanged(oldValue, newValue) { this.__updatePopupOverlay() }
    SLOT_dimChanged(oldValue, newValue)   { this.__updatePopupOverlay() }

    SLOT_paddingChanged(oldValue, newValue) {
        if (this.__contentDOM) this.__contentDOM.style.padding = newValue + 'px'
    }

    SLOT_opacityChanged(oldValue, newValue) {
        if (this.__contentDOM) this.__contentDOM.style.opacity = newValue
    }

    SLOT_zChanged(oldValue, newValue) {
        if (this.__overlayDOM) this.__overlayDOM.style.zIndex = String(10000 + (newValue || 0))
    }

    // ── Internal helpers ──────────────────────────────────────────────────────

    __updatePopupOverlay() {
        let dimEl = this.__dimDOM
        if (!dimEl) return
        if (!this.visible) {
            dimEl.style.display       = 'none'
            dimEl.style.pointerEvents = 'none'
            return
        }
        let modal = this.modal
        let dim   = this.dim
        if (modal || dim) {
            dimEl.style.display         = 'block'
            dimEl.style.backgroundColor = dim ? 'rgba(0,0,0,0.5)' : 'transparent'
            dimEl.style.pointerEvents   = modal ? 'auto' : 'none'
        } else {
            dimEl.style.display       = 'none'
            dimEl.style.pointerEvents = 'none'
        }
    }

    __applyPopupGeometry() {
        let box = this.__contentDOM
        if (!box) return
        box.style.left = (this.x || 0) + 'px'
        box.style.top  = (this.y || 0) + 'px'
        if (this.width  > 0) box.style.width  = this.width  + 'px'
        if (this.height > 0) box.style.height = this.height + 'px'
    }

    __clampToViewport() {
        let box = this.__contentDOM
        if (!box) return
        let rect = box.getBoundingClientRect()
        let vw   = window.innerWidth
        let vh   = window.innerHeight
        let x    = parseFloat(box.style.left) || 0
        let y    = parseFloat(box.style.top)  || 0
        if (x + rect.width  > vw) x = Math.max(0, vw - rect.width)
        if (y + rect.height > vh) y = Math.max(0, vh - rect.height)
        if (x < 0) x = 0
        if (y < 0) y = 0
        box.style.left = x + 'px'
        box.style.top  = y + 'px'
    }

    // ── Public API ────────────────────────────────────────────────────────────

    open()  { this.visible = true  }
    close() { this.visible = false }

    forceActiveFocus() {
        if (this.__contentDOM) this.__contentDOM.focus()
    }

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    __destroy() {
        if (this.__popupMouseHandler)
            document.removeEventListener('mousedown', this.__popupMouseHandler, true)
        if (this.__popupKeyHandler)
            document.removeEventListener('keydown', this.__popupKeyHandler)
        if (this.__overlayDOM && this.__overlayDOM.parentNode)
            this.__overlayDOM.parentNode.removeChild(this.__overlayDOM)
        super.__destroy()
    }
}



module.exports = Popup
