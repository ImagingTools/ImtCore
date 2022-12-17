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

		this.$childUpdater = null
    }

    $domCreate(){
        super.$domCreate()
        this.dom.style.overflow = "auto"

        this.impl = document.createElement("div")
        this.dom.appendChild(this.impl)
        this.impl.style.position = 'absolute'
    }

	$childChanged(){
        clearTimeout(this.$childUpdater)
        this.$childUpdater = setTimeout(()=>{
            this.$updateGeometry()
            this.$uP()
        }, 100)
    }

    $updateGeometry(){
		if(this.$contentHeightAuto)
        this.$sP('contentHeight', ()=>{ 
            let top = 0
			let bottom = 0
            if(this.children.length)
            for(let child of this.children) {
                let childTop = child.y
                let childBottom = childTop + child.height
                if(childTop < top) top = childTop
                if(childBottom > bottom) bottom = childBottom
            }
            return bottom - top
        })
		if(this.$contentWidthAuto)
        this.$sP('contentWidth', ()=>{ 
            let left = 0
			let right = 0
            if(this.children.length)
            for(let child of this.children) {
                let childLeft = child.x
                let childRight = childLeft + child.width
                if(childLeft < left) left = childLeft
                if(childRight > right) right = childRight
            }
            return right - left
        })
    }

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
        // this.$contentWAuto = false
        this.impl.style.width = `${this.contentWidth}px`

		if(this.contentX > this.contentWidth - this.width) this.contentX = this.contentWidth - this.width
    }
    $contentHeightChanged(){
        // this.$contentHAuto = false
        this.impl.style.height = `${this.contentHeight}px`

		if(this.contentY > this.contentHeight - this.height) this.contentY = this.contentHeight - this.height
    }
    $contentXChanged(){
        this.dom.scrollTo({left: this.contentX, behavior: "auto"})

        this.$flickTimerUpdate()
    }
    $contentYChanged(){
        this.dom.scrollTo({top: this.contentY, behavior: "auto"})
        
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
	$scroll(deltaX, deltaY){
		

		if(this.flickableDirection !== Flickable.VerticalFlick){
			if(deltaX > 0)
			if(this.contentX + deltaX <= this.contentWidth - this.width){
				this.contentX += deltaX
			} else {
				this.contentX = this.contentWidth - this.width
			}
			if(deltaX < 0)
			if(this.contentX + deltaX >= 0){
				this.contentX += deltaX
			} else {
				this.contentX = 0
			}
		}
		
		if(this.flickableDirection !== Flickable.HorizontalFlick){
			if(deltaY > 0)
			if(this.contentY + deltaY <= this.contentHeight - this.height){
				this.contentY += deltaY
			} else {
				this.contentY = this.contentHeight - this.height
			}
			if(deltaY < 0)
			if(this.contentY + deltaY >= 0){
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

			let tempContentX = this.contentX
			let tempContentY = this.contentY

			this.$scroll(deltaX, deltaY)

			if(tempContentX === this.contentX && tempContentY === this.contentY){
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

			
			let tempContentX = this.contentX
			let tempContentY = this.contentY

			this.$scroll(deltaX, deltaY)

			if(tempContentX === this.contentX && tempContentY === this.contentY){
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
		clearTimeout(this.$childUpdater)
		super.$destroy()
	}

}

QML.Flickable = Flickable