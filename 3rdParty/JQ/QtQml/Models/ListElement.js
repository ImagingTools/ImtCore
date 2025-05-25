const QtObject = require("../../QtQml/QtObject")
const Property = require("../../QtQml/Property")

class ListElement extends QtObject {
    static handle = {
        get(target, key){
            if(target.constructor.meta.hasOwnProperty(key)){
                let node = target.constructor.meta[key]
                return node.type.get(target, key, node)
            } else {
                if(target.hasOwnProperty(key)){
                    return typeof target[key] === 'function' ? target[key].call(target.__proxy) : target[key]
                }
                return target[key]
            }
        },

        set(target, key, value){
            if(target.constructor.meta.hasOwnProperty(key)){
                let node = target.constructor.meta[key]

                if(node.type.isAssignableFrom(Property)){
                    return node.type.reset(target, key, value, node)
                } else {
                    return node.type.set(target, key, value, node)
                }
            } else {
                target[key] = value

                return true
            }
        },
    }
}



module.exports = ListElement