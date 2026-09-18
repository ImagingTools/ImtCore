const MultiPointHandler = require("../QtQuick/MultiPointHandler")
const QtEnums = require("../Qt/enums")
const Signal = require("../QtQml/Signal")
const Int = require("../QtQml/Int")
const Var = require("../QtQml/Var")
const Axis = require("../QtQml/Axis")

class DragHandler extends MultiPointHandler {
    static NoSnap = 0
    static SnapAuto = 1
    static SnapWhenPressedOutsideTarget = 2
    static SnapAlways = 3

    static meta = Object.assign({}, MultiPointHandler.meta, {
        snapMode: { type: Int, value:DragHandler.SnapAuto},
        translation: { type: Var, value: { x: 0, y: 0 }},
        activeTranslation: { type: Var, value: { x: 0, y: 0 }},
        persistentTranslation: { type: Var, value: { x: 0, y: 0 }},
        xAxis: { type: Axis },
        yAxis: { type: Axis },

        snapModeChanged: { type:Signal, args:[] },
        translationChanged: { type:Signal, args:['delta'] },
        activeTranslationChanged: { type:Signal, args:[] },
        persistentTranslationChanged: { type:Signal, args:[] },
        xAxisChanged: { type:Signal, args:[] },
        yAxisChanged: { type:Signal, args:[] },
    })

    __dragStartX = 0
    __dragStartY = 0
    __targetStartX = 0
    __targetStartY = 0
    __dragging = false

    SLOT_activeTranslationChanged(oldValue, newValue){
        this.translation = newValue
    }

    __canStart(mouse){
        if(!this.__isHandlerEnabled()) return false
        if(!(mouse.button & this.acceptedButtons)) return false
        if(this.acceptedModifiers && mouse.modifiers && this.acceptedModifiers !== (this.acceptedModifiers & mouse.modifiers)) return false
        return true
    }

    __clampAxis(value, axis){
        if(!axis || axis.enabled === false) return null

        let min = Number(axis.minimum)
        let max = Number(axis.maximum)
        let result = Number(value)

        if(!Number.isFinite(result)) return null
        if(Number.isFinite(min) && result < min) result = min
        if(Number.isFinite(max) && result > max) result = max

        return result
    }

    __onMouseDown(mouse){
        if(!this.__canStart(mouse)) return
        if(mouse.target && mouse.target !== this.__getEffectiveTarget()) return

        let target = this.__getEffectiveTarget()

        this.__dragging = true
        this.active = true
        this.activeTranslation = { x: 0, y: 0 }

        if(this.xAxis) this.xAxis.activeValue = 0
        if(this.yAxis) this.yAxis.activeValue = 0

        mouse.target = target
    }

    __onMouseMove(mouse){
        if(!this.__dragging || !this.active) return

        let target = this.__getEffectiveTarget()
        if(target && target.__destroyed) return

        let deltaX = this.__clampAxis(-mouse.moveX, this.xAxis)
        let deltaY = this.__clampAxis(-mouse.moveY, this.yAxis)

        if(deltaX !== null){
            if(target) target.x += deltaX
            if(this.xAxis) this.xAxis.activeValue = deltaX
        }

        if(deltaY !== null){
            if(target) target.y += deltaY
            if(this.yAxis) this.yAxis.activeValue = deltaY
        }

        this.__updateCentroidFromMouse(mouse)
        this.activeTranslation = { x: deltaX || 0, y: deltaY || 0 }
        this.persistentTranslation = {
            x: (this.persistentTranslation && Number(this.persistentTranslation.x) || 0) + (mouse.moveX * -1),
            y: (this.persistentTranslation && Number(this.persistentTranslation.y) || 0) + (mouse.moveY * -1),
        }
    }

    __finishDrag(){
        this.__dragging = false
        this.active = false
        this.activeTranslation = { x: 0, y: 0 }
    }

    __onMouseUp(mouse){
        if(!this.__dragging) return
        this.__updateCentroidFromMouse(mouse)
        this.__finishDrag()
    }

    __onMouseCanceled(){
        if(!this.__dragging) return
        this.__finishDrag()
    }
}



module.exports = DragHandler