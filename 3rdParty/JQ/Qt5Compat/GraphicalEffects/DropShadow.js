const Item = require("../../QtQuick/Item")
const Color = require("../../QtQml/Color")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class DropShadow extends Item {
    static meta = Object.assign({}, Item.meta, {
        color: {type: Color, value:'black', signalName:'colorChanged'},
        horizontalOffset: {type: Real, value:0, signalName:'horizontalOffsetChanged'},
        verticalOffset: {type: Real, value:0, signalName:'verticalOffsetChanged'},
        radius: {type: Real, value:4, signalName:'radiusChanged'},
        samples: {type: Real, value:9, signalName:'samplesChanged'},
        spread: {type: Real, value:0, signalName:'spreadChanged'},
        source: {type: Var, value:undefined, signalName:'sourceChanged'},
        
        colorChanged: {type:Signal, slotName:'onColorChanged', args:[]},
        horizontalOffsetChanged: {type:Signal, slotName:'onHorizontalOffsetChanged', args:[]},
        verticalOffsetChanged: {type:Signal, slotName:'onVerticalOffsetChanged', args:[]},
        radiusChanged: {type:Signal, slotName:'onRadiusChanged', args:[]},
        samplesChanged: {type:Signal, slotName:'onSamplesChanged', args:[]},
        spreadChanged: {type:Signal, slotName:'onSpreadChanged', args:[]},
        sourceChanged: {type:Signal, slotName:'onSourceChanged', args:[]},
    })

    SLOT_visibleChanged(oldValue, newValue){
        super.SLOT_visibleChanged()
        this.__updateShadow()
    }

    SLOT_colorChanged(oldValue, newValue){
        this.__updateShadow()
    }

    SLOT_horizontalOffsetChanged(oldValue, newValue){
        this.__updateShadow()
    }

    SLOT_verticalOffsetChanged(oldValue, newValue){
        this.__updateShadow()
    }

    SLOT_radiusChanged(oldValue, newValue){
        this.__updateShadow()
    }

    SLOT_samplesChanged(oldValue, newValue){
        this.__updateShadow()
    }

    SLOT_spreadChanged(oldValue, newValue){
        this.__updateShadow()
    }

    SLOT_sourceChanged(oldValue, newValue){
        this.__updateShadow()
    }

    __updateShadow(){
        // if(this.source && this.visible){
        //     let rgba = this.__getDataQml('color').__toRGBA()
        //     this.source.__setDOMStyle({
        //         boxShadow: `${this.horizontalOffset}px ${this.verticalOffset}px ${this.radius}px ${this.spread}px rgba(${rgba.r},${rgba.g},${rgba.b},${this.color === 'transparent' ? 0 : rgba.a * this.opacity})`
        //     })
        // }
    }
}



module.exports = DropShadow