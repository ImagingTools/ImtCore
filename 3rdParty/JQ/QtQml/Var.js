const Property = require("./Property")
const Signal = require("./Signal")

class Var extends Property {
    __typecasting(value){
        if(typeof value === 'object') {
            if(value instanceof Property) return value.__get()
        }
        return value
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static set(target, name, value, meta){
        let oldValue = name in target ? target[name] : ('value' in meta ? meta.value : meta.type.getDefaultValue())

        if(typeof value === 'function'){
            try {
                this.queueLink.push({
                    target: target,
                    name: name,
                    meta: meta,
                    func: value,
                })
                target[name] = value.call(target)
            } catch(error) {
                console.error(error)
            } finally {
                this.queueLink.pop()
            }
        } else {
            target[name] = value
        }  

        let currentValue = name in target ? target[name] : ('value' in meta ? meta.value : meta.type.getDefaultValue())

        if(oldValue !== currentValue){
            let destructionFunc = ()=>{
                target[name] = null
            }
            destructionFunc.meta = {
                name: name+'__destruction',
                destruction: true
            }
            if(oldValue instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = oldValue
                oldValue['Component.destruction'].disconnect(destructionFunc)
            }
            if(currentValue instanceof JQModules.QtQml.QtObject){
                destructionFunc.meta.parent = currentValue
                currentValue['Component.destruction'].connect(destructionFunc)
            }

            Signal.get(target, name + 'Changed')(oldValue, currentValue)
        }

        return true
    }
}

module.exports = Var