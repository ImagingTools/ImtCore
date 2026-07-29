const PointerDeviceHandler = require("../QtQuick/PointerDeviceHandler")
const Signal = require("../QtQml/Signal")
const Var = require("../QtQml/Var")


class SinglePointHandler extends PointerDeviceHandler {
    static meta = Object.assign({}, PointerDeviceHandler.meta, {
        point: { type: Var, value:null},

        pointChanged: { type:Signal, args:[] },
    })

    __updatePointFromMouse(mouse){
        this.point = {
            x: mouse.x,
            y: mouse.y,
            sceneX: mouse.originX,
            sceneY: mouse.originY,
            pressedButtons: mouse.buttons,
            modifiers: mouse.modifiers,
        }
    }
}



module.exports = SinglePointHandler