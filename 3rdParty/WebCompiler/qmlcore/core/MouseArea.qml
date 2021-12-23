///item provide mouse and touch events API
Item {
	signal entered;				///< emitted when mouse entered the item's area
	signal exited;				///< emitted when mouse leaved the item's area
	signal clicked;				///< emitted on mouse click
	signal canceled;			///< emitted when mouse leaved the item's area while mouse button was pressed
	signal mousePressed;	 //by Artur
	signal mouseReleased; //by Artur
	//signal wheelEvent;			///< emitted when mouse wheel scrolling
	signal wheel; //by Artur, wheel signal
	signal verticalSwiped;		///< emitted on vertical swipe
	signal horizontalSwiped;	///< emitted on horizontal swipe
	signal mouseMove;			///< emitted on mouse moved inside the item's area
	signal touchEnd;			///< emitted on touch event end
	signal touchMove;			///< emitted on mouse move while touching
	signal touchStart;			///< emitted on touch event start

	property real mouseX;				///< mouse x coordinate
	property real mouseY;				///< mouse y coordinate
	//property string cursor;				///< mouse cursor inside the item's area
	property string cursorShape; //by Artur, mouse cursor
	property bool pressed;				///< mouse pressed flag
	property bool containsMouse;		///< mouse inside item's area flag
	// property bool clickable: true;		///< enable mouse click event handling flag
	// property bool pressable: true;		///< enable mouse click event handling flag
	// property bool touchEnabled: true;	///< enable touch events handling flag
	property bool hoverEnabled: true;	///< enable mouse hover event handling flag
	// property bool wheelEnabled: true;	///< enable mouse click event handling flag
	
	property alias hover: containsMouse;	///< containsMouse property alias
	//property alias hoverable: hoverEnabled;	///< hoverEnabled property alias

	/// @private
	onCursorShapeChanged: { this.style('cursor', value) }

	/// @private
	function _bindTouch(value) {
		if (value && !this._touchBinder) {
			this._touchBinder = new $core.EventBinder(this.element)

			var touchStart = function(event) { this.touchStart(event); if(this.mouse.accepted) event.stopPropagation();  }.bind(this)
			var touchEnd = function(event) { this.touchEnd(event); if(this.mouse.accepted) event.stopPropagation();  }.bind(this)
			var touchMove = (function(event) { this.touchMove(event); if(this.mouse.accepted) event.stopPropagation();  }).bind(this)

			this._touchBinder.on('touchstart', touchStart)
			this._touchBinder.on('touchend', touchEnd)
			this._touchBinder.on('touchmove', touchMove)
		}
		if (this._touchBinder)
			this._touchBinder.enable(value)
	}

	/// @private
	// onTouchEnabledChanged: {
	// 	this._bindTouch(value)
	// }

	/// @private
	onRecursiveVisibleChanged: {
		if (!value)
			this.containsMouse = false
	}

	/// @private
	function _bindClick(value) {
		if (value && !this._clickBinder) {
			this._clickBinder = new $core.EventBinder(this.element)
			this._clickBinder.on('click', function(event) { 
				this.mouse.accepted = false
				this.mouse.x = event.offsetX
				this.mouse.y = event.offsetY
				this.mouse.modifiers = 0x00000000
				if(event.altKey) this.mouse.modifiers |= 0x08000000
				if(event.shiftKey) this.mouse.modifiers |= 0x02000000
				if(event.ctrlKey) this.mouse.modifiers |= 0x04000000
				if(event.metaKey) this.mouse.modifiers |= 0x10000000
				switch(event.button){
					case 0: this.mouse.button = 0x00000001; this.mouse.buttons = 0x00000001; break;
					case 1: this.mouse.button = 0x00000004; this.mouse.buttons = 0x00000004; break;
					case 2: this.mouse.button = 0x00000002; this.mouse.buttons = 0x00000002; break;
				}
				this.updatePosition(event); 
				this.clicked(event); 
				if(this.mouse.accepted) event.stopPropagation();  
			}.bind(this))
		}
		if (this._clickBinder)
			this._clickBinder.enable(value)
	}

	/// @private
	// onClickableChanged: {
	// 	this._bindClick(value)
	// }

	onEnabledChanged: {
		this._bindClick(value)
		this._bindWheel(value)
		this._bindPressable(value)
		this._bindHover(value && this.hoverEnabled)
		this._bindTouch(value)
	}

	/// @private
	function _bindWheel(value) {
		if (value && !this._wheelBinder) {
			this._wheelBinder = new $core.EventBinder(this.element)
			this._wheelBinder.on('mousewheel', function(event) { 

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
				
				this.wheel(event.wheelDelta / 120); 
				if(this.wheel.accepted) {
					event.stopPropagation();  
					console.log('stopPropagation')
				}
			}.bind(this))
		}
		if (this._wheelBinder)
			this._wheelBinder.enable(value)
	}

	/// @private
	// onWheelEnabledChanged: {
	// 	this._bindWheel(value)
	// }

	/// @private
	function _bindPressable(value) {
		if (value && !this._pressableBinder) {
			this._pressableBinder = new $core.EventBinder(this.element)
			this._pressableBinder.on('mousedown', function(event) { 
				this.mouse.accepted = false
				this.mouse.x = event.offsetX
				this.mouse.y = event.offsetY
				this.mouse.modifiers = 0x00000000
				if(event.altKey) this.mouse.modifiers |= 0x08000000
				if(event.shiftKey) this.mouse.modifiers |= 0x02000000
				if(event.ctrlKey) this.mouse.modifiers |= 0x04000000
				if(event.metaKey) this.mouse.modifiers |= 0x10000000
				switch(event.button){
					case 0: this.mouse.button = 0x00000001; this.mouse.buttons = 0x00000001; break;
					case 1: this.mouse.button = 0x00000004; this.mouse.buttons = 0x00000004; break;
					case 2: this.mouse.button = 0x00000002; this.mouse.buttons = 0x00000002; break;
				}
				this.pressed = true; this.mousePressed(event); 
				if(this.mouse.accepted) event.stopPropagation(); 
			}.bind(this))
			this._pressableBinder.on('mouseup', function(event)	{ 
				this.mouse.accepted = false
				this.mouse.x = event.offsetX
				this.mouse.y = event.offsetY
				this.mouse.modifiers = 0x00000000
				if(event.altKey) this.mouse.modifiers |= 0x08000000
				if(event.shiftKey) this.mouse.modifiers |= 0x02000000
				if(event.ctrlKey) this.mouse.modifiers |= 0x04000000
				if(event.metaKey) this.mouse.modifiers |= 0x10000000
				switch(event.button){
					case 0: this.mouse.button = 0x00000001; this.mouse.buttons = 0x00000001; break;
					case 1: this.mouse.button = 0x00000004; this.mouse.buttons = 0x00000004; break;
					case 2: this.mouse.button = 0x00000002; this.mouse.buttons = 0x00000002; break;
				}
				this.pressed = false; this.mouseReleased(event); 
				if(this.mouse.accepted) event.stopPropagation(); 
			}.bind(this))
		}
		if (this._pressableBinder)
			this._pressableBinder.enable(value)
	}

	/// @private
	// onPressableChanged: {
	// 	this._bindPressable(value)
	// }

	/// @private
	function _bindHover(value) {
		if (value && !this._hoverBinder) {
			this._hoverBinder = new $core.EventBinder(this.element)
			this._hoverBinder.on('mouseenter', function() { 
				this.mouse.accepted = false
				this.mouse.x = event.offsetX
				this.mouse.y = event.offsetY
				this.mouse.modifiers = 0x00000000
				if(event.altKey) this.mouse.modifiers |= 0x08000000
				if(event.shiftKey) this.mouse.modifiers |= 0x02000000
				if(event.ctrlKey) this.mouse.modifiers |= 0x04000000
				if(event.metaKey) this.mouse.modifiers |= 0x10000000
				switch(event.button){
					case 0: this.mouse.button = 0x00000001; this.mouse.buttons = 0x00000001; break;
					case 1: this.mouse.button = 0x00000004; this.mouse.buttons = 0x00000004; break;
					case 2: this.mouse.button = 0x00000002; this.mouse.buttons = 0x00000002; break;
				}
				this.hover = true; 
				if(this.mouse.accepted) event.stopPropagation();  
			}.bind(this))
			this._hoverBinder.on('mouseleave', function() { 
				this.mouse.accepted = false
				this.mouse.x = event.offsetX
				this.mouse.y = event.offsetY
				this.mouse.modifiers = 0x00000000
				if(event.altKey) this.mouse.modifiers |= 0x08000000
				if(event.shiftKey) this.mouse.modifiers |= 0x02000000
				if(event.ctrlKey) this.mouse.modifiers |= 0x04000000
				if(event.metaKey) this.mouse.modifiers |= 0x10000000
				switch(event.button){
					case 0: this.mouse.button = 0x00000001; this.mouse.buttons = 0x00000001; break;
					case 1: this.mouse.button = 0x00000004; this.mouse.buttons = 0x00000004; break;
					case 2: this.mouse.button = 0x00000002; this.mouse.buttons = 0x00000002; break;
				}
				this.hover = false; 
				if(this.pressed){this.pressed = false;this.mouseReleased()}; 
				if(this.mouse.accepted) event.stopPropagation();  
			}.bind(this))
			this._hoverBinder.on('mousemove', function(event) { 
				this.mouse.accepted = false
				this.mouse.x = event.offsetX
				this.mouse.y = event.offsetY
				this.mouse.modifiers = 0x00000000
				if(event.altKey) this.mouse.modifiers |= 0x08000000
				if(event.shiftKey) this.mouse.modifiers |= 0x02000000
				if(event.ctrlKey) this.mouse.modifiers |= 0x04000000
				if(event.metaKey) this.mouse.modifiers |= 0x10000000
				switch(event.button){
					case 0: this.mouse.button = 0x00000001; this.mouse.buttons = 0x00000001; break;
					case 1: this.mouse.button = 0x00000004; this.mouse.buttons = 0x00000004; break;
					case 2: this.mouse.button = 0x00000002; this.mouse.buttons = 0x00000002; break;
				}
				if (this.updatePosition(event)) $core.callMethod(event, 'preventDefault'); 
				if(this.mouse.accepted) event.stopPropagation(); 
			}.bind(this))
		}
		if (this._hoverBinder)
			this._hoverBinder.enable(value)
	}

	/// @private
	onHoverEnabledChanged: {
		this._bindHover(value)
	}

	/// @private
	onContainsMouseChanged: {
		if (this.containsMouse) {
			this.entered()
		} else if (!this.containsMouse && this.pressable && this.pressed) {
			this.pressed = false
			this.canceled()
			this.mouseReleased()
		} else {
			this.exited()
		}
	}

	/// @private
	updatePosition(event): {
		if (!this.recursiveVisible)
			return false

		var x = event.offsetX
		var y = event.offsetY

		if (x >= 0 && y >= 0 && x < this.width && y < this.height) {
			this.mouseX = x
			this.mouseY = y
			return true
		}
		else
			return false
	}

	/// @private
	onTouchStart(event): {
		var box = this.toScreen()
		var e = event.touches[0]
		var x = e.pageX - box[0]
		var y = e.pageY - box[1]
		this._startX = x
		this._startY = y
		this._orientation = null;
		this._startTarget = event.target;
	}

	/// @private
	onTouchMove(event): {
		var box = this.toScreen()
		var e = event.touches[0]
		var x = e.pageX - box[0]
		var y = e.pageY - box[1]
		var dx = x - this._startX
		var dy = y - this._startY
		var adx = Math.abs(dx)
		var ady = Math.abs(dy)
		var motion = adx > 5 || ady > 5
		if (!motion)
			return

		if (!this._orientation)
			this._orientation = adx > ady ? 'horizontal' : 'vertical'

		// for delegated events, the target may change over time
		// this ensures we notify the right target and simulates the mouseleave behavior
		while (event.target && event.target !== this._startTarget)
			event.target = event.target.parentNode;
		if (event.target !== this._startTarget) {
			event.target = this._startTarget;
			return;
		}

		if (this._orientation === 'horizontal')
			this.horizontalSwiped(event)
		else
			this.verticalSwiped(event)
	}

	onMouseXChanged: { this.mouseMove() }
	onMouseYChanged: { this.mouseMove() }

	/// @private
	constructor: {
		// this._bindClick(this.clickable)
		// this._bindWheel(this.wheelEnabled)
		// this._bindPressable(this.pressable)
		// this._bindHover(this.hoverEnabled)
		// this._bindTouch(this.touchEnabled)
		this._bindClick(this.enabled)
		this._bindWheel(this.enabled)
		this._bindPressable(this.enabled)
		this._bindHover(this.hoverEnabled && this.enabled)
		this._bindTouch(this.enabled)
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
}
