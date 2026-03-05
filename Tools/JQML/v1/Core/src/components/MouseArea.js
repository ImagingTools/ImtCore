import {Item} from './Item'
import {Signal} from '../utils/Signal'

export class MouseArea extends Item {
    
    
    
    constructor(args) {
        super(args)

		this.$cS('clicked')
		this.$cS('entered')
		this.$cS('exited')
		this.$cS('canceled')
		this.$cS('pressAndHold')
		this.$cS('pressed')
		this.$cS('released')
		this.$cS('wheel')
		this.$cS('doubleClicked')
		this.$cS('positionChanged')

		this.mouse = {
			accepted: false,
			button: Qt.LeftButton,
			buttons: Qt.LeftButton,
			//flags: int,
			modifiers: 0,
			//source: int,
			wasHeld: false,
			x: 0,
			y: 0,
		}
		this.wheel.accepted = false
		this.wheel.buttons = Qt.LeftButton
		this.wheel.angleDelta = {
			x: 0,
			y: 0,
		}
		this.wheel.inverted = false
		this.wheel.pixelDelta = 0
		//this.wheel.//flags: int
		this.wheel.modifiers = 0
		this.wheel.x = 0
		this.wheel.y = 0
		

        this.$cP('acceptedButtons', Qt.LeftButton)
        this.$cP('containsMouse', false)
        this.$cP('hoverEnabled', false)
        // this.$cP('pressed', false)
		this.$cP('propagateComposedEvents', false)
		this.$cP('preventStealing', false)
        this.$cP('pressAndHoldInterval', 800)
        this.$cP('mouseX', 0)
        this.$cP('mouseY', 0)
        this.$cP('cursorShape', 'default', this.$cursorShapeChanged)

        
        this.tempMouse = {
			x: 0,
			y: 0,
		}
    }

    $domCreate(){
        super.$domCreate()
    }

	$cursorShapeChanged(){
		this.dom.style.cursor = this.cursorShape
	}

    $fillMouse(e){
		// this.mouse.accepted = false
		let rrr = this.dom.getBoundingClientRect()
		if(e.type.indexOf('touch') >= 0){
			this.mouse.x = e.changedTouches[0].pageX - rrr.x
			this.mouse.y = e.changedTouches[0].pageY - rrr.y
		} else {
			this.mouse.x = e.pageX - rrr.x
			this.mouse.y = e.pageY - rrr.y
		}
		
		this.mouseX = this.mouse.x
		this.mouseY = this.mouse.y
		this.mouse.modifiers = 0x00000000
		if(e.altKey) this.mouse.modifiers |= Qt.AltModifier
        if(e.shiftKey) this.mouse.modifiers |= Qt.ShiftModifier
        if(e.ctrlKey) this.mouse.modifiers |= Qt.ControlModifier
        if(e.metaKey) this.mouse.modifiers |= Qt.MetaModifier
		switch(e.button){
			case 0: this.mouse.button = Qt.LeftButton; this.mouse.buttons = Qt.LeftButton; break;
			case 1: this.mouse.button = Qt.MiddleButton; this.mouse.buttons = Qt.MiddleButton; break;
			case 2: this.mouse.button = Qt.RightButton; this.mouse.buttons = Qt.RightButton; break;
		}				
	}
	// $focusChanged(){
    //     super.$focusChanged()
    //     if(this.$p.focus.val) {
    //         this.dom.focus()
    //         Core.setFocus(this)
    //     } else {
    //         this.dom.blur()
    //     }
    // }
	$feelWheel(e){
		// this.wheel.accepted = false
		this.wheel.x = e.offsetX
		this.wheel.y = e.offsetY
		this.wheel.modifiers = 0x00000000
		this.wheel.angleDelta.y = e.wheelDeltaY / 8
        this.wheel.pixelDelta = e.wheelDeltaY
		if(e.altKey) this.wheel.modifiers |= Qt.AltModifier
        if(e.shiftKey) this.wheel.modifiers |= Qt.ShiftModifier
        if(e.ctrlKey) this.wheel.modifiers |= Qt.ControlModifier
        if(e.metaKey) this.wheel.modifiers |= Qt.MetaModifier				
	}
	availableButton(button){
		let btn = 0
		switch(button){
			case 0: btn = Qt.LeftButton; break;
			case 1: btn = Qt.MiddleButton; break;
			case 2: btn = Qt.RightButton; break;
		}	
		if((this.$p.acceptedButtons.val & button) || (this.$p.acceptedButtons.val & Qt.AllButtons)) return true
		return false
	}
	$mousedown(e, state) {
		e.preventDefault()
		if(this.$p.enabled.val){
			if(!this.mouse.accepted) state.blocked(this)
			if(this.availableButton(e.button)){
				this.$fillMouse(e)
				this.$pressed = true
				this.pressed()
			}

			this.tempMouse.x = this.mouse.x
			this.tempMouse.y = this.mouse.y

			if(!this.$p.hoverEnabled.val){
				this.containsMouse = true
				this.hover = true
			}

			if(this.$timerPressAndHold) clearTimeout(this.$timerPressAndHold)
			this.$timerPressAndHold = setTimeout(()=>{
				if(this.pressed && this.$s) this.pressAndHold()
			}, this.pressAndHoldInterval)
			
		}
	}
	$mouseup(e, state) {
		e.preventDefault()
		if(this.$p.enabled.val){
			if(!this.mouse.accepted) state.release()
			if(this.availableButton(e.button)){
				this.$fillMouse(e)
				this.$pressed = false
				this.released()

				let now = new Date().getTime()
				if(this.$lastClickOrTouch){
					if(now - this.$lastClickOrTouch > 250 || !this.doubleClicked.connections || Object.keys(this.doubleClicked.connections).length === 0){
						if(Core.focusedElement && (Core.focusedElement.$qmlClassName === 'TextInput' || Core.focusedElement.$qmlClassName === 'TextEdit')) Core.focusedElement.focus = false
						this.clicked();
						this.$lastClickOrTouch = now
					} else {
						this.doubleClicked();
						this.$lastClickOrTouch = now
					}
				} else {
					if(Core.focusedElement && (Core.focusedElement.$qmlClassName === 'TextInput' || Core.focusedElement.$qmlClassName === 'TextEdit')) Core.focusedElement.focus = false
					this.clicked();
					this.$lastClickOrTouch = now
				}
			}
			if(this.$p && !this.$p.hoverEnabled.val){
				this.containsMouse = false
				this.hover = false
			}
			if(this.$timerPressAndHold) clearTimeout(this.$timerPressAndHold)

			// console.log('CONSOLE::', this)
		}
	}
	$mousemove(e, state) {
		e.preventDefault()
		this.$mouseover(e, state)
		if(this.$p.enabled.val && (this.$pressed || this.$p.hoverEnabled.val)){
			this.$fillMouse(e)

			if(this.$pressed && state.map && this.parent === state.map && (Math.abs(this.mouse.x-this.tempMouse.x) > 10 || Math.abs(this.mouse.y-this.tempMouse.y) > 10) && !this.preventStealing){
				this.$pressed = false
				this.containsMouse = false
				this.hover = false
				this.exited()
				if(!this.mouse.accepted) state.release()
			} else if(this.$pressed && state.view && (Math.abs(this.mouse.x-this.tempMouse.x) > 10 || Math.abs(this.mouse.y-this.tempMouse.y) > 10) && !this.preventStealing){
				// console.log('CONSOLE::', this)
				this.$pressed = false
				this.containsMouse = false
				this.hover = false
				this.exited()
				if(!this.mouse.accepted) state.release()
				state.view.$mousedown(e, state)
				
			} else {
				this.positionChanged()
			}

			// console.log('CONSOLE::', state)
		}
		
	}
	$mousewheel(e, state) {
		e.preventDefault()
		this.$feelWheel(e)
		this.wheel()
		if(this.wheel.accepted) state.blocked(this)
	}
	$wheel(e, state) {
		e.preventDefault()
		this.$mousewheel(e, state)
	}
	$contextmenu(e, state) {
		e.preventDefault()
	}

	$mouseover(e, state){
		// e.preventDefault()
		if(this.$p.hoverEnabled.val && this.$p.enabled.val && !this.$p.containsMouse.val && (Core.root.eventState.target === null || Core.root.eventState.target === this)){
			
			this.$fillMouse(e)
			this.containsMouse = true
			this.hover = true
			this.entered()
		}
	}
	$mouseout(e, state){
		// e.preventDefault()
		// if(e.offsetX > this.width || e.offsetY > this.height || e.offsetX < 0 || e.offsetY < 0)
		if(this.$p.hoverEnabled.val && this.$p.enabled.val && this.$p.containsMouse.val && (Core.root.eventState.target === null || Core.root.eventState.target === this)){

			//this.$fillMouse(e)
			this.containsMouse = false
			this.hover = false
			this.exited()
		}
	}
	$touchstart(e, state) {
		e.preventDefault()
		if(this.$p.enabled.val && Core.velocityX === 0 && Core.velocityY === 0){
			if(!this.mouse.accepted) state.blocked(this)
			this.$fillMouse(e)
			this.$pressed = true
			this.pressed()

			this.tempMouse.x = this.mouse.x
			this.tempMouse.y = this.mouse.y

			if(!this.$p.hoverEnabled.val){
				this.containsMouse = true
				this.hover = true
			}

			if(this.$timerPressAndHold) clearTimeout(this.$timerPressAndHold)
			this.$timerPressAndHold = setTimeout(()=>{
				if(this.pressed && this.$s) this.pressAndHold()
			}, this.pressAndHoldInterval)
		}

	}
	$touchend(e, state) {
		e.preventDefault()
		if(this.$p.enabled.val){
			if(!this.mouse.accepted) state.release()
			this.$fillMouse(e)
			this.$pressed = false
			this.released()

			let now = new Date().getTime()
			if(this.$lastClickOrTouch){
				if(now - this.$lastClickOrTouch > 250 || !this.doubleClicked.connections || Object.keys(this.doubleClicked.connections).length === 0){
					if(Core.focusedElement && (Core.focusedElement.$qmlClassName === 'TextInput' || Core.focusedElement.$qmlClassName === 'TextEdit')) Core.focusedElement.focus = false
					this.clicked();
					this.$lastClickOrTouch = now
				} else {
					this.doubleClicked();
					this.$lastClickOrTouch = now
				}
			} else {
				if(Core.focusedElement && (Core.focusedElement.$qmlClassName === 'TextInput' || Core.focusedElement.$qmlClassName === 'TextEdit')) Core.focusedElement.focus = false
				this.clicked();
				this.$lastClickOrTouch = now
			}

			if(!this.$p.hoverEnabled.val){
				this.containsMouse = false
				this.hover = false
			}

			if(this.$timerPressAndHold) clearTimeout(this.$timerPressAndHold)
		}
	}
	$touchmove(e, state) {
		e.preventDefault()
		if(this.$p.enabled.val && (this.$pressed || this.$p.hoverEnabled.val)){
			this.$fillMouse(e)

			if(this.$pressed && state.map && this.parent === state.map && (Math.abs(this.mouse.x-this.tempMouse.x) > 10 || Math.abs(this.mouse.y-this.tempMouse.y) > 10) && !this.preventStealing){
				this.$pressed = false
				this.containsMouse = false
				this.hover = false
				this.exited()
				if(!this.mouse.accepted) state.release()
			} else if(this.$pressed && state.view && (Math.abs(this.mouse.x-this.tempMouse.x) > 10 || Math.abs(this.mouse.y-this.tempMouse.y) > 10) && !this.preventStealing){
				// console.log('CONSOLE::', this)
				this.$pressed = false
				this.containsMouse = false
				this.hover = false
				this.exited()
				if(!this.mouse.accepted) state.release()
				state.view.$touchstart(e, state)
				
			} else {
				this.positionChanged()
			}
		}
	}
}

QML.MouseArea = MouseArea