///item provide mouse and touch events API
Item {
	signal entered;				///< emitted when mouse entered the item's area
	signal exited;				///< emitted when mouse leaved the item's area
	signal clicked;				///< emitted on mouse click
	signal canceled;			///< emitted when mouse leaved the item's area while mouse button was pressed
	signal mousePressed;	 //by Artur
	signal mouseReleased; //by Artur
	signal doubleClicked;
	//signal wheelEvent;			///< emitted when mouse wheel scrolling
	signal wheel; //by Artur, wheel signal
	signal positionChanged;
	signal pressAndHold;
	

	property real mouseX;				///< mouse x coordinate
	property real mouseY;				///< mouse y coordinate
	property int pressAndHoldInterval: 800;
	//property string cursor;				///< mouse cursor inside the item's area
	property string cursorShape; //by Artur, mouse cursor
	property bool pressed;				///< mouse pressed flag
	property bool containsMouse;		///< mouse inside item's area flag
	property int acceptedButtons: 1;
	// property bool clickable: true;		///< enable mouse click event handling flag
	// property bool pressable: true;		///< enable mouse click event handling flag
	// property bool touchEnabled: true;	///< enable touch events handling flag
	property bool hoverEnabled: true;	///< enable mouse hover event handling flag
	// property bool wheelEnabled: true;	///< enable mouse click event handling flag
	
	/// @private
	onCursorShapeChanged: { this.style('cursor', value) }

	/// @private
	// function _bindTouch(value) {
	// 	if (value && !this._touchBinder) {
	// 		this._touchBinder = new $core.EventBinder(this.element)

	// 		var touchStart = function(event) { 
	// 			this._isMoved = false
	// 			if(this.acceptedButtons & 1){
	// 				this.touchStart(event); 
	// 				this.mousePressed(event); 
	// 				if(this.mouse.accepted) event.stopPropagation();
	// 			}  
	// 		}.bind(this)
	// 		var touchEnd = function(event) {
	// 			if(!this._isMoved && this.acceptedButtons & 1){ 
	// 				this.touchEnd(event); 
	// 				this.mouseReleased(event); 

	// 				let now = new Date().getTime()
	// 				if(this._lastClickOrTouch){
	// 					if(now - this._lastClickOrTouch > 600){
	// 						this.clicked(event);
	// 						this._lastClickOrTouch = now
	// 					} else {
	// 						this.doubleClicked(event);
	// 						this._lastClickOrTouch = now
	// 					}
	// 				} else {
	// 					this.clicked(event);
	// 					this._lastClickOrTouch = now
	// 				}

	// 				if(this.mouse.accepted) 
	// 				event.stopPropagation(); 
	// 			} 
	// 		}.bind(this)
	// 		var touchMove = (function(event) { 
	// 			this.touchMove(event);
	// 			this._isMoved = true 
	// 			if(this.mouse.accepted) event.stopPropagation();  
	// 		}).bind(this)

	// 		this._touchBinder.on('touchstart', touchStart)
	// 		this._touchBinder.on('touchend', touchEnd)
	// 		this._touchBinder.on('touchmove', touchMove)
	// 	}
	// 	if (this._touchBinder)
	// 		this._touchBinder.enable(value)

		
	// }

	/// @private
	// onTouchEnabledChanged: {
	// 	this._bindTouch(value)
	// }

	/// @private
	onRecursiveVisibleChanged: {
		if (!value)
			this.containsMouse = false
	}

	// /// @private
	// function _bindClick(value) {
	// 	if (value && !this._clickBinder) {
	// 		this._clickBinder = new $core.EventBinder(this.element)
	// 		this._clickBinder.on('click', function(event) { 
	// 			if(this.acceptedButtons & 1){
	// 				this.mouse.accepted = false
	// 				this.mouse.x = event.offsetX
	// 				this.mouse.y = event.offsetY
	// 				this.mouse.modifiers = 0x00000000
	// 				if(event.altKey) this.mouse.modifiers |= 0x08000000
	// 				if(event.shiftKey) this.mouse.modifiers |= 0x02000000
	// 				if(event.ctrlKey) this.mouse.modifiers |= 0x04000000
	// 				if(event.metaKey) this.mouse.modifiers |= 0x10000000
	// 				switch(event.button){
	// 					case 0: this.mouse.button = 0x00000001; this.mouse.buttons = 0x00000001; break;
	// 					case 1: this.mouse.button = 0x00000004; this.mouse.buttons = 0x00000004; break;
	// 					case 2: this.mouse.button = 0x00000002; this.mouse.buttons = 0x00000002; break;
	// 				}
	// 				this.updatePosition(event); 
	// 				let now = new Date().getTime()
	// 				if(this._lastClickOrTouch){
	// 					if(now - this._lastClickOrTouch > 600){
	// 						this.clicked(event);
	// 						this._lastClickOrTouch = now
	// 					} else {
	// 						this.doubleClicked(event);
	// 						this._lastClickOrTouch = now
	// 					}
	// 				} else {
	// 					this.clicked(event);
	// 					this._lastClickOrTouch = now
	// 				}
					
					
	// 				if(this.mouse.accepted) event.stopPropagation();
	// 			}
				  
	// 		}.bind(this))
	// 	}
	// 	if (this._clickBinder)
	// 		this._clickBinder.enable(value)

	// 	if (value && !this._clickRBinder) {
	// 		this._clickRBinder = new $core.EventBinder(this.element)
	// 		this._clickRBinder.on('contextmenu', function(event) { 
	// 			if(this.acceptedButtons & 2){
	// 				event.preventDefault()
	// 				this.mouse.accepted = false
	// 				this.mouse.x = event.offsetX
	// 				this.mouse.y = event.offsetY
	// 				this.mouse.modifiers = 0x00000000
	// 				if(event.altKey) this.mouse.modifiers |= 0x08000000
	// 				if(event.shiftKey) this.mouse.modifiers |= 0x02000000
	// 				if(event.ctrlKey) this.mouse.modifiers |= 0x04000000
	// 				if(event.metaKey) this.mouse.modifiers |= 0x10000000
	// 				switch(event.button){
	// 					case 0: this.mouse.button = 0x00000001; this.mouse.buttons = 0x00000001; break;
	// 					case 1: this.mouse.button = 0x00000004; this.mouse.buttons = 0x00000004; break;
	// 					case 2: this.mouse.button = 0x00000002; this.mouse.buttons = 0x00000002; break;
	// 				}
	// 				this.updatePosition(event); 
	// 				let now = new Date().getTime()
	// 				if(this._lastClickOrTouch){
	// 					if(now - this._lastClickOrTouch > 600){
	// 						this.clicked(event);
	// 						this._lastClickOrTouch = now
	// 					} else {
	// 						this.doubleClicked(event);
	// 						this._lastClickOrTouch = now
	// 					}
	// 				} else {
	// 					this.clicked(event);
	// 					this._lastClickOrTouch = now
	// 				}
					
					
	// 				if(this.mouse.accepted) event.stopPropagation();  
	// 			}
	// 		}.bind(this))
	// 	}
	// 	if (this._clickRBinder)
	// 		this._clickRBinder.enable(value)
	// }

	// /// @private
	// // onClickableChanged: {
	// // 	this._bindClick(value)
	// // }

	// onEnabledChanged: {
	// 	let os = this._context.system.os.toLowerCase()

	// 	if (os.indexOf("android") >= 0)
	// 		this._bindTouch(value)
	// 	else if (os.indexOf("ios") >= 0)
	// 		this._bindTouch(value)
	// 	else {
	// 		this._bindClick(value)
	// 		this._bindWheel(value)
	// 		this._bindPressable(value)
	// 		this._bindHover(this.hoverEnabled && value)
	// 	}
	// }

	// /// @private
	// function _bindWheel(value) {
	// 	if (value && !this._wheelBinder) {
	// 		this._wheelBinder = new $core.EventBinder(this.element)
	// 		this._wheelBinder.on('mousewheel', function(event) { 

	// 			this.wheel.accepted = false
	// 			this.wheel.x = event.offsetX
	// 			this.wheel.y = event.offsetY
	// 			this.wheel.modifiers = 0x00000000
	// 			this.wheel.angleDelta = event.wheelDeltaY / 8
    //     		this.wheel.pixelDelta = event.wheelDeltaY
	// 			if(event.altKey) this.wheel.modifiers |= 0x08000000
	// 			if(event.shiftKey) this.wheel.modifiers |= 0x02000000
	// 			if(event.ctrlKey) this.wheel.modifiers |= 0x04000000
	// 			if(event.metaKey) this.wheel.modifiers |= 0x10000000
				
	// 			this.wheel(event.wheelDelta / 120); 
	// 			if(this.wheel.accepted) {
	// 				event.stopPropagation();  
	// 				console.log('stopPropagation')
	// 			}
	// 		}.bind(this))
	// 	}
	// 	if (this._wheelBinder)
	// 		this._wheelBinder.enable(value)
	// }

	// /// @private
	// // onWheelEnabledChanged: {
	// // 	this._bindWheel(value)
	// // }

	// /// @private
	// function _bindPressable(value) {
	// 	if (value && !this._pressableBinder) {
	// 		this._pressableBinder = new $core.EventBinder(this.element)
	// 		this._pressableBinder.on('mousedown', function(event) { 
	// 			this.mouse.accepted = false
	// 			this.mouse.x = event.offsetX
	// 			this.mouse.y = event.offsetY
	// 			this.mouse.modifiers = 0x00000000
	// 			if(event.altKey) this.mouse.modifiers |= 0x08000000
	// 			if(event.shiftKey) this.mouse.modifiers |= 0x02000000
	// 			if(event.ctrlKey) this.mouse.modifiers |= 0x04000000
	// 			if(event.metaKey) this.mouse.modifiers |= 0x10000000
	// 			switch(event.button){
	// 				case 0: this.mouse.button = 0x00000001; this.mouse.buttons = 0x00000001; break;
	// 				case 1: this.mouse.button = 0x00000004; this.mouse.buttons = 0x00000004; break;
	// 				case 2: this.mouse.button = 0x00000002; this.mouse.buttons = 0x00000002; break;
	// 			}
	// 			this.pressed = true; this.mousePressed(event); 
	// 			if(this.mouse.accepted) event.stopPropagation(); 
	// 		}.bind(this))
	// 		this._pressableBinder.on('mouseup', function(event)	{ 
	// 			this.mouse.accepted = false
	// 			this.mouse.x = event.offsetX
	// 			this.mouse.y = event.offsetY
	// 			this.mouse.modifiers = 0x00000000
	// 			if(event.altKey) this.mouse.modifiers |= 0x08000000
	// 			if(event.shiftKey) this.mouse.modifiers |= 0x02000000
	// 			if(event.ctrlKey) this.mouse.modifiers |= 0x04000000
	// 			if(event.metaKey) this.mouse.modifiers |= 0x10000000
	// 			switch(event.button){
	// 				case 0: this.mouse.button = 0x00000001; this.mouse.buttons = 0x00000001; break;
	// 				case 1: this.mouse.button = 0x00000004; this.mouse.buttons = 0x00000004; break;
	// 				case 2: this.mouse.button = 0x00000002; this.mouse.buttons = 0x00000002; break;
	// 			}
	// 			this.pressed = false; this.mouseReleased(event); 
	// 			if(this.mouse.accepted) event.stopPropagation(); 
	// 		}.bind(this))
	// 	}
	// 	if (this._pressableBinder)
	// 		this._pressableBinder.enable(value)
	// }

	// /// @private
	// // onPressableChanged: {
	// // 	this._bindPressable(value)
	// // }

	// /// @private
	// function _bindHover(value) {
	// 	if (value && !this._hoverBinder) {
	// 		this._hoverBinder = new $core.EventBinder(this.element)
	// 		this._hoverBinder.on('mouseenter', function() { 
	// 			this.mouse.accepted = false
	// 			this.mouse.x = event.offsetX
	// 			this.mouse.y = event.offsetY
	// 			this.mouse.modifiers = 0x00000000
	// 			if(event.altKey) this.mouse.modifiers |= 0x08000000
	// 			if(event.shiftKey) this.mouse.modifiers |= 0x02000000
	// 			if(event.ctrlKey) this.mouse.modifiers |= 0x04000000
	// 			if(event.metaKey) this.mouse.modifiers |= 0x10000000
	// 			switch(event.button){
	// 				case 0: this.mouse.button = 0x00000001; this.mouse.buttons = 0x00000001; break;
	// 				case 1: this.mouse.button = 0x00000004; this.mouse.buttons = 0x00000004; break;
	// 				case 2: this.mouse.button = 0x00000002; this.mouse.buttons = 0x00000002; break;
	// 			}
	// 			this.hover = true; 
	// 			if(this.mouse.accepted) event.stopPropagation();  
	// 		}.bind(this))
	// 		this._hoverBinder.on('mouseleave', function() { 
	// 			this.mouse.accepted = false
	// 			this.mouse.x = event.offsetX
	// 			this.mouse.y = event.offsetY
	// 			this.mouse.modifiers = 0x00000000
	// 			if(event.altKey) this.mouse.modifiers |= 0x08000000
	// 			if(event.shiftKey) this.mouse.modifiers |= 0x02000000
	// 			if(event.ctrlKey) this.mouse.modifiers |= 0x04000000
	// 			if(event.metaKey) this.mouse.modifiers |= 0x10000000
	// 			switch(event.button){
	// 				case 0: this.mouse.button = 0x00000001; this.mouse.buttons = 0x00000001; break;
	// 				case 1: this.mouse.button = 0x00000004; this.mouse.buttons = 0x00000004; break;
	// 				case 2: this.mouse.button = 0x00000002; this.mouse.buttons = 0x00000002; break;
	// 			}
	// 			this.hover = false; 
	// 			if(this.pressed){this.pressed = false;this.mouseReleased()}; 
	// 			if(this.mouse.accepted) event.stopPropagation();  
	// 		}.bind(this))
	// 		this._hoverBinder.on('mousemove', function(event) { 
	// 			this.mouse.accepted = false
	// 			this.mouse.x = event.offsetX
	// 			this.mouse.y = event.offsetY
	// 			this.mouse.modifiers = 0x00000000
	// 			if(event.altKey) this.mouse.modifiers |= 0x08000000
	// 			if(event.shiftKey) this.mouse.modifiers |= 0x02000000
	// 			if(event.ctrlKey) this.mouse.modifiers |= 0x04000000
	// 			if(event.metaKey) this.mouse.modifiers |= 0x10000000
	// 			switch(event.button){
	// 				case 0: this.mouse.button = 0x00000001; this.mouse.buttons = 0x00000001; break;
	// 				case 1: this.mouse.button = 0x00000004; this.mouse.buttons = 0x00000004; break;
	// 				case 2: this.mouse.button = 0x00000002; this.mouse.buttons = 0x00000002; break;
	// 			}
	// 			if (this.updatePosition(event)) $core.callMethod(event, 'preventDefault'); 
	// 			if(this.mouse.accepted) event.stopPropagation(); 
	// 		}.bind(this))
	// 	}
	// 	if (this._hoverBinder)
	// 		this._hoverBinder.enable(value)
	// }

	// /// @private
	// onHoverEnabledChanged: {
	// 	this._bindHover(value)
	// }

	/// @private
	// onContainsMouseChanged: {
	// 	if (this.containsMouse) {
	// 		this.entered()
	// 	} else if (!this.containsMouse && this.pressable && this.pressed) {
	// 		this.pressed = false
	// 		this.canceled()
	// 		this.mouseReleased()
	// 	} else {
	// 		this.exited()
	// 	}
	// }

	// /// @private
	// updatePosition(event): {
	// 	if (!this.recursiveVisible)
	// 		return false

	// 	var x = event.offsetX
	// 	var y = event.offsetY

	// 	if (x >= 0 && y >= 0 && x < this.width && y < this.height) {
	// 		this.mouseX = x
	// 		this.mouseY = y
	// 		return true
	// 	}
	// 	else
	// 		return false
	// }

	// /// @private
	// onTouchStart(event): {
	// 	var box = this.toScreen()
	// 	var e = event.touches[0]
	// 	var x = e.pageX - box[0]
	// 	var y = e.pageY - box[1]
	// 	this._startX = x
	// 	this._startY = y
	// 	this._orientation = null;
	// 	this._startTarget = event.target;
	// }

	// /// @private
	// onTouchMove(event): {
	// 	var box = this.toScreen()
	// 	var e = event.touches[0]
	// 	var x = e.pageX - box[0]
	// 	var y = e.pageY - box[1]
	// 	var dx = x - this._startX
	// 	var dy = y - this._startY
	// 	var adx = Math.abs(dx)
	// 	var ady = Math.abs(dy)
	// 	var motion = adx > 5 || ady > 5
	// 	if (!motion)
	// 		return

	// 	if (!this._orientation)
	// 		this._orientation = adx > ady ? 'horizontal' : 'vertical'

	// 	// for delegated events, the target may change over time
	// 	// this ensures we notify the right target and simulates the mouseleave behavior
	// 	while (event.target && event.target !== this._startTarget)
	// 		event.target = event.target.parentNode;
	// 	if (event.target !== this._startTarget) {
	// 		event.target = this._startTarget;
	// 		return;
	// 	}

	// 	if (this._orientation === 'horizontal')
	// 		this.horizontalSwiped(event)
	// 	else
	// 		this.verticalSwiped(event)
	// }

	// onMouseXChanged: { this.mouseMove() }
	// onMouseYChanged: { this.mouseMove() }

	/// @private
	constructor: {
		// let os = this._context.system.os.toLowerCase()
		

		// if (os.indexOf("android") >= 0)
		// 	this._bindTouch(this.enabled)
		// else if (os.indexOf("ios") >= 0)
		// 	this._bindTouch(this.enabled)
		// else {
		// 	this._bindClick(this.enabled)
		// 	this._bindWheel(this.enabled)
		// 	this._bindPressable(this.enabled)
		// 	this._bindHover(this.hoverEnabled && this.enabled)
		// }
		
		
		this.mouse = {
			accepted: false,
			button: 0x00000001,
			buttons: 0x00000001,
			//flags: int,
			modifiers: 0,
			//source: int,
			wasHeld: false,
			x: 0,
			y: 0,
		}
		this.wheel.accepted = false;
		this.wheel.angleDelta = 0;
		this.wheel.buttons = 0x00000001,
		this.wheel.inverted = false;
		this.wheel.pixelDelta = 0;
		this.wheel.modifiers = 0;
		this.wheel.x = 0;
		this.wheel.y = 0;
	}

	onCompleted: {
		this.element.dom.classList.add('MouseArea')
	}
	function _feelMouse(e){
		this.mouse.accepted = false
		let rect = this.element.dom.getBoundingClientRect()
		if(e.type.indexOf('touch') >= 0){
			this.mouse.x = e.changedTouches[0].pageX - rect.x
			this.mouse.y = e.changedTouches[0].pageY - rect.y
		} else {
			this.mouse.x = e.pageX - rect.x
			this.mouse.y = e.pageY - rect.y
		}
		
		this.mouseX = this.mouse.x
		this.mouseY = this.mouse.Y
		this.mouse.modifiers = 0x00000000
		if(e.altKey) this.mouse.modifiers |= 0x08000000
		if(e.shiftKey) this.mouse.modifiers |= 0x02000000
		if(e.ctrlKey) this.mouse.modifiers |= 0x04000000
		if(e.metaKey) this.mouse.modifiers |= 0x10000000
		switch(e.button){
			case 0: this.mouse.button = 0x00000001; this.mouse.buttons = 0x00000001; break;
			case 1: this.mouse.button = 0x00000004; this.mouse.buttons = 0x00000004; break;
			case 2: this.mouse.button = 0x00000002; this.mouse.buttons = 0x00000002; break;
		}				
	}
	function _feelWheel(e){
		this.wheel.accepted = false
		this.wheel.x = event.offsetX
		this.wheel.y = event.offsetY
		this.wheel.modifiers = 0x00000000
		this.wheel.angleDelta = event.wheelDeltaY / 8
        this.wheel.pixelDelta = event.wheelDeltaY
		if(event.altKey) this.wheel.modifiers |= 0x08000000
		if(event.shiftKey) this.wheel.modifiers |= 0x02000000
		if(event.ctrlKey) this.wheel.modifiers |= 0x04000000
		if(event.metaKey) this.wheel.modifiers |= 0x10000000				
	}
	function availableButton(button){
		// 'Qt.NoButton': '0',
		// 'Qt.AllButtons': '134217727',
		// 'Qt.LeftButton': '1',
		// 'Qt.RightButton': '2',
		// 'Qt.MiddleButton': '4',
		// 'Qt.BackButton': '8',
		let btn = 0
		switch(button){
			case 0: btn = 1; break;
			case 1: btn = 4; break;
			case 2: btn = 2; break;
		}	
		if((this.acceptedButtons & button) || (this.acceptedButtons & 134217727)) return true
		return false
	}
	function _mousedown(e, state) {
		state.blocked(this)
		if(this.availableButton(e.button)){
			this._feelMouse(e)
			this.pressed = true
			this.mousePressed()
		}
	}
	function _mouseup(e, state) {
		state.release()
		if(this.availableButton(e.button)){
			this._feelMouse(e)
			this.pressed = false
			this.mouseReleased()

			let now = new Date().getTime()
			if(this._lastClickOrTouch){
				if(now - this._lastClickOrTouch > 400){
					this.clicked();
					this._lastClickOrTouch = now
				} else {
					this.doubleClicked();
					this._lastClickOrTouch = now
				}
			} else {
				this.clicked();
				this._lastClickOrTouch = now
			}
		}
	}
	function _mousemove(e, state) {
		if(this.pressed) this._feelMouse(e)
	}
	function _mousewheel(e, state) {
		this._feelWheel(e)
	}
	function _contextmenu(e, state) {

	}
	function _mouseenter(e, state){
		this._feelMouse(e)
		this.entered()
	}
	function _mouseleave(e, state){
		this._feelMouse(e)
		this.exited()
	}
	function _touchstart(e, state) {
		state.blocked(this)
		this._feelMouse(e)
		this.pressed = true
		this.mousePressed()

	}
	function _touchend(e, state) {
		state.release()
		this._feelMouse(e)
		this.pressed = false
		this.mouseReleased()

		let now = new Date().getTime()
		if(this._lastClickOrTouch){
			if(now - this._lastClickOrTouch > 400){
				this.clicked();
				this._lastClickOrTouch = now
			} else {
				this.doubleClicked();
				this._lastClickOrTouch = now
			}
		} else {
			this.clicked();
			this._lastClickOrTouch = now
		}
	}
	function _touchmove(e, state) {
		if(this.pressed) this._feelMouse(e)
	}
}
