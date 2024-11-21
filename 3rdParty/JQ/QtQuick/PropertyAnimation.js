const Animation = require("./Animation")
const String = require("../QtQml/String")
const Real = require("../QtQml/Real")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")

class PropertyAnimation extends Animation {
    static meta = Object.assign({}, Animation.meta, {
        duration: { type: Real, value: 250, signalName: 'durationChanged' },
        to: { type: Var, value: undefined, signalName: 'toChanged' },
        from: { type: Var, value: undefined, signalName: 'fromChanged' },
        property: { type: String, value: '', signalName: 'propertyChanged'},
        properties: { type: String, value: '', signalName: 'propertiesChanged' },
        target: { type: Var, value: undefined, signalName: 'targetChanged' },

        durationChanged: { type:Signal, slotName:'onDurationChanged', args:[] },
        toChanged: { type:Signal, slotName:'onToChanged', args:[] },
        fromChanged: { type:Signal, slotName:'onFromChanged', args:[] },
        propertyChanged: { type:Signal, slotName:'onPropertyChanged', args:[] },
        propertiesChanged: { type:Signal, slotName:'onPropertiesChanged', args:[] },
        targetChanged: { type:Signal, slotName:'onTargetChanged', args:[] },
    })

    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.target = parent

        return obj
    }

    onPropertyChanged(){
        this.properties = this.property
    }   
    
    restart() { 
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

            for(let prop of properties){
                if(this.to === undefined) this.to = this.target[prop]
                if(this.from === undefined) this.from = this.target[prop]
                
                let increment = (this.to - this.from) / (this.duration / (1000 / 60))
     
                this.target[prop] = this.target[prop] + increment
     

                if(this.to >= this.from && this.target[prop] >= this.to) {
                    this.target[prop] = this.to
                    let loops = this.__loopCounter + 1
                    if(loops < this.loops || this.loops === Animation.Infinite){
                        this.restart()
                        this.__loopCounter = loops
                    } else {
                        this.running = false
                        this.stopped()
                        this.finished()
                    }
                } else if(this.to <= this.from && this.target[prop] <= this.to) {
                    this.target[prop] = this.to
                    let loops = this.__loopCounter + 1
                    if(loops < this.loops || this.loops === Animation.Infinite){
                        this.restart()
                        this.__loopCounter = loops
                    } else {
                        this.running = false
                        this.stopped()
                        this.finished()
                    }
                }
            }
        }
    }
 
}

PropertyAnimation.initialize()

module.exports = PropertyAnimation