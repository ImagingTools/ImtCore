const QtObject = require("../QtQml/QtObject")
const QtEnums = require("../Qt/enums")
const Bool = require("../QtQml/Bool")
const Var = require("../QtQml/Var")
const Int = require("../QtQml/Int")
const Real = require("../QtQml/Real")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")

class PointerDevice extends QtObject {
    static Unknown = 0
    static Mouse = 1
    static TouchScreen = 2
    static TouchPad = 3
    static Stylus = 4
    static Airbrush = 5
    static Puck = 6
    static Keyboard = 7
    static AllDevices = 8

    static Generic = 0
    static Finger = 1
    static Pen = 2
    static Eraser = 3
    static Cursor = 4
    static AllPointerTypes = 5

    static meta = Object.assign({}, QtObject.meta, {
        buttonCount: { type: Int, value: 3},
        deviceType: { type: Int, value: PointerDevice.Unknown},
        maximumPoints: { type: Int, value: 1},
        acceptedPointerTypes: { type: Int, value: true},
        uniqueId: { type: Int, value: 0},


        buttonCountChanged: { type:Signal, args:[] },
        deviceTypeChanged: { type:Signal, args:[] },
        maximumPointsChanged: { type:Signal, args:[] },
        acceptedPointerTypesChanged: { type:Signal, args:[] },
        uniqueIdChanged: { type:Signal, args:[] },

        grabChanged: { type:Signal, args:['transition', 'point'] },
    })
}



module.exports = PointerDevice