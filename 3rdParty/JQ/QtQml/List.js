const Property = require("./Property")
const Signal = require("./Signal")
const QObject = require("./QObject")


// let arrayProto = ([]).__proto__

//         let arrayProtoSplice = arrayProto.splice
//         arrayProto.splice = function(){
//             if(this.name && this.target){
                
//             }
//             return arrayProtoSplice.call(this, ...arguments)
//         }

const ArrayEx = {
    __push(...args){
        super.push(...args)
    },

    __splice(...args){
        return super.splice(...args)
    },

    push(...args){
        super.push(...args)

        if(this.name && this.target){
            Signal.get(this.target, this.name+'Changed')(this.length-1, this.length, 'append')
        }
    },

    splice(...args){
        throw ''
    },

    __proto__: ([]).__proto__
}

class List extends Property {
    static getDefaultValue(){
        return []
    }

    /**
     * 
     * @param {Object} target 
     * @param {String} name
     * @param {Object} meta
     * @returns {Object}
     */
    static get(target, name, meta){
        if(!(name in target)){
            target[name] = this.getDefaultValue()
            target[name].target = target
            target[name].name = name
            target[name].__proto__ = ArrayEx
        }
        
        return super.get(target, name, meta)
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static set(target, name, value, meta){
        let newValue = this.__typecasting(value).slice()

        newValue.target = target
        newValue.name = name
        newValue.__proto__ = ArrayEx

        return super.set(target, name, newValue, meta)
    }

    static __typecasting(value){
        if(typeof value === 'object' && Array.isArray(value)){
            return value
        } else {
            throw `${value} не может быть преобразован в Array`
        }
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
            JQApplication.MemoryController.removeLink(obj, this)
        }
    }
}

module.exports = List