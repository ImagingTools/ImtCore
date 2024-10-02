const Property = require("../Property")
const Int = require("../Int")
const Signal = require("../Signal")
const JQApplication = require("../../core/JQApplication")


class AbstractItemModel {
    static create(parent, index, data){
        let properties = []

        let model = Object.assign({}, data ? data : {})

        let proxy
        proxy = new Proxy(model, { 
            has(target, key){
                return key in target
            },
            get(target, key){
                let caller = Property.queueLink[Property.queueLink.length-1]
                if(caller && properties.indexOf(caller) < 0) {
                    properties.push(caller)
                }

                if(key === 'index') {
                    if(!(key in target)) return parent.data.indexOf(proxy)
                }
                return target[key]
            },
            set(target, key, value){
                target[key] = value

                for(let property of properties){
                    property.__update()
                }

                parent.dataChanged()
                
                return true
            }
        })

        return proxy
    }
}

module.exports = AbstractItemModel