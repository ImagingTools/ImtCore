const { Item } = require('./Item')
const { QSignal } = require('../utils/signal')
const { Qt } = require('../utils/Qt')
const { QReal, QBool, QString, QInt } = require('../utils/properties')

class MouseArea extends Item {
    static defaultProperties = {
        acceptedButtons: { type: QInt, value: Qt.LeftButton },
        containsMouse: { type: QBool, value: false },
        containsPress: { type: QBool, value: false },
        hoverEnabled: { type: QBool, value: false },
        propagateComposedEvents: { type: QBool, value: false },
        preventStealing: { type: QBool, value: false },
        pressed: { type: QBool, value: false },
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
		// pressed: { params: [] },
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
        this.setStyle({
            pointerEvents: 'auto',
            touchAction: 'auto',
        })
        MouseController.add(this)
    }

    availableButton(button){
		this.mouse.button = 0
		switch(button){
			case 0: this.mouse.button = Qt.LeftButton; break;
			case 1: this.mouse.button = Qt.MiddleButton; break;
			case 2: this.mouse.button = Qt.RightButton; break;
		}	
		if(this.getPropertyValue('acceptedButtons') & this.mouse.button) return true
		return false
	}

    onDoubleClick(x, y, button){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible') && this.availableButton(button)) {
            this.mouse.accepted = true
            let rect = this.getDom().getBoundingClientRect()
            let norm = this.normalizeXY(x - rect.x, y - rect.y)
            this.mouse.x = norm.x
            this.mouse.y = norm.y
            this.getStatement('mouseX').reset(norm.x)
            this.getStatement('mouseY').reset(norm.y)

            if(this.$signals && this.$signals.doubleClicked) this.$signals.doubleClicked()

            return this.mouse ? !this.mouse.accepted : true
        }
    }
    onMouseDown(x, y, button){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible') && this.availableButton(button)) {
            
            let rect = this.getDom().getBoundingClientRect()
            let norm = this.normalizeXY(x - rect.x, y - rect.y)
            this.mouse.x = norm.x
            this.mouse.y = norm.y
            this.getStatement('mouseX').reset(norm.x)
            this.getStatement('mouseY').reset(norm.y)
            this.mouse.accepted = false
            this.$entered = true

            if((this.$signals.clicked || this.$signals.doubleClicked) && !this.getPropertyValue('propagateComposedEvents')) this.mouse.accepted = true

            if(this.$signals && this.$signals.entered) {
                this.mouse.accepted = true
                this.$signals.entered()
            }

            
            this.getStatement('pressed').value = true
            if(this.$properties && this.$properties.pressed && this.$properties.pressed.notify) {
                this.mouse.accepted = true
                this.$properties.pressed.notify()
            }
            

            // if(this.$signals && this.$signals.pressAndHold){
            //     clearTimeout(this.$timer)
            //     this.$timer = setTimeout(()=>{
            //         this.mouse.accepted = true
            //         this.$signals.pressAndHold()
            //         pressAndHold = this.mouse.accepted ? -1 : 1
            //     }, this.getPropertyValue('pressAndHoldInterval'))
            // }
            
            return this.mouse ? !this.mouse.accepted : true
        }
    }
    onMouseUp(x, y, button){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible') && this.availableButton(button)) {
            this.getStatement('pressed').value = false
            let rect = this.getDom().getBoundingClientRect()
            let norm = this.normalizeXY(x - rect.x, y - rect.y)
            this.mouse.x = norm.x
            this.mouse.y = norm.y
            this.getStatement('mouseX').reset(norm.x)
            this.getStatement('mouseY').reset(norm.y)
            this.mouse.accepted = true
  
            if(this.$signals && this.$signals.released) this.$signals.released()
        
            if(this.$signals && this.$signals.clicked) this.$signals.clicked()

            if(this.$signals && this.$signals.exited) this.$signals.exited()
            
            delete this.$entered
            // if(this.$signals && this.$signals.pressAndHold){
            //     clearTimeout(this.$timer)
            //     delete this.$timer
            // }
            return this.mouse ? !this.mouse.accepted : true
        }
    }
    normalizeXY(x, y){
        let target = this
        let rotation = 0
        while(target && rotation === 0){
            rotation = target.getPropertyValue('rotation')
            target = target.getPropertyValue('parent')
        }
        // let rad = rotation * Math.PI / 180
        // let dx = Math.abs(rotation < 90) ? 1 : -1
        // let dy = (rotation >= 0 && rotation < 180) ? -1 : 1
        // return {x: Math.abs(x*Math.cos(rad)) + Math.abs(y*Math.sin(rad)), y: Math.abs(x*Math.sin(rad)) + Math.abs(y*Math.cos(rad))}

        switch(rotation){
            case 0: return {x: x, y: y}
            case -90: return {x: this.getPropertyValue('width') - y, y: x}
            case 90: return {x: y, y: this.getPropertyValue('height') - x}
        }
        return {x: x, y: y}

    }
    onMouseMove(x, y, pressed){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible')) {
            this.getStatement('pressed').value = pressed
            this.mouse.accepted = true
            let rect = this.getDom().getBoundingClientRect()
            let norm = this.normalizeXY(x - rect.x, y - rect.y)
            this.mouse.x = norm.x
            this.mouse.y = norm.y
            this.getStatement('mouseX').reset(norm.x)
            this.getStatement('mouseY').reset(norm.y)


            if((pressed || this.getPropertyValue('hoverEnabled')) && this.$signals.positionChanged) this.$signals.positionChanged()

            if(this.getPropertyValue('mouseX') >= 0 && this.getPropertyValue('mouseY') >= 0 && this.getPropertyValue('mouseX') < this.getPropertyValue('width') && this.getPropertyValue('mouseY') < this.getPropertyValue('height')){
                this.getPropertyValue('context').setStyle({
                    cursor: this.getPropertyValue('cursorShape')
                })
                this.getProperty('containsMouse').reset(true)
                if(this.$signals && this.$signals.entered && !this.$entered) {
                    this.$signals.entered()
                }
                this.$entered = true
            } else {
                this.getProperty('containsMouse').reset(false) 
                if(this.$signals && this.$signals.exited && this.$entered) {  
                    this.$signals.exited()
                }
                delete this.$entered
            }

            return this.$entered
        }
    }
    onWheel(x, y, deltaX, deltaY){
        if(this.getPropertyValue('enabled') && this.getPropertyValue('visible')) {
            if(this.$signals && this.$signals.wheel) {
                this.$signals.wheel.accepted = false
                let rect = this.getDom().getBoundingClientRect()
                this.$signals.wheel.x = x - rect.x
                this.$signals.wheel.y = y - rect.y
                this.$signals.wheel.angleDelta = {
                    x: deltaX / 8,
                    y: -deltaY / 8
                }
                this.$signals.wheel()

                // return false
            }
            return true
        } else {
            return true
        }
    }

    destroy(){
        clearTimeout(this.$timer)
        delete this.$timer
        MouseController.remove(this)
        super.destroy()
    }

}

module.exports.MouseArea = MouseArea