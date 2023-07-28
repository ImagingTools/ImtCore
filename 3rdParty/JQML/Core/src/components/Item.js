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
        
        this.$cP('left', {'target': this, 'float': 'left'})
        this.$cP('right', {'target': this, 'float': 'right'})
        this.$cP('top', {'target': this, 'float': 'top'})
        this.$cP('bottom', {'target': this, 'float': 'bottom'})
        this.$cP('horizontalCenter', {'target': this, 'float': 'horizontalCenter'})
        this.$cP('verticalCenter', {'target': this, 'float': 'verticalCenter'})
        this.$cP('childrenRect', {})


        this.$cP('x', 0, this.$xChanged)
        this.$cP('y', 0, this.$yChanged)
        this.$cP('z', 0, this.$zChanged)
        this.$cP('visible', true, this.$visibleChanged)
        if(this.parent) this.$sP('visible', ()=>{return this.parent.visible})
        this.$cP('clip', false, this.$clipChanged)
        this.$cP('opacity', ()=>{
            let parent = this.parent
            while(parent && !parent.dom){
                parent = parent.parent
            }
            if(parent && parent.dom){
                return parent.opacity
            } else {
                return 1.0
            }
        }, this.$opacityChanged)
        this.$cP('enabled', true, this.$enabledChanged)
        this.$cP('focus', false, this.$focusChanged)
        this.$cP('activeFocus', ()=>{return this.focus}, this.$focusChanged)
        this.$cP('width', 0, this.$widthChanged)
        this.$cP('height', 0, this.$heightChanged)
        this.$cP('scale', 1.0, this.$rotationAndScaleChanged)
        this.$cP('rotation', 0, this.$rotationAndScaleChanged)
        this.$cP('transformOrigin', Item.Center, this.$transformChanged)

        // if(this.parent && this.parent.$qmlClassName === 'Loader'){
        //     this.$sP('width', ()=>{return this.parent.width})
        //     this.$sP('height', ()=>{return this.parent.height})
        // }

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
        }, this.$anchorsChanged)

        this.$cS('Keys.asteriskPressed')
        this.$cS('Keys.backPressed')
        this.$cS('Keys.backtabPressed')
        this.$cS('Keys.callPressed')
        this.$cS('Keys.cancelPressed')
        this.$cS('Keys.context1Pressed')
        this.$cS('Keys.context2Pressed')
        this.$cS('Keys.context3Pressed')
        this.$cS('Keys.context4Pressed')
        this.$cS('Keys.deletePressed')
        this.$cS('Keys.digit0Pressed')
        this.$cS('Keys.digit1Pressed')
        this.$cS('Keys.digit2Pressed')
        this.$cS('Keys.digit3Pressed')
        this.$cS('Keys.digit4Pressed')
        this.$cS('Keys.digit5Pressed')
        this.$cS('Keys.digit6Pressed')
        this.$cS('Keys.digit7Pressed')
        this.$cS('Keys.digit8Pressed')
        this.$cS('Keys.digit9Pressed')
        this.$cS('Keys.downPressed')
        this.$cS('Keys.enterPressed')
        this.$cS('Keys.escapePressed')
        this.$cS('Keys.flipPressed')
        this.$cS('Keys.hangupPressed')
        this.$cS('Keys.leftPressed')
        this.$cS('Keys.menuPressed')
        this.$cS('Keys.noPressed')
        this.$cS('Keys.pressed')
        this.$cS('Keys.released')
        this.$cS('Keys.returnPressed')
        this.$cS('Keys.rightPressed')
        this.$cS('Keys.selectPressed')
        this.$cS('Keys.shortcutOverride')
        this.$cS('Keys.spacePressed')
        this.$cS('Keys.tabPressed')
        this.$cS('Keys.upPressed')
        this.$cS('Keys.volumeDownPressed')
        this.$cS('Keys.volumeUpPressed')
        this.$cS('Keys.yesPressed')
    }
    $domCreate(tag = 'div'){
        super.$domCreate()
        if(this.parent) this.parent.$availableGeometry.push(this)
        this.dom = document.createElement(tag)

        // if(this.UID !== 0)
        // Core.root.dom.appendChild(this.dom)
        
        
        this.dom.style.position = 'absolute'
        this.dom.style.left = '0'
        this.dom.style.top = '0'
        this.dom.style.overflow = 'unset'
        this.dom.style.zIndex = this.z
        this.dom.classList.add(this.constructor.name)
        this.dom.id = `el-${this.UID}`

        // this.dom.addEventListener("focusin", (e)=>{
        //     e.preventDefault()
		// 	this.focus = true
		// })
		// this.dom.addEventListener("focusout", (e)=>{
        //     e.preventDefault()
		// 	this.focus = false
		// })
        if(this.parent && this.parent.dom){
            this.parent.dom.appendChild(this.dom)
            this.parent.$childDomChanged(this)
        }
    }

    $destroy() {
        this.dom.remove()
        super.$destroy()
    }

    setFocus(param){
        this.focus =  param === true ? true : false
    }
    $xChanged(){
        // let x = this.x
        // if(!this.$anchorsX){
        //     let parent = this.parent
        //     while(parent){
        //         x += parent.x
        //         parent = parent.parent
        //     }
        // }
        if(!this.$p.x.preventDefault) this.dom.style.left = `${this.x}px`
    }
    $yChanged(){
        // let y = this.y
        // if(!this.$anchorsY){
        //     let parent = this.parent
        //     while(parent){
        //         y += parent.y
        //         parent = parent.parent
        //     }
        // }
        if(!this.$p.y.preventDefault) this.dom.style.top = `${this.y}px`
    }
    $zChanged(){
        this.dom.style.zIndex = this.z
    }
    $widthChanged(){
        if(!this.$p.width.preventDefault) {
            this.dom.style.width = `${this.width}px`
            this.dom.style.minWidth = `${this.width}px`
        }
    }
    $heightChanged(){
        if(!this.$p.height.preventDefault) {
            this.dom.style.height = `${this.height}px`
            this.dom.style.minHeight = `${this.height}px`
        }
    }
    $visibleChanged(){
        if(this.visible) this.$cssDisplay = this.dom.style.display
        // this.dom.style.opacity = this.visible ? this.opacity : 0
        this.dom.style.visibility = this.visible ? 'visible' : 'hidden'
        this.dom.style.zIndex = this.visible ? /*2147483646/2 +*/ this.z : -1

        // for(let i = this.children.length-1; i >= 0; i--){
        //     if(this.children[i].$p.visible && this.children[i].$qmlClassName !== 'ListElement' && this.children[i].$qmlClassName !== 'FileDialog'){
        //         if(this.visible){
        //             this.children[i].$p.visible.blocked = false
        //             if(this.children[i].$p.visible.func) {
        //                 caller = this.children[i].$p.visible
        //                 this.children[i].visible = this.children[i].$p.visible.func()
        //                 caller = null
                        
        //             } else if('blockedVal' in this.children[i].$p.visible){
        //                 this.children[i].visible = this.children[i].$p.visible.blockedVal
        //             } else {
        //                 this.children[i].visible = this.visible
        //             }
        //         } else {
        //             this.children[i].$p.visible.blockedVal = this.children[i].$p.visible.val
        //             this.children[i].$p.visible.blocked = false
        //             this.children[i].visible = this.visible
        //             this.children[i].$p.visible.blocked = true
        //         }
                
                
        //     }
        // }

        if(this.parent && (this.parent.$qmlClassName === 'Row' || this.parent.$qmlClassName === 'Column')){
            this.parent.$childChanged(this)
        }
    }
    $clipChanged(){
        this.dom.style.overflow = this.clip ? "hidden" : "unset"
    }
    $opacityChanged(){
        // this.dom.style.opacity = this.opacity
    }
    $enabledChanged(){
       
    }
    $focusChanged(){
        // this.dom.focus()
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
        this.$p.x.freeze = false
        this.$p.y.freeze = false
        this.$p.width.freeze = false
        this.$p.height.freeze = false

        if(this.anchors.fill && (this.anchors.fill.$qmlClassName === 'Flickable' || this.anchors.fill.$qmlClassName === 'ListView' || this.anchors.fill.$qmlClassName === 'GridView')) this.$p['anchors.fill'].val = null
        if(this.anchors.centerIn && (this.anchors.centerIn.$qmlClassName === 'Flickable' || this.anchors.centerIn.$qmlClassName === 'ListView' || this.anchors.centerIn.$qmlClassName === 'GridView')) this.$p['anchors.centerIn'].val = null
        if(this.anchors.left && (this.anchors.left.$qmlClassName === 'Flickable' || this.anchors.left.$qmlClassName === 'ListView' || this.anchors.left.$qmlClassName === 'GridView')) this.$p['anchors.left'].val = null
        if(this.anchors.right && (this.anchors.right.$qmlClassName === 'Flickable' || this.anchors.right.$qmlClassName === 'ListView' || this.anchors.right.$qmlClassName === 'GridView')) this.$p['anchors.right'].val = null
        if(this.anchors.top && (this.anchors.top.$qmlClassName === 'Flickable' || this.anchors.top.$qmlClassName === 'ListView' || this.anchors.top.$qmlClassName === 'GridView')) this.$p['anchors.top'].val = null
        if(this.anchors.bottom && (this.anchors.bottom.$qmlClassName === 'Flickable' || this.anchors.bottom.$qmlClassName === 'ListView' || this.anchors.bottom.$qmlClassName === 'GridView')) this.$p['anchors.bottom'].val = null
        if(this.anchors.horizontalCenter && (this.anchors.horizontalCenter.$qmlClassName === 'Flickable' || this.anchors.horizontalCenter.$qmlClassName === 'ListView' || this.anchors.horizontalCenter.$qmlClassName === 'GridView')) this.$p['anchors.horizontalCenter'].val = null
        if(this.anchors.verticalCenter && (this.anchors.verticalCenter.$qmlClassName === 'Flickable' || this.anchors.verticalCenter.$qmlClassName === 'ListView' || this.anchors.verticalCenter.$qmlClassName === 'GridView')) this.$p['anchors.verticalCenter'].val = null

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
                this.$p.x.freeze = true
                this.$p.y.freeze = true
                this.$p.width.freeze = true
                this.$p.height.freeze = true
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
                this.$p.x.freeze = true
                this.$p.y.freeze = true
                this.$p.width.freeze = true
                this.$p.height.freeze = true
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
                this.$p.x.freeze = true
                this.$p.y.freeze = true
                this.$p.width.freeze = true
                this.$p.height.freeze = true
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
                this.$p.x.freeze = true
                this.$p.y.freeze = true
            } else if(this.parent === this.anchors.centerIn.parent ) {
                this.$sP('x', ()=>{
                    return this.anchors.centerIn.x + this.anchors.centerIn.width / 2 - this.width / 2 + this.anchors.leftMargin - this.anchors.rightMargin
                })
                this.$sP('y', ()=>{
                    return this.anchors.centerIn.y + this.anchors.centerIn.height / 2 - this.height / 2 + this.anchors.topMargin - this.anchors.bottomMargin
                })
                this.$p.x.freeze = true
                this.$p.y.freeze = true
            } else {
                this.$sP('x', ()=>{
                    return this.anchors.centerIn.x + this.anchors.centerIn.width / 2 - this.width / 2 + this.anchors.leftMargin - this.anchors.rightMargin - (this.anchors.centerIn.parent.width/2 - this.parent.width/2)
                })
                this.$sP('y', ()=>{
                    return this.anchors.centerIn.y + this.anchors.centerIn.height / 2 - this.height / 2 + this.anchors.topMargin - this.anchors.bottomMargin - (this.anchors.centerIn.parent.height/2 - this.parent.height/2)
                })
                this.$p.x.freeze = true
                this.$p.y.freeze = true
                // разные 
            }
            
        } else {
            if(this.anchors.horizontalCenter !== undefined){
                let horizontalCenterTarget = this.anchors.horizontalCenter.target
                if(horizontalCenterTarget === this.parent){
                    this.$sP('x', ()=>{
                        return horizontalCenterTarget.width / 2 - this.width / 2 + this.anchors.leftMargin - this.anchors.rightMargin
                    })
                    this.$p.x.freeze = true
                } else if(this.parent === horizontalCenterTarget.parent ) {
                    this.$sP('x', ()=>{
                        return horizontalCenterTarget.x + horizontalCenterTarget.width / 2 - this.width / 2 + this.anchors.leftMargin - this.anchors.rightMargin
                    })
                    this.$p.x.freeze = true
                } else {
                    this.$sP('x', ()=>{
                        return horizontalCenterTarget.x + horizontalCenterTarget.width / 2 - this.width / 2 + this.anchors.leftMargin - this.anchors.rightMargin - (horizontalCenterTarget.parent.width/2 - this.parent.width/2)
                    })
                    this.$p.x.freeze = true
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
                        this.$p.x.freeze = true
                    } else if(this.anchors.left.float === 'right'){
                        this.$sP('x', ()=>{
                            return this.anchors.leftMargin + leftTarget.width
                        })
                        this.$p.x.freeze = true
                    }
                    
                } else if(this.parent === leftTarget.parent ) {
                    if(this.anchors.left.float === 'left'){
                        this.$sP('x', ()=>{
                            return leftTarget.x + this.anchors.leftMargin
                        })
                        this.$p.x.freeze = true
                    } else if(this.anchors.left.float === 'right'){
                        this.$sP('x', ()=>{
                            return leftTarget.x + this.anchors.leftMargin + leftTarget.width
                        })
                        this.$p.x.freeze = true
                    }
                } else {
                    if(this.anchors.left.float === 'left'){
                        this.$sP('x', ()=>{
                            return leftTarget.x + this.anchors.leftMargin + (leftTarget.parent.x - this.parent.x)
                        })
                        this.$p.x.freeze = true
                    } else if(this.anchors.left.float === 'right'){
                        this.$sP('x', ()=>{
                            return leftTarget.x + this.anchors.leftMargin + leftTarget.width + (leftTarget.parent.x - this.parent.x)
                        })
                        this.$p.x.freeze = true
                    }
                    // разные 
                }
                if(rightTarget === this.parent){
                    if(this.anchors.right.float === 'left'){
                        this.$sP('width', ()=>{
                            return this.anchors.rightMargin - this.x
                        })
                        this.$p.width.freeze = true
                    } else if(this.anchors.right.float === 'right'){
                        this.$sP('width', ()=>{
                            return rightTarget.width - this.anchors.rightMargin - this.x
                        })
                        this.$p.width.freeze = true
                    }
                    
                } else if(this.parent === rightTarget.parent ) {
                    if(this.anchors.right.float === 'left'){
                        this.$sP('width', ()=>{
                            return rightTarget.x - this.anchors.rightMargin - this.x
                        })
                        this.$p.width.freeze = true
                    } else if(this.anchors.right.float === 'right'){
                        this.$sP('width', ()=>{
                            return rightTarget.x + rightTarget.width - this.anchors.rightMargin - this.x
                        })
                        this.$p.width.freeze = true
                    }
                } else {
                    if(this.anchors.right.float === 'left'){
                        this.$sP('width', ()=>{
                            return rightTarget.x - this.anchors.rightMargin - this.x + (rightTarget.parent.x - this.parent.x)
                        })
                        this.$p.width.freeze = true
                    } else if(this.anchors.right.float === 'right'){
                        this.$sP('width', ()=>{
                            return rightTarget.x + rightTarget.width - this.anchors.rightMargin - this.x + (rightTarget.parent.x - this.parent.x)
                        })
                        this.$p.width.freeze = true
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
                            this.$p.x.freeze = true
                        } else if(this.anchors.left.float === 'right'){
                            this.$sP('x', ()=>{
                                return this.anchors.leftMargin + leftTarget.width
                            })
                            this.$p.x.freeze = true
                        }
                        
                    } else if(this.parent === leftTarget.parent ) {
                        if(this.anchors.left.float === 'left'){
                            this.$sP('x', ()=>{
                                return leftTarget.x + this.anchors.leftMargin
                            })
                            this.$p.x.freeze = true
                        } else if(this.anchors.left.float === 'right'){
                            this.$sP('x', ()=>{
                                return leftTarget.x + this.anchors.leftMargin + leftTarget.width
                            })
                            this.$p.x.freeze = true
                        }
                    } else {
                        if(this.anchors.left.float === 'left'){
                            this.$sP('x', ()=>{
                                return leftTarget.x + this.anchors.leftMargin + (leftTarget.parent.x - this.parent.x)
                            })
                            this.$p.x.freeze = true
                        } else if(this.anchors.left.float === 'right'){
                            this.$sP('x', ()=>{
                                return leftTarget.x + this.anchors.leftMargin + leftTarget.width + (leftTarget.parent.x - this.parent.x)
                            })
                            this.$p.x.freeze = true
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
                            this.$p.x.freeze = true
                        } else if(this.anchors.right.float === 'right'){
                            this.$sP('x', ()=>{
                                return rightTarget.width - (this.width + this.anchors.rightMargin)
                            })
                            this.$p.x.freeze = true
                        }
                        
                    } else if(this.parent === rightTarget.parent ) {
                        if(this.anchors.right.float === 'left'){
                            this.$sP('x', ()=>{
                                return rightTarget.x - (this.width + this.anchors.rightMargin)
                            })
                            this.$p.x.freeze = true
                        } else if(this.anchors.right.float === 'right'){
                            this.$sP('x', ()=>{
                                return rightTarget.x + rightTarget.width - (this.width + this.anchors.rightMargin)
                            })
                            this.$p.x.freeze = true
                        }
                    } else {
                        if(this.anchors.right.float === 'left'){
                            this.$sP('x', ()=>{
                                return rightTarget.x - (this.width + this.anchors.rightMargin) + (rightTarget.parent.x - this.parent.x)
                            })
                            this.$p.x.freeze = true
                        } else if(this.anchors.right.float === 'right'){
                            this.$sP('x', ()=>{
                                return rightTarget.x + rightTarget.width - (this.width + this.anchors.rightMargin) + (rightTarget.parent.x - this.parent.x)
                            })
                            this.$p.x.freeze = true
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
                    this.$p.y.freeze = true
                } else if(this.parent === verticalCenterTarget.parent ) {
                    this.$sP('y', ()=>{
                        return verticalCenterTarget.y + verticalCenterTarget.height / 2 - this.height / 2 + this.anchors.topMargin - this.anchors.bottomMargin
                    })
                    this.$p.y.freeze = true
                } else {
                    this.$sP('y', ()=>{
                        return verticalCenterTarget.y + verticalCenterTarget.height / 2 - this.height / 2 + this.anchors.topMargin - this.anchors.bottomMargin - (verticalCenterTarget.parent.height/2 - this.parent.height/2)
                    })
                    this.$p.y.freeze = true
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
                        this.$p.y.freeze = true
                    } else if(this.anchors.top.float === 'bottom'){
                        this.$sP('y', ()=>{
                            return this.anchors.topMargin + topTarget.height
                        })
                        this.$p.y.freeze = true
                    }
                    
                } else if(this.parent === topTarget.parent ) {
                    if(this.anchors.top.float === 'top'){
                        this.$sP('y', ()=>{
                            return topTarget.y + this.anchors.topMargin
                        })
                        this.$p.y.freeze = true
                    } else if(this.anchors.top.float === 'bottom'){
                        this.$sP('y', ()=>{
                            return topTarget.y + this.anchors.topMargin + topTarget.height
                        })
                        this.$p.y.freeze = true
                    }
                } else {
                    
                    if(this.anchors.top.float === 'top'){
                        this.$sP('y', ()=>{
                            return topTarget.y + this.anchors.topMargin + (topTarget.parent.y - this.parent.y)
                        })
                        this.$p.y.freeze = true
                    } else if(this.anchors.top.float === 'bottom'){
                        this.$sP('y', ()=>{
                            return topTarget.y + this.anchors.topMargin + topTarget.height + (topTarget.parent.y - this.parent.y)
                        })
                        this.$p.y.freeze = true
                    }
                    // разные 
                }
                if(bottomTarget === this.parent){
                    if(this.anchors.bottom.float === 'top'){
                        this.$sP('height', ()=>{
                            return this.anchors.bottomMargin - this.y
                        })
                        this.$p.height.freeze = true
                    } else if(this.anchors.bottom.float === 'bottom'){
                        this.$sP('height', ()=>{
                            return bottomTarget.height - this.anchors.bottomMargin - this.y
                        })
                        this.$p.height.freeze = true
                    }
                    
                } else if(this.parent === bottomTarget.parent ) {
                    if(this.anchors.bottom.float === 'top'){
                        this.$sP('height', ()=>{
                            return bottomTarget.y - this.anchors.bottomMargin - this.y
                        })
                        this.$p.height.freeze = true
                    } else if(this.anchors.bottom.float === 'bottom'){
                        this.$sP('height', ()=>{
                            return bottomTarget.y + bottomTarget.height - this.anchors.bottomMargin - this.y
                        })
                        this.$p.height.freeze = true
                    }
                } else {
                    if(this.anchors.bottom.float === 'top'){
                        this.$sP('height', ()=>{
                            return bottomTarget.y - this.anchors.bottomMargin - this.y + (bottomTarget.parent.y - this.parent.y)
                        })
                        this.$p.height.freeze = true
                    } else if(this.anchors.bottom.float === 'bottom'){
                        this.$sP('height', ()=>{
                            return bottomTarget.y + bottomTarget.height - this.anchors.bottomMargin - this.y + (bottomTarget.parent.y - this.parent.y)
                        })
                        this.$p.height.freeze = true
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
                            this.$p.y.freeze = true
                        } else if(this.anchors.top.float === 'bottom'){
                            this.$sP('y', ()=>{
                                return this.anchors.topMargin + topTarget.height
                            })
                            this.$p.y.freeze = true
                        }
                        
                    } else if(this.parent === topTarget.parent ) {
                        if(this.anchors.top.float === 'top'){
                            this.$sP('y', ()=>{
                                return topTarget.y + this.anchors.topMargin
                            })
                            this.$p.y.freeze = true
                        } else if(this.anchors.top.float === 'bottom'){
                            this.$sP('y', ()=>{
                                return topTarget.y + this.anchors.topMargin + topTarget.height
                            })
                            this.$p.y.freeze = true
                        }
                    } else {
                        if(this.anchors.top.float === 'top'){
                            this.$sP('y', ()=>{
                                return topTarget.y + this.anchors.topMargin + (topTarget.parent.y - this.parent.y)
                            })
                            this.$p.y.freeze = true
                        } else if(this.anchors.top.float === 'bottom'){
                            this.$sP('y', ()=>{
                                return topTarget.y + this.anchors.topMargin + topTarget.height + (topTarget.parent.y - this.parent.y)
                            })
                            this.$p.y.freeze = true
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
                            this.$p.y.freeze = true
                        } else if(this.anchors.bottom.float === 'bottom'){
                            this.$sP('y', ()=>{
                                return bottomTarget.height - (this.height + this.anchors.bottomMargin)
                            })
                            this.$p.y.freeze = true
                        }
                        
                    } else if(this.parent === bottomTarget.parent ) {
                        if(this.anchors.bottom.float === 'top'){
                            this.$sP('y', ()=>{
                                return bottomTarget.y - (this.height + this.anchors.bottomMargin)
                            })
                            this.$p.y.freeze = true
                        } else if(this.anchors.bottom.float === 'bottom'){
                            this.$sP('y', ()=>{
                                return bottomTarget.y + bottomTarget.height - (this.height + this.anchors.bottomMargin)
                            })
                            this.$p.y.freeze = true
                        }
                    } else {
                        if(this.anchors.bottom.float === 'top'){
                            this.$sP('y', ()=>{
                                return bottomTarget.y - (this.height + this.anchors.bottomMargin) + (bottomTarget.parent.y - this.parent.y)
                            })
                            this.$p.y.freeze = true
                        } else if(this.anchors.bottom.float === 'bottom'){
                            this.$sP('y', ()=>{
                                return bottomTarget.y + bottomTarget.height - (this.height + this.anchors.bottomMargin) + (bottomTarget.parent.y - this.parent.y)
                            })
                            this.$p.y.freeze = true
                        }
                        // разные 
                    }
                    
                    // this.y = this.anchors.bottom - this.height - this.anchors.bottomMargin
                }
            }

        }
        if(this.$completed) this.$uP()
        
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
        let rect1 = this.dom.getBoundingClientRect()
        let res = {
            x: rect1.x + x,
			y: rect1.y + y,
        }
        if(item){
            let rect2 = item.dom.getBoundingClientRect()
            res.x -= rect2.x
            res.y -= rect2.y
            return res
        } else {
            return res
        }
        


        // let dx = 0
        // let dy = 0

        // if(item){
        //     let itemParent = item.parent
        //     while(itemParent)
        //     {
        //         dx -= itemParent.x
        //         dy -= itemParent.y
        //         itemParent = itemParent.parent
        //     }
        //     dx += x + this.x - item.x
		//     dy += y + this.y - item.y
        // } else {
            
        //     dx += x + this.x
		//     dy += y + this.y
        // }

		// let parent = this.parent
		
		
		// while(parent && parent !== item)
		// {
		// 	dx += parent.x
		// 	dy += parent.y
		// 	parent = parent.parent
		// }
		// return {
		// 	x: dx,
		// 	y: dy,
		// }
	}

}

QML.Item = Item