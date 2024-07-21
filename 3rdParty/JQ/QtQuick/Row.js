const Item = require("./Item")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class Row extends Item {
    static meta = Object.assign({}, Item.meta, {
        spacing: {type: Real, value:0, signalName:'spacingChanged'},
        
        spacingChanged: {type:Signal, slotName:'onSpacingChanged', args:[]},
    })

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        proxy.__DOM.classList.add('Row')

        return proxy
    }

    onSpacingChanged(){
        this.__setDOMStyle({
            gap: this.spacing + 'px'
        })
    }
}

module.exports = Row