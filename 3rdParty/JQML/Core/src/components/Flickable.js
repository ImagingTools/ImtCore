import {Item} from './Item'
import {Signal} from '../utils/Signal'

export class Flickable extends Item {
    $contentWAuto = true
    $contentHAuto = true

    static AutoFlickDirection = 0
    static AutoFlickIfNeeded = 1
    static HorizontalFlick = 2
    static VerticalFlick = 3
    static HorizontalAndVerticalFlick = 4

    webScroll = false

    constructor(parent) {
        super(parent)
        this.$cP('contentHeight', 0).connect(this.$contentHeightChanged.bind(this))
        this.$cP('contentWidth', 0).connect(this.$contentWidthChanged.bind(this))
        this.$cP('contentX', 0).connect(this.$contentXChanged.bind(this))
        this.$cP('contentY', 0).connect(this.$contentYChanged.bind(this))
        this.$cP('flickableDirection', Flickable.AutoFlickDirection).connect(this.$flickableDirectionChanged.bind(this))
        this.$cP('interactive', true)

        this.$s.flickEnded = Signal()
        this.$s.flickStarted = Signal()
        this.$s.movementEnded = Signal()
        this.$s.movementStarted = Signal()

    }

    $domCreate(){
        super.$domCreate()
        this.dom.style.overflow = "auto"

        this.impl = document.createElement("div")
        this.dom.appendChild(this.impl)
        this.impl.style.position = 'absolute'
    }

    $flickableDirectionChanged(){
        this.dom.style.overflow = "hidden";
        switch(this.$p.flickableDirection.val){
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
        this.$contentWAuto = false
        this.impl.style.width = `${this.$p.contentWidth.val}px`

		if(this.$p.contentX.val > this.$p.contentWidth.val - this.$p.width.val) this.contentX = this.$p.contentWidth.val - this.$p.width.val
    }
    $contentHeightChanged(){
        this.$contentHAuto = false
        this.impl.style.height = `${this.$p.contentHeight.val}px`

		if(this.$p.contentY.val > this.$p.contentHeight.val - this.$p.height.val) this.contentY = this.$p.contentHeight.val - this.$p.height.val
    }
    $contentXChanged(){
        this.dom.scrollTo({left: this.$p.contentX.val, behavior: "auto"})

        this.$flickTimerUpdate()
    }
    $contentYChanged(){
        this.dom.scrollTo({top: this.$p.contentY.val, behavior: "auto"})
        
        this.$flickTimerUpdate()
    }
    $clipChanged(){
        
    }

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

    $updateGeometry(){
        if(this.$contentWAuto || this.$contentHAuto){
            let left = 0
            let top = 0
            let right = 0
            let bottom = 0
            for(let child of this.children){
                if(child.$p.x && child.$p.x.val < left) left = child.$p.x.val
                if(child.$p.y && child.$p.y.val < top) top = child.$p.y.val
                if(child.$p.x && child.$p.x.val + child.$p.width.val > right) right = child.$p.x.val + child.$p.width.val
                if(child.$p.y && child.$p.y.val + child.$p.height.val > bottom) bottom = child.$p.y.val + child.$p.height.val
            }
            if(this.$contentWAuto) {
				this.contentWidth = right - left
				this.$contentWAuto = true
                // this.$p.contentWidth.val = right - left
                // this.impl.style.width = `${this.$p.contentWidth.val}px`
            }
            if(this.$contentHAuto) {
				this.contentHeight = bottom - top
				this.$contentHAuto = true
                // this.$p.contentHeight.val = bottom - top
                // this.impl.style.height = `${this.$p.contentHeight.val}px`
            }
        }

        super.$updateGeometry()
    }

    $mousewheel(e, state) {
		e.preventDefault()
		if(this.$p.interactive.val && this.$p.enabled.val){
			
			let tempX = this.contentX
			let tempY = this.contentY
			this.$scroll(e.deltaX, e.deltaY)

			if(tempX !== this.$p.contentX.val || tempY !== this.$p.contentY.val){
				state.blocked(this)
			} else {
				state.release()
			}
			
		}
	}
	$wheel(e, state) {
		e.preventDefault()
		
		if(this.$p.interactive.val && this.$p.enabled.val){
			
			let tempX = this.$p.contentX.val
			let tempY = this.$p.contentY.val
			this.$mousewheel(e, state)

			if(tempX !== this.$p.contentX.val || tempY !== this.$p.contentY.val){
				state.blocked(this)
			} else {
				state.release()
			}
		}

		

	}
	$scroll(deltaX, deltaY){
		

		if(this.$p.flickableDirection.val !== Flickable.VerticalFlick){
			if(deltaX > 0)
			if(this.$p.contentX.val + deltaX <= this.$p.contentWidth.val - this.$p.width.val){
				this.contentX += deltaX
			} else {
				this.contentX = this.$p.contentWidth.val - this.$p.width.val
			}
			if(deltaX < 0)
			if(this.contentX + deltaX >= 0){
				this.contentX += deltaX
			} else {
				this.contentX = 0
			}
		}
		
		if(this.$p.flickableDirection.val !== Flickable.HorizontalFlick){
			if(deltaY > 0)
			if(this.$p.contentY.val + deltaY <= this.$p.contentHeight.val - this.$p.height.val){
				this.contentY += deltaY
			} else {
				this.contentY = this.$p.contentHeight.val - this.$p.height.val
			}
			if(deltaY < 0)
			if(this.$p.contentY.val + deltaY >= 0){
				this.contentY += deltaY
			} else {
				this.contentY = 0
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
		if(this.$p.enabled.val && this.$p.interactive.val){
			state.blocked(this)
			this.$fillMouse(e)
			this.$pressed = true
			
		}
	}
	$mouseup(e, state) {
		e.preventDefault()
		if(this.$p.enabled.val && this.$p.interactive.val){
			state.release()
			this.$fillMouse(e)
			this.$pressed = false
		}
	}
	$mousemove(e, state) {
		e.preventDefault()
		if(this.$p.enabled.val && this.$p.interactive.val && this.$pressed){
			let deltaX = this.$mouseX
			let deltaY = this.$mouseY
			this.$fillMouse(e)
			deltaX -= this.$mouseX
			deltaY -= this.$mouseY

			let tempContentX = this.$p.contentX.val
			let tempContentY = this.$p.contentY.val

			this.$scroll(deltaX, deltaY)

			if(tempContentX === this.$p.contentX.val && tempContentY === this.$p.contentY.val){
				let parent = this.parent
				let find = false
				while(parent && !find){
					if(parent && parent.webScroll !== undefined){
						find = true
						this.$pressed = false
						parent.$mousedown(e, state)
					} else {
						parent = parent.parent
					}
				}
			}
		}
		
	}

	$touchstart(e, state) {
		e.preventDefault()
		if(this.$p.enabled.val && this.$p.interactive.val){
			state.blocked(this)
			this.$fillMouse(e)
			this.$pressed = true
		}

	}
	$touchend(e, state) {
		e.preventDefault()
		if(this.$p.enabled.val && this.$p.interactive.val){
			state.release()
			this.$fillMouse(e)
			this.$pressed = false
			
		}
	}
	$touchmove(e, state) {
		e.preventDefault()
		if(this.$p.enabled.val && this.$p.interactive.val && this.$pressed){
			let deltaX = this.$mouseX
			let deltaY = this.$mouseY
			this.$fillMouse(e)
			deltaX -= this.$mouseX
			deltaY -= this.$mouseY

			
			let tempContentX = this.$p.contentX.val
			let tempContentY = this.$p.contentY.val

			this.$scroll(deltaX, deltaY)

			if(tempContentX === this.$p.contentX.val && tempContentY === this.$p.contentY.val){
				let parent = this.parent
				let find = false
				while(parent && !find){
					if(parent && parent.webScroll !== undefined){
						find = true
						this.$pressed = false
						parent.$touchstart(e, state)
					} else {
						parent = parent.parent
					}
				}
			}
		}
	}

	$destroy(){
		clearTimeout(this.$flickTimer)
		super.$destroy()
	}

}

QML.Flickable = Flickable