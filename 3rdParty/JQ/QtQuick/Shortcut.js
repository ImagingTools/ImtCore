const QtObject = require("../QtQml/QtObject")
const QtEnums = require("../Qt/enums")
const Bool = require("../QtQml/Bool")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")

class Shortcut extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        context: { type: Real, value: QtEnums.WindowShortcut},
        autoRepeat: { type: Bool, value: true},
        enabled: { type: Bool, value: true},
        nativeText: { type: String, value: ''},
        portableText: { type: String, value: ''},
        sequence: { type: String, value: ''},
        sequences: { type: Var, value: undefined},

        contextChanged: { type:Signal, args:[] },
        autoRepeatChanged: { type:Signal, args:[] },
        enabledChanged: { type:Signal, args:[] },
        nativeTextChanged: { type:Signal, args:[] },
        portableTextChanged: { type:Signal, args:[] },
        sequenceChanged: { type:Signal, args:[] },
        sequencesChanged: { type:Signal, args:[] },

        activated: { type:Signal, args:[] },
        activatedAmbiguously: { type:Signal, args:[] },
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        JQApplication.KeyboardController.add(obj)

        return obj
    }

    __destroy(){
        JQApplication.KeyboardController.remove(this)
        super.__destroy()
    }
}



module.exports = Shortcut