const QtObject = require("./QtObject")
const Bool = require("./Bool")
const String = require("./String")
const Var = require("./Var")
const Signal = require("./Signal")

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

    onDelayedChanged(){
        this.__update()
    }

    onPropertyChanged(){
        this.__update()
    }

    onRestoreModeChanged(){
        this.__update()
    }

    onTargetChanged(){
        this.__update()
    }

    onValueChanged(){
        this.__update()
    }

    onWhenChanged(){
        this.__update()
    }  

    __update(){
        if(this.target && this.property){
            let path = this.property.split('.')
            let prop = this.target

            while(path.length-1){
                let propName = path.shift()
                prop = prop[propName]
            }

            prop[path.shift()] = this.value
        }
    }
}

module.exports = Binding