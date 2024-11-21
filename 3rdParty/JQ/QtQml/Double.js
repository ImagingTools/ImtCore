const Property = require("./Property")

class Double extends Property {
    static defaultValue = 0
    
    __typecasting(value){
        if(typeof value === 'object') {
            if(value instanceof Property) return value.__get()
        }
        return value
    }
}

module.exports = Double