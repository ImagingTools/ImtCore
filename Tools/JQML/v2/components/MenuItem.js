const { Item } = require('./Item')
const { QString, QBool, QReal, QVar, QColor } = require('../utils/properties')

class MenuItem extends Item {
    static defaultProperties = {
        text: { type: QString, value: '', changed: '$textChanged' },
        icon: { type: QVar },
        action: { type: QVar, changed: '$actionChanged' },
        enabled: { type: QBool, value: true, changed: '$enabledChanged' },
        highlighted: { type: QBool, value: false },
        checkable: { type: QBool, value: false },
        checked: { type: QBool, value: false },
        autoExclusive: { type: QBool, value: false },
        subMenu: { type: QVar },
        menu: { type: QVar },
        contentItem: { type: QVar },
    }

    static defaultSignals = {
        triggered: { params: [] },
        toggled: { params: [] },
    }

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)
        this.setStyle({
            display: 'flex',
            flexDirection: 'row',
            alignItems: 'center',
            padding: '6px 16px',
            cursor: 'pointer',
            userSelect: 'none',
            whiteSpace: 'nowrap',
            fontSize: '14px',
            fontFamily: 'sans-serif',
            color: '#1a1a1a',
            backgroundColor: 'transparent',
            minHeight: '28px',
            boxSizing: 'border-box',
            position: 'relative',
            transition: 'background-color 0.08s ease',
            pointerEvents: 'auto',
        })

        this.$textNode = document.createTextNode(this.getPropertyValue('text'))
        this.getDom().appendChild(this.$textNode)

        this.getDom().addEventListener('mouseenter', () => {
            if (this.getPropertyValue('enabled')){
                this.setStyle({ backgroundColor: '#e8e8e8', color: '#1a1a1a' })
                this.getProperty('highlighted').reset(true)
            }
        })
        this.getDom().addEventListener('mouseleave', () => {
            this.setStyle({ backgroundColor: 'transparent', color: '#1a1a1a' })
            this.getProperty('highlighted').reset(false)
        })
        this.getDom().addEventListener('click', (e) => {
            e.stopPropagation()
            if (!this.getPropertyValue('enabled')) return

            if (this.getPropertyValue('checkable')){
                let current = this.getPropertyValue('checked')
                this.getProperty('checked').reset(!current)
                if (this.$signals.toggled) this.$signals.toggled()
            }

            if (this.$signals.triggered) this.$signals.triggered()

            let actionObj = this.getPropertyValue('action')
            if (actionObj && actionObj.trigger) actionObj.trigger()

            let menuObj = this.getPropertyValue('menu')
            if (menuObj && menuObj.close) menuObj.close()
        })
    }

    $textChanged(){
        if (this.$textNode){
            this.$textNode.nodeValue = this.getPropertyValue('text')
        }
    }

    $enabledChanged(){
        if (this.getPropertyValue('enabled')){
            this.setStyle({ opacity: '1', cursor: 'pointer' })
        } else {
            this.setStyle({ opacity: '0.4', cursor: 'default' })
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

    destroy(){
        delete this.$textNode
        super.destroy()
    }
}

module.exports.MenuItem = MenuItem
