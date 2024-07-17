const Property = require("./Property")

class String extends Property {
    static defaultValue = ''

    __typecasting(value){
        if(typeof value === 'object') {
            if(value instanceof Property) return value[Symbol.toPrimitive]('string')
        }
        return value
    }
}

module.exports = String