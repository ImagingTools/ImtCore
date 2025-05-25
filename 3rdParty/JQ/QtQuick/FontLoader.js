const QtObject = require("../QtQml/QtObject")
const Real = require("../QtQml/Real")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")

class FontLoader extends QtObject {
    static Null = 0
    static Loading = 1
    static Ready = 2
    static Error = 3

    static meta = Object.assign({}, QtObject.meta, {
        name: { type: String, value: '', signalName: 'nameChanged' },
        source: { type: String, value: '', signalName: 'sourceChanged' },
        status: { type: Real, value: FontLoader.Null, signalName: 'statusChanged' },

        nameChanged: { type:Signal, slotName:'onNameChanged', args:[] },
        sourceChanged: { type:Signal, slotName:'onSourceChanged', args:[] },
        statusChanged: { type:Signal, slotName:'onStatusChanged', args:[] },
    })
}



module.exports = FontLoader