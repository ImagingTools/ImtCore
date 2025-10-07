const Property = require("../QtQml/Property")
// const BaseObject = require("../QtBase/BaseObject")


// class JQContext extends BaseObject {
//     static create(parent, ...args){
//         let proxy = super.create(parent, ...args)
//         let self = proxy.__self

//         if(parent){
//             self.__parent = parent
//             self.__queue = parent.__self.__queue
//         } else {
//             self.__queue = {}
//         }

//         return proxy
//     }

//     __toPrimitive(hint){
//         return this
//     }
    
//     __has(key){
//         if(key in this){
//             return true
//         }

//         return false
//     }

//     __get(key){
//         if(this.__has(key)){
//             if(this[key] instanceof Property){
//                 let link = Property.queueLink[Property.queueLink.length-1]
//                 if(link) link.__subscribe(this[key])
//                 return this[key].__get()
//             }
//             return this[key]
//         } else if(this.__parent){
//             return this.__parent[key]
//         }
//         return undefined
//     }

//     __set(key, value){
//         if(this.__has(key)){
//             this[key].__self.__reset(value)
//             return true
//         }
//         return false
//     }
// }

// class JQContext {
//     static create(parent){
//         let context = {}
//         let __queue = parent ? parent.__queue : {}

//         return new Proxy(context, { 
//             get(target, key){
//                 if(key === '__queue') return __queue

//                 let obj = key in target ? target[key] : (parent ? parent[key] : undefined)

//                 if(!obj){
//                     let link = Property.queueLink[Property.queueLink.length-1]
//                     if(link){
//                         if(key in __queue){
//                             __queue[key].push(link)
//                         } else {
//                             __queue[key] = [link]
//                         }
//                     }
//                 }

//                 return obj
//             },
//             set(target, key, value){
//                 target[key] = value

//                 if(key in __queue){
//                     for(let property of __queue[key]){
//                         property.__update()
//                     }
//                     delete __queue[key]
//                 }

//                 return true
//             }
//         })
//     }
// }



class JQContext {
    static handle = {
        get(target, key){
            if(key in target){
                let flag = global.queueFlag[global.queueFlag.length - 1]
                let link = Property.queueLink[Property.queueLink.length-1]
                if(link && flag && target[key]) {
                    let found = false

                    for(let _link of target[key+'__depends']){
                        if(link.name === _link.name && link.target === _link.target){
                            found = true
                            break
                        }
                    }

                    if(!found){
                        target[key+'__depends'].push(link)
                    }
                    
                }

                return target[key]
            } else if(target.__parentContext){
                return target.__parentContext[key]
            } else {
                return undefined
            }
        },

        set(target, key, value){
            target[key] = value
            target[key+'__depends'] = []

            target[key]['JQDestruction'].connect(()=>{
                // target[key] = null

                for(let property of target[key+'__depends']){
                    if(!property.target.__destroyed){
                        try {
                            property.target.__proxy[property.name] = property.func()
                        } catch (error) {
                            //console.error(error)
                            property.meta.type.error(property.target, property.name, property.meta)
                        }
                    }
                    
                    
                }

                delete target[key+'__depends']
            })

            return true
        },
    }

    static create(parent){
        let context = {}
        context.__parentContext = parent

        return new Proxy(context, this.handle)
    }
}

module.exports = JQContext