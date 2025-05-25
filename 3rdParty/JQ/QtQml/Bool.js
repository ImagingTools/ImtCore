const Property = require("./Property")

class Bool extends Property {
    static getDefaultValue(){
        return false
    }

    static typeCasting(value){
        // if(value === undefined) throw 'Cannot assign [undefined] to Bool'

        return value ? true : false
    }
}

module.exports = Bool