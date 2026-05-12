const { Item } = require('./Item')
const { QString, QBool, QReal, QVar, QColor } = require('../utils/properties')

class MenuSeparator extends Item {
    static defaultProperties = {
        contentItem: { type: QVar },
        padding: { type: QReal, value: 0, changed: '$paddingChanged' },
        topPadding: { type: QReal, value: 0 },
        bottomPadding: { type: QReal, value: 0 },
    }

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)
        this.setStyle({
            width: '100%',
            height: '1px',
            minHeight: '1px',
            boxSizing: 'border-box',
            borderTop: '1px solid #cccccc',
            marginTop: '4px',
            marginBottom: '4px',
            pointerEvents: 'none',
        })
    }

    $paddingChanged(){
        let p = this.getPropertyValue('padding')
        this.setStyle({
            marginTop: `${p}px`,
            marginBottom: `${p}px`,
        })
    }
}

module.exports.MenuSeparator = MenuSeparator
