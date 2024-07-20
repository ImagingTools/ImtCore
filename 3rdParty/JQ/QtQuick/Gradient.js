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

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        proxy.stops = []

        return proxy
    }
}

module.exports = Gradient