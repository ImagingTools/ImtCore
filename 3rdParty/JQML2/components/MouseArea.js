const { Item } = require('./Item')
const { QSignal } = require('../utils/signal')
const { Qt } = require('../utils/Qt')
const { QReal, QBool, QString, QInt } = require('../utils/properties')

class MouseArea extends Item {
    static defaultProperties = {
        acceptedButtons: { type: QInt, value: Qt.LeftButton },
        containsMouse: { type: QBool, value: false },
        hoverEnabled: { type: QBool, value: false },
        propagateComposedEvents: { type: QBool, value: false },
        preventStealing: { type: QBool, value: false },
        pressAndHoldInterval: { type: QReal, value: 800 },
        mouseX: { type: QReal, value: 0 },
        mouseY: { type: QReal, value: 0 },
        cursorShape: { type: QString, value: 'default' },

    }

    static defaultSignals = {
        clicked: { params: [] },
		entered: { params: [] },
		exited: { params: [] },
		canceled: { params: [] },
		pressAndHold: { params: [] },
		pressed: { params: [] },
		released: { params: [] },
		wheel: { params: [] },
		doubleClicked: { params: [] },
		positionChanged: { params: [] },
    }

    mouse = {
        accepted: true,//new QBool(true),
        button: Qt.LeftButton,
        buttons: Qt.LeftButton,
        //flags: int,
        modifiers: 0,
        //source: int,
        wasHeld: false,
        x: 0,
        y: 0,
    }

    $mousearea = true

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        MouseController.add(this)
    }

    onDoubleClick(x, y, button){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible')) {
            this.mouse.accepted = true
            let rect = this.getDom().getBoundingClientRect()
            this.mouse.x = x - rect.x
            this.mouse.y = y - rect.y
            this.getStatement('mouseX').reset(x - rect.x)
            this.getStatement('mouseX').reset(y - rect.y)

            if(this.$signals.doubleClicked) this.$signals.doubleClicked()

            if(!this.mouse.accepted) MouseController.stopPropogation(this)
        }
    }
    onMouseDown(x, y){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible')) {
            let rect = this.getDom().getBoundingClientRect()
            this.mouse.x = x - rect.x
            this.mouse.y = y - rect.y
            this.getStatement('mouseX').reset(x - rect.x)
            this.getStatement('mouseX').reset(y - rect.y)
            this.mouse.accepted = true
            this.$entered = true

            if(this.$signals.entered) this.$signals.entered()

            this.mouse.accepted = true
            if(this.$signals.pressed) this.$signals.pressed()
            

            // if(this.$signals.pressAndHold){
            //     clearTimeout(this.$timer)
            //     this.$timer = setTimeout(()=>{
            //         this.mouse.accepted = true
            //         this.$signals.pressAndHold()
            //         pressAndHold = this.mouse.accepted ? -1 : 1
            //     }, this.getPropertyValue('pressAndHoldInterval'))
            // }
            
            return !this.mouse.accepted
        }
    }
    onMouseUp(x, y){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible')) {
            let rect = this.getDom().getBoundingClientRect()
            this.mouse.x = x - rect.x
            this.mouse.y = y - rect.y
            this.getStatement('mouseX').reset(x - rect.x)
            this.getStatement('mouseX').reset(y - rect.y)
            this.mouse.accepted = true
  
            if(this.$signals.released) this.$signals.released()
        
            if(this.$signals.clicked) this.$signals.clicked()

            if(this.$signals.exited) this.$signals.exited()
            
            delete this.$entered
            // if(this.$signals.pressAndHold){
            //     clearTimeout(this.$timer)
            //     delete this.$timer
            // }
            return !this.mouse.accepted
        }
    }
    onMouseMove(x, y, pressed){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible')) {
            this.mouse.accepted = true
            let rect = this.getDom().getBoundingClientRect()
            this.mouse.x = x - rect.x
            this.mouse.y = y - rect.y
            this.getStatement('mouseX').reset(x - rect.x)
            this.getStatement('mouseY').reset(y - rect.y)


            if(pressed && this.$signals.positionChanged) this.$signals.positionChanged()

            if(this.getPropertyValue('mouseX') >= 0 && this.getPropertyValue('mouseY') >= 0 && this.getPropertyValue('mouseX') <= this.getPropertyValue('width') && this.getPropertyValue('mouseY') <= this.getPropertyValue('height')){
                this.getPropertyValue('context').setStyle({
                    cursor: this.getPropertyValue('cursorShape')
                })
                this.getProperty('containsMouse').reset(true)
                if(this.$signals.entered && !this.$entered) {
                    this.$signals.entered()
                }
                this.$entered = true
            } else {
                this.getProperty('containsMouse').reset(false) 
                if(this.$signals.exited && this.$entered) {  
                    this.$signals.exited()
                }
                delete this.$entered
            }

            return this.$entered
        }
    }
    onWheel(x, y, deltaX, deltaY){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible')) {
            if(this.$signals.wheel) {
                this.$signals.wheel.accepted = false
                let rect = this.getDom().getBoundingClientRect()
                this.$signals.wheel.x = x - rect.x
                this.$signals.wheel.y = y - rect.y
                this.$signals.wheel.angleDelta = {
                    x: deltaX / 8,
                    y: deltaY / 8
                }
                this.$signals.wheel()

                // return false
            }
            return true
        } else {
            return true
        }
    }

    $destroy(){
        clearTimeout(this.$timer)
        delete this.$timer
        MouseController.remove(this)
        super.$destroy()
    }
}

module.exports.MouseArea = MouseArea