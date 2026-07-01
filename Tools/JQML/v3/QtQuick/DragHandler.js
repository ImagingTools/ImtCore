const MultiPointHandler = require("../QtQuick/MultiPointHandler")
const Signal = require("../QtQml/Signal")
const Int = require("../QtQml/Int")
const Var = require("../QtQml/Var")
const Bool = require("../QtQml/Bool")
const Real = require("../QtQml/Real")

class DragHandler extends MultiPointHandler {
    static NoSnap = 0
    static SnapAuto = 1
    static SnapWhenPressedOutsideTarget = 2
    static SnapAlways = 3

    static meta = Object.assign({}, MultiPointHandler.meta, {
        snapMode: { type: Int, value:DragHandler.SnapAuto},
        blocking: { type: Bool, value: false},
        activeTranslation: { type: Real, value: 0},
        orientation: { type: Real, value: 0},
        translation: { type: Var, value: undefined},
        persistentTranslation: { type: Var, value: undefined},
        xAxix: { type: Var, value:null},
        yAxix: { type: Var, value:null},

        snapModeChanged: { type:Signal, args:[] },
        blockingChanged: { type:Signal, args:[] },
        activeTranslationChanged: { type:Signal, args:[] },
        orientationChanged: { type:Signal, args:[] },
        translationChanged: { type:Signal, args:[] },
        persistentTranslationChanged: { type:Signal, args:[] },
        xAxixChanged: { type:Signal, args:[] },
        yAxixChanged: { type:Signal, args:[] },
    })
}



module.exports = DragHandler