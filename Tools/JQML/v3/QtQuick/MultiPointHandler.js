const PointerDeviceHandler = require("../QtQuick/PointerDeviceHandler")
const Var = require("../QtQml/Var")


class MultiPointHandler extends PointerDeviceHandler {
    static meta = Object.assign({}, PointerDeviceHandler.meta, {
        centroid: { type: Var, value:null},
    })
}



module.exports = MultiPointHandler