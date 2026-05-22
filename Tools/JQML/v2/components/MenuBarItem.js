const { Item } = require('./Item')
const { QString, QBool, QReal, QVar } = require('../utils/properties')

class MenuBarItem extends Item {
    static defaultProperties = {
        text: { type: QString, value: '', changed: '$textChanged' },
        menu: { type: QVar, changed: '$menuChanged' },
        highlighted: { type: QBool, value: false, changed: '$highlightedChanged' },
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
            if (!this.getPropertyValue('enabled')) return
            this.setStyle({ backgroundColor: '#e0e0e0' })
            this.getProperty('highlighted').reset(true)

            // Hover-to-switch: if any sibling menu in the MenuBar is open, close it and open ours
            let menuObj = this.getPropertyValue('menu')
            if (menuObj && this.$menuBarHasOpenMenu()){
                this.$closeAllSiblingMenus()
                let rect = this.getDom().getBoundingClientRect()
                menuObj.popup(rect.left, rect.bottom)
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
                    this.$closeAllSiblingMenus()
                    menuObj.popup(rect.left, rect.bottom)
                }
            }

            if (this.$signals.triggered) this.$signals.triggered()
        })
    }

    // Check if any sibling MenuBarItem's menu is currently open
    $menuBarHasOpenMenu(){
        let parent = this.getProperty('parent').get()
        if (!parent || !parent.getProperty) return false
        let siblings = parent.getProperty('children').get()
        for (let sibling of siblings){
            if (sibling === this) continue
            if (sibling.getPropertyValue && sibling.getPropertyValue('menu')){
                let m = sibling.getPropertyValue('menu')
                if (m && m.getPropertyValue && m.getPropertyValue('visible')) return true
            }
        }
        return false
    }

    // Close all sibling menus
    $closeAllSiblingMenus(){
        let parent = this.getProperty('parent').get()
        if (!parent || !parent.getProperty) return
        let siblings = parent.getProperty('children').get()
        for (let sibling of siblings){
            if (sibling === this) continue
            if (sibling.getPropertyValue && sibling.getPropertyValue('menu')){
                let m = sibling.getPropertyValue('menu')
                if (m && m.getPropertyValue && m.getPropertyValue('visible')){
                    m.close()
                }
            }
        }
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

    $highlightedChanged(){
        let hl = this.getPropertyValue('highlighted')
        if (hl){
            this.setStyle({ backgroundColor: '#e0e0e0' })
        } else {
            this.setStyle({ backgroundColor: 'transparent' })
        }
    }

    destroy(){
        delete this.$textNode
        super.destroy()
    }
}

module.exports.MenuBarItem = MenuBarItem
