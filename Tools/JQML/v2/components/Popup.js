const { Item } = require('./Item')
const { QString, QBool, QReal, QVar, QInt, QColor } = require('../utils/properties')

/**
 * Popup — Qt Quick Controls Popup implementation for JQML v2
 * https://doc.qt.io/qt-6/qml-qtquick-controls-popup.html
 *
 * A popup window that can be used to display content above the application UI.
 * It supports modal/modeless operation, close policies, margins/padding/insets,
 * enter/exit transitions, overlay dimming, and anchors.centerIn.
 */

// ClosePolicy flags (combinable via bitwise OR)
const ClosePolicy = {
    NoAutoClose: 0,
    CloseOnPressOutside: 1,
    CloseOnPressOutsideParent: 2,
    CloseOnReleaseOutside: 4,
    CloseOnReleaseOutsideParent: 8,
    CloseOnEscape: 16,
}

class Popup extends Item {
    // Expose ClosePolicy as static enum on the class
    static NoAutoClose = ClosePolicy.NoAutoClose
    static CloseOnPressOutside = ClosePolicy.CloseOnPressOutside
    static CloseOnPressOutsideParent = ClosePolicy.CloseOnPressOutsideParent
    static CloseOnReleaseOutside = ClosePolicy.CloseOnReleaseOutside
    static CloseOnReleaseOutsideParent = ClosePolicy.CloseOnReleaseOutsideParent
    static CloseOnEscape = ClosePolicy.CloseOnEscape

    static defaultProperties = {
        // Visibility / state
        opened: { type: QBool, value: false },
        visible: { type: QBool, value: false, changed: '$visibleChanged' },
        modal: { type: QBool, value: false, changed: '$modalChanged' },
        dim: { type: QBool, value: false, changed: '$dimChanged' },

        // Close policy (default: CloseOnEscape | CloseOnPressOutside)
        closePolicy: { type: QInt, value: ClosePolicy.CloseOnEscape | ClosePolicy.CloseOnPressOutside, changed: '$closePolicyChanged' },

        // Geometry
        x: { type: QReal, value: 0, changed: '$geometryChanged' },
        y: { type: QReal, value: 0, changed: '$geometryChanged' },
        z: { type: QInt, value: 0, changed: '$zChanged' },
        width: { type: QReal, value: 0, changed: '$geometryChanged' },
        height: { type: QReal, value: 0, changed: '$geometryChanged' },

        // Padding
        padding: { type: QReal, value: 0, changed: '$paddingChanged' },
        topPadding: { type: QReal, value: -1, changed: '$paddingChanged' },
        bottomPadding: { type: QReal, value: -1, changed: '$paddingChanged' },
        leftPadding: { type: QReal, value: -1, changed: '$paddingChanged' },
        rightPadding: { type: QReal, value: -1, changed: '$paddingChanged' },
        horizontalPadding: { type: QReal, value: -1, changed: '$paddingChanged' },
        verticalPadding: { type: QReal, value: -1, changed: '$paddingChanged' },

        // Margins
        margins: { type: QReal, value: -1, changed: '$marginsChanged' },
        topMargin: { type: QReal, value: -1, changed: '$marginsChanged' },
        bottomMargin: { type: QReal, value: -1, changed: '$marginsChanged' },
        leftMargin: { type: QReal, value: -1, changed: '$marginsChanged' },
        rightMargin: { type: QReal, value: -1, changed: '$marginsChanged' },

        // Insets
        topInset: { type: QReal, value: 0 },
        bottomInset: { type: QReal, value: 0 },
        leftInset: { type: QReal, value: 0 },
        rightInset: { type: QReal, value: 0 },

        // Content
        contentItem: { type: QVar },
        contentWidth: { type: QReal, value: 0 },
        contentHeight: { type: QReal, value: 0 },
        contentChildren: { type: QVar },
        contentData: { type: QVar },
        background: { type: QVar, changed: '$backgroundChanged' },

        // Implicit sizes
        implicitContentWidth: { type: QReal, value: 0 },
        implicitContentHeight: { type: QReal, value: 0 },
        implicitBackgroundWidth: { type: QReal, value: 0 },
        implicitBackgroundHeight: { type: QReal, value: 0 },

        // Appearance
        opacity: { type: QReal, value: 1, changed: '$opacityChanged' },
        scale: { type: QReal, value: 1, changed: '$scaleChanged' },
        clip: { type: QBool, value: false, changed: '$clipChanged' },
        enabled: { type: QBool, value: true },
        focus: { type: QBool, value: false },
        spacing: { type: QReal, value: 0 },

        // Transitions
        enter: { type: QVar },
        exit: { type: QVar },

        // Overlay
        'Overlay.modal': { type: QVar },
        'Overlay.modeless': { type: QVar },

        // Anchors helper for centerIn
        anchors: { type: QVar },

        // Font/Locale/Palette
        font: { type: QVar },
        locale: { type: QVar },
        palette: { type: QVar },

        // Mirroring
        mirrored: { type: QBool, value: false },

        // Transform origin
        transformOrigin: { type: QInt, value: Item.Center },
    }

    static defaultSignals = {
        aboutToShow: { params: [] },
        aboutToHide: { params: [] },
        closed: { params: [] },
    }

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)

        // Popup root container (fixed overlay)
        this.setStyle({
            position: 'fixed',
            display: 'none',
            left: '0',
            top: '0',
            right: '0',
            bottom: '0',
            zIndex: '10000',
            pointerEvents: 'none',
        })

        // Overlay/dimming element
        this.$overlay = document.createElement('div')
        this.$overlay.style.cssText = 'position:absolute;left:0;top:0;right:0;bottom:0;pointer-events:auto;'
        this.getDom().appendChild(this.$overlay)

        // Content container (the actual popup box)
        this.$contentBox = document.createElement('div')
        this.$contentBox.style.cssText = 'position:absolute;display:flex;flex-direction:column;pointer-events:auto;background:#ffffff;border:1px solid #cccccc;border-radius:4px;box-shadow:0 2px 8px rgba(0,0,0,0.15);'
        this.getDom().appendChild(this.$contentBox)

        // Event handlers
        this.$onOverlayClick = (e) => {
            let policy = this.getPropertyValue('closePolicy')
            if (policy & ClosePolicy.CloseOnPressOutside){
                this.close()
            }
        }

        this.$onOverlayRelease = (e) => {
            let policy = this.getPropertyValue('closePolicy')
            if (policy & ClosePolicy.CloseOnReleaseOutside){
                this.close()
            }
        }

        this.$onEscapeKey = (e) => {
            if (e.key === 'Escape' || e.keyCode === 27){
                let policy = this.getPropertyValue('closePolicy')
                if (policy & ClosePolicy.CloseOnEscape){
                    this.close()
                }
            }
        }

        this.$overlay.addEventListener('mousedown', this.$onOverlayClick)
        this.$overlay.addEventListener('mouseup', this.$onOverlayRelease)

        // Apply initial dim/modal state
        this.$updateOverlay()
    }

    /**
     * Reparent child DOM elements into the content box instead of the root overlay
     */
    addChild(child){
        super.addChild(child)
        if (child.getDom && child.getDom()){
            this.$contentBox.appendChild(child.getDom())
        }
    }

    // --- Public methods ---

    open(){
        if (this.getPropertyValue('opened')) return
        if (this.$signals.aboutToShow) this.$signals.aboutToShow()
        this.getProperty('visible').reset(true)
        this.getProperty('opened').reset(true)
    }

    close(){
        if (!this.getPropertyValue('opened')) return
        if (this.$signals.aboutToHide) this.$signals.aboutToHide()
        this.getProperty('visible').reset(false)
        this.getProperty('opened').reset(false)
    }

    forceActiveFocus(){
        this.$contentBox.focus()
    }

    // --- Property change handlers ---

    $visibleChanged(){
        if (this.getPropertyValue('visible')){
            this.setStyle({ display: 'block' })
            this.$contentBox.style.display = 'flex'
            document.addEventListener('keydown', this.$onEscapeKey)
            this.$applyGeometry()
            this.$updateOverlay()
            if (this.$signals.opened) this.$signals.opened()
        } else {
            this.setStyle({ display: 'none' })
            document.removeEventListener('keydown', this.$onEscapeKey)
            this.$updateOverlay()
            if (this.$signals.closed) this.$signals.closed()
        }
    }

    $modalChanged(){
        this.$updateOverlay()
    }

    $dimChanged(){
        this.$updateOverlay()
    }

    $closePolicyChanged(){
        // no-op: policy is checked at event time
    }

    $geometryChanged(){
        this.$applyGeometry()
    }

    $paddingChanged(){
        this.$applyPadding()
    }

    $marginsChanged(){
        this.$applyGeometry()
    }

    $opacityChanged(){
        this.$contentBox.style.opacity = String(this.getPropertyValue('opacity'))
    }

    $scaleChanged(){
        this.$contentBox.style.transform = `scale(${this.getPropertyValue('scale')})`
    }

    $clipChanged(){
        this.$contentBox.style.overflow = this.getPropertyValue('clip') ? 'hidden' : 'visible'
    }

    $backgroundChanged(){
        // Background is a QML Item; if it's a color string, apply directly
        let bg = this.getPropertyValue('background')
        if (typeof bg === 'string'){
            this.$contentBox.style.background = bg
        }
    }

    $zChanged(){
        this.setStyle({ zIndex: String(10000 + this.getPropertyValue('z')) })
    }

    // --- Internal helpers ---

    $updateOverlay(){
        let visible = this.getPropertyValue('visible')
        let modal = this.getPropertyValue('modal')
        let dim = this.getPropertyValue('dim')

        if (!visible){
            this.$overlay.style.display = 'none'
            return
        }

        // modal popup blocks clicks on overlay; modeless allows pass-through
        if (modal){
            this.$overlay.style.display = 'block'
            this.$overlay.style.pointerEvents = 'auto'
            if (dim){
                this.$overlay.style.backgroundColor = 'rgba(0,0,0,0.4)'
            } else {
                this.$overlay.style.backgroundColor = 'transparent'
            }
        } else {
            // non-modal: overlay still catches clicks for CloseOnPressOutside
            let policy = this.getPropertyValue('closePolicy')
            if (policy & (ClosePolicy.CloseOnPressOutside | ClosePolicy.CloseOnReleaseOutside)){
                this.$overlay.style.display = 'block'
                this.$overlay.style.pointerEvents = 'auto'
            } else {
                this.$overlay.style.display = 'none'
                this.$overlay.style.pointerEvents = 'none'
            }
            if (dim){
                this.$overlay.style.backgroundColor = 'rgba(0,0,0,0.2)'
            } else {
                this.$overlay.style.backgroundColor = 'transparent'
            }
        }
    }

    $applyGeometry(){
        let x = this.getPropertyValue('x')
        let y = this.getPropertyValue('y')
        let w = this.getPropertyValue('width')
        let h = this.getPropertyValue('height')

        // Check if anchors.centerIn is set (parent)
        let anchorsVal = this.getPropertyValue('anchors')
        let centerIn = false
        if (anchorsVal && typeof anchorsVal === 'object'){
            if (anchorsVal.centerIn) centerIn = true
        }

        if (centerIn){
            this.$contentBox.style.left = '50%'
            this.$contentBox.style.top = '50%'
            this.$contentBox.style.transform = 'translate(-50%, -50%)'
        } else {
            this.$contentBox.style.left = (x != null && x !== '') ? `${x}px` : ''
            this.$contentBox.style.top = (y != null && y !== '') ? `${y}px` : ''
            this.$contentBox.style.transform = ''
        }

        if (w != null && w > 0) this.$contentBox.style.width = `${w}px`
        if (h != null && h > 0) this.$contentBox.style.height = `${h}px`
    }

    $applyPadding(){
        let pad = this.getPropertyValue('padding')
        let hPad = this.getPropertyValue('horizontalPadding')
        let vPad = this.getPropertyValue('verticalPadding')
        let tPad = this.getPropertyValue('topPadding')
        let bPad = this.getPropertyValue('bottomPadding')
        let lPad = this.getPropertyValue('leftPadding')
        let rPad = this.getPropertyValue('rightPadding')

        let top = tPad >= 0 ? tPad : (vPad >= 0 ? vPad : pad)
        let bottom = bPad >= 0 ? bPad : (vPad >= 0 ? vPad : pad)
        let left = lPad >= 0 ? lPad : (hPad >= 0 ? hPad : pad)
        let right = rPad >= 0 ? rPad : (hPad >= 0 ? hPad : pad)

        this.$contentBox.style.padding = `${top}px ${right}px ${bottom}px ${left}px`
    }

    destroy(){
        document.removeEventListener('keydown', this.$onEscapeKey)
        this.$overlay.removeEventListener('mousedown', this.$onOverlayClick)
        this.$overlay.removeEventListener('mouseup', this.$onOverlayRelease)
        delete this.$onOverlayClick
        delete this.$onOverlayRelease
        delete this.$onEscapeKey
        delete this.$overlay
        delete this.$contentBox
        super.destroy()
    }
}

module.exports.Popup = Popup
