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

    static create(parent, model, ...args){
        let proxy = super.create(parent, model, ...args)
        proxy.stops = []

        return proxy
    }

    onStopsChanged(){
        this.__updateGradient()
    }

    onOrientationChanged(){
        this.__updateGradient()
    }

    __updateGradient(){
        for(let rect of this.__rects){
            rect.__updateGradient()
        }
        
    }

    __addListener(obj){
        this.__rects.add(obj)
    }

    __removeListener(obj){
        this.__rects.delete(obj)
    }
}

module.exports = Gradient