const GroupProperty = require("./GroupProperty")
const Real = require("./Real")
const Signal = require("./Signal")

class SourceSize extends GroupProperty {
    static meta = {
        width: {type:Real, value:0, signalName:'widthChanged'},
        height: {type:Real, value:0, signalName:'heightChanged'},
        
        widthChanged: {type:Signal, slotName:'onWidthChanged', args:[]},
        heightChanged: {type:Signal, slotName:'onHeightChanged', args:[]},
    }

    onChanged(){
        if(this.__parent){
            this.__parent.__updateImage()
        }
    }

    onWidthChanged(){
        this.onChanged()
    }

    onHeightChanged(){
        this.onChanged()
    }
}

module.exports = SourceSize