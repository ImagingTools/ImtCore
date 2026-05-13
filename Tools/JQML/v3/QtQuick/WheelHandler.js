const SinglePointHandler = require("../QtQuick/SinglePointHandler")
const Signal = require("../QtQml/Signal")
const Int = require("../QtQml/Int")
const Bool = require("../QtQml/Bool")
const Var = require("../QtQml/Var")

class WheelHandler extends SinglePointHandler {
    static NoSnap = 0
    static SnapAuto = 1
    static SnapWhenPressedOutsideTarget = 2
    static SnapAlways = 3

    static meta = Object.assign({}, SinglePointHandler.meta, {
        blocking : { type: Int, value:true},
        
        blockingChanged: { type:Signal, args:[] },
    })
}



module.exports = WheelHandler