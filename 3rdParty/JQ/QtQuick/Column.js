const Item = require("./Item")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class Column extends Item {
    static meta = Object.assign({}, Item.meta, {
        spacing: {type: Real, value:0, signalName:'spacingChanged'},
        
        spacingChanged: {type:Signal, slotName:'onSpacingChanged', args:[]},
    })

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        proxy.__DOM.classList.add('Column')

        return proxy
    }

    onSpacingChanged(){
        this.__setDOMStyle({
            gap: this.spacing + 'px'
        })
        this.__updateGeometry()
    }

    __updateGeometry(){
        this.__setDOMStyle({
            width: 'fit-content',
            minWidth: 'fit-content',
            height: 'fit-content',
            minHeight: 'fit-content',
        })

        let rect = this.__DOM.getBoundingClientRect()

        this.__getObject('width').__setAuto(rect.width)
        this.__getObject('height').__setAuto(rect.height)

        this.__setDOMStyle({
            width: this.width + 'px',
            minWidth: this.width + 'px',
            height: this.height + 'px',
            minHeight: this.height + 'px',
        })
    }

    __endUpdate(...args){
        this.__updateGeometry()
    }
}

module.exports = Column