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

		entered: { type:Signal, slotName:'onEntered', args:[] },
		exited: { type:Signal, slotName:'onExited', args:[] },
		canceled: { type:Signal, slotName:'onCanceled', args:[] },
		pressAndHold: { type:Signal, slotName:'onPressAndHold', args:['mouse'] },
		pressed: { type:Signal, slotName:'onPressed', args:['mouse'] },
		released: { type:Signal, slotName:'onReleased', args:['mouse'] },
		wheel: { type:Signal, slotName:'onWheel', args:['wheel'] },
        clicked: { type:Signal, slotName:'onClicked', args:['mouse'] },
		doubleClicked: { type:Signal, slotName:'onDoubleClicked', args:['mouse'] },
		positionChanged: { type:Signal, slotName:'onPositionChanged', args:['mouse'] },
    })

    static create(parent, ...args){
        let proxy = super.create(parent, ...args)
        proxy.__DOM.classList.add('MouseArea')

        return proxy
    }

    __pressed = false
    __entered = false
    __timer = null

    __onMouseCanceled(mouse){
        this.canceled()
    }

    __onMouseEnter(mouse){
        if((this.hoverEnabled && !mouse.target) || (mouse.target === this)){
            this.__entered = true
            this.entered(mouse)
            return true
        }
    }

    __onMouseLeave(mouse){
        if((this.hoverEnabled && !mouse.target) || (mouse.target === this)){
            this.__entered = false
            this.exited(mouse)
        }
    }

    __onMouseMove(mouse){
        if((this.hoverEnabled && !mouse.target) || (mouse.target === this && this.__pressed)){
            this.positionChanged(mouse)
        }
    }

    __onMouseDown(mouse){
        if(!mouse.target){
            this.__pressed = true
            if(!this.__entered) this.entered()
            this.pressed(mouse)

            if(mouse.accepted) mouse.target = this
        }
    }

    __onMouseUp(mouse){
        if(!mouse.target || mouse.target === this){
            this.released(mouse)
        }
    }

    __onMouseClick(mouse){
        if(mouse.target === this && this.__pressed){
            this.clicked(mouse)
        }

        this.__pressed = false
    }

    __onMouseDblClick(mouse){
        if(mouse.target === this && this.__pressed){
            this.doubleClicked(mouse)
        }

        this.__pressed = false
    }
}

module.exports = MouseArea