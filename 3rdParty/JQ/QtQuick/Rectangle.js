const Item = require("./Item")
const Gradient = require("./Gradient")
const Color = require("../QtQml/Color")
const Real = require("../QtQml/Real")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")
const Border = require("../QtQml/Border")

class Rectangle extends Item {
    static meta = Object.assign({}, Item.meta, {
        color: {type: Color, value:'white', signalName:'colorChanged'},
        radius: {type: Real, value: 0, signalName: 'radiusChanged' },
        gradient: {type: Var, value: undefined, signalName: 'gradientChanged' },
        border: {type:Border},

        colorChanged: {type:Signal, slotName:'onColorChanged', args:[]},
        radiusChanged: {type:Signal, slotName:'onRadiusChanged', args:[]},
        gradientChanged: {type:Signal, slotName:'onGradientChanged', args:[]},
    })

    static create(parent, model, properties=[], ...args){
        let proxy = super.create(parent, model, properties, ...args)
        proxy.__getDOM().classList.add('Rectangle')

        return proxy
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
    onGradientChanged(){
        if(this.__gradient){
            this.__gradient.__removeListener(this)
        }

        this.__gradient = this.gradient

        if(this.__gradient){
            this.__gradient.__addListener(this)
        }

        this.__updateGradient()
    }

    onColorChanged(){
        this.__setDOMStyle({
            backgroundColor: this.color
        })
    }
    
    onRadiusChanged(){
        this.__setDOMStyle({
            borderRadius: `${this.radius}px`
        })
    }

    onOpacityChanged(){
        let rgba = this.__getObject('color').__toRGBA()
        this.__setDOMStyle({
            opacity: this.opacity > 0 ? 1 : 0,
            backgroundColor: `rgba(${rgba.r},${rgba.g},${rgba.b},${this.color === 'transparent' ? 0 : rgba.a * this.opacity})`
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