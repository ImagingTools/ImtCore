const GroupProperty = require("./GroupProperty")
const String = require("./String")
const Bool = require("./Bool")
const Real = require("./Real")
const Signal = require("./Signal")

class Font extends GroupProperty {
    static meta = {
        bold: { type: Bool, value: false},
        family: { type: String, value: 'Segoe UI'},
        italic: { type: Bool, value: false},
        underline: { type: Bool, value: false},
        pointSize: { type: Real, value: 14/0.75},
        pixelSize: { type: Real, value: 14},

        boldChanged: {type:Signal, args:[]},
        familyChanged: {type:Signal, args:[]},
        italicChanged: {type:Signal, args:[]},
        underlineChanged: {type:Signal, args:[]},
        pointSizeChanged: {type:Signal, args:[]},
        pixelSizeChanged: {type:Signal, args:[]},
    }

    onChanged(){
        if(this.__parent && this.__parent.onFontChanged) this.__parent.onFontChanged.call(this.__parent.__proxy)
    }

    SLOT_boldChanged(oldValue, newValue){
        this.onChanged()
    }
    SLOT_familyChanged(oldValue, newValue){
        this.onChanged()
    }
    SLOT_italicChanged(oldValue, newValue){
        this.onChanged()
    }
    SLOT_underlineChanged(oldValue, newValue){
        this.onChanged()
    }
    SLOT_pointSizeChanged(oldValue, newValue){
        this.onChanged()
    }
    SLOT_pixelSizeChanged(oldValue, newValue){
        this.onChanged()
    }
}



module.exports = Font