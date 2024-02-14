const { Item } = require('./Item')
const { QColor, QReal, QVar } = require('../utils/properties')

class DropShadow extends Item {
    static defaultProperties = {
        color: { type: QColor, value: 'black', changed: '$updateShadow' },
        horizontalOffset: { type: QReal, value: 0, changed: '$updateShadow' },
        verticalOffset: { type: QReal, value: 0, changed: '$updateShadow' },
        radius: { type: QReal, value: 4, changed: '$updateShadow' },
        samples: { type: QReal, value: 9, changed: '$updateShadow' },
        spread: { type: QReal, value: 0, changed: '$updateShadow' },
        source: { type: QVar, value: undefined, changed: '$sourceChanged' },
    }

    constructor(parent,exCtx,exModel) {
        super(parent,exCtx,exModel)

        this.getProperty('visible').getNotify().connect(this, this.$updateShadow)
    }

    $sourceChanged(){
        if(this.$source){
            let index = this.$source.$shadows.indexOf(this)
            if(index >= 0) this.$source.$shadows.splice(index, 1)
            if(this.$source.$shadows.length === 0) delete this.$source.$shadows
            this.$source.$updateShadow()
        }

        this.$source = this.getPropertyValue('source')
        if(this.$source){
            if(!this.$source.$shadows) this.$source.$shadows = []
            this.$source.$shadows.push(this)
            this.$source.$updateShadow()
        }

    }

    $updateShadow(){
        if(this.$source) this.$source.$updateShadow()
    }
}

module.exports.DropShadow = DropShadow