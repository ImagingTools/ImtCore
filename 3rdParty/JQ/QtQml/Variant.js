const BaseObject = require("../QtBase/BaseObject")
const Property = require("./Property")

class Variant extends Property {
    static create(parent, meta){
        let obj = super.create(parent, meta)

        if(meta.typeTarget) obj.__type = meta.typeTarget
        return obj
    }

    __type = BaseObject

    __typecasting(value){
        if(value !== null && value !== undefined){
            if(value instanceof this.__type) {
                return value
            } else {
                throw `${value} Не может быть преобразован в ${this.__type}`
            }
        } else {
            return value
        }
        
    }
}

module.exports = Variant