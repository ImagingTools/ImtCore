
const Property = require("./Property")

class Int extends Property {
    static defaultValue = 0
    
    __typecasting(value){
        if(value === undefined) throw 'Cannot assign [undefined] to int'
        if(value === null) throw 'Cannot assign std::nullptr_t to int'

        if(typeof value === 'object') {
            if(value instanceof Property) return value[Symbol.toPrimitive]('number')
            throw 'Cannot assign QJSValue to int'
        }

        if(typeof value === 'number') {
            if(isNaN(value) || value === Infinity || value === -Infinity) return -2147483648
            return Number.parseInt(value)
        }

        if(value === true) {
            return 1
        }

        if(value === false) {
            return 0
        }

        if(typeof value === 'string') {
            let number = Number.parseInt(value)
            if(isNaN(number) || number === Infinity || number === -Infinity) throw 'Cannot assign QString to int'
            return number
        }
    }
}

module.exports = Int