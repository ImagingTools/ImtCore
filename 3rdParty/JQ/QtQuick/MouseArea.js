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

    __mousePressed = false
    __mouseOver = false
    __timer = null

    __getMouseEventStructure(){
        return {
            accepted: true,
            button: QtEnums.LeftButton,
            buttons: QtEnums.LeftButton,
            modifiers: 0,
            wasHeld: false,
            x: 0,
            y: 0,
        }
    }

    __getWheelEventStructure(){
        return {
            accepted: false,
            angleDelta: {
                x: 0,
                y: 0,
            },
            x: 0,
            y: 0,
        }
    }

    __onEntered(){
        if(!this.__mouseOver && (this.hoverEnabled || this.__mousePressed)) {
            this.__mouseOver = true
            this.entered()
        }
    }

    __onExited(){
        if(this.__mouseOver) {
            this.__mouseOver = false
            this.exited()
        }
    }

    __onPositionChanged(x, y){
        let mouse = this.__getMouseEventStructure()
        mouse.x = x
        mouse.y = y
        this.mouseX = x
        this.mouseY = y

        if(this.hoverEnabled || this.__mousePressed) this.positionChanged(mouse)
    }

    __onPressed(x, y, button){
        this.__mousePressed = true
        this.__onEntered()

        let mouse = this.__getMouseEventStructure()
        mouse.x = x
        mouse.y = y
        this.mouseX = x
        this.mouseY = y

        this.pressed(mouse)
        return mouse.accepted
    }

    __onPressAndHold(x, y, button){
        clearTimeout(this.__timer)

        this.__timer = setTimeout(()=>{
            if(this.mouseX >= 0 && this.mouseY >= 0 && this.mouseX < this.width && this.mouseY < this.height){
                let mouse = this.__getMouseEventStructure()
                mouse.x = this.mouseX
                mouse.y = this.mouseY
                this.pressAndHold(mouse)
            }
        }, this.pressAndHoldInterval)
    }

    __onReleased(x, y, button){
        clearTimeout(this.__timer)
        this.__mousePressed = false

        let mouse = this.__getMouseEventStructure()
        mouse.x = x
        mouse.y = y
        this.mouseX = x
        this.mouseY = y

        this.released(mouse)
    }

    __onClick(x, y, button){
        let mouse = this.__getMouseEventStructure()
        mouse.x = x
        mouse.y = y
        this.mouseX = x
        this.mouseY = y

        this.clicked(mouse)
        if(this.propagateComposedEvents) return mouse.accepted
    }

    __onDblClick(x, y, button){
        let mouse = this.__getMouseEventStructure()
        mouse.x = x
        mouse.y = y
        this.mouseX = x
        this.mouseY = y

        this.doubleClicked(mouse)
        if(this.propagateComposedEvents) return mouse.accepted
    }

    __onWheel(x, y, deltaX, deltaY){
        let wheel = this.__getWheelEventStructure()
        wheel.x = x
        wheel.y = y
        wheel.angleDelta.x = deltaX
        wheel.angleDelta.y = deltaY

        this.wheel(wheel)

        return wheel.accepted
    }
}

module.exports = MouseArea