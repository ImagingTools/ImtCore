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
}

module.exports = Binding