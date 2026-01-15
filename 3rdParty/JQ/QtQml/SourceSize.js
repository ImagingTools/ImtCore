const GroupProperty = require("./GroupProperty")
const Real = require("./Real")
const Signal = require("./Signal")

class SourceSize extends GroupProperty {
    static meta = {
        width: {type:Real, value:0, },
        height: {type:Real, value:0, },
        
        widthChanged: {type:Signal, args:[]},
        heightChanged: {type:Signal, args:[]},
    }

    onChanged(){
        if(this.__parent){
            this.__parent.__updateImage()
        }
    }

    SLOT_widthChanged(oldValue, newValue){
        this.onChanged()
    }

    SLOT_heightChanged(oldValue, newValue){
        this.onChanged()
    }
}



module.exports = SourceSize