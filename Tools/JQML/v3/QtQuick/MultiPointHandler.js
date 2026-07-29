const PointerDeviceHandler = require("../QtQuick/PointerDeviceHandler")
const Int = require("../QtQml/Int")
const Signal = require("../QtQml/Signal")
const Var = require("../QtQml/Var")


class MultiPointHandler extends PointerDeviceHandler {
    static meta = Object.assign({}, PointerDeviceHandler.meta, {
        minimumPointCount: { type: Int, value: 1 },
        maximumPointCount: { type: Int, value: 1 },
        centroid: { type: Var, value:null},

        minimumPointCountChanged: { type: Signal, args: [] },
        maximumPointCountChanged: { type: Signal, args: [] },
        centroidChanged: { type: Signal, args: [] },
    })

    __updateCentroidFromMouse(mouse){
        this.centroid = {
            x: mouse.x,
            y: mouse.y,
            sceneX: mouse.originX,
            sceneY: mouse.originY,
            pressedButtons: mouse.buttons,
            modifiers: mouse.modifiers,
        }
    }
}



module.exports = MultiPointHandler