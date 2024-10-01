const BaseObject = require("../QtBase/BaseObject")

class Property extends BaseObject {
    static queueLink = []
    static defaultValue = null

    static create(parent, meta){
        let proxy = super.create(parent, meta)
        let self = proxy.__self 

        self.__parent = parent ? parent.__proxy : null
        self.__value = 'value' in meta ? meta.value : this.defaultValue
        self.__signalName = meta.signalName

        return proxy
    }

    __auto = true

    __toPrimitive(hint){ 
        let value = this.__get()
        switch(hint){
            case 'number': {
                if(value === undefined || value === null) throw `${value} не может быть преобразован в Number`
                switch(typeof value){
                    case 'number': return value
                    case 'string': {
                        let number = Number(value)
                        if(isNaN(number)) throw `${value} не может быть преобразован в Number`
                        return number
                    }
                    case 'object': throw `${value} не может быть преобразован в Number`
                }
            }
            case 'string': {
                if(value === undefined || value === null) throw `${value} не может быть преобразован в String`
                switch(typeof value){
                    case 'number': return value.toString()
                    case 'string': return value
                    case 'object': throw `${value} не может быть преобразован в String`
                }
            }
            default: return value
        }
    }
    
    __has(key){
        return false
    }

    __get(key){
        let caller = Property.queueLink[Property.queueLink.length-1]
        if(caller) caller.__subscribe(this)

        if(!this.__completed){
            this.__update()
        } 

        if(key && this.__value instanceof BaseObject){
            return this.__value[key]
        }

        return this.__value
    }

    __getSignal(){
        return this.__parent ? this.__parent.__get(this.__signalName) : undefined
    }

    __emitSignal(...args){
        let signal = this.__getSignal()
        if(signal) signal(...args)
    }

    __reset(value){
        this.__subscribersReset()
        if(this.__frozen) return
        this.__unsubscribe()
        this.__set(undefined, value)
    }

    __resetForce(value){
        this.__subscribersReset()
        this.__unsubscribe()
        this.__set(undefined, value)
    }

    __set(key, value){
        this.__auto = false

        if(typeof value === 'function' && value.bound){
            this.__setCompute(value)
            if(value.lazy) {
                this.__parent.__properties.push(this)
            } else {
                this.__update()
            }
            return true
        }

        let safeValue
        try {
            safeValue = this.__typecasting(value)
        } catch (error) {
            console.log(error)
            return false
        }
        

        if(key){
            if(safeValue !== this.__value[key]){
                this.__value[key] = safeValue
            }
        } else if(safeValue !== this.__value){
            this.__value = safeValue
            this.__emitSignal()
        }
        
        return true
    }

    __setAuto(newValue){
        if(this.__auto) {
            this.__set(undefined, newValue)
            this.__auto = true
        }
    }

    __typecasting(value){
        return value
    }

    __update(){
        if(this.__updating || !this.__compute) return
        
        this.__updating = true
        Property.queueLink.push(this)
        let value = this.__value
        try {
            value = this.__compute()
        } catch (error) {
            value = this.__value
        } finally {
            Property.queueLink.pop()
        }
        
        if(this.__compute) this.__set(undefined, value)
        
        this.__updating = false
        this.__completed = true
    }

    __addSubscriber(target){
        if(!this.__subscribers) this.__subscribers = []
        let signal = this.__getSignal()

        if(signal && this.__subscribers.indexOf(target) < 0) {
            this.__subscribers.push(target)
            signal.connect(target, target.__update)
        }
    }

    __removeSubscriber(target){
        let signal = this.__getSignal()
        if(!this.__subscribers || !signal) return

        signal.disconnect(target, target.__update)
        let index = this.__subscribers.indexOf(target)
        if(index >= 0){
            this.__subscribers.splice(index, 1)
            if(this.__subscribers.length === 0) {
                delete this.__subscribers
            }
        }
    }

    __unsubscribe(){
        delete this.__compute
        delete this.__updating

        if(!this.__depends) return
        
        while(this.__depends.length){
            let target = this.__depends.pop()
            target.__removeSubscriber(this)
        }
        delete this.__depends
    }

    __subscribe(...targets){
        if(!this.__depends) this.__depends = []

        for(let target of targets){
            if(target && target.__addSubscriber){
                if(this.__depends.indexOf(target) < 0) {
                    this.__depends.push(target)
                }
        
                target.__addSubscriber(this.__proxy)
            }
            
        }
        
    }

    __subscribersReset(){
        if(this.__subscribers)
        for(let subscriber of this.__subscribers){
            subscriber.__updating = false
        }
    }

    __setCompute(compute){
        if(this.__frozen) return
        this.__updating = false
        this.__compute = compute
        this.__completed = false
    }

    __freeze(){
        this.__frozen = true
    }

    __unfreeze(){
        this.__frozen = false
    }

    __destroy(){
        super.__destroy()
        this.__unsubscribe()
        let self = this.__self
        delete self.__proxy
        for(let key in self){
            delete self[key]
        }
    }
}

module.exports = Property