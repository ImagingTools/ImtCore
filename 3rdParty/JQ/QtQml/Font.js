const GroupProperty = require("./GroupProperty")
const String = require("./String")
const Bool = require("./Bool")
const Real = require("./Real")
const Signal = require("./Signal")

class Font extends GroupProperty {
    static meta = {
        bold: { type: Bool, value: false, signalName: 'boldChanged' },
        family: { type: String, value: 'Segoe UI', signalName: 'familyChanged' },
        italic: { type: Bool, value: false, signalName: 'italicChanged' },
        underline: { type: Bool, value: false, signalName: 'underlineChanged' },
        pointSize: { type: Real, value: 14/0.75, signalName: 'pointSizeChanged' },
        pixelSize: { type: Real, value: 14, signalName: 'pixelSizeChanged' },

        boldChanged: {type:Signal, slotName:'onBoldChanged', args:[]},
        familyChanged: {type:Signal, slotName:'onFamilyChanged', args:[]},
        italicChanged: {type:Signal, slotName:'onItalicChanged', args:[]},
        underlineChanged: {type:Signal, slotName:'onUnderlineChanged', args:[]},
        pointSizeChanged: {type:Signal, slotName:'onPointSizeChanged', args:[]},
        pixelSizeChanged: {type:Signal, slotName:'onPixelSizeChanged', args:[]},
    }

    onChanged(){
        if(this.__parent && this.__parent.onFontChanged) this.__parent.onFontChanged()
    }

    onBoldChanged(){
        this.onChanged()
    }
    onFamilyChanged(){
        this.onChanged()
    }
    onItalicChanged(){
        this.onChanged()
    }
    onUnderlineChanged(){
        this.onChanged()
    }
    onPointSizeChanged(){
        this.onChanged()
    }
    onPixelSizeChanged(){
        this.onChanged()
    }
}

Font.initialize()

module.exports = Font