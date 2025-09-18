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
        name: { type: String, value: ''},
        source: { type: String, value: ''},
        status: { type: Real, value: FontLoader.Null},

        nameChanged: { type:Signal, args:[] },
        sourceChanged: { type:Signal, args:[] },
        statusChanged: { type:Signal, args:[] },
    })
}



module.exports = FontLoader