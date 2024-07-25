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
        entered: false,
        exited: false,
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

    __getRelativePoint(absX, absY){
        let rect = this.__DOM.getBoundingClientRect()
        return {
            x: absX - rect.x,
            y: absY - rect.y,
        }
    }

    __onMouseOver(chain, x, y){}

    __onMouseOut(chain, x, y){
        if(chain.length){
            if(!this.hoverEnabled){
                let point = this.__getRelativePoint(x, y)

                if(point.x < 0 || point.y < 0 || point.x >= this.width || point.y >= this.height){
                    if(!this.mouse.exited){
                        this.mouse.exited = true
                        this.exited()
                    }
                    this.mouse.entered = false
                }
            }
            
        } else {
            if(this.hoverEnabled){
                let point = this.__getRelativePoint(x, y)

                if(point.x < 0 || point.y < 0 || point.x >= this.width || point.y >= this.height){
                    if(!this.mouse.exited){
                        this.mouse.exited = true
                        this.exited()
                    }
                    this.mouse.entered = false
                }
            }
        }
    }
    __onMouseMove(chain, x, y){
        if(chain.length){
            if(chain.indexOf(this) >= 0 && this.mouse.pressed){
                let point = this.__getRelativePoint(x, y)
                this.mouse.x = point.x
                this.mouse.y = point.y
                this.mouseX = point.x
                this.mouseY = point.y

                if(point.x >= 0 && point.y >= 0 && point.x < this.width && point.y < this.height){
                    if(!this.mouse.entered){
                        this.mouse.entered = true
                        this.entered()
                    }
                    this.mouse.exited = false
                }

                this.positionChanged()
            }
        } else if(this.hoverEnabled){
            let point = this.__getRelativePoint(x, y)
            this.mouse.x = point.x
            this.mouse.y = point.y
            this.mouseX = point.x
            this.mouseY = point.y

            if(point.x >= 0 && point.y >= 0 && point.x < this.width && point.y < this.height){
                if(!this.mouse.entered){
                    this.mouse.entered = true
                    this.entered()
                }
                this.mouse.exited = false
            }

            this.positionChanged()
        }
    }
    __onMouseDown(chain, x, y, button){
        let point = this.__getRelativePoint(x, y)
        this.mouse.x = point.x
        this.mouse.y = point.y
        this.mouseX = point.x
        this.mouseY = point.y

        if(point.x >= 0 && point.y >= 0 && point.x < this.width && point.y < this.height){
            if(!this.mouse.entered){
                this.mouse.entered = true
                this.entered()
            }
            this.mouse.exited = false
        }

        this.pressed()
        this.mouse.pressed = true

        if(this.mouse.accepted){
            for(let obj of chain){
                if(obj instanceof MouseArea){
                    obj.mouse.pressed = false
                }
            }
            // let i = 0
            // while(i < chain.length){
            //     if(chain[i] instanceof MouseArea){
            //         chain[i].mouse.pressed = false
            //         chain.splice(i, 1)
            //     } else {
            //         i++
            //     }
            // }
        }

        chain.push(this)
        return this.mouse.accepted
    }
    __onMouseUp(chain, x, y, button){
        if(this.mouse.pressed){
            let point = this.__getRelativePoint(x, y)
            this.mouse.x = point.x
            this.mouse.y = point.y
            this.mouseX = point.x
            this.mouseY = point.y

            this.released()
        }
    }
    __onMouseClick(chain, x, y, button){
        let point = this.__getRelativePoint(x, y)

        if(this.mouse.pressed){
            this.mouse.x = point.x
            this.mouse.y = point.y
            this.mouseX = point.x
            this.mouseY = point.y

            this.clicked()
            this.mouse.pressed = false
        }

        if(this.hoverEnabled){
            if(point.x < 0 || point.y < 0 || point.x >= this.width || point.y >= this.height){
                if(!this.mouse.exited){
                    this.mouse.exited = true
                    this.exited()
                }
                this.mouse.entered = false
            }
        } else {
            if(!this.mouse.exited){
                this.mouse.exited = true
                this.exited()
            }
            this.mouse.entered = false
        }
        
    }
    __onMouseDblClick(chain, x, y, button){
        let point = this.__getRelativePoint(x, y)

        if(this.mouse.pressed){
            this.mouse.x = point.x
            this.mouse.y = point.y
            this.mouseX = point.x
            this.mouseY = point.y

            this.doubleClicked()
            this.mouse.pressed = false
        }

        if(this.hoverEnabled){
            if(point.x < 0 || point.y < 0 || point.x >= this.width || point.y >= this.height){
                if(!this.mouse.exited){
                    this.mouse.exited = true
                    this.exited()
                }
                this.mouse.entered = false
            }
        } else {
            if(!this.mouse.exited){
                this.mouse.exited = true
                this.exited()
            }
            this.mouse.entered = false
        }
    }
    __onWheel(chain, x, y){}
}

module.exports = MouseArea