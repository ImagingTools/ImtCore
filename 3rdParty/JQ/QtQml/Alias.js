const Property = require("./Property")

class Alias extends Property {
    __typecasting(value){
        if(typeof value === 'object') {
            if(value instanceof Property) return value[Symbol.toPrimitive]()
        }
        return value
    }
}

module.exports = Alias