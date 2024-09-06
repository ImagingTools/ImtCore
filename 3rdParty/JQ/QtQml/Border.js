const GroupProperty = require("./GroupProperty")
const Real = require("./Real")
const Color = require("./Color")
const Signal = require("./Signal")

class Border extends GroupProperty {
    static meta = {
        width: {type:Real, value:1, signalName:'widthChanged'},
        color: {type:Color, value:'black', signalName:'colorChanged'},
        
        widthChanged: {type:Signal, slotName:'onWidthChanged', args:[]},
        colorChanged: {type:Signal, slotName:'onColorChanged', args:[]},
    }

    onChanged(){
        if(this.__parent)
        this.__parent.__setDOMStyle({
            outline: `${this.width}px solid ${this.color}`,
            outlineOffset: `-${this.width}px`,
        })
    }

    onWidthChanged(){
        this.onChanged()
    }

    onColorChanged(){
        this.onChanged()
    }
}

module.exports = Border