const Item = require("../../QtQuick/Item")
const Color = require("../../QtQml/Color")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Signal = require("../../QtQml/Signal")

class DropShadow extends Item {
    static meta = Object.assign({}, Item.meta, {
        color: {type: Color, value:'black', },
        horizontalOffset: {type: Real, value:0, },
        verticalOffset: {type: Real, value:0, },
        radius: {type: Real, value:4, },
        samples: {type: Real, value:9, },
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
        JQApplication.updateLater(this)
    }

    SLOT_colorChanged(oldValue, newValue){
        JQApplication.updateLater(this)
    }

    SLOT_horizontalOffsetChanged(oldValue, newValue){
        JQApplication.updateLater(this)
    }

    SLOT_verticalOffsetChanged(oldValue, newValue){
        JQApplication.updateLater(this)
    }

    SLOT_radiusChanged(oldValue, newValue){
        JQApplication.updateLater(this)
    }

    SLOT_samplesChanged(oldValue, newValue){
        JQApplication.updateLater(this)
    }

    SLOT_spreadChanged(oldValue, newValue){
        this.__updateShadow()
    }

    SLOT_sourceChanged(oldValue, newValue){
        JQApplication.updateLater(this)
    }

    __updateShadow(){
        if(this.visible){
            if(this.source){
                let rgba = Color.getRGBA(this.__proxy, 'color', this.__self.constructor.meta.color)
                this.source.__setDOMStyle({
                    boxShadow: `${this.horizontalOffset}px ${this.verticalOffset}px ${this.radius}px ${this.spread}px rgba(${rgba.r},${rgba.g},${rgba.b},${this.color === 'transparent' ? 0 : rgba.a * this.opacity})`
                })
            }
        } else {
            this.source.__setDOMStyle({
                boxShadow: `unset`
            })
        }
    }

    __endUpdate(){
        super.__endUpdate()
        this.__updateShadow()
    }
}



module.exports = DropShadow