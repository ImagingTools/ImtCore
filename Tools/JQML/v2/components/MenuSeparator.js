const { Item } = require('./Item')
const { QString, QBool, QReal, QVar, QColor } = require('../utils/properties')

class MenuSeparator extends Item {
    $isSeparator = true

    static defaultProperties = {
        contentItem: { type: QVar },
        padding: { type: QReal, value: 2, changed: '$paddingChanged' },
        topPadding: { type: QReal, value: 6 },
        bottomPadding: { type: QReal, value: 6 },
    }

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)
        // Qt Basic: padding=2, verticalPadding=6, contentItem: 188x1, color=palette.mid(#c0c0c0)
        this.setStyle({
            width: '100%',
            minHeight: '1px',
            boxSizing: 'border-box',
            paddingTop: '6px',
            paddingBottom: '6px',
            paddingLeft: '2px',
            paddingRight: '2px',
            pointerEvents: 'none',
        })

        this.$line = document.createElement('div')
        this.$line.style.cssText = 'width:188px;max-width:100%;height:1px;background:#d0d0d0;'
        this.getDom().appendChild(this.$line)
    }

    $paddingChanged(){
        let p = this.getPropertyValue('padding')
        // Qt: verticalPadding = padding + 4
        let vp = p + 4
        this.setStyle({
            paddingTop: `${vp}px`,
            paddingBottom: `${vp}px`,
            paddingLeft: `${p}px`,
            paddingRight: `${p}px`,
        })
    }
}

module.exports.MenuSeparator = MenuSeparator
