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
//                 let caller = Property.queueLink[Property.queueLink.length-1]
//                 if(caller) caller.__subscribe(this[key])
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
//                     let caller = Property.queueLink[Property.queueLink.length-1]
//                     if(caller){
//                         if(key in __queue){
//                             __queue[key].push(caller)
//                         } else {
//                             __queue[key] = [caller]
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
                return target[key]
            } else if(target.__parentContext){
                return target.__parentContext[key]
            } else {
                return undefined
            }
        },

        set(target, key, value){
            target[key] = value

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