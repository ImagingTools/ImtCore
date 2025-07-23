const GroupProperty = require("./GroupProperty")
const Real = require("./Real")
const Signal = require("./Signal")

class Rect extends GroupProperty {
    static meta = {
        x: {type:Real, value:0, signalName:'xChanged'},
        y: {type:Real, value:0, signalName:'yChanged'},
        width: {type:Real, value:0, signalName:'widthChanged'},
        height: {type:Real, value:0, signalName:'heightChanged'},
        left: {type:Real, value:0, signalName:'leftChanged'},
        right: {type:Real, value:0, signalName:'rightChanged'},
        top: {type:Real, value:0, signalName:'topChanged'},
        bottom: {type:Real, value:0, signalName:'bottomChanged'},
        
        xChanged: {type:Signal, slotName:'onXChanged', args:[]},
        yChanged: {type:Signal, slotName:'onYChanged', args:[]},
        widthChanged: {type:Signal, slotName:'onWidthChanged', args:[]},
        heightChanged: {type:Signal, slotName:'onHeightChanged', args:[]},
        leftChanged: {type:Signal, slotName:'onLeftChanged', args:[]},
        rightChanged: {type:Signal, slotName:'onRightChanged', args:[]},
        topChanged: {type:Signal, slotName:'onTopChanged', args:[]},
        bottomChanged: {type:Signal, slotName:'onBottomChanged', args:[]},
    }

    
}



module.exports = Rect