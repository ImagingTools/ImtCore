const Property = require("./Property")
const QObject = require("./QObject")

class List extends Property {
    static create(parent, meta){
        let obj = super.create(parent, meta)

        obj.__value = obj.__typecasting([])

        return obj
    }

    __typecasting(value){
        if(typeof value === 'object' && Array.isArray(value) && !value.isExtended){
            value.isExtended = true
            let push = value.push
            value.opush = (...args)=>{
                push.call(value, ...args)

                for(let obj of this.__getObjects(args)){
                    JQApplication.MemoryController.addLink(obj, this.__proxy)
                }
            }
            value.push = (...args)=>{
                push.call(value, ...args)

                for(let obj of this.__getObjects(args)){
                    JQApplication.MemoryController.addLink(obj, this.__proxy)
                }

                this.__emitSignal(value.length-1, value.length, 'append')
            }
            let splice = value.splice
            value.osplice = (...args)=>{
                let removed = splice.call(value, ...args)

                for(let obj of this.__getObjects(removed)){
                    JQApplication.MemoryController.removeLink(obj, this.__proxy)
                }

                return removed
            }
            value.splice = ()=>{
                throw `List doesn't define a Replace function`
            }
        } else {
            throw `${value} не может быть преобразован в Array`
        }
        return value
    }

    __getObjects(tree, result = []){
        if(!tree) tree = this.__value

        for(let key in tree){
            if(tree[key] && typeof tree[key] === 'object'){
                if(tree[key] instanceof QObject){
                    result.push(tree[key])
                } else {
                    this.__getObjects(tree[key], result)
                }
            }
        }

        return result
    }

    __removeObject(obj, tree){
        if(!tree) tree = this.__value

        for(let key in tree){
            if(tree[key] && typeof tree[key] === 'object'){
                if(tree[key] instanceof QObject){
                    if(tree[key] === obj){
                        tree[key] = null
                    }
                } else {
                    this.__removeObject(obj, tree[key])
                }
            }
        }
    }

    __destroy(){
        super.__destroy()

        for(let obj of this.__getObjects()){
            JQApplication.MemoryController.removeLink(obj, this.__proxy)
        }
    }
}

module.exports = List