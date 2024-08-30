const Property = require("./Property")

class Alias extends Property {
    __getter = null
    __setter = null

    __get(key){
        return this.__getter(key)
    }
    
    __set(key, value){
        this.__auto = false

        if(typeof value === 'function' && value.bound){
            this.__setCompute(value)
            if(value.queue) {
                value.queue.push(this)
            } else {
                this.__update()
            }
            return true
        }

        this.__setter(value)

        return true
    }

    __update(){
        if(this.__updating || (!this.__compute && !this.__getter)) return
        
        this.__updating = true
        Property.queueLink.push(this)
        let value = this.__value
        try {
            value = this.__getter()
            if(this.__compute) value = this.__compute()
        } catch (error) {
            
        } finally {
            Property.queueLink.pop()
        }
        
        if(this.__compute) this.__set(undefined, value)
        if(this.__value !== value){
            this.__value = value
            this.__emitSignal()
        }
        
        this.__updating = false
        this.__completed = true
    }

    __aliasInit(getter, setter, queue){
        this.__getter = getter
        this.__setter = setter
        if(queue){
            queue.push(this)
        }
    }
}

module.exports = Alias