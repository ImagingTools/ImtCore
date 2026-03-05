const PointerDeviceHandler = require("../QtQuick/PointerDeviceHandler")
const Signal = require("../QtQml/Signal")
const Var = require("../QtQml/Var")


class SinglePointHandler extends PointerDeviceHandler {
    static meta = Object.assign({}, PointerDeviceHandler.meta, {
        point: { type: Var, value:null},

        pointChanged: { type:Signal, args:[] },
    })
}



module.exports = SinglePointHandler