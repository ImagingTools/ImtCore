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

    static create(parent, model, ...args){
        let proxy = super.create(parent, model, ...args)
        proxy.__getDOM().classList.add('Rectangle')

        return proxy
    }

    onGradientChanged(){
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
}

module.exports = Rectangle