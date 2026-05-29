const Animation = require("./Animation")
const String = require("../QtQml/String")
const Real = require("../QtQml/Real")
const Var = require("../QtQml/Var")
const EasingGroup = require("./EasingGroup")
const Signal = require("../QtQml/Signal")
const { $easingFunc } = require("./easing")

class PropertyAnimation extends Animation {
    static meta = Object.assign({}, Animation.meta, {
        duration: { type: Real, value: 250},
        to: { type: Var, value: undefined},
        from: { type: Var, value: undefined},
        property: { type: String, value: '', signalName: 'propertyChanged'},
        properties: { type: String, value: ''},
        target: { type: Var, value: undefined},
        easing: { type: EasingGroup},

        durationChanged: { type:Signal, args:[] },
        toChanged: { type:Signal, args:[] },
        fromChanged: { type:Signal, args:[] },
        propertyChanged: { type:Signal, args:[] },
        propertiesChanged: { type:Signal, args:[] },
        targetChanged: { type:Signal, args:[] },
        easingChanged: { type:Signal, args:[] },
    })

    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.target = parent
        obj.__elapsed = 0

        return obj
    }

    SLOT_propertyChanged(oldValue, newValue){
        this.properties = this.property
    }

    restart() { 
        this.__elapsed = 0
        let properties = this.properties.split(',')
        for(let prop of properties){
            if(this.target[prop] !== this.from){
                this.target[prop] = this.from
            }
            
        }
		super.restart()
    }

    __tick(){
        if(this.target && this.running){
            
            let properties = this.properties.split(',')
            const duration = this.duration

            for(let prop of properties){
                if(this.to === undefined) this.to = this.target[prop]
                if(this.from === undefined) this.from = this.target[prop]

                const from = this.from
                const to = this.to

                this.__elapsed += 1000 / 60
                let t = Math.min(1, this.__elapsed / duration)
                let progress = $easingFunc(t, this.easing)
                let value = from + (to - from) * progress

                if(t >= 1){
                    this.target[prop] = to
                    this.__elapsed = 0
                    let loops = this.__loopCounter + 1
                    if(loops < this.loops || this.loops === Animation.Infinite){
                        this.restart()
                        this.__loopCounter = loops
                    } else {
                        this.running = false
                        this.stopped()
                        this.finished()
                    }
                } else {
                    this.target[prop] = value
                }
            }
        }
    }
 
}



module.exports = PropertyAnimation