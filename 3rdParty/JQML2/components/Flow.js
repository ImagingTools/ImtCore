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
        this.setStyle({
            maxWidth: this.getPropertyValue('width')+'px',
        })

        this.updateGeometry()
    }

    $heightChanged(){
        this.setStyle({
            maxHeight: this.getPropertyValue('height')+'px',
        })

        this.updateGeometry()
    }

    updateGeometry(){
        if(this.preventAutoUpdateGeometry || !this.getPropertyValue('visible')) return

        let children = this.getProperty('children').get()

        for(let i = 0; i < children.length; i++){
            if(children[i].getPropertyValue('visible')){
                if(children[i] instanceof Repeater) continue

                if(children[i].getPropertyValue('width') > 0 && children[i].getPropertyValue('height') > 0){
                    children[i].setStyle({
                        marginLeft: 0,
                        marginTop: 0,
                    })
                } else {
                    children[i].setStyle({
                        marginLeft: -this.getPropertyValue('spacing')+'px',
                        marginTop: -this.getPropertyValue('spacing')+'px',
                    })
                }
            }
        }

        this.setStyle({
            paddingLeft: this.$properties.leftPadding ? this.getPropertyValue('leftPadding')+'px' : this.getPropertyValue('padding')+'px',
            paddingRight: this.$properties.rightPadding ? this.getPropertyValue('rightPadding')+'px' : this.getPropertyValue('padding')+'px',
            paddingTop: this.$properties.topPadding ? this.getPropertyValue('topPadding')+'px' : this.getPropertyValue('padding')+'px',
            paddingBottom: this.$properties.bottomPadding ? this.getPropertyValue('bottomPadding')+'px' : this.getPropertyValue('padding')+'px',
        })

        this.getProperty('width').setAuto(this.$dom.scrollWidth)
        this.getProperty('height').setAuto(this.$dom.scrollHeight)
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
        this.setStyle({
            gap: this.getPropertyValue('spacing')+'px',
        })

        this.updateGeometry()
    }
}

module.exports.Flow = Flow