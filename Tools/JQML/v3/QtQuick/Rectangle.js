const Item = require("./Item")
const Gradient = require("./Gradient")
const Color = require("../QtQml/Color")
const Real = require("../QtQml/Real")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")
const Border = require("../QtQml/Border")

class Rectangle extends Item {
    static meta = Object.assign({}, Item.meta, {
        color: {type: Color, value:'white', },
        radius: {type: Real, value: 0},
        gradient: {type: Var, value: undefined},
        border: {type:Border},

        colorChanged: {type:Signal, args:[]},
        radiusChanged: {type:Signal, args:[]},
        gradientChanged: {type:Signal, args:[]},
    })

    static create(parent=null, properties = {}, context = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('Rectangle')

        return obj
    }

    __updateGradient(){
        if(this.gradient) {
            let style = []
            
            for(let stop of this.gradient.stops){
                style.push(`${stop.color} ${stop.position*100}%`)
            }
            if(this.gradient.orientation == Gradient.Vertical){
                this.__setDOMStyle({
                    background: `linear-gradient(180deg, ${style.join(',')})`
                })
            } else {
                this.__setDOMStyle({
                    background: `linear-gradient(90deg, ${style.join(',')})`
                })
            }
        }
    }

    SLOT_gradientChanged(oldValue, newValue){
        if(oldValue){
            oldValue.__removeListener(this)
        }

        if(newValue){
            newValue.__addListener(this)
        }

        this.__updateGradient()
    }

    SLOT_colorChanged(oldValue, newValue){
        // this.__setDOMStyle({
        //     backgroundColor: newValue
        // })
        let rgba = Color.getRGBA(this.__proxy, 'color', this.__self.constructor.meta.color)
        this.__setDOMStyle({
            opacity: 1,
            backgroundColor: `rgba(${rgba.r},${rgba.g},${rgba.b},${this.__proxy.color === 'transparent' ? 0 : rgba.a * this.opacity})`
        })
    }

    
    
    SLOT_radiusChanged(oldValue, newValue){
        this.__setDOMStyle({
            borderRadius: `${newValue}px`
        })
    }

    SLOT_opacityChanged(oldValue, newValue){
        let rgba = Color.getRGBA(this.__proxy, 'color', this.__self.constructor.meta.color)
        this.__setDOMStyle({
            opacity: newValue > 0 ? 1 : 0,
            backgroundColor: `rgba(${rgba.r},${rgba.g},${rgba.b},${this.__proxy.color === 'transparent' ? 0 : rgba.a * newValue})`
        })
    }

    __destroy(){
        if(this.__gradient){
            this.__gradient.__removeListener(this)
        }
        
        super.__destroy()
    }
}



module.exports = Rectangle