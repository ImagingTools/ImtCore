const QObject = require("../QObject")
const Property = require("../Property")
const Int = require("../Int")
const Signal = require("../Signal")
const JQApplication = require("../../core/JQApplication")


function recursiveAddLink(obj){
    if(typeof obj === 'object'){
        if(obj instanceof QObject){
            obj.__addLink()
        } else {
            for(let key in obj){
                recursiveAddLink(obj[key])
            }
        }
    }
    
}

function recursiveRemoveLink(obj){
    if(typeof obj === 'object'){
        if(obj instanceof QObject){
            obj.__removeLink()
        } else {
            for(let key in obj){
                recursiveRemoveLink(obj[key])
            }
        }
    }
}

class AbstractItemModel {
    static create(parent, index, data){
        let properties = {}

        let model = new this()

        if(data){
            for(let key in data){
                if(data[key] instanceof QObject){
                    data[key].__addLink()
                }
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

                    if(!properties[key]) properties[key] = []

                    for(let _caller of properties[key]){
                        if(caller.name === _caller.name && caller.target === _caller.target){
                            found = true
                            break
                        }
                    }

                    if(!found){
                        properties[key].push(caller)
                    }
                    
                }

                if(key === 'index') {
                    if(!(key in target)) return parent.data.indexOf(proxy)
                }
                if(key === '__self') {
                    return target
                }
                return target[key]
            },
            set(target, key, value){
                if(target[key] !== value){
                    recursiveRemoveLink(target[key])
                    recursiveAddLink(value)

                    target[key] = value
                    if(!properties[key]) properties[key] = []

                    for(let property of properties[key]){
                        // property.__update()
                        property.target.__proxy[property.name] = property.func()
                    }

                    parent.__propogate()
                }

                return true
            }
        })

        return proxy
    }
}

module.exports = AbstractItemModel