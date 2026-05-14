const Control = require("./Control")
const Real = require("../../QtQml/Real")
const Int = require("../../QtQml/Int")
const Var = require("../../QtQml/Var")
const String = require("../../QtQml/String")
const Signal = require("../../QtQml/Signal")

class AbstractButton extends Control {
    static meta = Object.assign({}, Control.meta, {
        action: { type: Var },
        autoExclusive: { type: Var, value: false },
        autoRepeat: { type: Var, value: false },
        autoRepeatDelay: { type: Int, value: 300 },
        autoRepeatInterval: { type: Int, value: 100 },
        checkable: { type: Var, value: false },
        checked: { type: Var, value: false },
        display: { type: Var, value: 1 },
        down: { type: Var, value: false },
        icon: { type: Var },
        implicitIndicatorHeight: { type: Real, value: 0 },
        implicitIndicatorWidth: { type: Real, value: 0 },
        indicator: { type: Var },
        pressX: { type: Real, value: 0 },
        pressY: { type: Real, value: 0 },
        pressed: { type: Var, value: false },
        text: { type: String, value: '' },
        
        actionChanged: { type: Signal, args: [] },
        autoExclusiveChanged: { type: Signal, args: [] },
        autoRepeatChanged: { type: Signal, args: [] },
        autoRepeatDelayChanged: { type: Signal, args: [] },
        autoRepeatIntervalChanged: { type: Signal, args: [] },
        checkableChanged: { type: Signal, args: [] },
        checkedChanged: { type: Signal, args: [] },
        displayChanged: { type: Signal, args: [] },
        downChanged: { type: Signal, args: [] },
        iconChanged: { type: Signal, args: [] },
        implicitIndicatorHeightChanged: { type: Signal, args: [] },
        implicitIndicatorWidthChanged: { type: Signal, args: [] },
        indicatorChanged: { type: Signal, args: [] },
        pressXChanged: { type: Signal, args: [] },
        pressYChanged: { type: Signal, args: [] },
        pressedChanged: { type: Signal, args: [] },
        textChanged: { type: Signal, args: [] },
        
        canceled: { type: Signal, args: [] },
        clicked: { type: Signal, args: [] },
        doubleClicked: { type: Signal, args: [] },
        pressAndHold: { type: Signal, args: [] },
        pressed: { type: Signal, args: [] },
        released: { type: Signal, args: [] },
        toggled: { type: Signal, args: [] }
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('AbstractButton')

        return obj
    }

}



module.exports = AbstractButton