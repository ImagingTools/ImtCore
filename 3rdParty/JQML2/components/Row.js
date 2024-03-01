const { Item } = require('./Item')
const { QColor, QBorder, QReal } = require('../utils/properties')

class Row extends Item {
    static defaultProperties = {
        spacing: { type: QReal, value: 0, changed: '$spacingChanged' },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
    }

    updateGeometry(){
        if(this.preventAutoUpdateGeometry) return

        let children = this.getProperty('children').get()
        let w = 0
        let h = 0
        for(let i = 0; i < children.length; i++){
            if(children[i].getPropertyValue('visible')){
                h = Math.max(h, children[i].getPropertyValue('height'))
                if(children[i].getPropertyValue('width') > 0 && children[i].getPropertyValue('height') > 0){
                    w += children[i].getPropertyValue('width')
                    if(i < children.length - 1 && children[i+1].getPropertyValue('width') > 0 && children[i+1].getPropertyValue('height') > 0){
                        w += this.getPropertyValue('spacing')
                        children[i].setStyle({
                            marginRight: `${this.getPropertyValue('spacing')}px`
                        })
                    } else {
                        children[i].setStyle({
                            marginRight: `0`
                        })
                    }
                }
            }
        }

        this.getProperty('width').setAuto(w)
        this.getProperty('height').setAuto(h)
    }

    addChild(child){
        super.addChild(child)
        
        child.getProperty('width').getNotify().connect(this, this.updateGeometry)
        child.getProperty('height').getNotify().connect(this, this.updateGeometry)
        child.getProperty('visible').getNotify().connect(this, this.updateGeometry)
    }
    addDomChild(child){
        super.addDomChild(child)

        child.setStyle({
            position: 'relative'
        })
    }

    $spacingChanged(){
        this.updateGeometry()
    }
   
}

module.exports.Row = Row