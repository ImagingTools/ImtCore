const PointerHandler = require("../QtQuick/PointerHandler")
const PointerDevice = require("../QtQuick/PointerDevice")
const QtEnums = require("../Qt/enums")
const Bool = require("../QtQml/Bool")
const Var = require("../QtQml/Var")
const Int = require("../QtQml/Int")
const Real = require("../QtQml/Real")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")

class PointerDeviceHandler extends PointerHandler {
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

    static meta = Object.assign({}, PointerHandler.meta, {
        acceptedButtons: { type: Int, value: QtEnums.LeftButton},
        acceptedDevices: { type: Int, value: PointerDevice.AllDevices},
        acceptedModifiers: { type: Int, value: QtEnums.KeyboardModifierMask},
        acceptedPointerTypes: { type: Int, value: PointerDevice.AllPointerTypes},
        dragThreshold: { type: Int, value: 15},

        acceptedButtonsChanged: { type:Signal, args:[] },
        acceptedDevicesChanged: { type:Signal, args:[] },
        acceptedModifiersChanged: { type:Signal, args:[] },
        acceptedPointerTypesChanged: { type:Signal, args:[] },
        dragThresholdChanged: { type:Signal, args:[] },

        canceled: { type:Signal, args:['point'] },
        grabChanged: { type:Signal, args:['transition', 'point'] },
    })
}



module.exports = PointerDeviceHandler