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
        antialiasing: { type: QBool, value: false },
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
        'Keys.asteriskPressed': { params: ['event'] },
        'Keys.backPressed': { params: ['event'] },
        'Keys.backtabPressed': { params: ['event'] },
        'Keys.callPressed': { params: ['event'] },
        'Keys.cancelPressed': { params: ['event'] },
        'Keys.context1Pressed': { params: ['event'] },
        'Keys.context2Pressed': { params: ['event'] },
        'Keys.context3Pressed': { params: ['event'] },
        'Keys.context4Pressed': { params: ['event'] },
        'Keys.deletePressed': { params: ['event'] },
        'Keys.digit0Pressed': { params: ['event'] },
        'Keys.digit1Pressed': { params: ['event'] },
        'Keys.digit2Pressed': { params: ['event'] },
        'Keys.digit3Pressed': { params: ['event'] },
        'Keys.digit4Pressed': { params: ['event'] },
        'Keys.digit5Pressed': { params: ['event'] },
        'Keys.digit6Pressed': { params: ['event'] },
        'Keys.digit7Pressed': { params: ['event'] },
        'Keys.digit8Pressed': { params: ['event'] },
        'Keys.digit9Pressed': { params: ['event'] },
        'Keys.downPressed': { params: ['event'] },
        'Keys.enterPressed': { params: ['event'] },
        'Keys.escapePressed': { params: ['event'] },
        'Keys.flipPressed': { params: ['event'] },
        'Keys.hangupPressed': { params: ['event'] },
        'Keys.leftPressed': { params: ['event'] },
        'Keys.menuPressed': { params: ['event'] },
        'Keys.noPressed': { params: ['event'] },
        'Keys.pressed': { params: ['event'] },
        'Keys.released': { params: ['event'] },
        'Keys.returnPressed': { params: ['event'] },
        'Keys.rightPressed': { params: ['event'] },
        'Keys.selectPressed': { params: ['event'] },
        'Keys.shortcutOverride': { params: ['event'] },
        'Keys.spacePressed': { params: ['event'] },
        'Keys.tabPressed': { params: ['event'] },
        'Keys.upPressed': { params: ['event'] },
        'Keys.volumeDownPressed': { params: ['event'] },
        'Keys.volumeUpPressed': { params: ['event'] },
        'Keys.yesPressed': { params: ['event'] },
        /* Keys */

        'ListView.reused': { params: [] },
        'ListView.pooled': { params: [] },
    }

    $anchorsUpdateList = []
    $focusedElement = null

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.createDom('div', {
            position: 'absolute',
            display: 'flex',
            overflow : 'unset',
            zIndex: 0,
            pointerEvents: 'none',
            touchAction: 'none',
        })
        
        if(parent){
            // this.getProperty('visible').setOriginCompute(()=>{this.getProperty('visible').subscribe(this.getProperty('parent').get().getProperty('visible')); return this.getProperty('parent').get().getProperty('visible').get()})
            this.getProperty('visible').set2(parent.getPropertyValue('visible'))

            this.getProperty('activeFocus').setCompute(()=>{this.getProperty('activeFocus').subscribe(this.getProperty('focus'));return this.getProperty('focus').get()})
            this.getProperty('activeFocus').update()
        }
    }

    $objectNameChanged(){
        if(this.$dom) this.$dom.setAttribute('objectName', this.objectName)
    }

    $complete(){
        super.$complete()
        this.$anchorsUpdate()
    }

    $anchorsUpdate(){
        for(let func of this.$anchorsUpdateList.splice(0, this.$anchorsUpdateList.length)){
            func()
        }
    }
    
    createDom(tag = 'div', style){
        this.$dom = document.createElement(tag)
        this.setStyle(style)
        this.$dom.id = this.UID
        this.$dom.setAttribute('_testId', this._testId)
        // this.$dom.classList.add(this.constructor.name)

        if(this.getProperty('parent').get() && this.getProperty('parent').get().$dom){
            this.getProperty('parent').get().$dom.appendChild(this.$dom)
            this.getProperty('parent').get().addDomChild(this)
        }
    }

    $parentChanged(){
        if(this.getProperty('parent').get() && this.getProperty('parent').get().$dom && this.$dom){
            let index = this.getProperty('parent').get().getProperty('children').get().indexOf(this)
            if(index > 0){
                this.getProperty('parent').get().$dom.insertBefore(this.$dom, this.getProperty('parent').get().getProperty('children').get()[index-1].$dom.nextSibling)
            } else {
                this.getProperty('parent').get().$dom.appendChild(this.$dom)
            }
            this.getProperty('parent').get().addDomChild(this)
        }
        super.$parentChanged()
    }

    $childrenChanged(topLeft, bottomRight, roles){
        if(roles === 'append'){
            for(let index = topLeft; index < bottomRight; index++){
                if(this.getProperty('children').get()[index].$dom){
                    if(this.getProperty('children').get().length > 1 && index > 0){
                        this.$dom.insertBefore(this.getProperty('children').get()[index].$dom, this.getProperty('children').get()[index-1].$dom.nextSibling)
                    } else {
                        this.$dom.insertBefore(this.getProperty('children').get()[index].$dom, null)
                        // this.$dom.appendChild(this.getProperty('children').get()[index].$dom)
                    }
                }                
            }
        }  
        super.$childrenChanged(topLeft, bottomRight, roles)
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

    $testRelation(obj){
        if(!obj || !obj.UID) return true

        let parent = obj.parent

        while(parent){
            if(parent === this) return true

            parent = parent.parent
        }

        parent = this.parent

        while(parent){
            if(parent === obj) return true

            parent = parent.parent
        }

        return false
    }

    forceActiveFocus(){
        if(!this.$testRelation(mainRoot.$forceElement)) return
        
        let focusScope = null
        let parent = this.parent
        while(parent){
            if(parent instanceof FocusScope){ 
                focusScope = parent
                break
            } else {
                parent = parent.parent
            }
        }

        if(!mainRoot.$forceElement) mainRoot.$forceElement = this
        if(focusScope){
            mainRoot.$setFocusScope(focusScope)
            focusScope.$setActiveFocusedElement(this)
        } else {
            mainRoot.$setFocusScope(null)
            mainRoot.$setActiveFocusedElement(this)
        }
        if(mainRoot.$forceElement === this) mainRoot.$forceElement = null
    }

    $updateShadow(){
        if(this.$shadows){
            for(let shadow of this.$shadows){
                if(shadow.getPropertyValue('visible')){
                    let rgba = shadow.getProperty('color').toRGBA()
                    this.setStyle({
                        boxShadow: `${shadow.getPropertyValue('horizontalOffset')}px ${shadow.getPropertyValue('verticalOffset')}px ${shadow.getPropertyValue('radius')}px ${shadow.getPropertyValue('spread')}px rgba(${rgba.r},${rgba.g},${rgba.b},${shadow.getPropertyValue('color') === 'transparent' ? 0 : rgba.a * shadow.getPropertyValue('opacity')})`
                    })
                }
            }
        }
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

    $setFocusScope(element){
        let oldElement = this.$focusScope

        this.$focusScope = element

        if(oldElement !== element && oldElement && oldElement.UID) {
            if(oldElement.$focusedElement && oldElement.$focusedElement.UID){
                oldElement.$focusedElement.activeFocus = false
            }

            oldElement.focus = false
            oldElement.activeFocus = false
        } 
    }

    $setFocusedElement(element){
        let oldElement = this.$focusedElement
        this.$focusedElement = element

        if(element instanceof FocusScope && element.UID && element.$focusedElement && element.$focusedElement.UID){
            element.$focusedElement.focus = true
        }

        if(oldElement !== element && oldElement && oldElement.UID) {
            oldElement.focus = false
            oldElement.activeFocus = false
        }

        element.focus = true
    }

    $setActiveFocusedElement(element){
        let oldElement = this.$focusedElement
        this.$focusedElement = element

        if(element instanceof FocusScope && element.UID && element.$focusedElement && element.$focusedElement.UID){
            element.$focusedElement.activeFocus = true
            element.$focusedElement.focus = true
            if(element.$focusedElement.$input) element.$focusedElement.$input.focus()
        }

        if(oldElement !== element && oldElement && oldElement.UID) {
            oldElement.focus = false
            oldElement.activeFocus = false
        }
        
        element.focus = true
        element.activeFocus = true
        if(element.$input) element.$input.focus()
    }

    $focusChanged(){
        if(this.focus){
            let focusScope = null
            let parent = this.parent
            while(parent){
                if(parent instanceof FocusScope){
                    focusScope = parent
                    break
                } else {
                    parent = parent.parent
                }
            }

        
            if(focusScope){
                mainRoot.$setFocusScope(focusScope)
                focusScope.$setFocusedElement(this)
            } else {
                mainRoot.$setFocusScope(null)
                mainRoot.$setFocusedElement(this)
            }
        }
        
    }

    $activeFocusChanged(){
       
    }

    $enabledChanged(){

    }

    $opacityChanged(){
        // if(this.getPropertyValue('opacity') > 0){
        //     this.setStyle({
        //         opacity: 1
        //     })
        // } else {
        //     this.setStyle({
        //         opacity: 0
        //     })
        // }

        this.setStyle({
            opacity: this.getPropertyValue('opacity')
        })
    }

    $keyNavigationChanged(){
        
    }

    $visibleChanged(){
        let currentValue = this.getPropertyValue('visible')
        this.setStyle({ display: currentValue ? Item.defaultCSS.display : 'none' })

        for(let child of this.children){
            if(child.UID) child.getProperty('visible').set2(currentValue)
        }
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
                this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('leftMargin'))

                this.getProperty('x').setCompute(()=>{ 
                    return this.getProperty('anchors').getProperty('leftMargin').get()
                })
                this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('topMargin'))

                this.getProperty('y').setCompute(()=>{
                    return this.getProperty('anchors').getProperty('topMargin').get()
                })
            } else {
                this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('fill').get().getProperty('x'), this.getProperty('anchors').getProperty('leftMargin'))
                    
                this.getProperty('x').setCompute(()=>{
                    return this.getProperty('anchors').getProperty('fill').get().getProperty('x').get() + this.getProperty('anchors').getProperty('leftMargin').get()
                })
                this.getProperty('y').setCompute(()=>{
                    this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('fill').get().getProperty('y'), this.getProperty('anchors').getProperty('topMargin'))
                    return this.getProperty('anchors').getProperty('fill').get().getProperty('y').get() + this.getProperty('anchors').getProperty('topMargin').get()
                })
            }
            this.getProperty('width').subscribePrimary(this.getProperty('anchors').getProperty('fill').get().getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'), this.getProperty('anchors').getProperty('rightMargin'))
                
            this.getProperty('width').setCompute(()=>{
                return this.getProperty('anchors').getProperty('fill').get().getProperty('width').get() - this.getProperty('anchors').getProperty('leftMargin').get() - this.getProperty('anchors').getProperty('rightMargin').get()
            })
            this.getProperty('height').subscribePrimary(this.getProperty('anchors').getProperty('fill').get().getProperty('height'), this.getProperty('anchors').getProperty('topMargin'), this.getProperty('anchors').getProperty('bottomMargin')) 
                
            this.getProperty('height').setCompute(()=>{
                return this.getProperty('anchors').getProperty('fill').get().getProperty('height').get() - this.getProperty('anchors').getProperty('topMargin').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
            })
            let update = ()=>{
                this.getProperty('anchors').getProperty('fill').get().getProperty('anchors').update()
                this.getProperty('x').update()
                this.getProperty('y').update()
                this.getProperty('width').update()
                this.getProperty('height').update()
            }
            if(this.$completed){
                update()
            } else {
                this.$anchorsUpdateList.push(update)
            }
            
            return
        } else if(this.getProperty('anchors').getPropertyValue('centerIn')){
            this.getProperty('x').freeze()
            this.getProperty('y').freeze()

            if(this.parent === this.getProperty('anchors').getProperty('centerIn').get()){
                this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('centerIn').get().getProperty('width'), this.getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'), this.getProperty('anchors').getProperty('rightMargin'), this.getProperty('anchors').getProperty('horizontalCenterOffset'))
                    
                this.getProperty('x').setCompute(()=>{
                    return this.getProperty('anchors').getProperty('centerIn').get().getProperty('width').get() / 2 - this.getProperty('width').get() / 2 + this.getProperty('anchors').getPropertyValue('horizontalCenterOffset')
                })
                this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('centerIn').get().getProperty('height'), this.getProperty('height'), this.getProperty('anchors').getProperty('topMargin'), this.getProperty('anchors').getProperty('bottomMargin'), this.getProperty('anchors').getProperty('verticalCenterOffset'))
                    
                this.getProperty('y').setCompute(()=>{
                    return this.getProperty('anchors').getProperty('centerIn').get().getProperty('height').get() / 2 - this.getProperty('height').get() / 2 + this.getProperty('anchors').getPropertyValue('verticalCenterOffset')
                })
            } else {
                this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('centerIn').get().getProperty('width'), this.getProperty('anchors').getProperty('centerIn').get().getProperty('x'), this.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenterOffset'))
                    
                this.getProperty('x').setCompute(()=>{
                    return this.getProperty('anchors').getProperty('centerIn').get().getProperty('x').get() + this.getProperty('anchors').getProperty('centerIn').get().getProperty('width').get() / 2 - this.getProperty('width').get() / 2 + this.getProperty('anchors').getPropertyValue('horizontalCenterOffset')
                })
                this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('centerIn').get().getProperty('height'), this.getProperty('anchors').getProperty('centerIn').get().getProperty('y'), this.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenterOffset'))
                    
                this.getProperty('y').setCompute(()=>{
                    return this.getProperty('anchors').getProperty('centerIn').get().getProperty('y').get() + this.getProperty('anchors').getProperty('centerIn').get().getProperty('height').get() / 2 - this.getProperty('height').get() / 2 + this.getProperty('anchors').getPropertyValue('verticalCenterOffset')
                })
            }

            let update = ()=>{
                this.getProperty('x').update()
                this.getProperty('y').update()
            }
            if(this.$completed){
                update()
            } else {
                this.$anchorsUpdateList.push(update)
            }
            
            
            return
        } 
        if(this.getProperty('anchors').getPropertyValue('left') || this.getProperty('anchors').getPropertyValue('right')){
            if(this.getProperty('anchors').getPropertyValue('left') && this.getProperty('anchors').getPropertyValue('right')){
                this.getProperty('x').freeze()
                this.getProperty('width').freeze()

                if(this.parent === this.getProperty('anchors').getProperty('left').get().target){
                    if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Left){
                        this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('leftMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Right){
                        this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('width').get() + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.HorizontalCenter){
                        this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('width').get() / 2 + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Left){
                        this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('x'), this.getProperty('anchors').getProperty('leftMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Right){
                        this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('x'), this.getProperty('anchors').getProperty('left').get().target.getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('left').get().target.getProperty('width').get() + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.HorizontalCenter){
                        this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('x'), this.getProperty('anchors').getProperty('left').get().target.getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('left').get().target.getProperty('width').get() / 2 + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    }
                }
                if(this.parent === this.getProperty('anchors').getProperty('right').get().target){
                    if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Left){
                        this.getProperty('width').subscribePrimary(this.getProperty('x'), this.getProperty('anchors').getProperty('rightMargin'))
                            
                        this.getProperty('width').setCompute(()=>{
                            return -this.getProperty('x').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Right){
                        this.getProperty('width').subscribePrimary(this.getProperty('x'), this.getProperty('anchors').getProperty('right').get().target.getProperty('width'), this.getProperty('anchors').getProperty('rightMargin'))
                            
                        this.getProperty('width').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('width').get() - this.getProperty('x').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.HorizontalCenter){
                        this.getProperty('width').subscribePrimary(this.getProperty('x'), this.getProperty('anchors').getProperty('right').get().target.getProperty('width'), this.getProperty('anchors').getProperty('rightMargin'))
                            
                        this.getProperty('width').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('width').get() / 2 - this.getProperty('x').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Left){
                        this.getProperty('width').subscribePrimary(this.getProperty('x'), this.getProperty('anchors').getProperty('right').get().target.getProperty('x'), this.getProperty('anchors').getProperty('rightMargin'))
                            
                        this.getProperty('width').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('x').get() - this.getProperty('x').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Right){
                        this.getProperty('width').subscribePrimary(this.getProperty('x'), this.getProperty('anchors').getProperty('right').get().target.getProperty('x'), this.getProperty('anchors').getProperty('right').get().target.getProperty('width'), this.getProperty('anchors').getProperty('rightMargin'))
                            
                        this.getProperty('width').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('right').get().target.getProperty('width').get() - this.getProperty('x').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.HorizontalCenter){
                        this.getProperty('width').subscribePrimary(this.getProperty('x'), this.getProperty('anchors').getProperty('right').get().target.getProperty('x'), this.getProperty('anchors').getProperty('right').get().target.getProperty('width'), this.getProperty('anchors').getProperty('rightMargin'))
                            
                        this.getProperty('width').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('right').get().target.getProperty('width').get() / 2 - this.getProperty('x').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    }
                }
                
                let update = ()=>{
                    this.getProperty('x').update()
                    this.getProperty('width').update()
                }
                if(this.$completed){
                    update()
                } else {
                    this.$anchorsUpdateList.push(update)
                }
        
            } else if(this.getProperty('anchors').getPropertyValue('left')){
                this.getProperty('x').freeze()

                if(this.parent === this.getProperty('anchors').getProperty('left').get().target){
                    if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Left){
                        this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('leftMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Right){
                        this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('width').get() + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.HorizontalCenter){
                        this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('width').get() / 2 + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Left){
                        this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('x'), this.getProperty('anchors').getProperty('leftMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.Right){
                        this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('x'), this.getProperty('anchors').getProperty('left').get().target.getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('left').get().target.getProperty('width').get() + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('left').get().float === QAnchorLine.HorizontalCenter){
                        this.getProperty('x').subscribePrimary(this.getProperty('anchors').getProperty('left').get().target.getProperty('x'), this.getProperty('anchors').getProperty('left').get().target.getProperty('width'), this.getProperty('anchors').getProperty('leftMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('left').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('left').get().target.getProperty('width').get() / 2 + this.getProperty('anchors').getProperty('leftMargin').get()
                        })
                    }
                }
                let update = ()=>{
                    this.getProperty('x').update()
                }
                if(this.$completed){
                    update()
                } else {
                    this.$anchorsUpdateList.push(update)
                }
            } else if(this.getProperty('anchors').getPropertyValue('right')){
                this.getProperty('x').freeze()

                if(this.parent === this.getProperty('anchors').getProperty('right').get().target){
                    if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Left){
                        this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('rightMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return -this.getProperty('width').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Right){
                        this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('right').get().target.getProperty('width'), this.getProperty('anchors').getProperty('rightMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('width').get() - this.getProperty('width').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.HorizontalCenter){
                        this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('right').get().target.getProperty('width'), this.getProperty('anchors').getProperty('rightMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('width').get() / 2 - this.getProperty('width').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Left){
                        this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('right').get().target.getProperty('x'), this.getProperty('anchors').getProperty('rightMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('x').get() - this.getProperty('width').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.Right){
                        this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('right').get().target.getProperty('x'), this.getProperty('anchors').getProperty('right').get().target.getProperty('width'), this.getProperty('anchors').getProperty('rightMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('right').get().target.getProperty('width').get() - this.getProperty('width').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('right').get().float === QAnchorLine.HorizontalCenter){
                        this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('right').get().target.getProperty('x'), this.getProperty('anchors').getProperty('right').get().target.getProperty('width'), this.getProperty('anchors').getProperty('rightMargin'))
                            
                        this.getProperty('x').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('right').get().target.getProperty('x').get() + this.getProperty('anchors').getProperty('right').get().target.getProperty('width').get() / 2 - this.getProperty('width').get() - this.getProperty('anchors').getProperty('rightMargin').get()
                        })
                    }
                }

                let update = ()=>{
                    this.getProperty('x').update()
                }
                if(this.$completed){
                    update()
                } else {
                    this.$anchorsUpdateList.push(update)
                }
            }
        } 
        if(this.getProperty('anchors').getPropertyValue('top') || this.getProperty('anchors').getPropertyValue('bottom')){
            if(this.getProperty('anchors').getPropertyValue('top') && this.getProperty('anchors').getPropertyValue('bottom')){
                this.getProperty('y').freeze()
                this.getProperty('height').freeze()

                if(this.parent === this.getProperty('anchors').getProperty('top').get().target){
                    if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Top){
                        this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('topMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Bottom){
                        this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('height'), this.getProperty('anchors').getProperty('topMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('height').get() + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.VerticalCenter){
                        this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('height'), this.getProperty('anchors').getProperty('topMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('height').get() / 2 + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Top){
                        this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('y'), this.getProperty('anchors').getProperty('topMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Bottom){
                        this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('y'), this.getProperty('anchors').getProperty('top').get().target.getProperty('height'), this.getProperty('anchors').getProperty('topMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('top').get().target.getProperty('height').get() + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.VerticalCenter){
                        this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('y'), this.getProperty('anchors').getProperty('top').get().target.getProperty('height'), this.getProperty('anchors').getProperty('topMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('top').get().target.getProperty('height').get() / 2 + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    }
                }
                if(this.parent === this.getProperty('anchors').getProperty('bottom').get().target){
                    if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Top){
                        this.getProperty('height').subscribePrimary(this.getProperty('y'), this.getProperty('anchors').getProperty('bottomMargin'))
                            
                        this.getProperty('height').setCompute(()=>{
                            return -this.getProperty('y').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Bottom){
                        this.getProperty('height').subscribePrimary(this.getProperty('y'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height'), this.getProperty('anchors').getProperty('bottomMargin'))
                            
                        this.getProperty('height').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height').get() - this.getProperty('y').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.VerticalCenter){
                        this.getProperty('height').subscribePrimary(this.getProperty('y'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height'), this.getProperty('anchors').getProperty('bottomMargin'))
                            
                        this.getProperty('height').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height').get() / 2 - this.getProperty('y').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Top){
                        this.getProperty('height').subscribePrimary(this.getProperty('y'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y'), this.getProperty('anchors').getProperty('bottomMargin'))
                            
                        this.getProperty('height').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y').get() - this.getProperty('y').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Bottom){
                        this.getProperty('height').subscribePrimary(this.getProperty('y'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height'), this.getProperty('anchors').getProperty('bottomMargin'))
                            
                        this.getProperty('height').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height').get() - this.getProperty('y').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.VerticalCenter){
                        this.getProperty('height').subscribePrimary(this.getProperty('y'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height'), this.getProperty('anchors').getProperty('bottomMargin'))
                            
                        this.getProperty('height').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height').get() / 2 - this.getProperty('y').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    }
                }
                
                let update = ()=>{
                    this.getProperty('y').update()
                    this.getProperty('height').update()
                }
                if(this.$completed){
                    update()
                } else {
                    this.$anchorsUpdateList.push(update)
                }
        
            } else if(this.getProperty('anchors').getPropertyValue('top')){
                this.getProperty('y').freeze()

                if(this.parent === this.getProperty('anchors').getProperty('top').get().target){
                    if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Top){
                        this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('topMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Bottom){
                        this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('height'), this.getProperty('anchors').getProperty('topMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('height').get() + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.VerticalCenter){
                        this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('height'), this.getProperty('anchors').getProperty('topMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('height').get() / 2 + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Top){
                        this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('y'), this.getProperty('anchors').getProperty('topMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.Bottom){
                        this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('y'), this.getProperty('anchors').getProperty('top').get().target.getProperty('height'), this.getProperty('anchors').getProperty('topMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('top').get().target.getProperty('height').get() + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('top').get().float === QAnchorLine.VerticalCenter){
                        this.getProperty('y').subscribePrimary(this.getProperty('anchors').getProperty('top').get().target.getProperty('y'), this.getProperty('anchors').getProperty('top').get().target.getProperty('height'), this.getProperty('anchors').getProperty('topMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('top').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('top').get().target.getProperty('height').get() / 2 + this.getProperty('anchors').getProperty('topMargin').get()
                        })
                    }
                }
                let update = ()=>{
                    this.getProperty('y').update()
                }
                if(this.$completed){
                    update()
                } else {
                    this.$anchorsUpdateList.push(update)
                }
            } else if(this.getProperty('anchors').getPropertyValue('bottom')){
                this.getProperty('y').freeze()

                if(this.parent === this.getProperty('anchors').getProperty('bottom').get().target){
                    if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Top){
                        this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('bottomMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return -this.getProperty('height').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Bottom){
                        this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height'), this.getProperty('anchors').getProperty('bottomMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height').get() - this.getProperty('height').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.VerticalCenter){
                        this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height'), this.getProperty('anchors').getProperty('bottomMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height').get() / 2 - this.getProperty('height').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    }
                    
                } else {
                    if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Top){
                        this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y'), this.getProperty('anchors').getProperty('bottomMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y').get() - this.getProperty('height').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.Bottom){
                        this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height'), this.getProperty('anchors').getProperty('bottomMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height').get() - this.getProperty('height').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    } else if(this.getProperty('anchors').getProperty('bottom').get().float === QAnchorLine.VerticalCenter){
                        this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y'), this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height'), this.getProperty('anchors').getProperty('bottomMargin'))
                            
                        this.getProperty('y').setCompute(()=>{
                            return this.getProperty('anchors').getProperty('bottom').get().target.getProperty('y').get() + this.getProperty('anchors').getProperty('bottom').get().target.getProperty('height').get() / 2 - this.getProperty('height').get() - this.getProperty('anchors').getProperty('bottomMargin').get()
                        })
                    }
                }
                let update = ()=>{
                    this.getProperty('y').update()
                }
                if(this.$completed){
                    update()
                } else {
                    this.$anchorsUpdateList.push(update)
                }
            }
        }

        if(this.getProperty('anchors').getPropertyValue('verticalCenter')){
            this.getProperty('y').freeze()

            if(this.parent === this.getProperty('anchors').getProperty('verticalCenter').get().target){
                if(this.getProperty('anchors').getProperty('verticalCenter').get().float === QAnchorLine.Top){
                    this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenterOffset'))
                        
                    this.getProperty('y').setCompute(()=>{
                        return -this.getProperty('height').get() / 2 + this.getProperty('anchors').getPropertyValue('verticalCenterOffset')
                    })
                } else if(this.getProperty('anchors').getProperty('verticalCenter').get().float === QAnchorLine.Bottom){
                    this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenterOffset'))
                        
                    this.getProperty('y').setCompute(()=>{
                        return this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height').get() - this.getProperty('height').get() / 2 + this.getProperty('anchors').getPropertyValue('verticalCenterOffset')
                    })
                } else if(this.getProperty('anchors').getProperty('verticalCenter').get().float === QAnchorLine.VerticalCenter){
                    this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenterOffset'))
                        
                    this.getProperty('y').setCompute(()=>{
                        return this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height').get() / 2 - this.getProperty('height').get() / 2 + this.getProperty('anchors').getPropertyValue('verticalCenterOffset')
                    })
                }
            } else {
                //
                if(this.getProperty('anchors').getProperty('verticalCenter').get().float === QAnchorLine.Top){
                    this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('y'), this.getProperty('anchors').getProperty('verticalCenterOffset'))
                        
                    this.getProperty('y').setCompute(()=>{
                        return this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('y').get()-this.getProperty('height').get() / 2 + this.getProperty('anchors').getPropertyValue('verticalCenterOffset')
                    })
                } else if(this.getProperty('anchors').getProperty('verticalCenter').get().float === QAnchorLine.Bottom){
                    this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('y'), this.getProperty('anchors').getProperty('verticalCenterOffset'))
                        
                    this.getProperty('y').setCompute(()=>{
                        return this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('y').get()+this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height').get() - this.getProperty('height').get() / 2 + this.getProperty('anchors').getPropertyValue('verticalCenterOffset')
                    })
                } else if(this.getProperty('anchors').getProperty('verticalCenter').get().float === QAnchorLine.VerticalCenter){
                    this.getProperty('y').subscribePrimary(this.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height'), this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('y'), this.getProperty('anchors').getProperty('verticalCenterOffset'))
                        
                    this.getProperty('y').setCompute(()=>{
                        return this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('y').get()+this.getProperty('anchors').getProperty('verticalCenter').get().target.getProperty('height').get() / 2 - this.getProperty('height').get() / 2 + this.getProperty('anchors').getProperty('verticalCenterOffset')
                    })
                }
            }
            let update = ()=>{
                this.getProperty('y').update()
            }
            if(this.$completed){
                update()
            } else {
                this.$anchorsUpdateList.push(update)
            }
        }

        if(this.getProperty('anchors').getPropertyValue('horizontalCenter')){
            this.getProperty('x').freeze()

            if(this.parent === this.getProperty('anchors').getProperty('horizontalCenter').get().target){
                if(this.getProperty('anchors').getProperty('horizontalCenter').get().float === QAnchorLine.Left){
                    this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenterOffset'))
                        
                    this.getProperty('x').setCompute(()=>{
                        return -this.getProperty('width').get() / 2 + this.getProperty('anchors').getPropertyValue('horizontalCenterOffset')
                    })
                } else if(this.getProperty('anchors').getProperty('horizontalCenter').get().float === QAnchorLine.Right){
                    this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenterOffset'))
                        
                    this.getProperty('x').setCompute(()=>{
                        return this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width').get() - this.getProperty('width').get() / 2 + this.getProperty('anchors').getPropertyValue('horizontalCenterOffset')
                    })
                } else if(this.getProperty('anchors').getProperty('horizontalCenter').get().float === QAnchorLine.HorizontalCenter){
                    this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenterOffset'))
                        
                    this.getProperty('x').setCompute(()=>{
                        return this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width').get() / 2 - this.getProperty('width').get() / 2 + this.getProperty('anchors').getPropertyValue('horizontalCenterOffset')
                    })
                }
            } else {
                //
                if(this.getProperty('anchors').getProperty('horizontalCenter').get().float === QAnchorLine.Left){
                    this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('x'), this.getProperty('anchors').getProperty('horizontalCenterOffset'))
                        
                    this.getProperty('x').setCompute(()=>{
                        return this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('x').get()-this.getProperty('width').get() / 2 + this.getProperty('anchors').getPropertyValue('horizontalCenterOffset')
                    })
                } else if(this.getProperty('anchors').getProperty('horizontalCenter').get().float === QAnchorLine.Right){
                    this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('x'), this.getProperty('anchors').getProperty('horizontalCenterOffset'))
                        
                    this.getProperty('x').setCompute(()=>{
                        return this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('x').get()+this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width').get() - this.getProperty('width').get() / 2 + this.getProperty('anchors').getPropertyValue('horizontalCenterOffset')
                    })
                } else if(this.getProperty('anchors').getProperty('horizontalCenter').get().float === QAnchorLine.HorizontalCenter){
                    this.getProperty('x').subscribePrimary(this.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width'), this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('x'), this.getProperty('anchors').getProperty('horizontalCenterOffset'))
                        
                    this.getProperty('x').setCompute(()=>{
                        return this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('x').get()+this.getProperty('anchors').getProperty('horizontalCenter').get().target.getProperty('width').get() / 2 - this.getProperty('width').get() / 2 + this.getProperty('anchors').getPropertyValue('horizontalCenterOffset')
                    })
                }
            }
            let update = ()=>{
                this.getProperty('x').update()
            }
            if(this.$completed){
                update()
            } else {
                this.$anchorsUpdateList.push(update)
            }
        }
    }

    destroy(){
        this.$dom.remove()
        super.destroy()

        let index = mainRoot.$focusedElements.indexOf(this)
        if(index >= 0) mainRoot.$focusedElements.splice(index, 1)
        index = mainRoot.$activeFocusedElements.indexOf(this)
        if(index >= 0) mainRoot.$activeFocusedElements.splice(index, 1)
    }
}



module.exports.Item = Item