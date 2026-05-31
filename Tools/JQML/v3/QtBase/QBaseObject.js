const BaseObject = require("./BaseObject")
const Property = require("../QtQml/Property")

class QBaseObject extends BaseObject {
    static singleton = false
    static uid = 0

    static meta = {}

    static handle = {
        get(target, key){
            if(target.constructor.meta.hasOwnProperty(key)){
                let node = target.constructor.meta[key]
                return node.type.get(target, key, node)
            } else {
                return target[key]
            }
        },

        set(target, key, value){
            if(target.constructor.meta.hasOwnProperty(key)){
                let node = target.constructor.meta[key]

                if(node.type.isAssignableFrom(Property)){
                    return node.type.reset(target, key, value, node)
                } else {
                    return node.type.set(target, key, value, node)
                }
            } else {
                target[key] = value

                return true
            }
        },
    }

    static create(parent = null, properties = {}){
        let obj = new this()
        let proxy = new Proxy(obj, this.handle)
        obj.__properties = properties
        // obj.__context = context
        obj.__proxy = proxy
        // obj.__proxy2 = new Proxy(obj, this.handle)
        obj.__self = obj
        return proxy
    }

    __connections = {}
    __subscribers = {}
    __depends = {}
    __properties = {}

    __getPropertyValue(name){
        if(this.__self.constructor.meta.hasOwnProperty(name)){
            let node = this.__self.constructor.meta[name]
            return node.type.simpleGet(this.__self, name, node)
        }
    }

    __setPropertyValue(name, value){
        if(this.__self.constructor.meta.hasOwnProperty(name)){
            let node = this.__self.constructor.meta[name]
            return node.type.simpleSet(this.__self, name, value, node)
        }
    }

    __resetPropertyValue(name, value){
        if(this.__self.constructor.meta.hasOwnProperty(name)){
            let node = this.__self.constructor.meta[name]
            return node.type.simpleReset(this.__self, name, value, node)
        }
    }

    __beginUpdate(){

    }

    __endUpdate(){

    }

    __resolve(){
        return this
    }

    __destroy(){
        
    }

    blockSignals(flag){
        this.__blockedSignals = flag
    }

    signalsBlocked(){
        return this.__blockedSignals
    }

    deleteLater(){
        this.destroy()
    }

    destroy(){
        JQApplication.deleteLater(this)
    }    
}

module.exports = QBaseObject