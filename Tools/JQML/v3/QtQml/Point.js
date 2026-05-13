const GroupProperty = require("./GroupProperty")
const Real = require("./Real")
const Signal = require("./Signal")

class Point extends GroupProperty {
    static meta = {
        x: {type:Real, value:0, },
        y: {type:Real, value:0, },
        
        xChanged: {type:Signal, args:[]},
        yChanged: {type:Signal, args:[]},
    }

    
}



module.exports = Point