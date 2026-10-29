const Property = require("./Property")

class String extends Property {
    static getDefaultValue(){
        return ''
    }

    static typeCasting(value){
        if(value === undefined) throw 'Cannot assign [undefined] to String'
        if(value === null) return ''
        if(typeof value === 'number') {
            if(isNaN(value)) return 'nan'
            if(value === Infinity) return 'inf'
            if(value === -Infinity) return '-inf'
            return value.toString()
        }
        if(value === true) return 'true'
        if(value === false) return 'false'
        if(typeof value === 'object') throw 'Cannot assign JSValue to String'
        return value
    }
}

module.exports = String