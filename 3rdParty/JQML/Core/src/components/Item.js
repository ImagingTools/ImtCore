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

    // $anchorsX = false
    // $anchorsY = false

    constructor(args) {
        super(args)

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
        // this.$cP('left', ()=>{return this.x})
        // this.$cP('right', ()=>{return this.x + this.width})
        // this.$cP('top', ()=>{return this.y})
        // this.$cP('bottom', ()=>{return this.y + this.height})
        // this.$cP('horizontalCenter', ()=>{return this.x+this.width/2})
        // this.$cP('verticalCenter', ()=>{return this.y+this.height/2})
        this.$cP('left', {'target': this, 'float': 'left'})
        this.$cP('right', {'target': this, 'float': 'right'})
        this.$cP('top', {'target': this, 'float': 'top'})
        this.$cP('bottom', {'target': this, 'float': 'bottom'})
        this.$cP('horizontalCenter', {'target': this, 'float': 'horizontalCenter'})
        this.$cP('verticalCenter', {'target': this, 'float': 'verticalCenter'})
        this.$cP('childrenRect', ()=>{return {}})


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
            left: undefined,
            top: undefined,
            right: undefined,
            bottom: undefined,
            fill: undefined,
            centerIn: undefined,
            horizontalCenter: undefined,
            verticalCenter: undefined,
            leftMargin: 0,
            rightMargin: 0,
            topMargin: 0,
            bottomMargin: 0,
            verticalCenterOffset: 0,
            horizontalCenterOffset: 0,
        }).connect(this.$anchorsChanged.bind(this))

        this.Keys = {}
        this.Keys.onAsteriskPressed = this.$s['Keys.asteriskPressed'] = Signal()
        this.Keys.onBackPressed = this.$s['Keys.backPressed'] = Signal()
        this.Keys.onBacktabPressed = this.$s['Keys.backtabPressed'] = Signal()
        this.Keys.onCallPressed = this.$s['Keys.callPressed'] = Signal()
        this.Keys.onCancelPressed = this.$s['Keys.cancelPressed'] = Signal()
        this.Keys.onContext1Pressed = this.$s['Keys.context1Pressed'] = Signal()
        this.Keys.onContext2Pressed = this.$s['Keys.context2Pressed'] = Signal()
        this.Keys.onContext3Pressed = this.$s['Keys.context3Pressed'] = Signal()
        this.Keys.onContext4Pressed = this.$s['Keys.context4Pressed'] = Signal()
        this.Keys.onDeletePressed = this.$s['Keys.deletePressed'] = Signal()
        this.Keys.onDigit0Pressed = this.$s['Keys.digit0Pressed'] = Signal()
        this.Keys.onDigit1Pressed = this.$s['Keys.digit1Pressed'] = Signal()
        this.Keys.onDigit2Pressed = this.$s['Keys.digit2Pressed'] = Signal()
        this.Keys.onDigit3Pressed = this.$s['Keys.digit3Pressed'] = Signal()
        this.Keys.onDigit4Pressed = this.$s['Keys.digit4Pressed'] = Signal()
        this.Keys.onDigit5Pressed = this.$s['Keys.digit5Pressed'] = Signal()
        this.Keys.onDigit6Pressed = this.$s['Keys.digit6Pressed'] = Signal()
        this.Keys.onDigit7Pressed = this.$s['Keys.digit7Pressed'] = Signal()
        this.Keys.onDigit8Pressed = this.$s['Keys.digit8Pressed'] = Signal()
        this.Keys.onDigit9Pressed = this.$s['Keys.digit9Pressed'] = Signal()
        this.Keys.onDownPressed = this.$s['Keys.downPressed'] = Signal()
        this.Keys.onEnterPressed = this.$s['Keys.enterPressed'] = Signal()
        this.Keys.onEscapePressed = this.$s['Keys.escapePressed'] = Signal()
        this.Keys.onFlipPressed = this.$s['Keys.flipPressed'] = Signal()
        this.Keys.onHangupPressed = this.$s['Keys.hangupPressed'] = Signal()
        this.Keys.onLeftPressed = this.$s['Keys.leftPressed'] = Signal()
        this.Keys.onMenuPressed = this.$s['Keys.menuPressed'] = Signal()
        this.Keys.onNoPressed = this.$s['Keys.noPressed'] = Signal()
        this.Keys.onPressed = this.$s['Keys.pressed'] = Signal()
        this.Keys.onReleased = this.$s['Keys.released'] = Signal()
        this.Keys.onReturnPressed = this.$s['Keys.returnPressed'] = Signal()
        this.Keys.onRightPressed = this.$s['Keys.rightPressed'] = Signal()
        this.Keys.onSelectPressed = this.$s['Keys.selectPressed'] = Signal()
        this.Keys.onShortcutOverride = this.$s['Keys.shortcutOverride'] = Signal()
        this.Keys.onSpacePressed = this.$s['Keys.spacePressed'] = Signal()
        this.Keys.onTabPressed = this.$s['Keys.tabPressed'] = Signal()
        this.Keys.onUpPressed = this.$s['Keys.upPressed'] = Signal()
        this.Keys.onVolumeDownPressed = this.$s['Keys.volumeDownPressed'] = Signal()
        this.Keys.onVolumeUpPressed = this.$s['Keys.volumeUpPressed'] = Signal()
        this.Keys.onYesPressed = this.$s['Keys.yesPressed'] = Signal()
    }
    $domCreate(){
        super.$domCreate()
        if(this.parent) this.parent.$availableGeometry.push(this)
        this.dom = document.createElement("div")

        // if(this.UID !== 0)
        // Core.root.dom.appendChild(this.dom)
        if(this.parent){
            this.parent.dom.appendChild(this.dom)
        }
        
        this.dom.style.position = 'absolute'
        this.dom.style.left = '0'
        this.dom.style.top = '0'
        this.dom.style.overflow = 'unset'
        this.dom.style.zIndex = this.z
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
        let x = this.x
        // if(!this.$anchorsX){
        //     let parent = this.parent
        //     while(parent){
        //         x += parent.x
        //         parent = parent.parent
        //     }
        // }
        this.dom.style.left = `${this.x}px`
    }
    $yChanged(){
        let y = this.y
        // if(!this.$anchorsY){
        //     let parent = this.parent
        //     while(parent){
        //         y += parent.y
        //         parent = parent.parent
        //     }
        // }
        this.dom.style.top = `${this.y}px`
    }
    $zChanged(){
        this.dom.style.zIndex = this.z
    }
    $widthChanged(){
        this.dom.style.width = `${this.width}px`
    }
    $heightChanged(){
        this.dom.style.height = `${this.height}px`
    }
    $visibleChanged(){
        this.dom.style.opacity = this.visible ? this.opacity : 0
        this.dom.style.visibility = this.visible ? 'visible' : 'hidden'
        this.dom.style.zIndex = this.visible ? /*2147483646/2 +*/ this.z : -1
    }
    $clipChanged(){
        this.dom.style.overflow = this.clip ? "hidden" : "unset"
    }
    $opacityChanged(){
        this.dom.style.opacity = this.opacity
    }
    $enabledChanged(){
       
    }
    $focusChanged(){
        this.dom.focus()
    }
    $rotationAndScaleChanged(){
        this.dom.style.transform = `scale(${this.scale}) rotate(${this.rotation}deg)`
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
    forceActiveFocus(){
        this.focus = true
    }
    $anchorsChanged(){
        if(this.anchors.fill){
            if(this.anchors.fill === this.parent){
                this.$sP('x', ()=>{
                    return this.anchors.leftMargin
                })
                this.$sP('y', ()=>{
                    return this.anchors.topMargin
                })
                this.$sP('width', ()=>{
                    return this.anchors.fill.width - this.anchors.rightMargin - this.anchors.leftMargin
                })
                this.$sP('height', ()=>{
                    return this.anchors.fill.height - this.anchors.bottomMargin - this.anchors.topMargin
                })
            } else if(this.parent === this.anchors.fill.parent ) {
                this.$sP('x', ()=>{
                    return this.anchors.fill.x + this.anchors.leftMargin
                })
                this.$sP('y', ()=>{
                    return this.anchors.fill.y + this.anchors.topMargin
                })
                this.$sP('width', ()=>{
                    return this.anchors.fill.width - this.anchors.rightMargin - this.anchors.leftMargin
                })
                this.$sP('height', ()=>{
                    return this.anchors.fill.height - this.anchors.bottomMargin - this.anchors.topMargin
                })
            } else {
                this.$sP('x', ()=>{
                    return this.anchors.fill.x + this.anchors.leftMargin + (this.anchors.fill.parent.x - this.parent.x)
                })
                this.$sP('y', ()=>{
                    return this.anchors.fill.y + this.anchors.topMargin + (this.anchors.fill.parent.y - this.parent.y)
                })
                this.$sP('width', ()=>{
                    return this.anchors.fill.width - this.anchors.rightMargin - this.anchors.leftMargin
                })
                this.$sP('height', ()=>{
                    return this.anchors.fill.height - this.anchors.bottomMargin - this.anchors.topMargin
                })
                // разные 
            }
        } else if(this.anchors.centerIn){
            
            
            if(this.anchors.centerIn === this.parent){
                this.$sP('x', ()=>{
                    return this.anchors.centerIn.width / 2 - this.width / 2 + this.anchors.leftMargin - this.anchors.rightMargin
                })
                this.$sP('y', ()=>{
                    return this.anchors.centerIn.height / 2 - this.height / 2 + this.anchors.topMargin - this.anchors.bottomMargin
                })
            } else if(this.parent === this.anchors.centerIn.parent ) {
                this.$sP('x', ()=>{
                    return this.anchors.centerIn.x + this.anchors.centerIn.width / 2 - this.width / 2 + this.anchors.leftMargin - this.anchors.rightMargin
                })
                this.$sP('y', ()=>{
                    return this.anchors.centerIn.y + this.anchors.centerIn.height / 2 - this.height / 2 + this.anchors.topMargin - this.anchors.bottomMargin
                })
            } else {
                this.$sP('x', ()=>{
                    return this.anchors.centerIn.x + this.anchors.centerIn.width / 2 - this.width / 2 + this.anchors.leftMargin - this.anchors.rightMargin - (this.anchors.centerIn.parent.width/2 - this.parent.width/2)
                })
                this.$sP('y', ()=>{
                    return this.anchors.centerIn.y + this.anchors.centerIn.height / 2 - this.height / 2 + this.anchors.topMargin - this.anchors.bottomMargin - (this.anchors.centerIn.parent.height/2 - this.parent.height/2)
                })
                // разные 
            }
            
        } else {
            if(this.anchors.horizontalCenter !== undefined){
                let horizontalCenterTarget = this.anchors.horizontalCenter.target
                if(horizontalCenterTarget === this.parent){
                    this.$sP('x', ()=>{
                        return horizontalCenterTarget.width / 2 - this.width / 2 + this.anchors.leftMargin - this.anchors.rightMargin
                    })
                } else if(this.parent === horizontalCenterTarget.parent ) {
                    this.$sP('x', ()=>{
                        return horizontalCenterTarget.x + horizontalCenterTarget.width / 2 - this.width / 2 + this.anchors.leftMargin - this.anchors.rightMargin
                    })
                } else {
                    this.$sP('x', ()=>{
                        return horizontalCenterTarget.x + horizontalCenterTarget.width / 2 - this.width / 2 + this.anchors.leftMargin - this.anchors.rightMargin - (horizontalCenterTarget.parent.width/2 - this.parent.width/2)
                    })
                    // разные 
                }
                // this.x = this.anchors.horizontalCenter - this.width / 2 + this.anchors.leftMargin - this.anchors.rightMargin + this.anchors.horizontalCenterOffset
            } else if(this.anchors.left !== undefined && this.anchors.right !== undefined){
                let leftTarget = this.anchors.left.target
                let rightTarget = this.anchors.right.target
                if(leftTarget === this.parent){
                    if(this.anchors.left.float === 'left'){
                        this.$sP('x', ()=>{
                            return this.anchors.leftMargin
                        })
                    } else if(this.anchors.left.float === 'right'){
                        this.$sP('x', ()=>{
                            return this.anchors.leftMargin + leftTarget.width
                        })
                    }
                    
                } else if(this.parent === leftTarget.parent ) {
                    if(this.anchors.left.float === 'left'){
                        this.$sP('x', ()=>{
                            return leftTarget.x + this.anchors.leftMargin
                        })
                    } else if(this.anchors.left.float === 'right'){
                        this.$sP('x', ()=>{
                            return leftTarget.x + this.anchors.leftMargin + leftTarget.width
                        })
                    }
                } else {
                    if(this.anchors.left.float === 'left'){
                        this.$sP('x', ()=>{
                            return leftTarget.x + this.anchors.leftMargin + (leftTarget.parent.x - this.parent.x)
                        })
                    } else if(this.anchors.left.float === 'right'){
                        this.$sP('x', ()=>{
                            return leftTarget.x + this.anchors.leftMargin + leftTarget.width + (leftTarget.parent.x - this.parent.x)
                        })
                    }
                    // разные 
                }
                if(rightTarget === this.parent){
                    if(this.anchors.right.float === 'left'){
                        this.$sP('width', ()=>{
                            return this.anchors.rightMargin - this.x
                        })
                    } else if(this.anchors.right.float === 'right'){
                        this.$sP('width', ()=>{
                            return rightTarget.width - this.anchors.rightMargin - this.x
                        })
                    }
                    
                } else if(this.parent === rightTarget.parent ) {
                    if(this.anchors.right.float === 'left'){
                        this.$sP('width', ()=>{
                            return rightTarget.x - this.anchors.rightMargin - this.x
                        })
                    } else if(this.anchors.right.float === 'right'){
                        this.$sP('width', ()=>{
                            return rightTarget.x + rightTarget.width - this.anchors.rightMargin - this.x
                        })
                    }
                } else {
                    if(this.anchors.right.float === 'left'){
                        this.$sP('width', ()=>{
                            return rightTarget.x - this.anchors.rightMargin - this.x + (rightTarget.parent.x - this.parent.x)
                        })
                    } else if(this.anchors.right.float === 'right'){
                        this.$sP('width', ()=>{
                            return rightTarget.x + rightTarget.width - this.anchors.rightMargin - this.x + (rightTarget.parent.x - this.parent.x)
                        })
                    }
                    // разные 
                }
            } else {
                if(this.anchors.left !== undefined){
                    let leftTarget = this.anchors.left.target
                    if(leftTarget === this.parent){
                        if(this.anchors.left.float === 'left'){
                            this.$sP('x', ()=>{
                                return this.anchors.leftMargin
                            })
                        } else if(this.anchors.left.float === 'right'){
                            this.$sP('x', ()=>{
                                return this.anchors.leftMargin + leftTarget.width
                            })
                        }
                        
                    } else if(this.parent === leftTarget.parent ) {
                        if(this.anchors.left.float === 'left'){
                            this.$sP('x', ()=>{
                                return leftTarget.x + this.anchors.leftMargin
                            })
                        } else if(this.anchors.left.float === 'right'){
                            this.$sP('x', ()=>{
                                return leftTarget.x + this.anchors.leftMargin + leftTarget.width
                            })
                        }
                    } else {
                        if(this.anchors.left.float === 'left'){
                            this.$sP('x', ()=>{
                                return leftTarget.x + this.anchors.leftMargin + (leftTarget.parent.x - this.parent.x)
                            })
                        } else if(this.anchors.left.float === 'right'){
                            this.$sP('x', ()=>{
                                return leftTarget.x + this.anchors.leftMargin + leftTarget.width + (leftTarget.parent.x - this.parent.x)
                            })
                        }
                        // разные 
                    }
                    // this.x = this.anchors.left + this.anchors.leftMargin
                }
                if(this.anchors.right !== undefined){
                    let rightTarget = this.anchors.right.target
                    if(rightTarget === this.parent){
                        if(this.anchors.right.float === 'left'){
                            this.$sP('x', ()=>{
                                return -(this.width + this.anchors.rightMargin)
                            })
                        } else if(this.anchors.right.float === 'right'){
                            this.$sP('x', ()=>{
                                return rightTarget.width - (this.width + this.anchors.rightMargin)
                            })
                        }
                        
                    } else if(this.parent === rightTarget.parent ) {
                        if(this.anchors.right.float === 'left'){
                            this.$sP('x', ()=>{
                                return rightTarget.x - (this.width + this.anchors.rightMargin)
                            })
                        } else if(this.anchors.right.float === 'right'){
                            this.$sP('x', ()=>{
                                return rightTarget.x + rightTarget.width - (this.width + this.anchors.rightMargin)
                            })
                        }
                    } else {
                        if(this.anchors.right.float === 'left'){
                            this.$sP('x', ()=>{
                                return rightTarget.x - (this.width + this.anchors.rightMargin) + (rightTarget.parent.x - this.parent.x)
                            })
                        } else if(this.anchors.right.float === 'right'){
                            this.$sP('x', ()=>{
                                return rightTarget.x + rightTarget.width - (this.width + this.anchors.rightMargin) + (rightTarget.parent.x - this.parent.x)
                            })
                        }
                        // разные 
                    }
                    // this.x = this.anchors.right - this.width - this.anchors.rightMargin
                }
            }
            
            if(this.anchors.verticalCenter !== undefined){
                let verticalCenterTarget = this.anchors.verticalCenter.target
                if(verticalCenterTarget === this.parent){
                    this.$sP('y', ()=>{
                        return verticalCenterTarget.height / 2 - this.height / 2 + this.anchors.topMargin - this.anchors.bottomMargin
                    })
                } else if(this.parent === verticalCenterTarget.parent ) {
                    this.$sP('y', ()=>{
                        return verticalCenterTarget.y + verticalCenterTarget.height / 2 - this.height / 2 + this.anchors.topMargin - this.anchors.bottomMargin
                    })
                } else {
                    this.$sP('y', ()=>{
                        return verticalCenterTarget.y + verticalCenterTarget.height / 2 - this.height / 2 + this.anchors.topMargin - this.anchors.bottomMargin - (verticalCenterTarget.parent.height/2 - this.parent.height/2)
                    })
                    // разные 
                }
                // this.y = this.anchors.verticalCenter - this.height / 2 + this.anchors.topMargin - this.anchors.bottomMargin + this.anchors.verticalCenterOffset
            } else if(this.anchors.top !== undefined && this.anchors.bottom !== undefined){
                let topTarget = this.anchors.top.target
                let bottomTarget = this.anchors.bottom.target
                if(topTarget === this.parent){
                    if(this.anchors.top.float === 'top'){
                        this.$sP('y', ()=>{
                            return this.anchors.topMargin
                        })
                    } else if(this.anchors.top.float === 'bottom'){
                        this.$sP('y', ()=>{
                            return this.anchors.topMargin + topTarget.height
                        })
                    }
                    
                } else if(this.parent === topTarget.parent ) {
                    if(this.anchors.top.float === 'top'){
                        this.$sP('y', ()=>{
                            return topTarget.y + this.anchors.topMargin
                        })
                    } else if(this.anchors.top.float === 'bottom'){
                        this.$sP('y', ()=>{
                            return topTarget.y + this.anchors.topMargin + topTarget.height
                        })
                    }
                } else {
                    
                    if(this.anchors.top.float === 'top'){
                        this.$sP('y', ()=>{
                            return topTarget.y + this.anchors.topMargin + (topTarget.parent.y - this.parent.y)
                        })
                    } else if(this.anchors.top.float === 'bottom'){
                        this.$sP('y', ()=>{
                            return topTarget.y + this.anchors.topMargin + topTarget.height + (topTarget.parent.y - this.parent.y)
                        })
                    }
                    // разные 
                }
                if(bottomTarget === this.parent){
                    if(this.anchors.bottom.float === 'top'){
                        this.$sP('height', ()=>{
                            return this.anchors.bottomMargin - this.y
                        })
                    } else if(this.anchors.bottom.float === 'bottom'){
                        this.$sP('height', ()=>{
                            return bottomTarget.height - this.anchors.bottomMargin - this.y
                        })
                    }
                    
                } else if(this.parent === bottomTarget.parent ) {
                    if(this.anchors.bottom.float === 'top'){
                        this.$sP('height', ()=>{
                            return bottomTarget.y - this.anchors.bottomMargin - this.y
                        })
                    } else if(this.anchors.bottom.float === 'bottom'){
                        this.$sP('height', ()=>{
                            return bottomTarget.y + bottomTarget.height - this.anchors.bottomMargin - this.y
                        })
                    }
                } else {
                    if(this.anchors.bottom.float === 'top'){
                        this.$sP('height', ()=>{
                            return bottomTarget.y - this.anchors.bottomMargin - this.y + (bottomTarget.parent.y - this.parent.y)
                        })
                    } else if(this.anchors.bottom.float === 'bottom'){
                        this.$sP('height', ()=>{
                            return bottomTarget.y + bottomTarget.height - this.anchors.bottomMargin - this.y + (bottomTarget.parent.y - this.parent.y)
                        })
                    }
                    // разные 
                }
            } else {
                if(this.anchors.top !== undefined){
                    let topTarget = this.anchors.top.target
                    if(topTarget === this.parent){
                        if(this.anchors.top.float === 'top'){
                            this.$sP('y', ()=>{
                                return this.anchors.topMargin
                            })
                        } else if(this.anchors.top.float === 'bottom'){
                            this.$sP('y', ()=>{
                                return this.anchors.topMargin + topTarget.height
                            })
                        }
                        
                    } else if(this.parent === topTarget.parent ) {
                        if(this.anchors.top.float === 'top'){
                            this.$sP('y', ()=>{
                                return topTarget.y + this.anchors.topMargin
                            })
                        } else if(this.anchors.top.float === 'bottom'){
                            this.$sP('y', ()=>{
                                return topTarget.y + this.anchors.topMargin + topTarget.height
                            })
                        }
                    } else {
                        if(this.anchors.top.float === 'top'){
                            this.$sP('y', ()=>{
                                return topTarget.y + this.anchors.topMargin + (topTarget.parent.y - this.parent.y)
                            })
                        } else if(this.anchors.top.float === 'bottom'){
                            this.$sP('y', ()=>{
                                return topTarget.y + this.anchors.topMargin + topTarget.height + (topTarget.parent.y - this.parent.y)
                            })
                        }
                        // разные 
                    }
                    // this.y = this.anchors.top + this.anchors.topMargin
                }
                if(this.anchors.bottom !== undefined){
                    let bottomTarget = this.anchors.bottom.target
                    if(bottomTarget === this.parent){
                        if(this.anchors.bottom.float === 'top'){
                            this.$sP('y', ()=>{
                                return -(this.height + this.anchors.bottomMargin)
                            })
                        } else if(this.anchors.bottom.float === 'bottom'){
                            this.$sP('y', ()=>{
                                return bottomTarget.height - (this.height + this.anchors.bottomMargin)
                            })
                        }
                        
                    } else if(this.parent === bottomTarget.parent ) {
                        if(this.anchors.bottom.float === 'top'){
                            this.$sP('y', ()=>{
                                return bottomTarget.y - (this.height + this.anchors.bottomMargin)
                            })
                        } else if(this.anchors.bottom.float === 'bottom'){
                            this.$sP('y', ()=>{
                                return bottomTarget.y + bottomTarget.height - (this.height + this.anchors.bottomMargin)
                            })
                        }
                    } else {
                        if(this.anchors.bottom.float === 'top'){
                            this.$sP('y', ()=>{
                                return bottomTarget.y - (this.height + this.anchors.bottomMargin) + (bottomTarget.parent.y - this.parent.y)
                            })
                        } else if(this.anchors.bottom.float === 'bottom'){
                            this.$sP('y', ()=>{
                                return bottomTarget.y + bottomTarget.height - (this.height + this.anchors.bottomMargin) + (bottomTarget.parent.y - this.parent.y)
                            })
                        }
                        // разные 
                    }
                    // this.y = this.anchors.bottom - this.height - this.anchors.bottomMargin
                }
            }
        }
    }

    // $updateChildrenRect(){
    //     let left = 99999999
    //     let right = -99999999
    //     let top = 99999999
    //     let bottom = -99999999

    //     for(let child of this.children){
    //         if(child.dom){
    //             let cRect = child.dom.getBoundingClientRect()
    //             if(left > cRect.left) left = cRect.left
    //             if(right < cRect.right) right = cRect.right
    //             if(top > cRect.top) top = cRect.top
    //             if(bottom < cRect.bottom) bottom = cRect.bottom

    //         }
    //     }
    //     this.childrenRect = {
    //         x: left,
    //         y: top,
    //         width: right - left,
    //         height: bottom - top,
    //     }
    // }

    mapToItem(item, x, y){
        let dx = 0
        let dy = 0

        if(item){
            let itemParent = item.parent
            while(itemParent)
            {
                dx -= itemParent.x
                dy -= itemParent.y
                itemParent = itemParent.parent
            }
            dx += x + this.x - item.x
		    dy += y + this.y - item.y
        } else {
            
            dx += x + this.x
		    dy += y + this.y
        }

		let parent = this.parent
		
		
		while(parent && parent !== item)
		{
			dx += parent.x
			dy += parent.y
			parent = parent.parent
		}
		return {
			x: dx,
			y: dy,
		}
	}

}

QML.Item = Item