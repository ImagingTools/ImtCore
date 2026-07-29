const QtObject = require("../QtQml/QtObject")
const QtEnums = require("../Qt/enums")
const Bool = require("../QtQml/Bool")
const Var = require("../QtQml/Var")
const Int = require("../QtQml/Int")
const Real = require("../QtQml/Real")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")

class PointerHandler extends QtObject {
    static TakeOverForbidden = 0
    static CanTakeOverFromHandlersOfSameType = 1
    static CanTakeOverFromHandlersOfDifferentType = 2
    static CanTakeOverFromItems = 3
    static CanTakeOverFromAnything = 4
    static ApprovesTakeOverByHandlersOfSameType = 5
    static ApprovesTakeOverByHandlersOfDifferentType = 6
    static ApprovesTakeOverByItems = 7
    static ApprovesCancellation = 8
    static ApprovesTakeOverByAnything = 9

    static meta = Object.assign({}, QtObject.meta, {
        active: { type: Bool, value: false},
        cursorShape: { type: String, value: ''},
        enabled: { type: Bool, value: true},
        grabPermissions: { type: Int, value: PointerHandler.CanTakeOverFromItems | PointerHandler.CanTakeOverFromHandlersOfDifferentType | PointerHandler.ApprovesTakeOverByAnything },
        margin: { type: Real, value: 0},
        target: { type: Var, value: undefined},

        activeChanged: { type:Signal, args:[] },
        cursorShapeChanged: { type:Signal, args:[] },
        enabledChanged: { type:Signal, args:[] },
        grabPermissionsChanged: { type:Signal, args:[] },
        marginChanged: { type:Signal, args:[] },
        targetChanged: { type:Signal, args:[] },

        canceled: { type:Signal, args:['point'] },
        grabChanged: { type:Signal, args:['transition', 'point'] },
    })

    __isHandlerEnabled(){
        return !!this.enabled && !this.__destroyed
    }

    __getEffectiveTarget(){
        if(this.target !== undefined) return this.target
        if(this.parent && this.parent instanceof JQModules.QtQuick.Item) return this.parent
        return null
    }

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)

        let dom = obj.__getDOM()
        dom.classList.add('PointerHandler')
        dom.qml = obj

        JQApplication.MouseController.add(obj)
        obj.__connectDOM(parent)

        return obj
    }

    __getDOM(){
        let dom = this.__DOM
        if(dom) {
            return dom
        }

        dom = document.createElement('div')
        dom.style.position = 'absolute'
        dom.style.left = '0px'
        dom.style.top = '0px'
        dom.style.right = '0px'
        dom.style.bottom = '0px'
        dom.style.width = '100%'
        dom.style.height = '100%'
        dom.style.pointerEvents = 'auto'
        dom.style.opacity = '0'
        this.__DOM = dom
        return dom
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
            if(typeof target.__getDOM === 'function'){
                let parentDOM = target.__getDOM()
                if(parentDOM) parentDOM.appendChild(dom)
            } else if(target instanceof Node){
                target.appendChild(dom)
            }
        }
    }

    SLOT_parentChanged(oldValue, newValue){
        super.SLOT_parentChanged(oldValue, newValue)
        this.__connectDOM(newValue)
    }

    __destroy(){
        JQApplication.MouseController.remove(this)
        if(this.__DOM && this.__DOM.parentNode){
            this.__DOM.parentNode.removeChild(this.__DOM)
        }
        super.__destroy()
    }
}



module.exports = PointerHandler