const Item = require("./Item")
const Int = require("../QtQml/Int")
const Real = require("../QtQml/Real")
const Bool = require("../QtQml/Bool")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")
const QtEnums = require("../Qt/enums")
const JQApplication = require("../core/JQApplication")

class MouseArea extends Item {
    static meta = Object.assign({}, Item.meta, {
        acceptedButtons: { type: Int, value: QtEnums.LeftButton, },
        containsMouse: { type: Bool, value: false, },
        containsPress: { type: Bool, value: false, },
        hoverEnabled: { type: Bool, value: false, },
        propagateComposedEvents: { type: Bool, value: false, },
        preventStealing: { type: Bool, value: false, },
        pressed: { type: Bool, value: false, },
        pressAndHoldInterval: { type: Real, value: 800, },
        mouseX: { type: Real, value: 0, },
        mouseY: { type: Real, value: 0, },
        cursorShape: { type: String, value: QtEnums.ArrowCursor, },

        acceptedButtonsChanged: { type:Signal, args:[] },
        containsMouseChanged: { type:Signal, args:[] },
        containsPressChanged: { type:Signal, args:[] },
        hoverEnabledChanged: { type:Signal, args:[] },
        propagateComposedEventsChanged: { type:Signal, args:[] },
        preventStealingChanged: { type:Signal, args:[] },
        pressedChanged: { type:Signal, args:['mouse'] },
        pressAndHoldIntervalChanged: { type:Signal, args:[] },
        mouseXChanged: { type:Signal, args:[] },
        mouseYChanged: { type:Signal, args:[] },
        cursorShapeChanged: { type:Signal, args:[] },

		entered: { type:Signal, args:[] },
		exited: { type:Signal, args:[] },
		canceled: { type:Signal, args:[] },
		pressAndHold: { type:Signal, args:['mouse'] },
		// pressed: { type:Signal, args:['mouse'] },
		released: { type:Signal, args:['mouse'] },
		wheel: { type:Signal, args:['wheel'] },
        clicked: { type:Signal, args:['mouse'] },
		doubleClicked: { type:Signal, args:['mouse'] },
		positionChanged: { type:Signal, args:['mouse'] },
    })

    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.__DOM.classList.add('MouseArea')
        JQApplication.MouseController.add(obj)
        return obj
    }

    __pressed = false
    __entered = false
    __timer = null

    SLOT_cursorShapeChanged(oldValue, newValue){
        this.__setDOMStyle({
            cursor: this.cursorShape
        })
    }

    __onMouseCanceled(mouse){
        if(!this.enabled || !this.visible) return

        this.canceled()
    }

    __onMouseEnter(mouse){
        if(!this.enabled || !this.visible) return

        if((this.hoverEnabled && !mouse.target) || (mouse.target === this)){
            this.containsMouse = true
            this.__entered = true
            // JQApplication.setCursor(this.cursorShape) 
            this.entered(mouse)
            return true
        }
    }

    __onMouseLeave(mouse){
        if(!this.enabled || !this.visible) return

        if((this.hoverEnabled && !mouse.target) || (mouse.target === this)){
            this.containsMouse = false
            this.__entered = false
            this.exited(mouse)
        }
    }

    __onMouseMove(mouse){
        if(!this.enabled || !this.visible) return

        if((this.hoverEnabled && !mouse.target) || (mouse.target === this && this.__pressed)){
            this.mouseX = mouse.x
            this.mouseY = mouse.y
            // console.log(this)
            JQApplication.setCursor(this.cursorShape)
            this.positionChanged(mouse)
        }
    }

    __onMouseDown(mouse){
        if(!this.enabled || !this.visible || !(mouse.button & this.acceptedButtons)) return

        if(!mouse.target){
            this.__pressed = true
            this.__self.pressed = true
            if(!this.__entered) this.entered()
            this.mouseX = mouse.x
            this.mouseY = mouse.y
            this.pressedChanged(mouse)

            if(mouse.accepted) mouse.target = this
        }
    }

    __onMouseUp(mouse){
        if(!this.enabled || !this.visible || !(mouse.button & this.acceptedButtons)) return

        if(!mouse.target || mouse.target === this){
            this.mouseX = mouse.x
            this.mouseY = mouse.y
            this.released(mouse)
        }
    }

    __onMouseClick(mouse){
        if(!this.enabled || !this.visible || !(mouse.button & this.acceptedButtons)) return

        this.__self.pressed = false

        if(mouse.target === this && this.__pressed){
            this.clicked(mouse)
        }

        this.__pressed = false 
    }

    __onMouseDblClick(mouse){
        if(!this.enabled || !this.visible || !(mouse.button & this.acceptedButtons)) return

        this.__self.pressed = false

        if(mouse.target === this && this.__pressed){
            this.clicked(mouse)
            this.doubleClicked(mouse)
        }

        this.__pressed = false
    }

    __onWheel(wheel){
        if(!this.enabled || !this.visible) return

        if(!wheel.target){
            this.wheel(wheel)

            if(wheel.accepted) wheel.target = this
        }
    }

    __destroy(){
        JQApplication.MouseController.remove(this)
        super.__destroy()
    }
}



module.exports = MouseArea