const GroupProperty = require("./GroupProperty")
const Real = require("./Real")
const Signal = require("./Signal")

class Point extends GroupProperty {
    static meta = {
        x: {type:Real, value:0, signalName:'xChanged'},
        y: {type:Real, value:0, signalName:'yChanged'},
        
        xChanged: {type:Signal, slotName:'onXChanged', args:[]},
        yChanged: {type:Signal, slotName:'onYChanged', args:[]},
    }

    
}

Point.initialize()

module.exports = Point