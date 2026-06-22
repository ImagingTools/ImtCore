const BaseObject = require("../QtBase/BaseObject")
const QtObject = require("../QtQml/QtObject")
const Real = require("../QtQml/Real")
const Int = require("../QtQml/Int")
const Bool = require("../QtQml/Bool")
const LinkedBool = require("../QtQml/LinkedBool")
const Geometry = require("../QtQml/Geometry")
const Signal = require("../QtQml/Signal")
const SpecialSignal = require("../QtQml/SpecialSignal")
const SpecialProperty = require("../QtQml/SpecialProperty")
const KeyNavigation = require("../QtQml/KeyNavigation")
const Anchors = require("../QtQml/Anchors")
const AnchorLine = require("../QtQml/AnchorLine")
const Layout = require("./Layouts/Layout")
const Property = require("../QtQml/Property")

class Item extends QtObject {
    static TopLeft = 0
    static Top = 1
    static TopRight = 2
    static Right = 3
    static BottomRight = 4
    static Bottom = 5
    static BottomLeft = 6
    static Left = 7
    static Center = 8

    static meta = Object.assign({}, QtObject.meta, {
        x: {type:Geometry, value:0, },
        y: {type:Geometry, value:0, },
        width: {type:Geometry, value:0, },
        height: {type:Geometry, value:0, },
        implicitWidth: {type:Geometry, value:0, },
        implicitHeight: {type:Geometry, value:0, },
        z: {type:Int, value:0, },
        rotation: { type: Real, value: 0},
        transformOrigin: { type: Real, value: Item.Center},
        scale: { type: Real, value: 1},
        opacity: { type: Real, value: 1},
        visible: { type: LinkedBool, link: 'visible', value: true},
        enabled: { type: LinkedBool, link: 'enabled', value: true},
        focus: { type: Bool, value: false},
        activeFocus: { type: Bool, value: false},
        clip: { type: Bool, value: false},

        Layout: {type:Layout},

        KeyNavigation: {type:KeyNavigation},
        anchors: {type:Anchors},
        left: {type:AnchorLine, value: AnchorLine.Left},
        right: {type:AnchorLine, value: AnchorLine.Right},
        top: {type:AnchorLine, value: AnchorLine.Top},
        bottom: {type:AnchorLine, value: AnchorLine.Bottom},
        horizontalCenter: {type:AnchorLine, value: AnchorLine.HorizontalCenter},
        verticalCenter: {type:AnchorLine, value: AnchorLine.VerticalCenter},

        AX: {type:Geometry, value:0, },
        AY: {type:Geometry, value:0, },
        AWidth: {type:Geometry, value:0, },
        AHeight: {type:Geometry, value:0, },
        AXChanged: {type:SpecialSignal, args:[]},
        AYChanged: {type:SpecialSignal, args:[]},
        AWidthChanged: {type:SpecialSignal, args:[]},
        AHeightChanged: {type:SpecialSignal, args:[]},

        xChanged: {type:Signal, args:[]},
        yChanged: {type:Signal, args:[]},
        widthChanged: {type:Signal, args:[]},
        heightChanged: {type:Signal, args:[]},
        implicitWidthChanged: {type:Signal, args:[]},
        implicitHeightChanged: {type:Signal, args:[]},
        zChanged: {type:Signal, args:[]},
        rotationChanged: {type:Signal, args:[]},
        transformOriginChanged: {type:Signal, args:[]},
        scaleChanged: {type:Signal, args:[]},
        opacityChanged: {type:Signal, args:[]},
        visibleChanged: {type:Signal, args:[]},
        enabledChanged: {type:Signal, args:[]},
        focusChanged: {type:Signal, args:[]},
        activeFocusChanged: {type:Signal, args:[]},
        clipChanged: {type:Signal, args:[]},

        'Keys.asteriskPressed': {type:Signal, args: ['event'] },
        'Keys.backPressed': {type:Signal, args: ['event'] },
        'Keys.backtabPressed': {type:Signal, args: ['event'] },
        'Keys.callPressed': {type:Signal, args: ['event'] },
        'Keys.cancelPressed': {type:Signal, args: ['event'] },
        'Keys.context1Pressed': {type:Signal, args: ['event'] },
        'Keys.context2Pressed': {type:Signal, args: ['event'] },
        'Keys.context3Pressed': {type:Signal, args: ['event'] },
        'Keys.context4Pressed': {type:Signal, args: ['event'] },
        'Keys.deletePressed': {type:Signal, args: ['event'] },
        'Keys.digit0Pressed': {type:Signal, args: ['event'] },
        'Keys.digit1Pressed': {type:Signal, args: ['event'] },
        'Keys.digit2Pressed': {type:Signal, args: ['event'] },
        'Keys.digit3Pressed': {type:Signal, args: ['event'] },
        'Keys.digit4Pressed': {type:Signal, args: ['event'] },
        'Keys.digit5Pressed': {type:Signal, args: ['event'] },
        'Keys.digit6Pressed': {type:Signal, args: ['event'] },
        'Keys.digit7Pressed': {type:Signal, args: ['event'] },
        'Keys.digit8Pressed': {type:Signal, args: ['event'] },
        'Keys.digit9Pressed': {type:Signal, args: ['event'] },
        'Keys.downPressed': {type:Signal, args: ['event'] },
        'Keys.enterPressed': {type:Signal, args: ['event'] },
        'Keys.escapePressed': {type:Signal, args: ['event'] },
        'Keys.flipPressed': {type:Signal, args: ['event'] },
        'Keys.hangupPressed': {type:Signal, args: ['event'] },
        'Keys.leftPressed': {type:Signal, args: ['event'] },
        'Keys.menuPressed': {type:Signal, args: ['event'] },
        'Keys.noPressed': {type:Signal, args: ['event'] },
        'Keys.pressed': {type:Signal, args: ['event'] },
        'Keys.released': {type:Signal, args: ['event'] },
        'Keys.returnPressed': {type:Signal, args: ['event'] },
        'Keys.rightPressed': {type:Signal, args: ['event'] },
        'Keys.selectPressed': {type:Signal, args: ['event'] },
        'Keys.shortcutOverride': {type:Signal, args: ['event'] },
        'Keys.spacePressed': {type:Signal, args: ['event'] },
        'Keys.tabPressed': {type:Signal, args: ['event'] },
        'Keys.upPressed': {type:Signal, args: ['event'] },
        'Keys.volumeDownPressed': {type:Signal, args: ['event'] },
        'Keys.volumeUpPressed': {type:Signal, args: ['event'] },
        'Keys.yesPressed': {type:Signal, args: ['event'] },

        'ListView.reused': { type:Signal, args: [] },
        'ListView.pooled': { type:Signal, args: [] },
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)

        let dom = obj.__getDOM()
        dom.classList.add('Item')

        dom.qml = obj
        obj.__connectDOM(this.parent)

        return obj
    }

    visible = {
        parentValue: true,
        value: true,
    }

    enabled = {
        parentValue: true,
        value: true,
    }

    // __updatePrimaryProperties(){
    //     super.__updatePrimaryProperties()
    //     this.__updateProperty('anchors')
    // }

    __updateProperty(propName){
        if(propName === 'width'){
            this.__updateProperty('height')
        }
        super.__updateProperty(propName)
    }

    __updateProperties(){
        if(this.parent){
            LinkedBool.parentSet(this, 'visible', this.parent.visible)
        }
        super.__updateProperties()
    }

    __complete(){
        this.__checkVisibility()
        super.__complete()
    }

    // __removeChild(child){
    //     let index = -1

    //     index = this.data.indexOf(child)
    //     if(index >= 0) this.data.__splice(index, 1)

    //     if(child instanceof Item){
    //         index = this.children.indexOf(child)
    //         if(index >= 0) this.children.__splice(index, 1)
    //     } else {
    //         index = this.resources.indexOf(child)
    //         if(index >= 0) this.resources.__splice(index, 1)
    //     }
    // }

    // __addChild(child){
    //     let index = -1

    //     index = this.data.indexOf(child)
    //     if(index < 0) this.data.__push(child)

    //     if(child instanceof Item){
    //         index = this.children.indexOf(child)

    //         if(index < 0) this.children.__push(child)
    //     } else {
    //         index = this.resources.indexOf(child)

    //         if(index < 0) this.resources.__push(child)
    //     }
    // }

    __getDOM(){
        let dom = this.__DOM
        if(dom) {
            return dom
        } else {
            dom = document.createElement('div')
            this.__DOM = dom
            return dom
        }
    }

    __setDOMStyle(style){
        let dom = this.__DOM

        if(dom) {
            for(let name in style){
                dom.style[name] = style[name]
            }
        }
    }

    __connectDOM(target){
        let dom = this.__getDOM()
        if(target && dom){
            if(target instanceof Item){
                let parentDOM = target.__getDOM()
                if(typeof target.__onChildDOM === 'function') target.__onChildDOM(this.__self || this, dom)
                parentDOM.appendChild(dom)
            } else if(typeof target.__getDOM === 'function'){
                let parentDOM = target.__getDOM()
                if(typeof target.__onChildDOM === 'function') target.__onChildDOM(this.__self || this, dom)
                if(parentDOM) parentDOM.appendChild(dom)
            } else if(target instanceof Node){
                // Native DOM element (e.g. document.body)
                target.appendChild(dom)
            }
            // QtObject parents without DOM (e.g. non-visual) are silently ignored
        }
    }

    __checkVisibility(){
        if(this.__getPropertyValue('visible')){
            this.__proxy.__DOM.setAttribute('visible', '') // temp
            this.__proxy.__DOM.removeAttribute('invisible')
            if(this.__getPropertyValue('width') > 0 && this.__getPropertyValue('height') > 0){
                this.__proxy.__DOM.removeAttribute('no-view')
            } else {
                this.__proxy.__DOM.setAttribute('no-view', '')
            }
        } else {
            this.__proxy.__DOM.removeAttribute('visible') // temp
            this.__proxy.__DOM.setAttribute('invisible', '')
        }
        
    }

    'SLOT_ListView.reused'(){
        this.__DOM.removeAttribute('cached')
    }

    'SLOT_ListView.pooled'(){
        this.__DOM.setAttribute('cached', '')
    }

    SLOT_objectNameChanged(oldValue, newValue){
        this.__DOM.setAttribute('objectName', newValue)
    }

    SLOT_focusChanged(oldValue, newValue){
        if(newValue){
            if(this.__isListViewDelegateItem()){
                return
            }

            let tree = this.__getTree()
            let accepted = JQApplication.setFocusTree(tree, {
                owner: this,
                firstWins: true
            })
            if(!accepted){
                this.focus = false
            }
        } else {
            this.activeFocus = false
        }
    }

    __isListViewDelegateItem(){
        let parent = this.parent
        while(parent){
            if(parent.parent instanceof JQModules.QtQuick.ListView && parent === parent.parent.contentItem){
                return true
            }
            parent = parent.parent
        }
        return false
    }

    SLOT_activeFocusChanged(oldValue, newValue){
        if(newValue){
            if(this.parent instanceof JQModules.QtQuick.FocusScope){
                this.parent.activeFocus = true
            }
        }
    }

    forceActiveFocus(){
        if(this.parent instanceof JQModules.QtQuick.FocusScope){
            this.parent.focus = true
        }

        if(!this.focus){
            this.focus = true
        }

        JQApplication.setFocusTree(this.__getTree(), {
            owner: this,
            immediate: true
        })
    }

    __getTree(){
        let tree = [this]
        let parent = this.parent

        while(parent){
            tree.push(parent)
            parent = parent.parent
        }

        return tree
    }

    __setFocusTree(tree){
        for(let child of this.children){
            if(tree.indexOf(child) < 0){
                if(!(typeof child.__isListViewDelegateItem === 'function' && child.__isListViewDelegateItem())){
                    child.focus = false
                }
            }

            // Don't recurse into FocusScopes — they manage their own children's
            // internal focus state. In Qt, items inside a FocusScope don't fire
            // focusChanged when the scope loses focus.
            if(!(child instanceof JQModules.QtQuick.FocusScope)){
                child.__setFocusTree(tree)
            }
        }
    }

    SLOT_parentChanged(oldValue, newValue){
        super.SLOT_parentChanged(oldValue, newValue)
        this.__connectDOM(newValue)
    }

    SLOT_xChanged(oldValue, newValue){
        this.__setDOMStyle({
            left: newValue+'px'
        })
        Geometry.setAuto(this.__self, 'AX', newValue, this.__self.constructor.meta.AX)
    }

    SLOT_yChanged(oldValue, newValue){
        this.__setDOMStyle({
            top: newValue+'px'
        })
        Geometry.setAuto(this.__self, 'AY', newValue, this.__self.constructor.meta.AY)
    }

    SLOT_enabledChanged(oldValue, newValue){
        for(let child of this.children){
            if(child.__self.constructor.meta.enabled.type === LinkedBool)
            LinkedBool.parentSet(child, 'enabled', newValue)
        }
    }

    SLOT_visibleChanged(oldValue, newValue){
        for(let child of this.children){
            if(child.__self.constructor.meta.visible.type === LinkedBool)
            LinkedBool.parentSet(child, 'visible', newValue)
        }

        this.__checkVisibility()
        JQApplication.updateLater(this.parent)
    }

    SLOT_opacityChanged(oldValue, newValue){
        this.__setDOMStyle({
            opacity: newValue
        })
    }

    SLOT_zChanged(oldValue, newValue){
        this.__setDOMStyle({ zIndex: newValue })
    }

    SLOT_widthChanged(oldValue, newValue){
        this.__checkVisibility()
        this.__setDOMStyle({
            width: newValue > 0 ? newValue + 'px' : '0px',
            minWidth: newValue > 0 ? newValue + 'px' : '0px',
        })
        this.implicitWidth = newValue
        JQApplication.updateLater(this.parent)
        Geometry.setAuto(this.__self, 'AWidth', newValue, this.__self.constructor.meta.AWidth)
        this.__updateSiblingAnchors()
    }

    SLOT_heightChanged(oldValue, newValue){
        this.__checkVisibility()
        this.__setDOMStyle({
            height: newValue > 0 ? newValue + 'px' : '0px',
            minHeight: newValue > 0 ? newValue + 'px' : '0px',
        })
        this.implicitHeight = newValue
        JQApplication.updateLater(this.parent)
        Geometry.setAuto(this.__self, 'AHeight', newValue, this.__self.constructor.meta.AHeight)
        this.__updateSiblingAnchors()
    }

    __updateSiblingAnchors(){
        if(!this.parent) return
        for(const sibling of this.parent.children){
            if(sibling === this) continue
            const s = sibling.__self || sibling
            if(typeof s.AY === 'function'){
                const val = s.AY()
                if(s.AY__prevent){
                    if(s.y !== val) s.__proxy.y = val
                } else {
                    Real.set(s, 'y', val, s.constructor.meta.y)
                }
            }
            if(typeof s.AX === 'function'){
                const val = s.AX()
                if(s.AX__prevent){
                    if(s.x !== val) s.__proxy.x = val
                } else {
                    Real.set(s, 'x', val, s.constructor.meta.x)
                }
            }
            if(typeof s.AHeight === 'function'){
                const val = s.AHeight()
                if(s.AHeight__prevent){
                    if(s.height !== val) s.__proxy.height = val
                } else {
                    Real.set(s, 'height', val, s.constructor.meta.height)
                }
            }
            if(typeof s.AWidth === 'function'){
                const val = s.AWidth()
                if(s.AWidth__prevent){
                    if(s.width !== val) s.__proxy.width = val
                } else {
                    Real.set(s, 'width', val, s.constructor.meta.width)
                }
            }
        }
    }

    SLOT_AXChanged(oldValue, newValue){
        if(this.__self.AX__prevent){
            this.x = newValue
        } else {
            Real.set(this.__self, 'x', newValue, this.__self.constructor.meta.x)
        }
    }

    SLOT_AYChanged(oldValue, newValue){
        if(this.__self.AY__prevent){
            this.y = newValue
        } else {
            Real.set(this.__self, 'y', newValue, this.__self.constructor.meta.y)
        }
    }

    SLOT_AWidthChanged(oldValue, newValue){
        if(this.__self.AWidth__prevent){
            this.width = newValue
        } else {
            Real.set(this.__self, 'width', newValue, this.__self.constructor.meta.width)
        }
    }

    SLOT_AHeightChanged(oldValue, newValue){
        if(this.__self.AHeight__prevent){
            this.height = newValue
        } else {
            Real.set(this.__self, 'height', newValue, this.__self.constructor.meta.height)
        }
    }

    SLOT_rotationChanged(oldValue, newValue){
        this.__setDOMStyle({
            transform: `scale(${this.__proxy.scale}) rotate(${newValue}deg)`
        })
        JQApplication.updateLater(this.parent)
    }
    SLOT_scaleChanged(oldValue, newValue){
        this.__setDOMStyle({
            transform: `scale(${newValue}) rotate(${this.__proxy.rotation}deg)`
        })
        JQApplication.updateLater(this.parent)
    }
    SLOT_clipChanged(oldValue, newValue){
        this.__setDOMStyle({
            overflow: newValue ? "hidden" : "unset"
        })
    }

    mapToItem(item, x, y){
        let rect1 = this.__getDOM().getBoundingClientRect()
        let res = {
            x: rect1.x + x,
			y: rect1.y + y,
        }
        if(item){
            let rect2 = item.__getDOM().getBoundingClientRect()
            res.x -= rect2.x
            res.y -= rect2.y
            return res
        } else {
            return res
        }
    }

    __destroy(){
        let dom = this.__DOM
        if(dom) dom.remove()
        super.__destroy()
        
        let index = JQApplication.focusTree.indexOf(this)
        if(index >= 0) JQApplication.focusTree.splice(index, 1)
    }

    __onMouseCanceled(mouse){}
    __onMouseEnter(mouse){}
    __onMouseLeave(mouse){}
    __onMouseMove(mouse){}
    __onMouseDown(mouse){}
    __onMouseUp(mouse){}
    __onMouseClick(mouse){}
    __onMouseDblClick(mouse){}
    __onWheel(wheel){}
}



module.exports = Item