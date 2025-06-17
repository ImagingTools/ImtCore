const Property = require("../Property")
const Int = require("../Int")
const Signal = require("../Signal")
const JQApplication = require("../../core/JQApplication")


class AbstractItemModel {
    static create(parent, index, data){
        let properties = []

        let model = new this()

        if(data){
            for(let key in data){
                model[key] = data[key]
            }
        }

        let proxy
        proxy = new Proxy(model, { 
            has(target, key){
                return key in target
            },
            get(target, key){
                let caller = Property.queueLink[Property.queueLink.length-1]
                if(caller) {
                    let found = false

                    for(let _caller of properties){
                        if(caller.name === _caller.name && caller.target === _caller.target){
                            found = true
                            break
                        }
                    }

                    if(!found){
                        properties.push(caller)
                    }
                    
                }

                if(key === 'index') {
                    if(!(key in target)) return parent.data.indexOf(proxy)
                }
                return target[key]
            },
            set(target, key, value){
                target[key] = value

                for(let property of properties){
                    // property.__update()
                    property.target.__proxy[property.name] = property.func()
                }

                parent.__propogate()
                
                return true
            }
        })

        return proxy
    }
}

module.exports = AbstractItemModel