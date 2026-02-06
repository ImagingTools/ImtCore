const { Item } = require('./Item')
const { QColor, QBorder, QReal, QInt } = require('../utils/properties')

class Row extends Item {
    static LeftToRight = 0
    static RightToLeft = 1

    static defaultProperties = {
        spacing: { type: QReal, value: 0, changed: '$spacingChanged' },
        layoutDirection: { type: QInt, value: Row.LeftToRight, changed: '$layoutDirectionChanged' },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
    }

    updateGeometry(){
        if(this.preventAutoUpdateGeometry || !this.getPropertyValue('visible')) return

        let children = this.getProperty('children').get()
        let w = 0
        let h = 0
        for(let i = 0; i < children.length; i++){
            if(children[i].getPropertyValue('visible')){
                if(children[i] instanceof Repeater) continue
                h = Math.max(h, children[i].getPropertyValue('height'))
                if(children[i].getPropertyValue('width') > 0 && children[i].getPropertyValue('height') > 0){
                    w += children[i].getPropertyValue('width')
                    let find = false
                    for(let k = i + 1; k < children.length; k++){
                        if(children[k] instanceof Repeater) continue
                        if(children[k].getPropertyValue('width') > 0 && children[k].getPropertyValue('height') > 0){
                            find = true
                        }
                    }
                    if(i < children.length - 1 && find){
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

    $layoutDirectionChanged(){
        // not implemented yet
        this.updateGeometry()
    }
   
}

module.exports.Row = Row