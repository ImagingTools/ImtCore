import {QtObject} from './QtObject'
import {Signal} from '../utils/Signal'

export class Item extends QtObject {

    static TopLeft = 0
    static Top = 1
    static TopRight = 2
    static Right = 3
    static BottomRight = 4
    static Bottom = 5
    static BottomLeft = 6
    static Left = 7
    static Center = 8

    constructor(parent) {
        super(parent)

        // this.$createReadOnlyProperty('left', ()=>{return this.dom.getBoundingClientRect().left - (this.width*(1-this.scale)/2)})
        // this.$createReadOnlyProperty('right', ()=>{return this.dom.getBoundingClientRect().right})
        // this.$createReadOnlyProperty('top', ()=>{return this.dom.getBoundingClientRect().top - (this.height*(1-this.scale)/2)})
        // this.$createReadOnlyProperty('bottom', ()=>{return this.dom.getBoundingClientRect().bottom})
        // this.$createReadOnlyProperty('horizontalCenter', ()=>{return this.dom.getBoundingClientRect().left+this.width/2})
        // this.$createReadOnlyProperty('verticalCenter', ()=>{return this.dom.getBoundingClientRect().top+this.height/2})
        this.$createReadOnlyProperty('implicitWidth', ()=>{
            let left = 0
            let right = this.width
            let pRect = this.dom.getBoundingClientRect()
            for(let child of this.dom.childNodes){
                let cRect = child.getBoundingClientRect()
                if(cRect.left - pRect.left < left) left = cRect.left - pRect.left
                if(cRect.right - pRect.left > right) left = cRect.right - pRect.left
            }
            return right - left
        })
        this.$createReadOnlyProperty('implicitHeight', ()=>{
            let top = 0
            let bottom = this.height
            let pRect = this.dom.getBoundingClientRect()
            for(let child of this.dom.childNodes){
                let cRect = child.getBoundingClientRect()
                if(cRect.top - pRect.top < top) top = cRect.top - pRect.top
                if(cRect.bottom - pRect.top > bottom) left = cRect.bottom - pRect.top
            }
            return bottom - top
        })
        // this.$cP('left', 0)
        // this.$cP('right', 0)
        // this.$cP('top', 0)
        // this.$cP('bottom', 0)
        // this.$cP('horizontalCenter', 0)
        // this.$cP('verticalCenter', 0)
        // this.$cP('implicitWidth', 'real', 0)
        // this.$cP('implicitHeight', 'real', 0)

        this.$cP('left', ()=>{return this.x - (this.width*(1-this.scale)/2)})
        this.$cP('right', ()=>{return this.x + this.width - (this.width*(1-this.scale)/2)})
        this.$cP('top', ()=>{return this.y - (this.height*(1-this.scale)/2)})
        this.$cP('bottom', ()=>{return this.y + this.height - (this.height*(1-this.scale)/2)})
        this.$cP('horizontalCenter', ()=>{return this.x+this.width/2})
        this.$cP('verticalCenter', ()=>{return this.y+this.height/2})


        this.$cP('x', 0).connect(this.$xChanged.bind(this))
        this.$cP('y', 0).connect(this.$yChanged.bind(this))
        this.$cP('z', 0).connect(this.$zChanged.bind(this))
        this.$cP('visible', true).connect(this.$visibleChanged.bind(this))
        this.$cP('clip', false).connect(this.$clipChanged.bind(this))
        this.$cP('opacity', 1.0).connect(this.$opacityChanged.bind(this))
        this.$cP('enabled', true).connect(this.$enabledChanged.bind(this))
        this.$cP('focus', false).connect(this.$focusChanged.bind(this))
        this.$cP('width', 0).connect(this.$widthChanged.bind(this))
        this.$cP('height', 0).connect(this.$heightChanged.bind(this))
        this.$cP('scale', 1.0).connect(this.$rotationAndScaleChanged.bind(this))
        this.$cP('rotation', 0).connect(this.$rotationAndScaleChanged.bind(this))
        this.$cP('transformOrigin', Item.Center).connect(this.$transformChanged.bind(this))

        this.$cPC('anchors', {
            // left: undefined,
            // top: undefined,
            // right: undefined,
            // bottom: undefined,
            // fill: undefined,
            // centerIn: undefined,
            // horizontalCenter: undefined,
            // verticalCenter: undefined,
            leftMargin: 0,
            rightMargin: 0,
            topMargin: 0,
            bottomMargin: 0,
            verticalCenterOffset: 0,
            horizontalCenterOffset: 0,
        })//.connect(this.$anchorsChanged.bind(this))

        this.$s['Keys.asteriskPressed'] = Signal()
        this.$s['Keys.backPressed'] = Signal()
        this.$s['Keys.backtabPressed'] = Signal()
        this.$s['Keys.callPressed'] = Signal()
        this.$s['Keys.cancelPressed'] = Signal()
        this.$s['Keys.context1Pressed'] = Signal()
        this.$s['Keys.context2Pressed'] = Signal()
        this.$s['Keys.context3Pressed'] = Signal()
        this.$s['Keys.context4Pressed'] = Signal()
        this.$s['Keys.deletePressed'] = Signal()
        this.$s['Keys.digit0Pressed'] = Signal()
        this.$s['Keys.digit1Pressed'] = Signal()
        this.$s['Keys.digit2Pressed'] = Signal()
        this.$s['Keys.digit3Pressed'] = Signal()
        this.$s['Keys.digit4Pressed'] = Signal()
        this.$s['Keys.digit5Pressed'] = Signal()
        this.$s['Keys.digit6Pressed'] = Signal()
        this.$s['Keys.digit7Pressed'] = Signal()
        this.$s['Keys.digit8Pressed'] = Signal()
        this.$s['Keys.digit9Pressed'] = Signal()
        this.$s['Keys.downPressed'] = Signal()
        this.$s['Keys.enterPressed'] = Signal()
        this.$s['Keys.escapePressed'] = Signal()
        this.$s['Keys.flipPressed'] = Signal()
        this.$s['Keys.hangupPressed'] = Signal()
        this.$s['Keys.leftPressed'] = Signal()
        this.$s['Keys.menuPressed'] = Signal()
        this.$s['Keys.noPressed'] = Signal()
        this.$s['Keys.pressed'] = Signal()
        this.$s['Keys.released'] = Signal()
        this.$s['Keys.returnPressed'] = Signal()
        this.$s['Keys.rightPressed'] = Signal()
        this.$s['Keys.selectPressed'] = Signal()
        this.$s['Keys.shortcutOverride'] = Signal()
        this.$s['Keys.spacePressed'] = Signal()
        this.$s['Keys.tabPressed'] = Signal()
        this.$s['Keys.upPressed'] = Signal()
        this.$s['Keys.volumeDownPressed'] = Signal()
        this.$s['Keys.volumeUpPressed'] = Signal()
        this.$s['Keys.yesPressed'] = Signal()
    }
    $domCreate(){
        super.$domCreate()
        this.dom = document.createElement("div")

        if(this.parent) {
            this.parent.dom.appendChild(this.dom)
        }
        this.dom.style.position = 'absolute'
        this.dom.style.overflow = 'hidden'
        this.dom.style.zIndex = /*2147483646/2 + */this.$p.z.val
        this.dom.classList.add(this.constructor.name)
        this.dom.id = `el-${this.UID}`

        this.dom.addEventListener("focusin", (e)=>{
			this.focus = true
		})
		this.dom.addEventListener("focusout", (e)=>{
			this.focus = false
		})
    }

    $destroy() {
        this.dom.remove()
        super.$destroy()
    }

    $xChanged(){
        this.dom.style.left = `${this.$p.x.val}px`
        // this.$updateGeometry()
        this.$updateRect()
    }
    $yChanged(){
        this.dom.style.top = `${this.$p.y.val}px`
        // this.$updateGeometry()
        this.$updateRect()
    }
    $zChanged(){
        this.dom.style.zIndex = /*2147483646/2 + */this.$p.z.val
    }
    $widthChanged(){
        this.dom.style.width = `${this.$p.width.val}px`
        this.$updateRect()
        this.$updateGeometry()
    }
    $heightChanged(){
        this.dom.style.height = `${this.$p.height.val}px`
        this.$updateRect()
        this.$updateGeometry()
    }
    $visibleChanged(){
        this.dom.style.visibility = this.$p.visible.val ? 'visible' : 'hidden'
        this.dom.style.zIndex = this.$p.visible.val ? /*2147483646/2 +*/ this.$p.z.val : -1
    }
    $clipChanged(){
        this.dom.style.overflow = this.$p.clip.val ? "hidden" : "unset"
    }
    $opacityChanged(){
        this.dom.style.opacity = this.$p.opacity.val
    }
    $enabledChanged(){
       
    }
    $focusChanged(){
        this.dom.focus();
    }
    $rotationAndScaleChanged(){
        this.dom.style.transform = `scale(${this.$p.scale.val}) rotate(${this.$p.rotation.val}deg)`
    }
    $transformChanged(){
        switch(this.transformOrigin){
            case Item.Center: this.dom.style.transformOrigin = 'center'; break;
            case Item.TopLeft: this.dom.style.transformOrigin = 'top left'; break;
            case Item.Top: this.dom.style.transformOrigin = 'top'; break;
            case Item.TopRight: this.dom.style.transformOrigin = 'top right'; break;
            case Item.Right: this.dom.style.transformOrigin = 'right'; break;
            case Item.BottomRight: this.dom.style.transformOrigin = 'bottom right'; break;
            case Item.Bottom: this.dom.style.transformOrigin = 'bottom'; break;
            case Item.BottomLeft: this.dom.style.transformOrigin = 'bottom left'; break;
            case Item.Left: this.dom.style.transformOrigin = 'left'; break;
        }
        
    }
    // $anchorsChanged(){
        

        // if(this.anchors.fill){
        //     this.$sP('x', ()=>{
        //         let offsetX = this.parent.left
        //         let val = this.anchors.leftMargin - (offsetX - this.anchors.fill.left)
        //         return val
        //     })
        //     this.$sP('y', ()=>{
        //         let offsetY = this.parent.top
        //         let val = this.anchors.topMargin - (offsetY - this.anchors.fill.top)
        //         return val
        //     })
        //     this.$sP('width', ()=>{
        //         let val = this.anchors.fill.width - this.anchors.rightMargin - this.anchors.leftMargin
        //         return val
        //     })
        //     this.$sP('height', ()=>{
        //         let val = this.anchors.fill.height - this.anchors.bottomMargin - this.anchors.topMargin
        //         return val
        //     })
        // } else if(this.anchors.centerIn){
        //     this.$sP('x', ()=>{
        //         let offsetX = this.parent.left
        //         let val = this.anchors.centerIn.width / 2 - this.width / 2 + this.anchors.leftMargin - this.anchors.rightMargin - (offsetX - this.anchors.centerIn.left)
        //         return val
        //     })
        //     this.$sP('y', ()=>{
        //         let offsetY = this.parent.top
        //         let val = this.anchors.centerIn.height / 2 - this.height / 2 + this.anchors.topMargin - this.anchors.bottomMargin - (offsetY - this.anchors.centerIn.top)
        //         return val
        //     })
        // } else {
            // let offsetX = this.parent.left
            // let offsetY = this.parent.top
            // if(this.anchors.horizontalCenter !== undefined){
            //     this.x = this.anchors.horizontalCenter - this.width / 2 + this.anchors.leftMargin - this.anchors.rightMargin - offsetX + this.anchors.horizontalCenterOffset
            // } else if(this.anchors.left !== undefined && this.anchors.right !== undefined){
            //     this.x = this.anchors.left + this.anchors.leftMargin - offsetX
            //     this.width = this.anchors.right - this.anchors.left - this.anchors.rightMargin
            // } else {
            //     if(this.anchors.left !== undefined){
            //         this.x = this.anchors.left + this.anchors.leftMargin - offsetX
            //     }
            //     if(this.anchors.right !== undefined){
            //         this.x = this.anchors.right - this.width - this.anchors.rightMargin - offsetX
            //     }
            // }
            
            // if(this.anchors.verticalCenter !== undefined){
            //     this.y = this.anchors.verticalCenter - this.height / 2 + this.anchors.topMargin - this.anchors.bottomMargin - offsetY + this.anchors.verticalCenterOffset
            // } else if(this.anchors.top !== undefined && this.anchors.bottom !== undefined){
            //     this.y = this.anchors.top + this.anchors.topMargin - offsetY
            //     this.height = this.anchors.bottom - this.anchors.top - this.anchors.bottomMargin
            // } else {
            //     if(this.anchors.top !== undefined){
            //         this.y = this.anchors.top + this.anchors.topMargin - offsetY
            //     }
            //     if(this.anchors.bottom !== undefined){
            //         this.y = this.anchors.bottom - this.height - this.anchors.bottomMargin - offsetY
            //     }
            // }
        // }
    // }

    $updateRect(){
        // let rect = this.dom.getBoundingClientRect()
        // this.left = rect.left - (this.$p.width.val*(1-this.$p.scale.val)/2)
        // this.right = rect.right
        // this.top = rect.top - (this.$p.height.val*(1-this.$p.scale.val)/2)
        // this.bottom = rect.bottom
        // this.horizontalCenter = rect.left+this.$p.width.val/2
        // this.verticalCenter = rect.top+this.$p.height.val/2


        // if(this.$p['anchors.left'].val || this.$p['anchors.right'].val || this.$p['anchors.top'].val || this.$p['anchors.bottom'].val){
        //     this.$anchorsChanged()
        // }
    }

    mapToItem(item, x, y){
		let parent = this.parent
		let dx = item.x + x + this.x
		let dy = item.y + y + this.y
		
		while(parent)
		{
			if(parent != item){
				dx += parent.x
				dy += parent.y
			}
			parent = parent.parent
		}
		return {
			x: dx,
			y: dy,
		}
	}

}

QML.Item = Item