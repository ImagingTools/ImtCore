import {Item} from './Item'
import {Signal} from '../utils/Signal'

export class MouseArea extends Item {
    
    mouse = {
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
    wheel = {
        accepted: false,
        buttons: Qt.LeftButton,
        angleDelta: 0,
        inverted: false,
        pixelDelta: 0,
        //flags: int,
        modifiers: 0,
        x: 0,
        y: 0,
    }
    constructor(args) {
        super(args)

        this.$s.clicked = Signal()
        this.$s.entered = Signal()
        this.$s.exited = Signal()
        this.$s.canceled = Signal()
        this.$s.pressAndHold = Signal()
        this.$s.pressed = Signal()
        this.$s.released = Signal()
        this.$s.wheel = Signal()
        this.$s.doubleClicked = Signal()
        this.$s.positionChanged = Signal()

		this.clicked = this.$s.clicked
		this.entered = this.$s.entered
		this.exited = this.$s.exited
		this.canceled = this.$s.canceled
		this.pressAndHold  = this.$s.pressAndHold 
		// this.pressed = this.$s.pressed
		this.released = this.$s.released
		this.wheel = this.$s.wheel
		this.doubleClicked = this.$s.doubleClicked
		this.positionChanged = this.$s.positionChanged

        this.$cP('acceptedButtons', Qt.LeftButton)
        this.$cP('containsMouse', false)
        this.$cP('hoverEnabled', false)
        this.$cP('pressed', false)
		this.$cP('propagateComposedEvents', false)
		this.$cP('preventStealing', false)
        this.$cP('pressAndHoldInterval', 800)
        this.$cP('mouseX', 0)
        this.$cP('mouseY', 0)
        this.$cP('cursorShape', 'default').connect(this.$cursorShapeChanged.bind(this))

        
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
	$feelWheel(e){
		// this.wheel.accepted = false
		this.wheel.x = e.offsetX
		this.wheel.y = e.offsetY
		this.wheel.modifiers = 0x00000000
		this.wheel.angleDelta = e.wheelDeltaY / 8
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
				this.pressed = true
				this.$s.pressed()
			}

			this.tempMouse.x = this.mouse.x
			this.tempMouse.y = this.mouse.y

			if(!this.$p.hoverEnabled.val){
				this.containsMouse = true
				this.hover = true
			}

			if(this.$timerPressAndHold) clearTimeout(this.$timerPressAndHold)
			this.$timerPressAndHold = setTimeout(()=>{
				if(this.$s) this.$s.pressAndHold()
			}, this.pressAndHoldInterval)
			
		}
	}
	$mouseup(e, state) {
		e.preventDefault()
		if(this.$p.enabled.val){
			if(!this.mouse.accepted) state.release()
			if(this.availableButton(e.button)){
				this.$fillMouse(e)
				this.pressed = false
				this.$s.released()

				let now = new Date().getTime()
				if(this.$lastClickOrTouch){
					if(now - this.$lastClickOrTouch > 250 || Object.keys(this.$s.doubleClicked.connections).length === 0){
						this.$s.clicked();
						this.$lastClickOrTouch = now
					} else {
						this.$s.doubleClicked();
						this.$lastClickOrTouch = now
					}
				} else {
					this.$s.clicked();
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
		if(this.$p.enabled.val && (this.$p.pressed.val || this.$p.hoverEnabled.val)){
			this.$fillMouse(e)

			if(this.$p.pressed.val && state.view && (Math.abs(this.mouse.x-this.tempMouse.x) > 10 || Math.abs(this.mouse.y-this.tempMouse.y) > 10)){
				// console.log('CONSOLE::', this)
				this.pressed = false
				this.containsMouse = false
				this.hover = false
				this.$s.exited()
				if(!this.mouse.accepted) state.release()
				state.view.$mousedown(e, state)
				
			} else {
				this.$s.positionChanged()
			}

			// console.log('CONSOLE::', state)
		}
		
	}
	$mousewheel(e, state) {
		e.preventDefault()
		this.$feelWheel(e)
	}
	$wheel(e, state) {
		e.preventDefault()
		this.$mousewheel(e, state)
	}
	$contextmenu(e, state) {
		e.preventDefault()
	}

	$mouseover(e, state){
		e.preventDefault()
		if(this.$p.hoverEnabled.val && this.$p.enabled.val && !this.$p.containsMouse.val && (Core.root.eventState.target === null || Core.root.eventState.target === this)){
			
			this.$fillMouse(e)
			this.containsMouse = true
			this.hover = true
			this.$s.entered()
		}
	}
	$mouseout(e, state){
		e.preventDefault()
		// if(e.offsetX > this.width || e.offsetY > this.height || e.offsetX < 0 || e.offsetY < 0)
		if(this.$p.hoverEnabled.val && this.$p.enabled.val && this.$p.containsMouse.val && (Core.root.eventState.target === null || Core.root.eventState.target === this)){

			//this.$fillMouse(e)
			this.containsMouse = false
			this.hover = false
			this.$s.exited()
		}
	}
	$touchstart(e, state) {
		e.preventDefault()
		if(this.$p.enabled.val){
			if(!this.mouse.accepted) state.blocked(this)
			this.$fillMouse(e)
			this.pressed = true
			this.$s.pressed()

			if(!this.$p.hoverEnabled.val){
				this.containsMouse = true
				this.hover = true
			}

			if(this.$timerPressAndHold) clearTimeout(this.$timerPressAndHold)
			this.$timerPressAndHold = setTimeout(()=>{
				this.$s.pressAndHold()
			}, this.pressAndHoldInterval)
		}

	}
	$touchend(e, state) {
		e.preventDefault()
		if(this.$p.enabled.val){
			if(!this.mouse.accepted) state.release()
			this.$fillMouse(e)
			this.pressed = false
			this.$s.released()

			let now = new Date().getTime()
			if(this.$lastClickOrTouch){
				if(now - this.$lastClickOrTouch > 250 || Object.keys(this.$s.doubleClicked.connections).length === 0){
					this.$s.clicked();
					this.$lastClickOrTouch = now
				} else {
					this.$s.doubleClicked();
					this.$lastClickOrTouch = now
				}
			} else {
				this.$s.clicked();
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
		if(this.$p.enabled.val && (this.$p.pressed.val || this.$p.hoverEnabled.val)){
			this.$fillMouse(e)
			if(this.$p.pressed.val && state.view && (Math.abs(this.mouse.x-this.tempMouse.x) > 10 || Math.abs(this.mouse.y-this.tempMouse.y) > 10)){
				// console.log('CONSOLE::', this)
				this.pressed = false
				this.containsMouse = false
				this.hover = false
				this.$s.exited()
				if(!this.mouse.accepted) state.release()
				state.view.$mousedown(e, state)
				
			} else {
				this.$s.positionChanged()
			}
		}
	}
}

QML.MouseArea = MouseArea