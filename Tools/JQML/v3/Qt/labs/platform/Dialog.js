const QtObject = require("../../../QtQml/QtObject")
const String = require("../../../QtQml/String")
const Bool = require("../../../QtQml/Bool")
const Var = require("../../../QtQml/Var")
const Int = require("../../../QtQml/Int")
const Real = require("../../../QtQml/Real")
const List = require("../../../QtQml/List")
const Signal = require("../../../QtQml/Signal")

class Dialog extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        data: { type: List},
        flags: { type: Real, value:0},
        modality: { type: Real, value:0},
        parentWindow: { type: Var, value:undefined},
        result: { type: Int, value:0},
        title: { type: String, value:''},
        visible: { type: Bool, value:false},

        dataChanged: {type:Signal, args:[]},
        flagsChanged: {type:Signal, args:[]},
        modalityChanged: {type:Signal, args:[]},
        parentWindowChanged: {type:Signal, args:[]},
        resultChanged: {type:Signal, args:[]},
        titleChanged: {type:Signal, args:[]},
        visibleChanged: {type:Signal, args:[]},

        accepted: {type:Signal, args:[]},
        rejected: {type:Signal, args:[]},
    })

    __getDOM(){
        let dom = this.__DOM
        if(dom) {
            return dom
        } else {
            dom = document.createElement('input')
            dom.type = 'file'
            dom.style.display = 'none'
            this.__DOM = dom

            this.__DOM.addEventListener('change', (e)=>{this.__change(e)})
            return dom
        }
    }

    __change(){

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
            if(target instanceof JQModules.QtQuick.Item){
                target.__getDOM().appendChild(dom)
            } else {
                target.appendChild(dom)
            }
        }
    }

    SLOT_parentChanged(oldValue, newValue){
        super.SLOT_parentChanged(oldValue, newValue)
        this.__connectDOM(newValue)
    }

    accept(){}
    close(){}
    done(result){}
    open(){}
    reject(){}

    destroy(){
        this.__DOM.remove()
        super.destroy()
    }
}



module.exports = Dialog