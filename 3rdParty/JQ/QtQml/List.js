const Property = require("./Property")

class List extends Property {
    static create(parent, meta){
        let proxy = super.create(parent, meta)
        let self = proxy.__self 

        self.__value = []

        return proxy
    }
}

module.exports = List