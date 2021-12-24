/// @private content for base view scrolling area
Item {
	onXChanged:		{ this.parent._scheduleLayout() } //fixme: if you need sync _layout here, please note that discarding delegate can result in recursive createDelegate() call from _layout, do not change it without fixing that first.
	onYChanged:		{ this.parent._scheduleLayout() }

	constructor: {
		this.style('will-change', 'scroll-position, transform, left, top')
	}

	function _scroll(dx, dy){
		if(this.x !== this.x - dx && this.width > this.parent.width){
			if(this.x - dx > 0){
				this.x = 0
			} else if(this.x - dx < this.parent.width - this.width) {
				this.x = this.parent.width - this.width
			} else {
				this.x -= dx
			}
		}
		if(this.y !== this.y - dy && this.height > this.parent.height){
			if(this.y - dy > 0){
				this.y = 0
			} else if(this.y - dy < this.parent.height - this.height) {
				this.y = this.parent.height - this.height
			} else {
				this.y -= dy
			}
		}
		if(this.width === 0){
			this.parent.element.dom.scrollLeft += dx
			this.x = -this.parent.element.dom.scrollLeft
		} else {
			this.parent.element.dom.scrollLeft = 0
		}
		if(this.height === 0){
			this.parent.element.dom.scrollTop += dy
			this.y = -this.parent.element.dom.scrollTop
		} else {
			this.parent.element.dom.scrollTop = 0
		}
		this.parent._context._processActions()
	}
	///@private silently updates scroll positions, because browser animates scroll
	function _updateScrollPositions(x, y, layout) {
		this._setProperty('x', -x)
		this._setProperty('y', -y)
		if (layout === undefined || layout) //default true
			this.parent._scheduleLayout(true) //schedule layout but skip positioning
	}

	onCompleted: {
		this.style('overflow', 'hidden')
	}
}
