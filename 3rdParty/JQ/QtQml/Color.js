const Property = require("./Property")

class Color extends Property {
    static defaultValue = '#000000'
    
    __typecasting(value){
        if(typeof value === 'object') {
            if(value instanceof Property) return value[Symbol.toPrimitive]()
        }
        return value
    }
}

module.exports = Color