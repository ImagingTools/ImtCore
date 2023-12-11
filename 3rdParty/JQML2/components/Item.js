const { QtObject } = require('./QtObject')
const { QReal, QAnchors, QAnchorLine, QAutoGeometry, QBool, QInt, QProperty, QKeyNavigation, QVisible } = require('../utils/properties')

class Item extends QtObject {
    static defaultCSS = {
        display: 'flex'
    }

    static TopLeft = 0
    static Top = 1
    static TopRight = 2
    static Right = 3
    static BottomRight = 4
    static Bottom = 5
    static BottomLeft = 6
    static Left = 7
    static Center = 8

    static defaultProperties = {
        x: { type: QAutoGeometry, value: 0, changed: '$xChanged' },
        y: { type: QAutoGeometry, value: 0, changed: '$yChanged' },
        z: { type: QInt, value: 0, changed: '$zChanged' },
        width: { type: QAutoGeometry, value: 0, changed: '$widthChanged' },
        height: { type: QAutoGeometry, value: 0, changed: '$heightChanged' },
        visible: { type: QVisible, value: true, changed: '$visibleChanged' },
        enabled: { type: QBool, value: true, changed: '$enabledChanged' },
        clip: { type: QBool, value: false, changed: '$clipChanged' },
        focus: { type: QBool, value: false, changed: '$focusChanged' },
        activeFocus: { type: QBool, value: false, changed: '$activeFocusChanged' },
        opacity: { type: QReal, value: 1, changed: '$opacityChanged' },
        scale: { type: QReal, value: 1, changed: '$rotationAndScaleChanged' },
        rotation: { type: QReal, value: 0, changed: '$rotationAndScaleChanged' },
        transformOrigin: { type: QProperty, value: Item.Center, changed: '$transformOriginChanged' },
        anchors: { type: QAnchors, changed: '$anchorsChanged' },
        KeyNavigation: { type: QKeyNavigation, changed: '$keyNavigationChanged' },

        // override getProperty
        left: { type: QAnchorLine, value: QAnchorLine.Left},
        right: { type: QAnchorLine, value: QAnchorLine.Right},
        top: { type: QAnchorLine, value: QAnchorLine.Top},
        bottom: { type: QAnchorLine, value: QAnchorLine.Bottom},
        horizontalCenter: { type: QAnchorLine, value: QAnchorLine.HorizontalCenter},
        verticalCenter: { type: QAnchorLine, value: QAnchorLine.VerticalCenter},
    }

    static defaultSignals = {
        /* Keys */
        'Keys.asteriskPressed': { params: [] },
        'Keys.backPressed': { params: [] },
        'Keys.backtabPressed': { params: [] },
        'Keys.callPressed': { params: [] },
        'Keys.cancelPressed': { params: [] },
        'Keys.context1Pressed': { params: [] },
        'Keys.context2Pressed': { params: [] },
        'Keys.context3Pressed': { params: [] },
        'Keys.context4Pressed': { params: [] },
        'Keys.deletePressed': { params: [] },
        'Keys.digit0Pressed': { params: [] },
        'Keys.digit1Pressed': { params: [] },
        'Keys.digit2Pressed': { params: [] },
        'Keys.digit3Pressed': { params: [] },
        'Keys.digit4Pressed': { params: [] },
        'Keys.digit5Pressed': { params: [] },
        'Keys.digit6Pressed': { params: [] },
        'Keys.digit7Pressed': { params: [] },
        'Keys.digit8Pressed': { params: [] },
        'Keys.digit9Pressed': { params: [] },
        'Keys.downPressed': { params: [] },
        'Keys.enterPressed': { params: [] },
        'Keys.escapePressed': { params: [] },
        'Keys.flipPressed': { params: [] },
        'Keys.hangupPressed': { params: [] },
        'Keys.leftPressed': { params: [] },
        'Keys.menuPressed': { params: [] },
        'Keys.noPressed': { params: [] },
        'Keys.pressed': { params: [] },
        'Keys.released': { params: [] },
        'Keys.returnPressed': { params: [] },
        'Keys.rightPressed': { params: [] },
        'Keys.selectPressed': { params: [] },
        'Keys.shortcutOverride': { params: [] },
        'Keys.spacePressed': { params: [] },
        'Keys.tabPressed': { params: [] },
        'Keys.upPressed': { params: [] },
        'Keys.volumeDownPressed': { params: [] },
        'Keys.volumeUpPressed': { params: [] },
        'Keys.yesPressed': { params: [] },
        /* Keys */
    }

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.createDom('div', {
            position: 'absolute',
            display: 'flex',
            overflow : 'unset',
            zIndex: 0,
        })
        

        if(parent){
            // if(parent.$loader){
            //     this.getProperty('width').setCompute(()=>{
            //         this.getProperty('width').subscribe(parent.getProperty('width'))
            //         return parent.getProperty('width').get()
            //     })
            //     this.getProperty('height').setCompute(()=>{
            //         this.getProperty('height').subscribe(parent.getProperty('height'))
            //         return parent.getProperty('height').get()
            //     })
            //     this.getProperty('width').update()
            //     this.getProperty('height').update()
            // }

            this.getProperty('visible').setOriginCompute(()=>{this.getProperty('visible').subscribe(parent.getProperty('visible')); return parent.getProperty('visible').get()})
            this.getProperty('visible').update()
        }
    }
    
    createDom(tag = 'div', style){
        if(this.parent && this.parent.$dom){
            this.$dom = document.createElement(tag)
            this.parent.$dom.appendChild(this.$dom)
            this.setStyle(style)
            this.parent.addDomChild(this)
            this.$dom.id = this.UID
            this.$dom.classList.add(this.constructor.name)
        }
    }

    addDomChild(child){

    }

    setStyle(style){
        if(style){
            for(let name in style){
                this.$dom.style[name] = style[name]
            }
        }
    }

    getProperty(name){
        if(this.$properties[name]){
            return this.$properties[name]
        } else {
            if(Item.defaultProperties[name] && Item.defaultProperties[name].type === QAnchorLine){
                this.$properties[name] = new QAnchorLine(this, Item.defaultProperties[name].value)
                return this.$properties[name]
            } else {
                return super.getProperty(name)
            }
        }
    }

    getDom(){
        return this.$dom
    }

    setDom(element){
        this.$dom = element
    }

    mapToItem(item, x, y){
        let rect1 = this.getDom().getBoundingClientRect()
        let res = {
            x: rect1.x + x,
			y: rect1.y + y,
        }
        if(item){
            let rect2 = item.getDom().getBoundingClientRect()
            res.x -= rect2.x
            res.y -= rect2.y
            return res
        } else {
            return res
        }
    }

    forceActiveFocus(){
        
    }

    $clipChanged(){
        this.setStyle({ overflow: this.getPropertyValue('clip') ? "hidden" : "unset" })
    }

    $rotationAndScaleChanged(){
        this.setStyle({ transform: `scale(${this.getPropertyValue('scale')}) rotate(${this.getPropertyValue('rotation')}deg)` })
    }
    $transformOriginChanged(){
        switch(this.getPropertyValue('transformOrigin')){
            case Item.Center: this.setStyle({ transformOrigin: 'center' }); break;
            case Item.TopLeft: this.setStyle({ transformOrigin: 'top left' }); break;
            case Item.Top: this.setStyle({ transformOrigin: 'top' }); break;
            case Item.TopRight: this.setStyle({ transformOrigin: 'top right'}); break;
            case Item.Right: this.setStyle({ transformOrigin: 'right' }); break;
            case Item.BottomRight: this.setStyle({ transformOrigin: 'bottom right' }); break;
            case Item.Bottom: this.setStyle({ transformOrigin: 'bottom' }); break;
            case Item.BottomLeft: this.setStyle({ transformOrigin: 'bottom left' }); break;
            case Item.Left: this.setStyle({ transformOrigin: 'left' }); break;
        }
        
    }

    $focusChanged(){
        
    }

    $activeFocusChanged(){
        
    }

    $enabledChanged(){

    }

    $opacityChanged(){
       
    }

    $keyNavigationChanged(){
        
    }

    $visibleChanged(){
        this.setStyle({ display: this.getProperty('visible').get() ? Item.defaultCSS.display : 'none' })
    }

    $xChanged(){
        this.$dom.style.left = `${this.getProperty('x').get()}px`
    }

    $yChanged(){
        this.$dom.style.top = `${this.getProperty('y').get()}px`
    }

    $zChanged(){
        this.setStyle({ zIndex: this.getPropertyValue('z') })
    }

    $widthChanged(){
        this.setStyle({
            width: `${this.getProperty('width').get()}px`,
            minWidth: `${this.getProperty('width').get()}px`,
        })
        // this.$dom.style.width = `${this.getProperty('width').get() > 0 ? this.getProperty('width').get() : 0}px`
    }

    $heightChanged(){
        this.setStyle({
            height: `${this.getProperty('height').get()}px`,
            minHeight: `${this.getProperty('height').get()}px`,
        })
        // this.$dom.style.height = `${this.getProperty('height').get() > 0 ? this.getProperty('height').get() : 0}px`
    }

    $anchorsChanged(){
        this.getProperty('x').unfreeze()
        this.getProperty('width').unfreeze()
        this.getProperty('y').unfreeze()
        this.getProperty('height').unfreeze()

        if(this.getProperty('anchors').getPropertyValue('fill')){
            this.getProperty('x').freeze()
            this.getProperty('width').freeze()
            this.getProperty('y').freeze()
            this.getProperty('height').freeze()

            if(this.parent === this.getProperty('anchors').getProperty('fill').get()){
                this.getProperty('x').setCompute(()=>{
                    this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('leftMargin'))
                    return this.getProperty('anchors').getProperty('leftMargin').get()
                })
                this.getProperty('y').setCompute(()=>{
                    this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('topMargin'))
                    return this.getProperty('anchors').getProperty('topMargin').get()
                })
            } else {
                this.getProperty('x').setCompute(()=>{
                    this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('fill').get().getProperty('x'), this.getProperty('anchors').getProperty('leftMargin'))
                    return this.getProperty('anchors').getProperty('fill').get().getProperty('x').get() + this.getProperty('anchors').getProperty('leftMargin').get()
                })
                this.getProperty('y').setCompute(()=>{
                    this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('fill').get().getProperty('y'), this.getProperty('anchors').getProperty('topMargin'))
                    return this.getProperty('anchors').getProperty('fill').get().getProperty('y').get() + this.getProperty('anchors').getProperty('topMargin').get()
                })
            }
            this.getProperty('width').setCompute(()=>{
                this.getProperty('width').subscribePrimary(this.getProperty('anchors').getProperty('fill').get().getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'), this.getProperty('anchors').getProperty('rightMargin'))
                return this.getProperty('anchors').getProperty('fill').get().getProperty('width').get() - this.getProperty('anchors').getProperty('leftMargin').get() - this.getProperty('anchors').getProperty('rightMargin').get()
            })
            this.getProperty('height').setCompute(()=>{
                this.getProperty('height').subscribePrimary(this.getProperty('anchors').getProperty('fill').get().getProperty('height'), this.getProperty('anchors').getProperty('topMargin'), this.getProperty('anchors').getProperty('bottomMargin')) 
                return this.getProperty('anchors').getProperty('fill').get().getProperty('height').get() - this.getProperty('anchors').getProperty('topMargin').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
            })
            this.getProperty('x').update()
            this.getProperty('y').update()
            this.getProperty('width').update()
            this.getProperty('height').update()
        } else if(this.getProperty('anchors').getPropertyValue('centerIn')){
            this.getProperty('x').freeze()
            this.getProperty('y').freeze()

            if(this.parent === this.getProperty('anchors').getProperty('centerIn').get()){
                this.getProperty('x').setCompute(()=>{
                    this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('centerIn').get().getProperty('width'), this.getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'), this.getProperty('anchors').getProperty('rightMargin'))
                    return this.getProperty('anchors').getProperty('centerIn').get().getProperty('width').get() / 2 - this.getProperty('width').get() / 2
                })
                this.getProperty('y').setCompute(()=>{this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('centerIn').get().getProperty('height'), this.getProperty('height'), this.getProperty('anchors').getProperty('topMargin'), this.getProperty('anchors').getProperty('bottomMargin'))
                    return this.getProperty('anchors').getProperty('centerIn').get().getProperty('height').get() / 2 - this.getProperty('height').get() / 2
                })
            } else {
                this.getProperty('x').setCompute(()=>{
                    this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('centerIn').get().getProperty('width'), this.getProperty('anchors').getProperty('centerIn').get().getProperty('x'), this.getProperty('width'))
                    return this.getProperty('anchors').getProperty('centerIn').get().getProperty('x').get() + this.getProperty('anchors').getProperty('centerIn').get().getProperty('width').get() / 2 - this.getProperty('width').get() / 2
                })
                this.getProperty('y').setCompute(()=>{this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('centerIn').get().getProperty('height'), this.getProperty('anchors').getProperty('centerIn').get().getProperty('y'), this.getProperty('height'))
                    return this.getProperty('anchors').getProperty('centerIn').get().getProperty('y').get() + this.getProperty('anchors').getProperty('centerIn').get().getProperty('height').get() / 2 - this.getProperty('height').get() / 2
                })
            }
            
            this.getProperty('x').update()
            this.getProperty('y').update()
        } 
        if(this.getProperty('anchors').getPropertyValue('left') || this.getProperty('anchors').getPropertyValue('right')){
            if(this.getProperty('anchors').getPropertyValue('left') && this.getProperty('anchors').getPropertyValue('right')){
                this.getProperty('x').freeze()
                this.getProperty('width').freeze()

                if(this.parent === this.getProperty('anchors').getProperty('left').get().target){
                    if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Left){
                        this.getProperty('x').setCompute(()=>{
                            this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('leftMargin'))
                            return this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Right){
                        this.getProperty('x').setCompute(()=>{
                            this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'))
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('width').get() + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Left){
                        this.getProperty('x').setCompute(()=>{
                            this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('x'), this.getProperty('anchors').getProperty('leftMargin'))
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Right){
                        this.getProperty('x').setCompute(()=>{
                            this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('x'), this.getProperty('anchors').getProperty('left').get().target.getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'))
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('left').get().target.getProperty('width').get() + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    }
                }
                if(this.parent === this.getProperty('anchors').getProperty('right').get().target){
                    if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Left){
                        this.getProperty('width').setCompute(()=>{
                            this.getProperty('width').subscribePrimary(this.getProperty('x'), this.getProperty('anchors').getProperty('rightMargin'))
                            return -this.getProperty('x').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Right){
                        this.getProperty('width').setCompute(()=>{
                            this.getProperty('width').subscribePrimary(this.getProperty('x'), this.getProperty('anchors').getProperty('right').get().target.getProperty('width'), this.getProperty('anchors').getProperty('rightMargin'))
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('width').get() - this.getProperty('x').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Left){
                        this.getProperty('width').setCompute(()=>{
                            this.getProperty('width').subscribePrimary(this.getProperty('x'), this.getProperty('anchors').getProperty('right').get().target.getProperty('x'), this.getProperty('anchors').getProperty('rightMargin'))
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('x').get() - this.getProperty('x').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Right){
                        this.getProperty('width').setCompute(()=>{
                            this.getProperty('width').subscribePrimary(this.getProperty('x'), this.getProperty('anchors').getProperty('right').get().target.getProperty('x'), this.getProperty('anchors').getProperty('right').get().target.getProperty('width'), this.getProperty('anchors').getProperty('rightMargin'))
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('right').get().target.getProperty('width').get() - this.getProperty('x').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    }
                }
                this.getProperty('x').update()
                this.getProperty('width').update()
        
            } else if(this.getProperty('anchors').getPropertyValue('left')){
                this.getProperty('x').freeze()

                if(this.parent === this.getProperty('anchors').getProperty('left').get().target){
                    if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Left){
                        this.getProperty('x').setCompute(()=>{
                            this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('leftMargin'))
                            return this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Right){
                        this.getProperty('x').setCompute(()=>{
                            this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'))
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('width').get() + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Left){
                        this.getProperty('x').setCompute(()=>{
                            this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('x'), this.getProperty('anchors').getProperty('leftMargin'))
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Right){
                        this.getProperty('x').setCompute(()=>{
                            this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('x'), this.getProperty('anchors').getProperty('left').get().target.getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'))
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('left').get().target.getProperty('width').get() + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    }
                }
                this.getProperty('x').update()
            } else if(this.getProperty('anchors').getPropertyValue('right')){
                this.getProperty('x').freeze()

                if(this.parent === this.getProperty('anchors').getProperty('right').get().target){
                    if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Left){
                        this.getProperty('x').setCompute(()=>{
                            this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('rightMargin'))
                            return -this.getProperty('width').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Right){
                        this.getProperty('x').setCompute(()=>{
                            this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('right').get().target.getProperty('width'), this.getProperty('anchors').getProperty('rightMargin'))
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('width').get() - this.getProperty('width').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Left){
                        this.getProperty('x').setCompute(()=>{
                            this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('right').get().target.getProperty('x'), this.getProperty('anchors').getProperty('rightMargin'))
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('x').get() - this.getProperty('width').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Right){
                        this.getProperty('x').setCompute(()=>{
                            this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('right').get().target.getProperty('x'), this.getProperty('anchors').getProperty('right').get().target.getProperty('width'), this.getProperty('anchors').getProperty('rightMargin'))
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('right').get().target.getProperty('width').get() - this.getProperty('width').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    }
                }
                this.getProperty('x').update()
            }
        } 
        if(this.getProperty('anchors').getPropertyValue('top') || this.getProperty('anchors').getPropertyValue('bottom')){
            if(this.getProperty('anchors').getPropertyValue('top') && this.getProperty('anchors').getPropertyValue('bottom')){
                this.getProperty('y').freeze()
                this.getProperty('height').freeze()

                if(this.parent === this.getProperty('anchors').getProperty('top').get().target){
                    if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Top){
                        this.getProperty('y').setCompute(()=>{
                            this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('topMargin'))
                            return this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Bottom){
                        this.getProperty('y').setCompute(()=>{
                            this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('height'), this.getProperty('anchors').getProperty('topMargin'))
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('height').get() + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Top){
                        this.getProperty('y').setCompute(()=>{
                            this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('y'), this.getProperty('anchors').getProperty('topMargin'))
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Bottom){
                        this.getProperty('y').setCompute(()=>{
                            this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('y'), this.getProperty('anchors').getProperty('top').get().target.getProperty('height'), this.getProperty('anchors').getProperty('topMargin'))
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('top').get().target.getProperty('height').get() + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    }
                }
                if(this.parent === this.getProperty('anchors').getProperty('bottom').get().target){
                    if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Top){
                        this.getProperty('height').setCompute(()=>{
                            this.getProperty('height').subscribePrimary(this.getProperty('y'), this.getProperty('anchors').getProperty('bottomMargin'))
                            return -this.getProperty('y').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Bottom){
                        this.getProperty('height').setCompute(()=>{
                            this.getProperty('height').subscribePrimary(this.getProperty('y'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height'), this.getProperty('anchors').getProperty('bottomMargin'))
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height').get() - this.getProperty('y').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Top){
                        this.getProperty('height').setCompute(()=>{
                            this.getProperty('height').subscribePrimary(this.getProperty('y'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y'), this.getProperty('anchors').getProperty('bottomMargin'))
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y').get() - this.getProperty('y').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Bottom){
                        this.getProperty('height').setCompute(()=>{
                            this.getProperty('height').subscribePrimary(this.getProperty('y'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height'), this.getProperty('anchors').getProperty('bottomMargin'))
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height').get() - this.getProperty('y').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    }
                }
                this.getProperty('y').update()
                this.getProperty('height').update()
        
            } else if(this.getProperty('anchors').getPropertyValue('top')){
                this.getProperty('y').freeze()

                if(this.parent === this.getProperty('anchors').getProperty('top').get().target){
                    if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Top){
                        this.getProperty('y').setCompute(()=>{
                            this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('topMargin'))
                            return this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Bottom){
                        this.getProperty('y').setCompute(()=>{
                            this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('height'), this.getProperty('anchors').getProperty('topMargin'))
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('height').get() + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Top){
                        this.getProperty('y').setCompute(()=>{
                            this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('y'), this.getProperty('anchors').getProperty('topMargin'))
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Bottom){
                        this.getProperty('y').setCompute(()=>{
                            this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('y'), this.getProperty('anchors').getProperty('top').get().target.getProperty('height'), this.getProperty('anchors').getProperty('topMargin'))
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('top').get().target.getProperty('height').get() + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    }
                }
                this.getProperty('y').update()
            } else if(this.getProperty('anchors').getPropertyValue('bottom')){
                this.getProperty('y').freeze()

                if(this.parent === this.getProperty('anchors').getProperty('bottom').get().target){
                    if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Top){
                        this.getProperty('y').setCompute(()=>{
                            this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('bottomMargin'))
                            return -this.getProperty('height').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Bottom){
                        this.getProperty('y').setCompute(()=>{
                            this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height'), this.getProperty('anchors').getProperty('bottomMargin'))
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height').get() - this.getProperty('height').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Top){
                        this.getProperty('y').setCompute(()=>{
                            this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y'), this.getProperty('anchors').getProperty('bottomMargin'))
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y').get() - this.getProperty('height').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Bottom){
                        this.getProperty('y').setCompute(()=>{
                            this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height'), this.getProperty('anchors').getProperty('bottomMargin'))
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height').get() - this.getProperty('height').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    }
                }
                this.getProperty('y').update()
            }
        }

        if(this.getProperty('anchors').getPropertyValue('verticalCenter')){
            this.getProperty('y').freeze()

            if(this.parent === this.getProperty('anchors').getProperty('verticalCenter').get().target){
                if(this.getProperty('anchors').getProperty('verticalCenter').get().float === QAnchorLine.Top){
                    this.getProperty('y').setCompute(()=>{
                        this.getProperty('y').subscribePrimary(this.getProperty('height'))
                        return -this.getProperty('height').get() / 2
                    })
                } else if(this.getProperty('anchors').getProperty('verticalCenter').get().float === QAnchorLine.Bottom){
                    this.getProperty('y').setCompute(()=>{
                        this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height'))
                        return this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height').get() - this.getProperty('height').get() / 2
                    })
                } else if(this.getProperty('anchors').getProperty('verticalCenter').get().float === QAnchorLine.VerticalCenter){
                    this.getProperty('y').setCompute(()=>{
                        this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height'))
                        return this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height').get() / 2 - this.getProperty('height').get() / 2
                    })
                }
            } else {
                //
                if(this.getProperty('anchors').getProperty('verticalCenter').get().float === QAnchorLine.Top){
                    this.getProperty('y').setCompute(()=>{
                        this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('y'))
                        return this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('y').get()-this.getProperty('height').get() / 2
                    })
                } else if(this.getProperty('anchors').getProperty('verticalCenter').get().float === QAnchorLine.Bottom){
                    this.getProperty('y').setCompute(()=>{
                        this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('y'))
                        return this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('y').get()+this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height').get() - this.getProperty('height').get() / 2
                    })
                } else if(this.getProperty('anchors').getProperty('verticalCenter').get().float === QAnchorLine.VerticalCenter){
                    this.getProperty('y').setCompute(()=>{
                        this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('y'))
                        return this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('y').get()+this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height').get() / 2 - this.getProperty('height').get() / 2
                    })
                }
            }
            this.getProperty('y').update()
        }

        if(this.getProperty('anchors').getPropertyValue('horizontalCenter')){
            this.getProperty('x').freeze()

            if(this.parent === this.getProperty('anchors').getProperty('horizontalCenter').get().target){
                if(this.getProperty('anchors').getProperty('horizontalCenter').get().float === QAnchorLine.Left){
                    this.getProperty('x').setCompute(()=>{
                        this.getProperty('x').subscribePrimary(this.getProperty('width'))
                        return -this.getProperty('width').get() / 2
                    })
                } else if(this.getProperty('anchors').getProperty('horizontalCenter').get().float === QAnchorLine.Right){
                    this.getProperty('x').setCompute(()=>{
                        this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width'))
                        return this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width').get() - this.getProperty('width').get() / 2
                    })
                } else if(this.getProperty('anchors').getProperty('horizontalCenter').get().float === QAnchorLine.HorizontalCenter){
                    this.getProperty('x').setCompute(()=>{
                        this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width'))
                        return this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width').get() / 2 - this.getProperty('width').get() / 2
                    })
                }
            } else {
                //
                if(this.getProperty('anchors').getProperty('horizontalCenter').get().float === QAnchorLine.Left){
                    this.getProperty('x').setCompute(()=>{
                        this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('x'))
                        return this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('x').get()-this.getProperty('width').get() / 2
                    })
                } else if(this.getProperty('anchors').getProperty('horizontalCenter').get().float === QAnchorLine.Right){
                    this.getProperty('x').setCompute(()=>{
                        this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('x'))
                        return this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('x').get()+this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width').get() - this.getProperty('width').get() / 2
                    })
                } else if(this.getProperty('anchors').getProperty('horizontalCenter').get().float === QAnchorLine.HorizontalCenter){
                    this.getProperty('x').setCompute(()=>{
                        this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('x'))
                        return this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('x').get()+this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width').get() / 2 - this.getProperty('width').get() / 2
                    })
                }
            }
            this.getProperty('x').update()
        }
    }

    $destroy(){
        this.$dom.remove()
        super.$destroy()
    }
}



module.exports.Item = Item