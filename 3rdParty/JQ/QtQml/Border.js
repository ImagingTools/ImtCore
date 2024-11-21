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

    static create(parent, meta){
        let obj = super.create(parent, meta)

        obj.onChanged()

        return obj
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

Border.initialize()

module.exports = Border