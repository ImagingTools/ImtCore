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
    }

    __updateGeometry(){
        let rect = this.__DOM.getBoundingClientRect()
        this.width = rect.width
        this.height = rect.height
    }

    __onUpdate(sender, options){
        this.__updateGeometry()
    }

    __onUpdateAll(list){
        this.__updateGeometry()
    }
}

module.exports = Column