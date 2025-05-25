const QtObject = require("../QtQml/QtObject")
const Real = require("../QtQml/Real")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")

class Gradient extends QtObject {
    static Vertical = 0
    static Horizontal = 1

    static meta = Object.assign({}, QtObject.meta, {
        stops: {type:Var, value:undefined, signalName:'stopsChanged'},
        orientation: {type:Real, value:Gradient.Vertical, signalName:'orientationChanged'},

        stopsChanged: {type:Signal, slotName:'onStopsChanged', args:[]},
        orientationChanged: {type:Signal, slotName:'onOrientationChanged', args:[]},
    })

    __rects = new Set()

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.stops = []

        return obj
    }

    SLOT_stopsChanged(oldValue, newValue){
        this.__updateGradient()
    }

    SLOT_orientationChanged(oldValue, newValue){
        this.__updateGradient()
    }

    __updateGradient(){
        for(let rect of this.__rects){
            rect.__updateGradient()
        }
        
    }

    __addListener(obj){
        this.__rects.add(obj)
        obj.__updateGradient()
    }

    __removeListener(obj){
        this.__rects.delete(obj)
    }
}



module.exports = Gradient