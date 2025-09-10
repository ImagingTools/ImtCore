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
        x: {type:Geometry, value:0, signalName:'xChanged'},
        y: {type:Geometry, value:0, signalName:'yChanged'},
        width: {type:Geometry, value:0, signalName:'widthChanged'},
        height: {type:Geometry, value:0, signalName:'heightChanged'},
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

    __complete(){
        this.__checkVisibility()
        super.__complete()
    }

    __removeChild(child){
        let index = -1

        index = this.data.indexOf(child)
        if(index >= 0) this.data.__splice(index, 1)

        if(child instanceof Item){
            index = this.children.indexOf(child)
            if(index >= 0) this.children.__splice(index, 1)
        } else {
            index = this.resources.indexOf(child)
            if(index >= 0) this.resources.__splice(index, 1)
        }
    }

    __addChild(child){
        let index = -1

        index = this.data.indexOf(child)
        if(index < 0) this.data.__push(child)

        if(child instanceof Item){
            index = this.children.indexOf(child)

            if(index < 0) this.children.__push(child)
        } else {
            index = this.resources.indexOf(child)

            if(index < 0) this.resources.__push(child)
        }
    }

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
                target.__getDOM().appendChild(dom)
            } else {
                target.appendChild(dom)
            }
        }
    }

    __checkVisibility(){
        if(this.__getPropertyValue('visible')){
            this.__proxy.__DOM.removeAttribute('invisible')
            if(this.__getPropertyValue('width') > 0 && this.__getPropertyValue('height') > 0){
                this.__proxy.__DOM.removeAttribute('no-view')
            } else {
                this.__proxy.__DOM.setAttribute('no-view', '')
            }
        } else {
            this.__proxy.__DOM.setAttribute('invisible', '')
        }
        
    }

    SLOT_focusChanged(oldValue, newValue){
        if(newValue){
            let tree = this.__getTree()
            JQApplication.setFocusTree(tree)

            if(this.parent instanceof JQModules.QtQuick.FocusScope && this.parent.focus){
                this.activeFocus = true
            }
        } else {
            this.activeFocus = false
        }
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

    SLOT_parentChanged(oldValue, newValue){
        super.SLOT_parentChanged(oldValue, newValue)
        this.__connectDOM(newValue)
    }

    SLOT_xChanged(oldValue, newValue){
        this.__setDOMStyle({
            left: newValue+'px'
        })
    }

    SLOT_yChanged(oldValue, newValue){
        this.__setDOMStyle({
            top: newValue+'px'
        })
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
        JQApplication.updateLater(this.parent)
    }

    SLOT_heightChanged(oldValue, newValue){

        this.__checkVisibility()
        this.__setDOMStyle({
            height: newValue > 0 ? newValue + 'px' : '0px',
            minHeight: newValue > 0 ? newValue + 'px' : '0px',
        })
        JQApplication.updateLater(this.parent)
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