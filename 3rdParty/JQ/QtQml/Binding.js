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
        delayed: { type: Bool, value: false},
        property: { type: String, value: ''},
        restoreMode: { type: Var, value: Binding.RestoreBindingOrValue},
        target: { type: Var, value: undefined},
        value: { type: Var, value: undefined},
        when: { type: Bool, value: true},

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
        if(this.target && this.property){
           
            let path = this.property.split('.')
            let propName = path.pop()

            let obj = this.target

            while(path.length){
                let name = path.shift()
                obj = obj[name]
            }

            obj[propName] = this.value
        }
    }
}



module.exports = Binding