const Property = require("./Property")

class String extends Property {
    static getDefaultValue(){
        return ''
    }

    static typeCasting(value){
        if(typeof value === 'object') throw 'Cannot assign JSValue to String'
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