const Property = require("./Property")

class Bool extends Property {
    static defaultValue = false

    __typecasting(value){
        if(typeof value === 'object') {
            if(value instanceof Property) return value[Symbol.toPrimitive]('number') ? true : false
        }
        return value
    }
}

module.exports = Bool