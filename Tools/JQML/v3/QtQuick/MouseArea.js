const Item = require("./Item")
const Int = require("../QtQml/Int")
const Real = require("../QtQml/Real")
const Bool = require("../QtQml/Bool")
const String = require("../QtQml/String")
const Drag = require("../QtQml/Drag")
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
        drag: { type: Drag },

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
        dragChanged: { type:Signal, args:[] },

		entered: { type:Signal, args:[] },
		exited: { type:Signal, args:[] },
		canceled: { type:Signal, args:[] },
		pressAndHold: { type:Signal, args:['mouse'] },
		// Qt's pressed(mouse): the property 'pressed' takes the name, so onPressed is compiled to this signal
		pressedEvent: { type:Signal, args:['mouse'] },
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
    __keepMouseGrab = false
    __longPress = false
    __doubleClick = false
    __holdTimer = null

    SLOT_cursorShapeChanged(oldValue, newValue){
        this.__setDOMStyle({
            cursor: this.cursorShape
        })
    }

    SLOT_visibleChanged(oldValue, newValue){
        super.SLOT_visibleChanged(oldValue, newValue)

        // Qt: an area hidden while pressed is no longer the grabber
        if(!this.visible && this.__pressed) this.__ungrab(true)
    }

    __acceptsHover(){
        return this.hoverEnabled && this.enabled && this.visible
    }

    __hasHandler(name){
        let self = this.__self
        if(typeof self['SLOT_' + name] === 'function') return true
        let list = self.__connections[name]
        return !!(list && list.length)
    }

    // Qt emits pressedChanged and containsPressChanged without comparing the values
    __emitPressedChanged(mouse){
        this.__self.pressed = this.__pressed
        this.pressedChanged(mouse)
        this.__self.containsPress = this.__pressed && this.containsMouse
        this.containsPressChanged()
    }

    // Qt: QQuickMouseArea::setHovered
    __setHovered(hovered){
        if(this.containsMouse === hovered) return

        this.__self.containsMouse = hovered
        this.containsMouseChanged()
        if(hovered) this.entered()
        else this.exited()
        if(this.__pressed){
            this.__self.containsPress = this.__pressed && this.containsMouse
            this.containsPressChanged()
        }
    }

    __setPosition(mouse){
        this.mouseX = mouse.x
        this.mouseY = mouse.y
    }

    __contains(mouse){
        return mouse.x >= 0 && mouse.y >= 0 && mouse.x < this.width && mouse.y < this.height
    }

    __isDragActive(){
        let drag = this.drag
        return !!(drag && typeof drag === 'object' && drag.active)
    }

    __stopHoldTimer(){
        if(this.__holdTimer !== null){
            clearTimeout(this.__holdTimer)
            this.__holdTimer = null
        }
    }

    // Qt: QQuickMouseArea::timerEvent
    __startHoldTimer(mouse){
        this.__stopHoldTimer()
        let button = mouse.button
        let buttons = mouse.buttons
        let modifiers = mouse.modifiers
        this.__holdTimer = setTimeout(()=>{
            this.__holdTimer = null
            if(this.__destroyed || !this.__pressed || this.__isDragActive() || !this.containsMouse) return

            this.__longPress = true
            let event = {
                x: this.mouseX,
                y: this.mouseY,
                button: button,
                buttons: buttons,
                modifiers: modifiers,
                wasHeld: true,
                isClick: false,
                accepted: this.__hasHandler('pressAndHold'),
            }
            this.pressAndHold(event)
            if(!event.accepted) JQApplication.MouseController.__propagateComposedEvent(event, this, 'pressAndHold')
            // Nobody handled the long press: the click is allowed
            if(!event.accepted) this.__longPress = false
        }, this.pressAndHoldInterval)
    }

    // Qt: QQuickMouseArea::ungrabMouse
    __ungrab(lostHover){
        this.__stopHoldTimer()
        if(!this.__pressed) return

        this.__pressed = false
        this.__self.pressed = false
        this.__doubleClick = false
        this.canceled()
        this.__emitPressedChanged()

        if(lostHover && this.containsMouse){
            this.__self.containsMouse = false
            this.containsMouseChanged()
        }
    }

    __onMouseCanceled(mouse){
        this.__ungrab(true)
    }

    // Hover without a press, as QQuickMouseArea::hoverEnterEvent
    __onMouseEnter(mouse){
        if(this.__pressed || mouse.target || !this.__acceptsHover()) return

        this.__setPosition(mouse)
        this.__setHovered(true)
        return true
    }

    __onMouseLeave(mouse){
        if(this.__pressed) return

        this.__setHovered(false)
    }

    __onMouseMove(mouse){
        if(mouse.target === this && this.__pressed){
            // Qt: QQuickMouseArea::mouseMoveEvent
            this.__setHovered(this.__contains(mouse))
            JQApplication.MouseController.__updateMouseAreaDrag(this, mouse)
            this.__setPosition(mouse)
            JQApplication.setCursor(this.cursorShape)
            this.positionChanged(mouse)
            return
        }

        // Qt: QQuickMouseArea::hoverMoveEvent emits only when the position changes
        if(mouse.target || !this.__acceptsHover() || !this.containsMouse) return
        JQApplication.setCursor(this.cursorShape)
        if(this.mouseX === mouse.x && this.mouseY === mouse.y) return

        this.__setPosition(mouse)
        this.positionChanged(mouse)
    }

    // Qt: QQuickMouseArea::mousePressEvent and setPressed(true)
    __onMouseDown(mouse){
        if(mouse.target || !this.enabled || !this.visible || !(mouse.button & this.acceptedButtons)) return

        this.__longPress = false
        this.__setPosition(mouse)
        this.__setHovered(true)

        let wasPressed = this.__pressed
        mouse.accepted = true
        mouse.wasHeld = false
        mouse.isClick = false
        this.__pressed = true
        this.__self.pressed = true
        if(!this.__doubleClick) this.pressedEvent(mouse)

        if(!mouse.accepted){
            this.__pressed = false
            this.__self.pressed = false
            if(!this.hoverEnabled) this.__setHovered(false)
        }

        if(!wasPressed) this.__emitPressedChanged(mouse)

        if(mouse.accepted){
            mouse.target = this
            JQApplication.MouseController.__startMouseAreaDrag(this, mouse)
            this.__startHoldTimer(mouse)
        }
    }

    // Qt: QQuickMouseArea::mouseDoubleClickEvent, delivered after the second press
    __onMouseDoubleClick(mouse){
        if(!this.enabled) return

        let connected = this.__hasHandler('doubleClicked')
        mouse.accepted = connected
        mouse.isClick = true
        mouse.wasHeld = false
        this.doubleClicked(mouse)
        if(!mouse.accepted) JQApplication.MouseController.__propagateComposedEvent(mouse, this, 'dblclick')
        if(this.__pressed) this.__doubleClick = connected || mouse.accepted
    }

    // Qt: QQuickMouseArea::mouseReleaseEvent and setPressed(false)
    __onMouseUp(mouse){
        this.__stopHoldTimer()
        if(!this.__pressed){
            this.__doubleClick = false
            return
        }

        let isClick = !this.__isDragActive() && this.containsMouse
        this.__pressed = false
        this.__setPosition(mouse)
        mouse.isClick = isClick
        mouse.wasHeld = this.__longPress
        mouse.accepted = true
        this.__self.pressed = false
        this.released(mouse)
        this.__emitPressedChanged(mouse)

        if(isClick && !this.__longPress && !this.__doubleClick){
            mouse.accepted = this.__hasHandler('clicked')
            this.clicked(mouse)
            if(!mouse.accepted) JQApplication.MouseController.__propagateComposedEvent(mouse, this, 'click')
        }

        if(!this.hoverEnabled) this.__setHovered(false)
        this.__doubleClick = false
    }

    // An area under the one that rejected a click, double click or press and hold (propagateComposedEvents)
    __emitComposed(mouse, signalName){
        if(!this.enabled || !this.visible || !(mouse.button & this.acceptedButtons) || !this.__hasHandler(signalName) || !this.__contains(mouse)) return false

        // It is connected: the handler has to reject it explicitly to let it go further
        mouse.accepted = true
        this[signalName](mouse)
        return true
    }

    __onWheel(wheel){
        if(!this.enabled || !this.visible) return

        if(!wheel.target){
            this.wheel(wheel)

            if(wheel.accepted) wheel.target = this
        }
    }

    __destroy(){
        this.__stopHoldTimer()
        JQApplication.MouseController.remove(this)
        super.__destroy()
    }
}



module.exports = MouseArea