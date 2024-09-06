const QtObject = require("../QtQml/QtObject")
const QtEnums = require("../Qt/enums")
const Bool = require("../QtQml/Bool")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")

class Shortcut extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        context: { type: Real, value: QtEnums.WindowShortcut, signalName: 'contextChanged' },
        autoRepeat: { type: Bool, value: true, signalName: 'autoRepeatChanged' },
        enabled: { type: Bool, value: true, signalName: 'enabledChanged' },
        nativeText: { type: String, value: '', signalName: 'nativeTextChanged' },
        portableText: { type: String, value: '', signalName: 'portableTextChanged' },
        sequence: { type: String, value: '', signalName: 'sequenceChanged' },
        sequences: { type: Var, value: undefined, signalName: 'sequencesChanged' },

        contextChanged: { type:Signal, slotName:'onContextChanged', args:[] },
        autoRepeatChanged: { type:Signal, slotName:'onAutoRepeatChanged', args:[] },
        enabledChanged: { type:Signal, slotName:'onEnabledChanged', args:[] },
        nativeTextChanged: { type:Signal, slotName:'onNativeTextChanged', args:[] },
        portableTextChanged: { type:Signal, slotName:'onPortableTextChanged', args:[] },
        sequenceChanged: { type:Signal, slotName:'onSequenceChanged', args:[] },
        sequencesChanged: { type:Signal, slotName:'onSequencesChanged', args:[] },

        activated: { type:Signal, slotName:'onActivated', args:[] },
        activatedAmbiguously: { type:Signal, slotName:'onActivatedAmbiguously', args:[] },
    })

    static create(parent, model, properties=[], ...args){
        let proxy = super.create(parent, model, properties, ...args)
        let self = proxy.__self 

        return proxy
    }

 
}

module.exports = Shortcut