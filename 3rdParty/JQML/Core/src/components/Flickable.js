import {Item} from './Item'
import {Signal} from '../utils/Signal'

export class Flickable extends Item {
    static AutoFlickDirection = 0
    static AutoFlickIfNeeded = 1
    static HorizontalFlick = 2
    static VerticalFlick = 3
    static HorizontalAndVerticalFlick = 4

    webScroll = false
	$contentWidthAuto = true
	$contentHeightAuto = true

	$flickable = true

    constructor(args) {
        super(args)
        this.$cP('contentHeight', -1).connect(this.$contentHeightChanged.bind(this))
        this.$cP('contentWidth', -1).connect(this.$contentWidthChanged.bind(this))
        this.$cP('contentX', 0).connect(this.$contentXChanged.bind(this))
        this.$cP('contentY', 0).connect(this.$contentYChanged.bind(this))
        this.$cP('flickableDirection', Flickable.AutoFlickDirection).connect(this.$flickableDirectionChanged.bind(this))
        this.$cP('interactive', true)
        this.$cP('contentItem', null)

		this.$cP('originX', 0)
		this.$cP('originY', 0)
		

        this.$s.flickEnded = Signal()
        this.$s.flickStarted = Signal()
        this.$s.movementEnded = Signal()
        this.$s.movementStarted = Signal()

		this.$childUpdater = null

		this.$scrollInterval = null
    }

    $domCreate(){
        super.$domCreate()
		this.contentItem = this.createComponent('Item', this)
		this.contentItem.$sP('width', ()=>{return this.width})
		this.contentItem.$sP('height', ()=>{return this.height})
        this.dom.style.overflow = "unset"
    }

	$childChanged(){
        clearTimeout(this.$childUpdater)
        this.$childUpdater = setTimeout(()=>{
            this.$updateGeometry()
            this.$uP()
        }, 100)
    }

	$minYExtent(){
		let minY = undefined
		for(let child of this.contentItem.children){
			if(child.dom){
				if(child.y < minY || minY === undefined){
					minY = child.y
				}
			}
		}
		return minY ? minY : 0
	}
	$minXExtent(){
		let minX = undefined
		for(let child of this.contentItem.children){
			if(child.dom){
				if(child.x < minX || minX === undefined){
					minX = child.x
				}
			}
		}
		return minX ? minX : 0
	}

    // $updateGeometry(){
	// 	if(this.$contentHeightAuto && this.contentItem){
	// 		let contentHeightFunc = ()=>{
	// 			let top = 0
	// 			let bottom = 0
	// 			if(this.contentItem.children.length)
	// 			for(let child of this.contentItem.children) {
	// 				let childTop = child.y
	// 				let childBottom = childTop + child.height
	// 				if(childTop < top) top = childTop
	// 				if(childBottom > bottom) bottom = childBottom
	// 			}
	// 			return bottom - top
	// 		}
	// 		this.contentHeight = contentHeightFunc()
	// 		if(this.contentHeight === 0){
	// 			this.$sP('contentHeight', ()=>{ return this.parent ? this.parent.height : 0})
	// 		} else {
	// 			this.$sP('contentHeight', contentHeightFunc)
	// 		}
	// 	}
        
	// 	if(this.$contentWidthAuto && this.contentItem){
	// 		let contentWidthFunc = ()=>{
	// 			let left = 0
	// 			let right = 0
	// 			if(this.contentItem.children.length)
	// 			for(let child of this.contentItem.children) {
	// 				let childLeft = child.x
	// 				let childRight = childLeft + child.width
	// 				if(childLeft < left) left = childLeft
	// 				if(childRight > right) right = childRight
	// 			}
	// 			return right - left
	// 		}
	// 		this.contentWidth = contentWidthFunc()
	// 		if(this.contentWidth === 0){
	// 			this.$sP('contentWidth', ()=>{ return this.parent ? this.parent.width : 0})
	// 		} else {
	// 			this.$sP('contentWidth', contentWidthFunc)
	// 		}
	// 	}
    // }

    $flickableDirectionChanged(){
        this.dom.style.overflow = "hidden";
        switch(this.flickableDirection){
            case Flickable.AutoFlickDirection:
                this.dom.style.overflow = "auto";
                break;
            case Flickable.AutoFlickIfNeeded:
                this.dom.style.overflow = "auto";
                break;
            case Flickable.HorizontalFlick:
                this.dom.style.overflowX = "auto";
                break;
            case Flickable.VerticalFlick:
                this.dom.style.overflowY = "auto";
                break;
            case Flickable.HorizontalAndVerticalFlick:
                this.dom.style.overflow = "auto";
                break;

        }
    }

    $contentWidthChanged(){
        this.contentItem.width = this.contentWidth
    }
    $contentHeightChanged(){
        this.contentItem.height = this.contentHeight
    }
    $contentXChanged(){
		if(this.flickableDirection !== Flickable.VerticalFlick){
			//this.originX = -this.$minXExtent()
			this.contentItem.x = -this.contentX
			
        	this.$flickTimerUpdate()
		}
        
    }
    $contentYChanged(){
		if(this.flickableDirection !== Flickable.HorizontalFlick){
			//this.originY = -this.$minYExtent()
			this.contentItem.y = -this.contentY
			
			this.$flickTimerUpdate()
		}
    }
    // $clipChanged(){
        
    // }

    $flickTimerUpdate(){
        if(!this.$flickTimer) {
            this.$s.flickStarted()
        }
        clearTimeout(this.$flickTimer)
        
        this.$flickTimer = setTimeout(()=>{
            this.$s.flickEnded()
            this.$flickTimer = null
        }, 100)
    }

    $mousewheel(e, state) {
		e.preventDefault()
		if(this.interactive && this.enabled){
			
			let tempX = this.contentX
			let tempY = this.contentY
			this.$scroll(e.deltaX, e.deltaY)

			if(tempX !== this.contentX || tempY !== this.contentY){
				state.blocked(this)
			} else {
				state.release()
			}
			
		}
	}
	$wheel(e, state) {
		e.preventDefault()
		
		if(this.interactive && this.enabled){
			
			let tempX = this.contentX
			let tempY = this.contentY
			this.$mousewheel(e, state)

			if(tempX !== this.contentX || tempY !== this.contentY){
				state.blocked(this)
			} else {
				state.release()
			}
		}

		

	}
	$parentScroll(deltaX, deltaY){
		let parent = this.parent
		while(parent && !(parent.$qmlClassName === 'Flickable' || parent.$qmlClassName === 'ListView' || parent.$qmlClassName === 'GridView')){
			parent = parent.parent
		}

		if(parent) parent.$scroll(deltaX, deltaY)
	}
	$scroll(deltaX, deltaY){
		if(this.interactive && this.enabled){
			if(this.flickableDirection !== Flickable.VerticalFlick && this.contentWidth > 0 && this.contentWidth > this.width){
				if(deltaX > 0)
				if(this.contentX + deltaX < this.contentWidth - this.width){
					this.contentX += deltaX
				} else {
					this.contentX = this.contentWidth - this.width
				}
				if(deltaX < 0)
				if(this.contentX + deltaX > 0){
					this.contentX += deltaX
				} else {
					this.contentX = 0
				}
			} else {
				// this.$parentScroll(deltaX, 0)
			}
			
			if(this.flickableDirection !== Flickable.HorizontalFlick && this.contentHeight > 0 && this.contentHeight > this.height){
				if(deltaY > 0)
				if(this.contentY + deltaY < this.contentHeight - this.height){
					this.contentY += deltaY
				} else {
					this.contentY = this.contentHeight - this.height
				}
				if(deltaY < 0)
				if(this.contentY + deltaY > 0){
					this.contentY += deltaY
				} else {
					this.contentY = 0
				}
			} else {
				// this.$parentScroll(0, deltaY)
			}
		}

	}
	$fillMouse(e){
		let rrr = this.dom.getBoundingClientRect()
		if(e.type.indexOf('touch') >= 0){
			this.$mouseX = e.changedTouches[0].pageX - rrr.x
			this.$mouseY = e.changedTouches[0].pageY - rrr.y
		} else {
			this.$mouseX = e.pageX - rrr.x
			this.$mouseY = e.pageY - rrr.y
		}	
	}
	$mousedown(e, state) {
		e.preventDefault()
		Core.velocityX = 0
		Core.velocityY = 0
		if(this.enabled && this.interactive){
			state.blocked(this)
			this.$fillMouse(e)
			this.$pressed = true
			
		}
	}
	$mouseup(e, state) {
		e.preventDefault()
		if(this.enabled && this.interactive){
			state.release()
			this.$fillMouse(e)
			this.$pressed = false

			if(Core.velocityX !== 0 || Core.velocityY !== 0){
				
				this.$scrollInterval = setInterval(() => {
					this.$scroll(Core.velocityX, Core.velocityY)
					if(Math.abs(Core.velocityX) < 0.03){
						Core.velocityX = 0
					} else if(Core.velocityX > 0){
						Core.velocityX -= 0.01
					} else if(Core.velocityX < 0){
						Core.velocityX += 0.01
					}
					if(Math.abs(Core.velocityY) < 0.03){
						Core.velocityY = 0
					} else if(Core.velocityY > 0){
						Core.velocityY -= 0.01
					} else if(Core.velocityY < 0){
						Core.velocityY += 0.01
					}
					if(Core.velocityX === 0 && Core.velocityY === 0) clearInterval(this.$scrollInterval)
				}, 1);
			}
		}
	}
	$mousemove(e, state) {
		e.preventDefault()
		if(this.enabled && this.interactive && this.$pressed){
			let deltaX = this.$mouseX
			let deltaY = this.$mouseY
			this.$fillMouse(e)
			deltaX -= this.$mouseX
			deltaY -= this.$mouseY
			Core.velocityX = deltaX / 5
			Core.velocityY = deltaY / 5

			let tempContentX = this.contentX
			let tempContentY = this.contentY

			this.$scroll(deltaX, deltaY)

			if((this.contentX + deltaX < 0 || this.contentX + deltaX > this.contentWidth - this.width) && (this.contentY + deltaY < 0 || this.contentY + deltaY > this.contentHeight - this.height)){
				let parent = this.parent
				let find = false
				while(parent && !find){
					if(parent && parent.webScroll !== undefined){
						find = true
						if(parent.interactive && parent.enabled){
							this.$pressed = false
							parent.$mousedown(e, state)
						}
					} else {
						parent = parent.parent
					}
				}
			} else {
				this.$scroll(deltaX, deltaY)
			}

			// if(tempContentX === this.contentX && tempContentY === this.contentY){
			// 	let parent = this.parent
			// 	let find = false
			// 	while(parent && !find){
			// 		if(parent && parent.webScroll !== undefined){
			// 			find = true
			// 			this.$pressed = false
			// 			parent.$mousedown(e, state)
			// 		} else {
			// 			parent = parent.parent
			// 		}
			// 	}
			// }
		}
		
	}

	$touchstart(e, state) {
		e.preventDefault()
		Core.velocityX = 0
		Core.velocityY = 0
		clearInterval(this.$scrollInterval)
		if(this.enabled && this.interactive){
			state.blocked(this)
			this.$fillMouse(e)
			this.$pressed = true
		}

	}
	$touchend(e, state) {
		e.preventDefault()
		if(this.enabled && this.interactive){
			state.release()
			this.$fillMouse(e)
			this.$pressed = false
			
			if(Core.velocityX !== 0 || Core.velocityY !== 0){
				
				this.$scrollInterval = setInterval(() => {
					this.$scroll(Core.velocityX, Core.velocityY)
					if(Math.abs(Core.velocityX) < 0.03){
						Core.velocityX = 0
					} else if(Core.velocityX > 0){
						Core.velocityX -= 0.01
					} else if(Core.velocityX < 0){
						Core.velocityX += 0.01
					}
					if(Math.abs(Core.velocityY) < 0.03){
						Core.velocityY = 0
					} else if(Core.velocityY > 0){
						Core.velocityY -= 0.01
					} else if(Core.velocityY < 0){
						Core.velocityY += 0.01
					}
					if(Core.velocityX === 0 && Core.velocityY === 0) clearInterval(this.$scrollInterval)
				}, 1);
			}
		}
	}
	$touchmove(e, state) {
		e.preventDefault()
		if(this.enabled && this.interactive && this.$pressed){
			let deltaX = this.$mouseX
			let deltaY = this.$mouseY
			this.$fillMouse(e)
			deltaX -= this.$mouseX
			deltaY -= this.$mouseY
			Core.velocityX = deltaX / 5
			Core.velocityY = deltaY / 5

			
			let tempContentX = this.contentX
			let tempContentY = this.contentY

			if((this.contentX + deltaX < 0 || this.contentX + deltaX > this.contentWidth - this.width) && (this.contentY + deltaY < 0 || this.contentY + deltaY > this.contentHeight - this.height)){
				let parent = this.parent
				let find = false
				while(parent && !find){
					if(parent && parent.webScroll !== undefined){
						find = true
						if(parent.interactive && parent.enabled){
							this.$pressed = false
							parent.$touchstart(e, state)
						}
						
					} else {
						parent = parent.parent
					}
				}
			} else {
				this.$scroll(deltaX, deltaY)
			}

			// this.$scroll(deltaX, deltaY)

			// if(tempContentX === this.contentX && tempContentY === this.contentY){
			// 	let parent = this.parent
			// 	let find = false
			// 	while(parent && !find){
			// 		if(parent && parent.webScroll !== undefined){
			// 			find = true
			// 			this.$pressed = false
			// 			parent.$touchstart(e, state)
			// 		} else {
			// 			parent = parent.parent
			// 		}
			// 	}
			// }
		}
	}

	$destroy(){
		clearInterval(this.$scrollInterval)
		clearTimeout(this.$flickTimer)
		clearTimeout(this.$childUpdater)
		super.$destroy()
	}

}

QML.Flickable = Flickable