const Property = require("./Property")

class Date extends Property {
    __typecasting(value){
        if(typeof value === 'object') {
            if(value instanceof Property) return value.__get()
        }
        return value
    }
}

module.exports = Date