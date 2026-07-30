const SinglePointHandler = require("../QtQuick/SinglePointHandler")
const PointerDevice = require("../QtQuick/PointerDevice")
const QtEnums = require("../Qt/enums")
const Signal = require("../QtQml/Signal")
const Int = require("../QtQml/Int")
const Bool = require("../QtQml/Bool")
const Real = require("../QtQml/Real")
const String = require("../QtQml/String")
const Var = require("../QtQml/Var")

class WheelHandler extends SinglePointHandler {
    static meta = Object.assign({}, SinglePointHandler.meta, {
        acceptedDevices: { type: Int, value: PointerDevice.Mouse },
        activeTimeout: { type: Real, value: 0.1 },
        blocking: { type: Bool, value: true },
        invertible: { type: Bool, value: true },
        orientation: { type: Int, value: QtEnums.Vertical },
        property: { type: String, value: '' },
        rotation: { type: Real, value: 0 },
        rotationScale: { type: Real, value: 1 },
        targetScaleMultiplier: { type: Real, value: 1 },
        targetTransformAroundCursor: { type: Bool, value: true },

        acceptedDevicesChanged: { type: Signal, args: [] },
        activeTimeoutChanged: { type: Signal, args: [] },
        blockingChanged: { type:Signal, args:[] },
        invertibleChanged: { type: Signal, args: [] },
        orientationChanged: { type: Signal, args: [] },
        propertyChanged: { type: Signal, args: [] },
        rotationChanged: { type: Signal, args: [] },
        rotationScaleChanged: { type: Signal, args: [] },
        targetScaleMultiplierChanged: { type: Signal, args: [] },
        targetTransformAroundCursorChanged: { type: Signal, args: [] },

        wheel: { type: Signal, args: ['wheel'] },
    })

    __onWheel(wheelEvent){
        if(!this.__isHandlerEnabled()) return

        let target = this.__getEffectiveTarget()
        if(!target) return

        let delta = this.orientation === QtEnums.Horizontal ? wheelEvent.angleDelta.x : wheelEvent.angleDelta.y
        if(!Number.isFinite(delta) || delta === 0) return

        let signedDelta = this.invertible ? -delta : delta
        let rotationStep = signedDelta * Number(this.rotationScale || 1)
        this.rotation += rotationStep

        this.__updatePointFromMouse({
            x: wheelEvent.x,
            y: wheelEvent.y,
            originX: wheelEvent.originX,
            originY: wheelEvent.originY,
            buttons: 0,
            modifiers: wheelEvent.modifiers || 0,
        })

        if(this.property && this.property in target){
            let currentValue = Number(target[this.property])
            if(Number.isFinite(currentValue)){
                target[this.property] = currentValue + rotationStep
            }
        } else if('scale' in target){
            let currentScale = Number(target.scale)
            let multiplier = Number(this.targetScaleMultiplier)
            if(Number.isFinite(currentScale) && Number.isFinite(multiplier)){
                target.scale = currentScale + rotationStep * multiplier
            }
        }

        this.wheel(wheelEvent)

        if(this.blocking){
            wheelEvent.accepted = true
            if(!wheelEvent.target) wheelEvent.target = target
        }
    }
}



module.exports = WheelHandler