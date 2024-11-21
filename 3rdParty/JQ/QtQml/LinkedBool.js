const Bool = require("./Bool")

class LinkedBool extends Bool {
    static create(parent, meta){
        let obj = super.create(parent, meta)

        obj.__originValue = meta.value
        obj.__originCompute = ()=>{
            if(obj.__parent && obj.__parent.parent && meta.link){
                return obj.__parent.parent[meta.link] && obj.__originValue
            } else {
                return obj.__originValue
            }
        }
        obj.__compute = obj.__originCompute

        return obj
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