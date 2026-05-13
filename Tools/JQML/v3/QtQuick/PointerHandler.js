const QtObject = require("../QtQml/QtObject")
const QtEnums = require("../Qt/enums")
const Bool = require("../QtQml/Bool")
const Var = require("../QtQml/Var")
const Int = require("../QtQml/Int")
const Real = require("../QtQml/Real")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")

class PointerHandler extends QtObject {
    static TakeOverForbidden = 0
    static CanTakeOverFromHandlersOfSameType = 1
    static CanTakeOverFromHandlersOfDifferentType = 2
    static CanTakeOverFromItems = 3
    static CanTakeOverFromAnything = 4
    static ApprovesTakeOverByHandlersOfSameType = 5
    static ApprovesTakeOverByHandlersOfDifferentType = 6
    static ApprovesTakeOverByItems = 7
    static ApprovesCancellation = 8
    static ApprovesTakeOverByAnything = 9

    static meta = Object.assign({}, QtObject.meta, {
        active: { type: Bool, value: true},
        cursorShape: { type: String, value: QtEnums.ArrowCursor},
        enabled: { type: Bool, value: true},
        grabPermissions: { type: Int, value: PointerHandler.CanTakeOverFromItems | PointerHandler.CanTakeOverFromHandlersOfDifferentType | PointerHandler.ApprovesTakeOverByAnything },
        margin: { type: Real, value: 0},
        target: { type: Var, value: null},

        activeChanged: { type:Signal, args:[] },
        cursorShapeChanged: { type:Signal, args:[] },
        enabledChanged: { type:Signal, args:[] },
        grabPermissionsChanged: { type:Signal, args:[] },
        marginChanged: { type:Signal, args:[] },
        targetChanged: { type:Signal, args:[] },

        canceled: { type:Signal, args:['point'] },
        grabChanged: { type:Signal, args:['transition', 'point'] },
    })
}



module.exports = PointerHandler