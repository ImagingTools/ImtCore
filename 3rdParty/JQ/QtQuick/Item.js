const BaseObject = require("../QtBase/BaseObject")
const QtObject = require("../QtQml/QtObject")
const Real = require("../QtQml/Real")
const Int = require("../QtQml/Int")
const Bool = require("../QtQml/Bool")
const LinkedBool = require("../QtQml/LinkedBool")
const Signal = require("../QtQml/Signal")
const SpecialSignal = require("../QtQml/SpecialSignal")
const SpecialProperty = require("../QtQml/SpecialProperty")
const KeyNavigation = require("../QtQml/KeyNavigation")
const Anchors = require("../QtQml/Anchors")
const AnchorLine = require("../QtQml/AnchorLine")
const JQApplication = require("../core/JQApplication")


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
        // AX: {type:SpecialProperty, value:0, signalName:'AXChanged'},
        // AY: {type:SpecialProperty, value:0, signalName:'AYChanged'},
        // AWidth: {type:SpecialProperty, value:0, signalName:'AWidthChanged'},
        // AHeight: {type:SpecialProperty, value:0, signalName:'AHeightChanged'},

        x: {type:Real, value:0, signalName:'xChanged'},
        y: {type:Real, value:0, signalName:'yChanged'},
        width: {type:Real, value:0, signalName:'widthChanged'},
        height: {type:Real, value:0, signalName:'heightChanged'},
        z: {type:Int, value:0, signalName:'zChanged'},
        rotation: { type: Real, value: 0, signalName: 'rotationChanged' },
        transformOrigin: { type: Real, value: Item.Center, signalName: 'transformOriginChanged' },
        scale: { type: Real, value: 1, signalName: 'scaleChanged' },
        opacity: { type: Real, value: 1, signalName: 'opacityChanged' },
        visible: { type: LinkedBool, link: 'visible', value: true, signalName: 'visibleChanged' },
        enabled: { type: LinkedBool, link: 'enabled', value: true, signalName: 'enabledChanged' },
        focus: { type: Bool, value: false, signalName: 'focusChanged' },
        activeFocus: { type: Bool, value: false, signalName: 'activeFocusChanged' },
        clip: { type: Bool, value: false, signalName: 'clipChanged' },

        KeyNavigation: {type:KeyNavigation},
        anchors: {type:Anchors},
        left: {type:AnchorLine, value: AnchorLine.Left},
        right: {type:AnchorLine, value: AnchorLine.Right},
        top: {type:AnchorLine, value: AnchorLine.Top},
        bottom: {type:AnchorLine, value: AnchorLine.Bottom},
        horizontalCenter: {type:AnchorLine, value: AnchorLine.HorizontalCenter},
        verticalCenter: {type:AnchorLine, value: AnchorLine.VerticalCenter},

        AXChanged: {type:SpecialSignal, slotName:'onAXChanged', args:[]},
        AYChanged: {type:SpecialSignal, slotName:'onAYChanged', args:[]},
        AWidthChanged: {type:SpecialSignal, slotName:'onAWidthChanged', args:[]},
        AHeightChanged: {type:SpecialSignal, slotName:'onAHeightChanged', args:[]},

        xChanged: {type:Signal, slotName:'onXChanged', args:[]},
        yChanged: {type:Signal, slotName:'onYChanged', args:[]},
        widthChanged: {type:Signal, slotName:'onWidthChanged', args:[]},
        heightChanged: {type:Signal, slotName:'onHeightChanged', args:[]},
        zChanged: {type:Signal, slotName:'onZChanged', args:[]},
        rotationChanged: {type:Signal, slotName:'onRotationChanged', args:[]},
        transformOriginChanged: {type:Signal, slotName:'onTransformOriginChanged', args:[]},
        scaleChanged: {type:Signal, slotName:'onScaleChanged', args:[]},
        opacityChanged: {type:Signal, slotName:'onOpacityChanged', args:[]},
        visibleChanged: {type:Signal, slotName:'onVisibleChanged', args:[]},
        enabledChanged: {type:Signal, slotName:'onEnabledChanged', args:[]},
        focusChanged: {type:Signal, slotName:'onFocusChanged', args:[]},
        activeFocusChanged: {type:Signal, slotName:'onActiveFocusChanged', args:[]},
        clipChanged: {type:Signal, slotName:'onClipChanged', args:[]},

        'Keys.asteriskPressed': {type:Signal, slotName: 'Keys.onAsteriskPressed', args: ['event'] },
        'Keys.backPressed': {type:Signal, slotName: 'Keys.onBackPressed', args: ['event'] },
        'Keys.backtabPressed': {type:Signal, slotName: 'Keys.onBacktabPressed', args: ['event'] },
        'Keys.callPressed': {type:Signal, slotName: 'Keys.onCallPressed', args: ['event'] },
        'Keys.cancelPressed': {type:Signal, slotName: 'Keys.onCancelPressed', args: ['event'] },
        'Keys.context1Pressed': {type:Signal, slotName: 'Keys.onContext1Pressed', args: ['event'] },
        'Keys.context2Pressed': {type:Signal, slotName: 'Keys.onContext2Pressed', args: ['event'] },
        'Keys.context3Pressed': {type:Signal, slotName: 'Keys.onContext3Pressed', args: ['event'] },
        'Keys.context4Pressed': {type:Signal, slotName: 'Keys.onContext4Pressed', args: ['event'] },
        'Keys.deletePressed': {type:Signal, slotName: 'Keys.onDeletePressed', args: ['event'] },
        'Keys.digit0Pressed': {type:Signal, slotName: 'Keys.onDigit0Pressed', args: ['event'] },
        'Keys.digit1Pressed': {type:Signal, slotName: 'Keys.onDigit1Pressed', args: ['event'] },
        'Keys.digit2Pressed': {type:Signal, slotName: 'Keys.onDigit2Pressed', args: ['event'] },
        'Keys.digit3Pressed': {type:Signal, slotName: 'Keys.onDigit3Pressed', args: ['event'] },
        'Keys.digit4Pressed': {type:Signal, slotName: 'Keys.onDigit4Pressed', args: ['event'] },
        'Keys.digit5Pressed': {type:Signal, slotName: 'Keys.onDigit5Pressed', args: ['event'] },
        'Keys.digit6Pressed': {type:Signal, slotName: 'Keys.onDigit6Pressed', args: ['event'] },
        'Keys.digit7Pressed': {type:Signal, slotName: 'Keys.onDigit7Pressed', args: ['event'] },
        'Keys.digit8Pressed': {type:Signal, slotName: 'Keys.onDigit8Pressed', args: ['event'] },
        'Keys.digit9Pressed': {type:Signal, slotName: 'Keys.onDigit9Pressed', args: ['event'] },
        'Keys.downPressed': {type:Signal, slotName: 'Keys.onDownPressed', args: ['event'] },
        'Keys.enterPressed': {type:Signal, slotName: 'Keys.onEnterPressed', args: ['event'] },
        'Keys.escapePressed': {type:Signal, slotName: 'Keys.onEscapePressed', args: ['event'] },
        'Keys.flipPressed': {type:Signal, slotName: 'Keys.onFlipPressed', args: ['event'] },
        'Keys.hangupPressed': {type:Signal, slotName: 'Keys.onHangupPressed', args: ['event'] },
        'Keys.leftPressed': {type:Signal, slotName: 'Keys.onLeftPressed', args: ['event'] },
        'Keys.menuPressed': {type:Signal, slotName: 'Keys.onMenuPressed', args: ['event'] },
        'Keys.noPressed': {type:Signal, slotName: 'Keys.onNoPressed', args: ['event'] },
        'Keys.pressed': {type:Signal, slotName: 'Keys.onPressed', args: ['event'] },
        'Keys.released': {type:Signal, slotName: 'Keys.onReleased', args: ['event'] },
        'Keys.returnPressed': {type:Signal, slotName: 'Keys.onReturnPressed', args: ['event'] },
        'Keys.rightPressed': {type:Signal, slotName: 'Keys.onRightPressed', args: ['event'] },
        'Keys.selectPressed': {type:Signal, slotName: 'Keys.onSelectPressed', args: ['event'] },
        'Keys.shortcutOverride': {type:Signal, slotName: 'Keys.onShortcutOverride', args: ['event'] },
        'Keys.spacePressed': {type:Signal, slotName: 'Keys.onSpacePressed', args: ['event'] },
        'Keys.tabPressed': {type:Signal, slotName: 'Keys.onTabPressed', args: ['event'] },
        'Keys.upPressed': {type:Signal, slotName: 'Keys.onUpPressed', args: ['event'] },
        'Keys.volumeDownPressed': {type:Signal, slotName: 'Keys.onVolumeDownPressed', args: ['event'] },
        'Keys.volumeUpPressed': {type:Signal, slotName: 'Keys.onVolumeUpPressed', args: ['event'] },
        'Keys.yesPressed': {type:Signal, slotName: 'Keys.onYesPressed', args: ['event'] },
    })

    static create(parent=null, model=null, meta={}, properties=[], isRoot=true){
        let obj = super.create(parent, model, meta, properties, isRoot)

        let dom = obj.__getDOM()
        dom.classList.add('Item')
        dom.qml = obj
        obj.__connectDOM(this.parent)

        return obj
    }

    __removeChild(child){
        let index = -1

        index = this.data.indexOf(child)
        if(index >= 0) this.data.osplice(index, 1)

        if(child instanceof Item){
            index = this.children.indexOf(child)
            if(index >= 0) this.children.osplice(index, 1)
        } else {
            index = this.resources.indexOf(child)
            if(index >= 0) this.resources.osplice(index, 1)
        }
    }

    __addChild(child){
        let index = -1

        index = this.data.indexOf(child)
        if(index < 0) this.data.opush(child)

        if(child instanceof Item){
            index = this.children.indexOf(child)
            if(index < 0) this.children.opush(child)
        } else {
            index = this.resources.indexOf(child)
            if(index < 0) this.resources.opush(child)
        }
    }

    __getDOM(){
        if(this.__DOM) {
            return this.__DOM
        } else {
            let dom = document.createElement('div')
            this.__DOM = dom
            return dom
        }
    }

    __setDOMStyle(style){
        if(this.__DOM) {
            for(let name in style){
                this.__DOM.style[name] = style[name]
            }
        }
    }

    __connectDOM(target){
        let dom = this.__getDOM()
        if(target && dom){
            if(target instanceof Item){
                target.__getDOM().appendChild(dom)
            } else {
                target.appendChild(dom)
            }
        }
    }

    __checkVisibility(){
        if(this.visible){
            this.__DOM.removeAttribute('invisible')
            if(this.width > 0 && this.height > 0){
                this.__DOM.removeAttribute('no-view')
            } else {
                this.__DOM.setAttribute('no-view', '')
            }
        } else {
            this.__DOM.setAttribute('invisible', '')
        }
        
    }

    onFocusChanged(){
        if(this.focus){
            let tree = this.__getTree()
            JQApplication.setFocusTree(tree)

            if(this.parent instanceof JQModules.QtQuick.FocusScope && this.parent.focus){
                this.activeFocus = true
            }
        } else {
            this.activeFocus = false
        }
    }

    onActiveFocusChanged(){
        if(this.activeFocus){
            if(this.parent instanceof JQModules.QtQuick.FocusScope){
                this.parent.activeFocus = true
            }
        }
    }

    forceActiveFocus(){
        if(this.parent instanceof JQModules.QtQuick.FocusScope){
            this.parent.focus = true
        }

        this.focus = true

        if(this.parent instanceof JQModules.QtQuick.FocusScope){
            this.parent.activeFocus = true
        }

        this.activeFocus = true
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
                child.focus = false
                
            }

            child.__setFocusTree(tree)
        }
    }

    // onAXChanged(){
    //     if(this.AX !== this.x) 
    //     this.__getDataQml('x').__resetForce(this.AX)
    // }

    // onAYChanged(){
    //     if(this.AY !== this.y) 
    //     this.__getDataQml('y').__resetForce(this.AY)
    // }   

    // onAWidthChanged(){
    //     if(this.AWidth !== this.width) 
    //     this.__getDataQml('width').__resetForce(this.AWidth)
    // }

    // onAHeightChanged(){
    //     if(this.AHeight !== this.height) 
    //     this.__getDataQml('height').__resetForce(this.AHeight)
    // }

    onParentChanged(){
        super.onParentChanged()
        this.__connectDOM(this.parent)
    }

    onXChanged(){
        // this.AX = this.x
        
        this.__setDOMStyle({
            left: this.x+'px'
        })
    }

    onYChanged(){
        // this.AY = this.y

        this.__setDOMStyle({
            top: this.y+'px'
        })
    }

    onEnabledChanged(){
        for(let child of this.children){
            child.__getDataQml('enabled').__update()
        }
    }

    onVisibleChanged(){
        for(let child of this.children){
            child.__getDataQml('visible').__update()
        }

        this.__checkVisibility()
        JQApplication.updateLater(this.parent)
    }

    onOpacityChanged(){
        this.__setDOMStyle({
            opacity: this.opacity
        })
    }

    onZChanged(){
        this.__setDOMStyle({ zIndex: this.z })
    }

    onWidthChanged(){
        // this.AWidth = this.width
        
        this.__checkVisibility()
        this.__setDOMStyle({
            width: this.width > 0 ? this.width + 'px' : '0px',
            minWidth: this.width > 0 ? this.width + 'px' : '0px',
        })
        JQApplication.updateLater(this.parent)
    }

    onHeightChanged(){
        // this.AHeight = this.height

        this.__checkVisibility()
        this.__setDOMStyle({
            height: this.height > 0 ? this.height + 'px' : '0px',
            minHeight: this.height > 0 ? this.height + 'px' : '0px',
        })
        JQApplication.updateLater(this.parent)
    }

    onRotationChanged(){
        this.__setDOMStyle({
            transform: `scale(${this.scale}) rotate(${this.rotation}deg)`
        })
        JQApplication.updateLater(this.parent)
    }
    onScaleChanged(){
        this.__setDOMStyle({
            transform: `scale(${this.scale}) rotate(${this.rotation}deg)`
        })
        JQApplication.updateLater(this.parent)
    }
    onClipChanged(){
        this.__setDOMStyle({
            overflow: this.clip ? "hidden" : "unset"
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
        if(this.__DOM) this.__DOM.remove()
        super.__destroy()
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

Item.initialize()

module.exports = Item