const Item = require("../../QtQuick/Item")
const Color = require("../../QtQml/Color")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class InnerShadow extends Item {
    static meta = Object.assign({}, Item.meta, {
        color: {type: Color, value:'black', },
        horizontalOffset: {type: Real, value:0, },
        verticalOffset: {type: Real, value:0, },
        radius: {type: Real, value:0, },
        samples: {type: Real, value:0, },
        spread: {type: Real, value:0, },
        source: {type: Var, value:undefined, },
        
        colorChanged: {type:Signal, args:[]},
        horizontalOffsetChanged: {type:Signal, args:[]},
        verticalOffsetChanged: {type:Signal, args:[]},
        radiusChanged: {type:Signal, args:[]},
        samplesChanged: {type:Signal, args:[]},
        spreadChanged: {type:Signal, args:[]},
        sourceChanged: {type:Signal, args:[]},
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
        if(this.visible){
            if(this.source){
                let rgba = Color.getRGBA(this.__proxy, 'color', this.__self.constructor.meta.color)
                this.source.__setDOMStyle({
                    boxShadow: `${this.horizontalOffset}px ${this.verticalOffset}px ${this.radius}px ${this.spread}px rgba(${rgba.r},${rgba.g},${rgba.b},${this.color === 'transparent' ? 0 : rgba.a * this.opacity}) inset`
                })
            }
        } else {
            this.source.__setDOMStyle({
                boxShadow: `unset`
            })
        }
    }
}



module.exports = InnerShadow