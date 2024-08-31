const Property = require("./Property")

class List extends Property {
    static create(parent, meta){
        let proxy = super.create(parent, meta)
        let self = proxy.__self 

        self.__value = []

        return proxy
    }

    __typecasting(value){
        if(typeof value === 'object' && Array.isArray(value) && !value.isExtended){
            value.isExtended = true
            let push = value.push
            value.push = (...args)=>{
                push.call(value, ...args)
                this.__emitSignal(value.length-1, value.length, 'append')
            }
        } else {
            throw `${value} не может быть преобразован в Array`
        }
        return value
    }
}

module.exports = List