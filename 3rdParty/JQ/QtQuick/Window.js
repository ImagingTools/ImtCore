const QtObject = require("../QtQml/QtObject")
const Item = require("../QtQuick/Item")
const String = require("../QtQml/String")
const Geometry = require("../QtQml/Geometry")
const LinkedBool = require("../QtQml/LinkedBool")
const Signal = require("../QtQml/Signal")

class Window extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        title: {type:String, value:0, },
        width: {type:Geometry, value:0, },
        height: {type:Geometry, value:0, },
        visible: { type: LinkedBool, link: 'visible', value: true},

        titleChanged: {type:Signal, args:[]},
        widthChanged: {type:Signal, args:[]},
        heightChanged: {type:Signal, args:[]},
        visibleChanged: {type:Signal, args:[]},

    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)

        let dom = obj.__getDOM()
        dom.classList.add('Window')

        window.addEventListener('resize', ()=>{
            obj.width = window.innerWidth
            obj.height = window.innerHeight
        })

        dom.qml = obj
        obj.__connectDOM(this.parent)

        return obj
    }

    visible = {
        parentValue: true,
        value: true,
    }

    appendChild(dom){
        this.__DOM.appendChild(dom)
    }

    __complete(){
        this.width = window.innerWidth
        this.height = window.innerHeight

        this.__checkVisibility()
        super.__complete()
    }

    __removeChild(child){
        let index = -1

        index = this.data.indexOf(child)
        if(index >= 0) this.data.__splice(index, 1)

        // if(child instanceof Item){
        //     index = this.children.indexOf(child)
        //     if(index >= 0) this.children.__splice(index, 1)
        // } else {
        //     index = this.resources.indexOf(child)
        //     if(index >= 0) this.resources.__splice(index, 1)
        // }
    }

    __addChild(child){
        this.data.__push(child)

        // if(child instanceof Item){
        //     this.children.__push(child)
        // } else {
        //     this.data.__push(child)
        // }
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


    SLOT_parentChanged(oldValue, newValue){
        super.SLOT_parentChanged(oldValue, newValue)
        this.__connectDOM(newValue)
    }

    SLOT_titleChanged(oldValue, newValue){
        document.title = newValue
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

}



module.exports = Window