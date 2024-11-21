const Property = require("./Property")

class Real extends Property {
    static defaultValue = 0
    
    __typecasting(value){
        if(value === undefined) throw 'Cannot assign [undefined] to double'
        if(value === null) throw 'Cannot assign std::nullptr_t to double'

        if(typeof value === 'object') {
            if(value instanceof Property) return value.__get()
            throw 'Cannot assign QJSValue to double'
        }

        if(typeof value === 'number') {
            return value
        }

        if(value === true) {
            return 1
        }

        if(value === false) {
            return 0
        }

        if(typeof value === 'string') {
            let number = Number.parseFloat(value)
            if(isNaN(number) || number === Infinity || number === -Infinity) throw 'Cannot assign QString to double'
            return number
        }
        
        return value
    }
}

module.exports = Real