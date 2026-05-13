const GroupProperty = require("./GroupProperty")
const Real = require("./Real")
const Signal = require("./Signal")

class Rect extends GroupProperty {
    static meta = {
        x: {type:Real, value:0, },
        y: {type:Real, value:0, },
        width: {type:Real, value:0, },
        height: {type:Real, value:0, },
        left: {type:Real, value:0, },
        right: {type:Real, value:0, },
        top: {type:Real, value:0, },
        bottom: {type:Real, value:0, },
        
        xChanged: {type:Signal, args:[]},
        yChanged: {type:Signal, args:[]},
        widthChanged: {type:Signal, args:[]},
        heightChanged: {type:Signal, args:[]},
        leftChanged: {type:Signal, args:[]},
        rightChanged: {type:Signal, args:[]},
        topChanged: {type:Signal, args:[]},
        bottomChanged: {type:Signal, args:[]},
    }

    
}



module.exports = Rect