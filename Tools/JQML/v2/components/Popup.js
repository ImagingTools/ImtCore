const { Item } = require('./Item')
const { QString, QBool, QReal, QVar, QInt, QColor, QVisible, QFont } = require('../utils/properties')

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
        visible: { type: QVisible, value: false, changed: '$visibleChanged' },
        modal: { type: QBool, value: false, changed: '$modalChanged' },
        dim: { type: QBool, value: false, changed: '$dimChangedExplicit' },

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
        enabled: { type: QBool, value: true, changed: '$enabledChanged' },
        focus: { type: QBool, value: false },
        spacing: { type: QReal, value: 0, changed: '$spacingChanged' },

        // Transitions
        enter: { type: QVar },
        exit: { type: QVar },

        // Overlay
        'Overlay.modal': { type: QVar },
        'Overlay.modeless': { type: QVar },

        // Anchors helper for centerIn
        anchors: { type: QVar },

        // Font/Locale/Palette
        font: { type: QFont, changed: '$fontChanged' },
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

        // Move popup DOM out of parent tree into document.body
        // so it is never clipped by ancestor overflow or broken by ancestor transforms
        if (this.$dom && this.$dom.parentNode) {
            this.$dom.parentNode.removeChild(this.$dom)
        }
        document.body.appendChild(this.$dom)

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

        // Overlay/dimming element (visual only — pointer events handled at document level)
        this.$overlay = document.createElement('div')
        this.$overlay.style.cssText = 'position:absolute;left:0;top:0;right:0;bottom:0;pointer-events:none;'
        this.getDom().appendChild(this.$overlay)

        // Content container (the actual popup box)
        this.$contentBox = document.createElement('div')
        this.$contentBox.style.cssText = 'position:absolute;display:flex;flex-direction:column;pointer-events:auto;background:#ffffff;border:1px solid #e0e0e0;border-radius:8px;box-shadow:0 4px 12px rgba(0,0,0,0.12);'
        this.getDom().appendChild(this.$contentBox)

        // Event handlers — outside click detection via document listeners
        this.$onDocumentMouseDown = (e) => {
            if (!this.getPropertyValue('visible')) return
            if (this.$contentBox.contains(e.target)) return
            let policy = this.getPropertyValue('closePolicy')
            if (policy & (ClosePolicy.CloseOnPressOutside | ClosePolicy.CloseOnPressOutsideParent)){
                this.close()
            }
        }

        this.$onDocumentMouseUp = (e) => {
            if (!this.getPropertyValue('visible')) return
            if (this.$contentBox.contains(e.target)) return
            let policy = this.getPropertyValue('closePolicy')
            if (policy & (ClosePolicy.CloseOnReleaseOutside | ClosePolicy.CloseOnReleaseOutsideParent)){
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

        // Modal: block scroll/wheel behind overlay
        this.$onWheelBlock = (e) => { e.preventDefault(); e.stopPropagation() }

        this.$onResize = () => {
            if (this.getPropertyValue('visible')) this.$applyGeometry()
        }

        // Apply initial dim/modal state
        this.$updateOverlay()
    }

    /**
     * Reparent child DOM elements into the content box instead of the root overlay.
     * addChild is called before child DOM exists, so we use addDomChild hook instead.
     */
    addChild(child){
        super.addChild(child)
    }

    addDomChild(child){
        if (child.$dom && this.$contentBox){
            this.$contentBox.appendChild(child.$dom)
        }
    }

    // --- Public methods ---

    open(){
        if (this.getPropertyValue('opened')) return
        this.getSignal('aboutToShow')()
        this.getProperty('visible').reset(true)
        this.getProperty('opened').reset(true)
    }

    close(){
        if (!this.getPropertyValue('opened')) return
        this.getSignal('aboutToHide')()
        this.getProperty('visible').reset(false)
        this.getProperty('opened').reset(false)
    }

    forceActiveFocus(){
        this.$contentBox.focus()
    }

    // --- Property change handlers ---

    $visibleChanged(){
        let currentValue = this.getPropertyValue('visible')
        if (currentValue){
            this.setStyle({ display: 'block' })
            this.$contentBox.style.display = 'flex'
            document.addEventListener('keydown', this.$onEscapeKey)
            document.addEventListener('mousedown', this.$onDocumentMouseDown, true)
            document.addEventListener('mouseup', this.$onDocumentMouseUp, true)
            window.addEventListener('resize', this.$onResize)
            this.$applyGeometry()
            this.$updateOverlay()
        } else {
            this.setStyle({ display: 'none' })
            document.removeEventListener('keydown', this.$onEscapeKey)
            document.removeEventListener('mousedown', this.$onDocumentMouseDown, true)
            document.removeEventListener('mouseup', this.$onDocumentMouseUp, true)
            window.removeEventListener('resize', this.$onResize)
            this.$updateOverlay()
            this.getSignal('closed')()
        }

        // Propagate visibility to children (QVisible.set2) — 
        // Item.$visibleChanged does this, but Popup overrides $visibleChanged
        // so children's value2 would never be updated without this.
        let children = this.getProperty('children').get()
        for(let child of children){
            if(child.UID) child.getProperty('visible').set2(currentValue)
        }
    }

    $modalChanged(){
        // In Qt, dim follows modal unless explicitly set
        if (!this.$dimExplicit) {
            this.$dimAutoSetting = true
            this.getProperty('dim').reset(this.getPropertyValue('modal'))
            this.$dimAutoSetting = false
        }
        this.$updateOverlay()
    }

    $dimChangedExplicit(){
        // Mark dim as explicitly set by user so $modalChanged won't override it
        if (!this.$dimAutoSetting) {
            this.$dimExplicit = true
        }
        this.$updateOverlay()
    }

    $closePolicyChanged(){
        // Update overlay visibility when policy changes at runtime
        this.$updateOverlay()
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
        this.$applyTransform()
    }

    $clipChanged(){
        this.$contentBox.style.overflow = this.getPropertyValue('clip') ? 'hidden' : 'visible'
    }

    $enabledChanged(){
        this.$contentBox.style.pointerEvents = this.getPropertyValue('enabled') ? 'auto' : 'none'
    }

    $spacingChanged(){
        this.$contentBox.style.gap = `${this.getPropertyValue('spacing')}px`
    }

    $fontChanged(){
        if (!this.$contentBox) return
        let font = this.getProperty('font')
        this.$contentBox.style.fontWeight = font.getPropertyValue('bold') ? 'bold' : 'normal'
        this.$contentBox.style.fontSize = font.getPropertyValue('pixelSize') + 'px'
        this.$contentBox.style.fontFamily = `'${font.getPropertyValue('family')}'`
        this.$contentBox.style.fontStyle = font.getPropertyValue('italic') ? 'italic' : 'normal'
        this.$contentBox.style.textDecoration = font.getPropertyValue('underline') ? 'underline' : 'none'
    }

    $backgroundChanged(){
        let bg = this.getPropertyValue('background')
        if (typeof bg === 'string'){
            this.$contentBox.style.background = bg
        } else if (bg && bg.$dom) {
            // QML Item used as background (e.g. background: Rectangle { ... })
            // Clear default contentBox visual styling — the background Item provides its own
            this.$contentBox.style.background = 'transparent'
            this.$contentBox.style.border = 'none'
            this.$contentBox.style.borderRadius = '0'
            this.$contentBox.style.boxShadow = 'none'

            // Ensure background is first child (renders behind content)
            if (this.$contentBox.firstChild !== bg.$dom) {
                this.$contentBox.insertBefore(bg.$dom, this.$contentBox.firstChild)
            }

            // Bind background size to popup size
            this.$bgItem = bg
            bg.getProperty('x').reset(0)
            bg.getProperty('y').reset(0)
            bg.getProperty('width').freeze()
            bg.getProperty('width').subscribePrimary(this.getProperty('width'))
            bg.getProperty('width').setCompute(() => this.getPropertyValue('width'))
            bg.getProperty('height').freeze()
            bg.getProperty('height').subscribePrimary(this.getProperty('height'))
            bg.getProperty('height').setCompute(() => this.getPropertyValue('height'))
            bg.getProperty('width').update()
            bg.getProperty('height').update()
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
            this.$overlay.style.pointerEvents = 'none'
            this.$overlay.removeEventListener('wheel', this.$onWheelBlock)
            this.$overlay.removeEventListener('touchmove', this.$onWheelBlock)
            return
        }

        if (modal){
            // Modal: overlay blocks scroll/wheel and shows dim
            this.$overlay.style.display = 'block'
            this.$overlay.style.pointerEvents = 'auto'
            this.$overlay.addEventListener('wheel', this.$onWheelBlock, { passive: false })
            this.$overlay.addEventListener('touchmove', this.$onWheelBlock, { passive: false })
            this.$overlay.style.backgroundColor = dim ? 'rgba(0,0,0,0.25)' : 'transparent'
        } else {
            // Non-modal: overlay only provides visual dimming, no pointer interception
            this.$overlay.removeEventListener('wheel', this.$onWheelBlock)
            this.$overlay.removeEventListener('touchmove', this.$onWheelBlock)
            this.$overlay.style.pointerEvents = 'none'
            if (dim){
                this.$overlay.style.display = 'block'
                this.$overlay.style.backgroundColor = 'rgba(0,0,0,0.12)'
            } else {
                this.$overlay.style.display = 'none'
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
            this.$isCenterIn = true
        } else {
            this.$isCenterIn = false
            // Compute viewport-relative offset from QML parent's DOM position
            let offsetX = 0
            let offsetY = 0
            let qmlParent = this.getProperty('parent').get()
            if (qmlParent && qmlParent.$dom){
                let rect = qmlParent.$dom.getBoundingClientRect()
                offsetX = rect.left
                offsetY = rect.top
            }

            let finalX = offsetX + (x || 0)
            let finalY = offsetY + (y || 0)

            // Clamp to viewport using margins (Qt: margins prevent popup going off-screen)
            let mg = this.getPropertyValue('margins')
            let tM = this.getPropertyValue('topMargin')
            let bM = this.getPropertyValue('bottomMargin')
            let lM = this.getPropertyValue('leftMargin')
            let rM = this.getPropertyValue('rightMargin')
            let mTop = tM >= 0 ? tM : (mg >= 0 ? mg : -1)
            let mBottom = bM >= 0 ? bM : (mg >= 0 ? mg : -1)
            let mLeft = lM >= 0 ? lM : (mg >= 0 ? mg : -1)
            let mRight = rM >= 0 ? rM : (mg >= 0 ? mg : -1)

            if (mLeft >= 0 && finalX < mLeft) finalX = mLeft
            if (mTop >= 0 && finalY < mTop) finalY = mTop
            if (mRight >= 0 && w > 0 && finalX + w > window.innerWidth - mRight)
                finalX = Math.max(mLeft >= 0 ? mLeft : 0, window.innerWidth - mRight - w)
            if (mBottom >= 0 && h > 0 && finalY + h > window.innerHeight - mBottom)
                finalY = Math.max(mTop >= 0 ? mTop : 0, window.innerHeight - mBottom - h)

            this.$contentBox.style.left = `${finalX}px`
            this.$contentBox.style.top = `${finalY}px`
        }

        if (w != null && w > 0) this.$contentBox.style.width = `${w}px`
        if (h != null && h > 0) this.$contentBox.style.height = `${h}px`
        this.$applyTransform()
    }

    $applyTransform(){
        let parts = []
        if (this.$isCenterIn) parts.push('translate(-50%, -50%)')
        let s = this.getPropertyValue('scale')
        if (s !== 1) parts.push(`scale(${s})`)
        this.$contentBox.style.transform = parts.join(' ')
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
        window.removeEventListener('resize', this.$onResize)
        document.removeEventListener('mousedown', this.$onDocumentMouseDown, true)
        document.removeEventListener('mouseup', this.$onDocumentMouseUp, true)
        if(this.$overlay){
            this.$overlay.removeEventListener('wheel', this.$onWheelBlock)
            this.$overlay.removeEventListener('touchmove', this.$onWheelBlock)
        }
        // Remove from document.body since we reparented there in constructor
        if(this.$dom && this.$dom.parentNode === document.body){
            document.body.removeChild(this.$dom)
        }
        delete this.$onDocumentMouseDown
        delete this.$onDocumentMouseUp
        delete this.$onEscapeKey
        delete this.$overlay
        delete this.$contentBox
        super.destroy()
    }
}

module.exports.Popup = Popup
