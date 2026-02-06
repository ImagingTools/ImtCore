const MultiPointHandler = require("../QtQuick/MultiPointHandler")
const Signal = require("../QtQml/Signal")
const Int = require("../QtQml/Int")
const Var = require("../QtQml/Var")

class DragHandler extends MultiPointHandler {
    static NoSnap = 0
    static SnapAuto = 1
    static SnapWhenPressedOutsideTarget = 2
    static SnapAlways = 3

    static meta = Object.assign({}, MultiPointHandler.meta, {
        snapMode: { type: Int, value:DragHandler.SnapAuto},
        xAxix: { type: Var, value:null},
        yAxix: { type: Var, value:null},

        snapModeChanged: { type:Signal, args:[] },
        xAxixChanged: { type:Signal, args:[] },
        yAxixChanged: { type:Signal, args:[] },
    })
}



module.exports = DragHandler