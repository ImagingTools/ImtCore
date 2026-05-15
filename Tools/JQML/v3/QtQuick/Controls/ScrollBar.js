const Control = require("./Control")
const Real = require("../../QtQml/Real")
const Var = require("../../QtQml/Var")
const Int = require("../../QtQml/Int")
const Signal = require("../../QtQml/Signal")

class ScrollBar extends Control {
    static meta = Object.assign({}, Control.meta, {
        active: { type: Var, value: false },
        horizontal: { type: Var, value: false },
        interactive: { type: Var, value: true },
        minimumSize: { type: Real, value: 0 },
        orientation: { type: Int, value: 1 },
        policy: { type: Int, value: 0 },
        position: { type: Real, value: 0 },
        pressed: { type: Var, value: false },
        size: { type: Real, value: 0 },
        snapMode: { type: Int, value: 0 },
        stepSize: { type: Real, value: 0 },
        vertical: { type: Var, value: true },
        visualPosition: { type: Real, value: 0 },
        visualSize: { type: Real, value: 0 },
        
        activeChanged: { type: Signal, args: [] },
        horizontalChanged: { type: Signal, args: [] },
        interactiveChanged: { type: Signal, args: [] },
        minimumSizeChanged: { type: Signal, args: [] },
        orientationChanged: { type: Signal, args: [] },
        policyChanged: { type: Signal, args: [] },
        positionChanged: { type: Signal, args: [] },
        pressedChanged: { type: Signal, args: [] },
        sizeChanged: { type: Signal, args: [] },
        snapModeChanged: { type: Signal, args: [] },
        stepSizeChanged: { type: Signal, args: [] },
        verticalChanged: { type: Signal, args: [] },
        visualPositionChanged: { type: Signal, args: [] },
        visualSizeChanged: { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('ScrollBar')

        return obj
    }

}



module.exports = ScrollBar