const { Item } = require('./Item')
const { QReal } = require('../utils/properties')
const { Qt } = require('../utils/Qt')

class Flow extends Item {
    static LeftToRight = 0
    static TopToBottom = 1

    static defaultProperties = {
        spacing: { type: QReal, value: 0, changed: '$spacingChanged' },
        flow: { type: QReal, value: Qt.LeftToRight, changed: '$flowChanged' },
        layoutDirection: { type: QReal, value: Flow.LeftToRight, changed: '$layoutDirectionChanged' },

        padding: { type: QReal, value: 0, changed: '$paddingChanged' },
        leftPadding: { type: QReal, value: 0, changed: '$paddingChanged' },
        rightPadding: { type: QReal, value: 0, changed: '$paddingChanged' },
        topPadding: { type: QReal, value: 0, changed: '$paddingChanged' },
        bottomPadding: { type: QReal, value: 0, changed: '$paddingChanged' },
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)

        this.setStyle({
            flexDirection: 'row',
            flexWrap: 'wrap'
        })
    }

    $paddingChanged(){
        this.updateGeometry()
    }

    $flowChanged(){
        if(this.getPropertyValue('flow') === Flow.LeftToRight){
            this.setStyle({
                flexDirection: 'row'
            })
        } else {
            this.setStyle({
                flexDirection: 'column'
            })
        }
    }

    $layoutDirectionChanged(){

    }

    $widthChanged(){
        super.$widthChanged()
        this.updateGeometry()
    }

    $heightChanged(){
        super.$heightChanged()
        this.updateGeometry()
    }

    updateGeometry(){
        if(this.preventAutoUpdateGeometry || !this.getPropertyValue('visible')) return

        let children = this.getProperty('children').get()

        for(let i = 0; i < children.length; i++){
            if(children[i].getPropertyValue('visible')){
                if(children[i] instanceof Repeater) continue

                if(children[i].getPropertyValue('width') > 0 && children[i].getPropertyValue('height') > 0){
                    let find = false
                    for(let k = i + 1; k < children.length; k++){
                        if(children[k] instanceof Repeater) continue
                        if(children[k].getPropertyValue('width') > 0 && children[k].getPropertyValue('height') > 0){
                            find = true
                        }
                    }
                    if(i < children.length - 1 && find){
                        children[i].setStyle({
                            marginRight: `${this.getPropertyValue('spacing')}px`,
                            marginBottom: `${this.getPropertyValue('spacing')}px`
                        })
                    } else {
                        children[i].setStyle({
                            marginRight: `0`,
                            marginBottom: `0`
                        })
                    }
                }
            }
        }

        this.setStyle({
            minWidth: !this.getProperty('width').auto ? this.getPropertyValue('width')+'px' : 0,
            minHeight: !this.getProperty('height').auto ? this.getPropertyValue('height')+'px' : 0,
            width: !this.getProperty('width').auto ? this.getPropertyValue('width')+'px' : 0,
            height: !this.getProperty('height').auto ? this.getPropertyValue('height')+'px' : 0,

            paddingLeft: this.$properties.leftPadding ? this.getPropertyValue('leftPadding')+'px' : this.getPropertyValue('padding')+'px',
            paddingRight: this.$properties.rightPadding ? this.getPropertyValue('rightPadding')+'px' : this.getPropertyValue('padding')+'px',
            paddingTop: this.$properties.topPadding ? this.getPropertyValue('topPadding')+'px' : this.getPropertyValue('padding')+'px',
            paddingBottom: this.$properties.bottomPadding ? this.getPropertyValue('bottomPadding')+'px' : this.getPropertyValue('padding')+'px',
        })

        this.getProperty('width').setAuto(this.$dom.scrollWidth)
        this.getProperty('height').setAuto(this.$dom.scrollHeight)

        this.setStyle({
            minWidth: this.getPropertyValue('width')+'px',
            minHeight: this.getPropertyValue('height')+'px',
            width: this.getPropertyValue('width')+'px',
            height: this.getPropertyValue('height')+'px',
        })
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

module.exports.Flow = Flow