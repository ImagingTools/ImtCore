const { Item } = require('./Item')
const { QString, QBool, QReal, QVar, QColor, QIcon, QFont } = require('../utils/properties')

class MenuItem extends Item {
    static defaultProperties = {
        text: { type: QString, value: '', changed: '$textChanged' },
        font: { type: QFont, changed: '$fontChanged' },
        icon: { type: QIcon, changed: '$iconChanged' },
        action: { type: QVar, changed: '$actionChanged' },
        enabled: { type: QBool, value: true, changed: '$enabledChanged' },
        highlighted: { type: QBool, value: false },
        checkable: { type: QBool, value: false, changed: '$checkableChanged' },
        checked: { type: QBool, value: false, changed: '$checkedChanged' },
        autoExclusive: { type: QBool, value: false },
        down: { type: QBool, value: false },
        subMenu: { type: QVar },
        menu: { type: QVar },
        contentItem: { type: QVar },
        indicator: { type: QVar },
        arrow: { type: QVar },
        background: { type: QVar, changed: '$backgroundChanged' },
        spacing: { type: QReal, value: 6 },
        display: { type: QVar, value: 0 },
        mirrored: { type: QBool, value: false },
        textPadding: { type: QReal, value: 0 },
        implicitTextPadding: { type: QReal, value: 0 },
    }

    static defaultSignals = {
        triggered: { params: [] },
        toggled: { params: [] },
    }

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)

        // Qt Basic: background implicitWidth=200 implicitHeight=40, padding=6
        this.setStyle({
            display: 'flex',
            flexDirection: 'row',
            alignItems: 'center',
            padding: '4px 12px',
            cursor: 'pointer',
            userSelect: 'none',
            whiteSpace: 'nowrap',
            color: '#000000',
            backgroundColor: 'transparent',
            minWidth: '200px',
            minHeight: '30px',
            boxSizing: 'border-box',
            position: 'relative',
            transition: 'background-color 0.08s ease',
            pointerEvents: 'auto',
        })

        // Checkmark indicator (visible when checkable && checked)
        this.$checkIndicator = document.createElement('span')
        this.$checkIndicator.style.cssText = 'width:16px;flex-shrink:0;text-align:center;font-size:14px;margin-right:4px;visibility:hidden;'
        this.$checkIndicator.textContent = '\u2713' // ✓
        this.getDom().appendChild(this.$checkIndicator)

        // Icon element (hidden by default, shown when icon.source is set)
        this.$iconImg = document.createElement('img')
        this.$iconImg.style.width = '16px'
        this.$iconImg.style.height = '16px'
        this.$iconImg.style.flexShrink = '0'
        this.$iconImg.style.display = 'none'
        this.$iconImg.style.objectFit = 'contain'
        this.getDom().appendChild(this.$iconImg)

        // Spacer between icon and text (Qt spacing: 6)
        this.$iconSpacer = document.createElement('span')
        this.$iconSpacer.style.width = '6px'
        this.$iconSpacer.style.flexShrink = '0'
        this.$iconSpacer.style.display = 'none'
        this.getDom().appendChild(this.$iconSpacer)

        // Text node
        this.$textSpan = document.createElement('span')
        this.$textSpan.style.flex = '1'
        this.$textSpan.style.textAlign = 'left'
        this.$textSpan.textContent = this.getPropertyValue('text')
        this.getDom().appendChild(this.$textSpan)

        this.getDom().addEventListener('mouseenter', () => {
            if (this.getPropertyValue('enabled')){
                this.setStyle({ backgroundColor: '#e8e8e8' })
                this.getProperty('highlighted').reset(true)
            }
        })
        this.getDom().addEventListener('mouseleave', () => {
            this.setStyle({ backgroundColor: 'transparent' })
            this.getProperty('highlighted').reset(false)
        })
        this.getDom().addEventListener('mousedown', () => {
            if (this.getPropertyValue('enabled')){
                this.setStyle({ backgroundColor: '#d0d0d0' })
                this.getProperty('down').reset(true)
            }
        })
        this.getDom().addEventListener('mouseup', () => {
            this.getProperty('down').reset(false)
            if (this.getPropertyValue('highlighted')){
                this.setStyle({ backgroundColor: '#e8e8e8' })
            }
        })
        this.getDom().addEventListener('click', (e) => {
            e.stopPropagation()
            if (!this.getPropertyValue('enabled')) return

            if (this.getPropertyValue('checkable')){
                let checkedProp = this.getProperty('checked')
                // Only toggle manually if there's no binding (compute).
                // When a binding exists (e.g. checked: id === currentTenantId),
                // the onTriggered handler changes the source data and the binding
                // re-evaluates automatically — manual toggle would break the binding.
                if (!checkedProp.compute){
                    checkedProp.reset(!checkedProp.get())
                }
                if (this.$signals.toggled) this.$signals.toggled()
            }

            if (this.$signals.triggered) this.$signals.triggered()

            let actionObj = this.getPropertyValue('action')
            if (actionObj && actionObj.trigger) actionObj.trigger()

            // Close the entire menu hierarchy (dismiss walks up parent chain)
            let menuObj = this.getPropertyValue('menu')
            if (menuObj && menuObj.dismiss) menuObj.dismiss()
            else if (menuObj && menuObj.close) menuObj.close()
        })
    }

    $complete(){
        super.$complete()
        // Force initial evaluation of checked binding (compute may be pending)
        this.getProperty('checked').updateOnce()
        this.$updateCheckIndicator()
    }

    $textChanged(){
        if (this.$textSpan){
            this.$textSpan.textContent = this.getPropertyValue('text')
        }
    }

    $fontChanged(){
        if (!this.$textSpan) return
        let font = this.getProperty('font')
        this.$textSpan.style.fontWeight = font.getPropertyValue('bold') ? 'bold' : 'normal'
        this.$textSpan.style.fontSize = font.getPropertyValue('pixelSize') + 'px'
        this.$textSpan.style.fontFamily = `'${font.getPropertyValue('family')}'`
        this.$textSpan.style.fontStyle = font.getPropertyValue('italic') ? 'italic' : 'normal'
        this.$textSpan.style.textDecoration = font.getPropertyValue('underline') ? 'underline' : 'none'
    }

    $enabledChanged(){
        if (this.getPropertyValue('enabled')){
            this.setStyle({ opacity: '1', cursor: 'pointer' })
        } else {
            this.setStyle({ opacity: '0.4', cursor: 'default' })
        }
    }

    $updateCheckIndicator(){
        if (!this.$checkIndicator) return
        let checkable = this.getPropertyValue('checkable')
        let checked = this.getPropertyValue('checked')
        this.$checkIndicator.style.visibility = (checkable && checked) ? 'visible' : 'hidden'
    }

    $checkableChanged(){ this.$updateCheckIndicator() }
    $checkedChanged(){ this.$updateCheckIndicator() }

    $iconChanged(){
        let icon = this.getProperty('icon')
        let src = icon.getPropertyValue('source')
        if (src){
            let w = icon.getPropertyValue('width')
            let h = icon.getPropertyValue('height')
            this.$iconImg.style.width = (w > 0 ? w : 16) + 'px'
            this.$iconImg.style.height = (h > 0 ? h : 16) + 'px'
            this.$iconImg.style.display = 'block'
            this.$iconSpacer.style.display = 'block'

            // Resolve URL same as Image.js: rootPath + source, strip qrc:/
            let url = src.startsWith('data:image') ? src : (rootPath+'/'+src.replaceAll('../','')).replaceAll('qrc:','').replaceAll('//','/')
            this.$iconUrl = url
            ImageController.load(url, (img)=>{
                if(!this.UID || this.$iconUrl !== url) return
                this.$iconImg.src = img.data
            }, ()=>{
                if(!this.UID) return
                this.$iconImg.style.display = 'none'
                this.$iconSpacer.style.display = 'none'
            })
        } else {
            this.$iconImg.style.display = 'none'
            this.$iconSpacer.style.display = 'none'
        }
    }

    $actionChanged(){
        let actionObj = this.getPropertyValue('action')
        if (actionObj){
            if (actionObj.getPropertyValue){
                let text = actionObj.getPropertyValue('text')
                if (text) this.getProperty('text').reset(text)
                let enabled = actionObj.getPropertyValue('enabled')
                this.getProperty('enabled').reset(enabled)
                let checkable = actionObj.getPropertyValue('checkable')
                this.getProperty('checkable').reset(checkable)
                let checked = actionObj.getPropertyValue('checked')
                this.getProperty('checked').reset(checked)
            }
        }
    }

    $backgroundChanged(){
        let bg = this.getPropertyValue('background')
        if (bg && bg.$dom){
            bg.$dom.style.position = 'absolute'
            bg.$dom.style.left = '0'
            bg.$dom.style.top = '0'
            bg.$dom.style.width = '100%'
            bg.$dom.style.height = '100%'
            bg.$dom.style.zIndex = '-1'
            if (this.getDom().firstChild !== bg.$dom){
                this.getDom().insertBefore(bg.$dom, this.getDom().firstChild)
            }
        }
    }

    destroy(){
        delete this.$textSpan
        delete this.$iconImg
        delete this.$iconSpacer
        super.destroy()
    }
}

module.exports.MenuItem = MenuItem
