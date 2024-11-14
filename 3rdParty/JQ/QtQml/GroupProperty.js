const BaseObject = require("../QtBase/BaseObject")
const Property = require("./Property")

class GroupProperty extends BaseObject {
    static create(parent, meta){
        let proxy = super.create(parent, meta)
        let self = proxy.__self 
        self.__properties = parent.__properties
        self.__parent = parent.__proxy

        return proxy
    }

    __toPrimitive(hint){
        return this
    }

    __emitSignal(){
        if(this.__parent) {
            let signal = this.__parent.__get(this.__signalName)
            if(signal) signal()
        }
    }

    __assign(obj){
        if(obj && typeof obj === 'object'){
            for(let name in obj){
                if(this.__has(name)) {
                    this.__set(name, obj[name])
                } else {
                    throw `Can not assign ${name} to this group property`
                }
            }
        } else {
            throw `Can not assign ${obj} to this group property`
        }
    }
    
    __has(key){
        if(key in this){
            return true
        } else if(key in this.constructor.meta){
            let meta = this.constructor.meta[key]
            this[key] = meta.type.create(this, meta)

            return true
        }

        return false
    }

    __get(key){
        if(this.__has(key)){
            if(this[key] instanceof Property){
                // let caller = Property.queueLink[Property.queueLink.length-1]
                // if(caller) caller.__subscribe(this[key])
                return this[key].__get()
            }
            return this[key]
        }
        
        return undefined
    }

    __set(key, value){
        if(this.__has(key)){
            if(this[key] instanceof Property){
                this[key].__reset(value)
                return true
            }
        }
        this[key] = value
        return true
    }

    __destroy(){
        super.__destroy()

        let self = this.__self
        delete self.__proxy
        for(let key in self){
            if(self[key] instanceof Property || self[key] instanceof GroupProperty){
                self[key].__destroy()
            }
            delete self[key]
        }
    }
}

module.exports = GroupProperty