const Bool = require("./Bool")

class LinkedBool extends Bool {
    static create(parent, meta){
        let proxy = super.create(parent, meta)
        let self = proxy.__self 

        self.__originValue = meta.value
        self.__originCompute = ()=>{
            if(self.__parent && self.__parent.parent && meta.link){
                return self.__parent.parent[meta.link] && self.__originValue
            } else {
                return self.__originValue
            }
        }
        self.__compute = self.__originCompute

        return proxy
    }

    __reset(value){
        if(typeof value === 'function' && value.bound){
            this.__setCompute(value)
            if(value.lazy) {
                this.__parent.__properties.push(this)
            } else {
                this.__update()
            }
            return true
        }

        let safeValue = this.__typecasting(value)
        this.__originValue = safeValue

        this.__subscribersReset()
        this.__unsubscribe()

        this.__compute = this.__originCompute

        this.__update()
    }

    __setCompute(compute){
        this.__compute = ()=>{ return this.__originCompute() && compute()}
        this.__completed = false
    }
}

module.exports = LinkedBool