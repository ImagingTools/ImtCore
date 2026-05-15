const { Item } = require('./Item')
const { QReal, QVar, QBool } = require('../utils/properties')

class MenuBar extends Item {
    static defaultProperties = {
        delegate: { type: QVar },
        contentItem: { type: QVar },
        menus: { type: QVar },
    }

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)
        this.setStyle({
            display: 'flex',
            flexDirection: 'row',
            alignItems: 'center',
            backgroundColor: '#f0f0f0',
            borderBottom: '1px solid #cccccc',
            minHeight: '30px',
            boxSizing: 'border-box',
        })
    }

    addChild(child){
        super.addChild(child)
    }

    addDomChild(child){
        super.addDomChild(child)
        child.setStyle({
            position: 'relative',
        })
    }

    updateGeometry(){
        if (this.preventAutoUpdateGeometry || !this.getPropertyValue('visible')) return

        let children = this.getProperty('children').get()
        let w = 0
        let h = 0
        for (let i = 0; i < children.length; i++){
            if (children[i].getPropertyValue('visible')){
                if (children[i] instanceof Repeater) continue
                h = Math.max(h, children[i].getPropertyValue('height'))
                if (children[i].getPropertyValue('width') > 0 && children[i].getPropertyValue('height') > 0){
                    w += children[i].getPropertyValue('width')
                }
            }
        }

        this.getProperty('width').setAuto(w)
        this.getProperty('height').setAuto(h)
    }
}

module.exports.MenuBar = MenuBar
