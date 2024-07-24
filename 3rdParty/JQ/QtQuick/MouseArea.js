const Item = require("./Item")
const Int = require("../QtQml/Int")
const Real = require("../QtQml/Real")
const Bool = require("../QtQml/Bool")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")
const QtEnums = require("../Qt/enums")

class MouseArea extends Item {
    static meta = Object.assign({}, Item.meta, {
        acceptedButtons: { type: Int, value: QtEnums.LeftButton, signalName:'acceptedButtonsChanged' },
        containsMouse: { type: Bool, value: false, signalName:'containsMouseChanged' },
        containsPress: { type: Bool, value: false, signalName:'containsPressChanged' },
        hoverEnabled: { type: Bool, value: false, signalName:'hoverEnabledChanged' },
        propagateComposedEvents: { type: Bool, value: false, signalName:'propagateComposedEventsChanged' },
        preventStealing: { type: Bool, value: false, signalName:'preventStealingChanged' },
        // pressed: { type: Bool, value: false, signalName:'pressedChanged' },
        pressAndHoldInterval: { type: Real, value: 800, signalName:'pressAndHoldIntervalChanged' },
        mouseX: { type: Real, value: 0, signalName:'mouseXChanged' },
        mouseY: { type: Real, value: 0, signalName:'mouseYChanged' },
        cursorShape: { type: String, value: 'default', signalName:'cursorShapeChanged' },

        acceptedButtonsChanged: { type:Signal, slotName:'onAcceptedButtonsChanged', args:[] },
        containsMouseChanged: { type:Signal, slotName:'onContainsMouseChanged', args:[] },
        containsPressChanged: { type:Signal, slotName:'onContainsPressChanged', args:[] },
        hoverEnabledChanged: { type:Signal, slotName:'onHoverEnabledChanged', args:[] },
        propagateComposedEventsChanged: { type:Signal, slotName:'onPropagateComposedEventsChanged', args:[] },
        preventStealingChanged: { type:Signal, slotName:'onPreventStealingChanged', args:[] },
        // pressedChanged: { type:Signal, slotName:'onPressedChanged', args:[] },
        pressAndHoldIntervalChanged: { type:Signal, slotName:'onPressAndHoldIntervalChanged', args:[] },
        mouseXChanged: { type:Signal, slotName:'onMouseXChanged', args:[] },
        mouseYChanged: { type:Signal, slotName:'onMouseYChanged', args:[] },
        cursorShapeChanged: { type:Signal, slotName:'onCursorShapeChanged', args:[] },

        clicked: { type:Signal, slotName:'onClicked', args:[] },
		entered: { type:Signal, slotName:'onEntered', args:[] },
		exited: { type:Signal, slotName:'onExited', args:[] },
		canceled: { type:Signal, slotName:'onCanceled', args:[] },
		pressAndHold: { type:Signal, slotName:'onPressAndHold', args:[] },
		pressed: { type:Signal, slotName:'onPressed', args:[] },
		released: { type:Signal, slotName:'onReleased', args:[] },
		wheel: { type:Signal, slotName:'onWheel', args:[] },
		doubleClicked: { type:Signal, slotName:'onDoubleClicked', args:[] },
		positionChanged: { type:Signal, slotName:'onPositionChanged', args:[] },
    })

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        proxy.__DOM.classList.add('MouseArea')

        return proxy
    }

    mouse = {
        pressed: false,
        accepted: true,
        button: QtEnums.LeftButton,
        buttons: QtEnums.LeftButton,
        //flags: int,
        modifiers: 0,
        //source: int,
        wasHeld: false,
        x: 0,
        y: 0,
    }

    onMouseMove(chain, x, y, button){}
    onMouseDown(chain, x, y, button){
        this.pressed()
        this.mouse.pressed = true

        if(this.mouse.accepted){
            for(let obj of chain){
                if(obj instanceof MouseArea){
                    obj.mouse.pressed = false
                }
            }
        }

        chain.push(this)
        return this.mouse.accepted
    }
    onMouseUp(chain, x, y, button){
        if(this.mouse.pressed){
            this.released()
        }
    }
    onMouseClick(chain, x, y, button){
        if(this.mouse.pressed){
            this.clicked()
            this.mouse.pressed = false
        }
    }
    onMouseDblClick(chain, x, y, button){
        if(this.mouse.pressed){
            this.doubleClicked()
            this.mouse.pressed = false
        }
    }
    onWheel(chain, x, y, button){}
}

module.exports = MouseArea