const { Item } = require('./Item')
const { QString, QBool, QReal, QVar } = require('../utils/properties')

class MenuBarItem extends Item {
    static defaultProperties = {
        text: { type: QString, value: '', changed: '$textChanged' },
        menu: { type: QVar, changed: '$menuChanged' },
        highlighted: { type: QBool, value: false },
        enabled: { type: QBool, value: true },
    }

    static defaultSignals = {
        triggered: { params: [] },
    }

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)
        this.setStyle({
            display: 'flex',
            alignItems: 'center',
            padding: '4px 12px',
            cursor: 'pointer',
            userSelect: 'none',
            whiteSpace: 'nowrap',
            fontSize: '14px',
            fontFamily: 'sans-serif',
            color: '#333333',
            backgroundColor: 'transparent',
            position: 'relative',
        })

        this.$textNode = document.createTextNode(this.getPropertyValue('text'))
        this.getDom().appendChild(this.$textNode)

        this.getDom().addEventListener('mouseenter', () => {
            if (this.getPropertyValue('enabled')){
                this.setStyle({ backgroundColor: '#e0e0e0' })
                this.getProperty('highlighted').reset(true)
            }
        })
        this.getDom().addEventListener('mouseleave', () => {
            this.setStyle({ backgroundColor: 'transparent' })
            this.getProperty('highlighted').reset(false)
        })
        this.getDom().addEventListener('click', (e) => {
            e.stopPropagation()
            if (!this.getPropertyValue('enabled')) return

            let menuObj = this.getPropertyValue('menu')
            if (menuObj){
                let rect = this.getDom().getBoundingClientRect()
                if (menuObj.getPropertyValue('visible')){
                    menuObj.close()
                } else {
                    menuObj.popup(rect.left, rect.bottom)
                }
            }

            if (this.$signals.triggered) this.$signals.triggered()
        })
    }

    $textChanged(){
        if (this.$textNode){
            this.$textNode.nodeValue = this.getPropertyValue('text')
        }
    }

    $menuChanged(){
        let menuObj = this.getPropertyValue('menu')
        if (menuObj && menuObj.getPropertyValue){
            let title = menuObj.getPropertyValue('title')
            if (title && !this.getPropertyValue('text')){
                this.getProperty('text').reset(title)
            }
        }
    }

    destroy(){
        delete this.$textNode
        super.destroy()
    }
}

module.exports.MenuBarItem = MenuBarItem
