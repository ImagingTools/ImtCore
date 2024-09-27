const Property = require("./Property")

class List extends Property {
    static create(parent, meta){
        let proxy = super.create(parent, meta)
        let self = proxy.__self 

        self.__value = self.__typecasting([])

        return proxy
    }

    __typecasting(value){
        if(typeof value === 'object' && Array.isArray(value) && !value.isExtended){
            value.isExtended = true
            let push = value.push
            value.opush = (...args)=>{
                push.call(value, ...args)
            }
            value.push = (...args)=>{
                push.call(value, ...args)
                this.__emitSignal(value.length-1, value.length, 'append')
            }
            let splice = value.splice
            value.osplice = (...args)=>{
                splice.call(value, ...args)
            }
            value.splice = ()=>{
                throw `List doesn't define a Replace function`
            }
        } else {
            throw `${value} не может быть преобразован в Array`
        }
        return value
    }
}

module.exports = List