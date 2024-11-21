const Property = require("./Property")

class String extends Property {
    static defaultValue = ''

    __typecasting(value){
        if(typeof value === 'object') {
            if(value instanceof Property) {
                return value.__get()
            } else {
                throw 'Cannot assign JSValue to String'
            }
        }

        if(value === undefined) throw 'Cannot assign [undefined] to String'
        if(typeof value === 'number') {
            if(isNaN(value)) return 'nan'
            if(value === Infinity) return 'inf'
            if(value === -Infinity) return '-inf'
            return value.toString()
        }
        if(value === null) return ''
        return value
    }
}

module.exports = String