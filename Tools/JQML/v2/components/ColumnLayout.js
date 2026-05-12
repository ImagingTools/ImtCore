const { Item } = require('./Item')
const { QReal } = require('../utils/properties')

class ColumnLayout extends Item {
    static defaultProperties = {
        spacing: { type: QReal, value: 0, changed: '$spacingChanged' },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)

        this.setStyle({
            flexDirection: 'column'
        })
    }

    updateGeometry(){
        if(this.preventAutoUpdateGeometry || !this.getPropertyValue('visible')) return

        let children = this.getProperty('children').get()
        let w = 0
        let h = 0
        for(let i = 0; i < children.length; i++){
            if(children[i].getPropertyValue('visible')){
                if(children[i] instanceof Repeater) continue

                let layout = children[i].getProperty('Layout')
                let fillWidth = layout && layout.$properties.fillWidth ? layout.$properties.fillWidth.get() : false
                let fillHeight = layout && layout.$properties.fillHeight ? layout.$properties.fillHeight.get() : false

                if(fillWidth){
                    children[i].setStyle({ alignSelf: 'stretch' })
                } else {
                    children[i].setStyle({ alignSelf: '' })
                }
                if(fillHeight){
                    children[i].setStyle({ flexGrow: '1' })
                } else {
                    children[i].setStyle({ flexGrow: '' })
                }

                w = Math.max(w, children[i].getPropertyValue('width'))
                if(children[i].getPropertyValue('width') > 0 && children[i].getPropertyValue('height') > 0){
                    h += children[i].getPropertyValue('height')
                    let find = false
                    for(let k = i + 1; k < children.length; k++){
                        if(children[k] instanceof Repeater) continue
                        if(children[k].getPropertyValue('width') > 0 && children[k].getPropertyValue('height') > 0){
                            find = true
                        }
                    }
                    if(i < children.length - 1 && find){
                        h += this.getPropertyValue('spacing')
                        children[i].setStyle({
                            marginBottom: `${this.getPropertyValue('spacing')}px`
                        })
                    } else {
                        children[i].setStyle({
                            marginBottom: `0`
                        })
                    }
                } else {
                    children[i].setStyle({
                        marginBottom: `0`
                    })
                }
            } else {
                children[i].setStyle({
                    marginBottom: `0`
                })
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

        let layout = child.getProperty('Layout')
        if(layout && layout.getNotify){
            layout.getNotify().connect(this, this.updateGeometry)
        }
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

module.exports.ColumnLayout = ColumnLayout
