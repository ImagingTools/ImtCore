const Item = require("./Item")
const Color = require("../QtQml/Color")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")
const Border = require("../QtQml/Border")

class Rectangle extends Item {
    static meta = Object.assign({}, Item.meta, {
        color: {type: Color, value:'white', signalName:'colorChanged'},
        radius: {type: Real, value: 0, signalName: 'radiusChanged' },
        border: {type:Border},

        colorChanged: {type:Signal, slotName:'onColorChanged', args:[]},
        radiusChanged: {type:Signal, slotName:'onRadiusChanged', args:[]},
    })

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        let self = proxy.__self 

        self.__setDOMStyle({
            backgroundColor: 'white'
        })

        return proxy
    }

    onColorChanged(){
        this.__setDOMStyle({
            backgroundColor: this.color
        })
    }
    
    onRadiusChanged(){
        this.__setDOMStyle({
            borderRadius: `${this.radius}px`
        })
    }
}

module.exports = Rectangle