const Property = require("./Property")

class Bool extends Property {
    static defaultValue = false

    __typecasting(value){
        if(value === undefined) throw 'Cannot assign [undefined] to QBool'
        
        if(typeof value === 'object') {
            if(value instanceof Property) return value[Symbol.toPrimitive]() ? true : false
        }

        return value ? true : false
    }
}

module.exports = Bool