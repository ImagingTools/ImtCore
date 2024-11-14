class BaseObject{
    static handler = {
        has(target, key){ 
            if(key[0] === '_' && key[1] === '_') {
                if(key === '__self') return true
                return key in target
            }

            return target.__has(key)
        },
        get(target, key){
            if(key === Symbol.toPrimitive) return target.__toPrimitive
            // if(key === Symbol.iterator) return target.__toPrimitive

            if(key[0] === '_' && key[1] === '_') {
                if(key === '__self') return target
                return target[key]
            }


            return target.__get(key)
        },
        set(target, key, value){
            if(key[0] === '_' && key[1] === '_') {
                if(key === '__self') return false
                target[key] = value
                return true
            }

            return target.__set(key, value)
        },
    }

    static create(...args){
        let obj = new this(...args)
        let proxy = new Proxy(obj, this.handler)
        obj.__proxy = proxy
        return proxy
    }

    static isAssignableFrom(cls){
        let prototype = this
        while(prototype){
            if(cls === prototype) return true

            prototype = prototype.__proto__
        }
        return false
    }

    __toPrimitive(hint){

    }
    
    __has(key){

    }

    __get(key){

    }

    __getObject(key){
        if(this.__has(key)){
            return this.__self[key]
        }
        return undefined
    }

    __set(key, value){

    }

    __destroy(){
        
    }

    
    __beginUpdate(){

    }

    __endUpdate(){
        
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
        this.__destroying = true
        JQApplication.deleteLater(this)
    }
}

module.exports = BaseObject