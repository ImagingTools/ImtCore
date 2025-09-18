const QtObject = require("../QtQml/QtObject")
const Color = require("../QtQml/Color")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class GradientStop extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        color: {type:Color, value:'black', },
        position: {type:Real, value:0, },

        colorChanged: {type:Signal, args:[]},
        positionChanged: {type:Signal, args:[]},
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        parent.stops.push(obj)

        return obj
    }

    SLOT_colorChanged(oldValue, newValue){
        this.__updateGradient()
    }

    SLOT_positionChanged(oldValue, newValue){
        this.__updateGradient()
    }

    __updateGradient(){
        this.parent.__updateGradient()
    }
}



module.exports = GradientStop