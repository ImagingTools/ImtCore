const { Item } = require('./Item')
const { QColor, QBorder, QReal } = require('../utils/properties')

class Column extends Item {
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
        if(this.preventAutoUpdateGeometry) return
        this.setStyle({
            width: '0px',
            minWidth: '0px',
            height: '0px',
            minHeight: '0px',
        })
        this.getProperty('width').setAuto(this.getDom().scrollWidth)
        this.getProperty('height').setAuto(this.getDom().scrollHeight)

        this.setStyle({
            width: `${this.getProperty('width').get()}px`,
            minWidth: `${this.getProperty('width').get()}px`,
            height: `${this.getProperty('height').get()}px`,
            minHeight: `${this.getProperty('height').get()}px`,
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
        this.setStyle({
            gap: `${this.getPropertyValue('spacing')}px`
        })
        this.updateGeometry()
    }
}

module.exports.Column = Column