const QtObject = require("./QtObject")
const Bool = require("./Bool")
const String = require("./String")
const Var = require("./Var")
const Signal = require("./Signal")

const Property = require("./Property")

class Binding extends QtObject {
    static RestoreNone = 0
    static RestoreBinding = 1
    static RestoreValue = 2
    static RestoreBindingOrValue = 3

    static meta = Object.assign({}, QtObject.meta, {
        delayed: { type: Bool, value: false, signalName: 'delayedChanged' },
        property: { type: String, value: '', signalName: 'propertyChanged' },
        restoreMode: { type: Var, value: Binding.RestoreBindingOrValue, signalName: 'restoreModeChanged' },
        target: { type: Var, value: undefined, signalName: 'targetChanged' },
        value: { type: Var, value: undefined, signalName: 'valueChanged' },
        when: { type: Bool, value: true, signalName: 'whenChanged' },

        delayedChanged: {type:Signal, slotName:'onDelayedChanged', args:[]},
        propertyChanged: {type:Signal, slotName:'onPropertyChanged', args:[]},
        restoreModeChanged: {type:Signal, slotName:'onRestoreModeChanged', args:[]},
        targetChanged: {type:Signal, slotName:'onTargetChanged', args:[]},
        valueChanged: {type:Signal, slotName:'onValueChanged', args:[]},
        whenChanged: {type:Signal, slotName:'onWhenChanged', args:[]},
    })

    __complete(){
        super.__complete()
        this.__update()
    }

    SLOT_delayedChanged(oldValue, newValue){
        this.__update()
    }

    SLOT_propertyChanged(oldValue, newValue){
        this.__update()
    }

    SLOT_restoreModeChanged(oldValue, newValue){
        this.__update()
    }

    SLOT_targetChanged(oldValue, newValue){
        this.__update()
    }

    SLOT_valueChanged(oldValue, newValue){
        this.__update()
    }

    SLOT_whenChanged(oldValue, newValue){
        this.__update()
    }  

    __update(){
        if(this.__getPropertyValue('target') && this.__getPropertyValue('property')){
            // if(this.$prop) this.getProperty('value').removeSubscriber(this.$prop)
            
            let path = this.__getPropertyValue('property').split('.')
            let propName = path.pop()

            let obj = this.__getPropertyValue('target')

            while(path.length){
                let name = path.shift()
                obj = obj.__getPropertyValue(name)
            }

            obj.__resetPropertyValue(propName, this.__getPropertyValue('value'))

            // this.$prop = prop
            // prop.subscribe(this.getProperty('value'))
            // prop.setCompute(()=>{return this.getPropertyValue('value')})
            // this.getProperty('value').update()
            // prop.update()
        }
    }
}



module.exports = Binding