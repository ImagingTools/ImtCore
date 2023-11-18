const { Item } = require('./Item')
const { QColor, QReal, QVar } = require('../utils/properties')

class InnerShadow extends Item {
    static defaultProperties = {
        color: { type: QColor, value: 'black', changed: '$updateShadow' },
        horizontalOffset: { type: QReal, value: 0, changed: '$updateShadow' },
        verticalOffset: { type: QReal, value: 0, changed: '$updateShadow' },
        radius: { type: QReal, value: 0, changed: '$updateShadow' },
        samples: { type: QReal, value: 0, changed: '$updateShadow' },
        spread: { type: QReal, value: 0, changed: '$updateShadow' },
        source: { type: QVar, value: undefined, changed: '$updateShadow' },
    }

    constructor(parent) {
        super(parent)
    }

    $updateShadow(){
        if(this.getPropertyValue('source')) {
            let rgba = this.getProperty('color').toRGBA()
            this.getPropertyValue('source').setStyle({
                boxShadow: `${this.getPropertyValue('horizontalOffset')}px ${this.getPropertyValue('verticalOffset')}px ${this.getPropertyValue('radius')}px ${this.getPropertyValue('spread')}px rgba(${rgba.r},${rgba.g},${rgba.b},${this.getPropertyValue('color') === 'transparent' ? 0 : rgba.a * this.getPropertyValue('opacity')}) inset`
            })
        }
    }
}

module.exports.InnerShadow = InnerShadow