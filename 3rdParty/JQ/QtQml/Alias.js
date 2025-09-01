const QBaseObject = require("../QtBase/QBaseObject")
const Property = require("./Property")
const Signal = require("./Signal")

class Alias extends Property {
    /**
     * 
     * @param {Object} target 
     * @param {String} name
     * @param {Object} meta
     * @returns {Object}
     */
    static simpleGet(target, name, meta){      
        return target[name].getter.call(target)
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static simpleSet(target, name, value, meta){
        let oldValue = target[name].getter.call(target)
        let currentValue = oldValue

        if(typeof value === 'function'){
            try {
                currentValue = value.call(target)
            } finally {
                this.queueLink.pop()
            }
        } else {
            currentValue = value
        }  

        if(oldValue !== currentValue){
            target[name].setter.call(target, value)
        }

        return true
    }

    /**
     * 
     * @param {Object} target 
     * @param {String} name
     * @param {Object} obj 
     * @param {String} propName
     */
    static init(target, name, obj, propName){
        target.__self[name] = {
            getter: ()=>{
                return obj[propName]
            },
            setter: (val)=>{
                obj[propName] = val
            },
        }

        if(obj instanceof QBaseObject){
            Signal.get(obj, propName+'Changed').connect((oldValue, newValue)=>{
                Signal.get(target, name+'Changed')(oldValue, newValue)
            })
        }
    }

    /**
     * 
     * @param {Object} target 
     * @param {String} name
     * @param {Object} meta
     * @returns {Object}
     */
    static get(target, name, meta){
        let flag = global.queueFlag[global.queueFlag.length - 1]
        let link = this.queueLink[this.queueLink.length - 1]
        if(link && flag){
            if(!link.target.__depends[link.name]) link.target.__depends[link.name] = []

            let found = false
            for(let connectionObj of link.target.__depends[link.name]){
                if(connectionObj.name === name + 'Changed' && connectionObj.target === target){
                    found = true
                    break
                }
            }

            if(!found){
                let connectionObj = Signal.get(target, name + 'Changed').connect(()=>{
                    if(!link.target[link.name+'__updating']){
                        link.target[link.name+'__updating'] = true
                        link.meta.type.set(link.target, link.name, link.func, link.meta)
                        delete link.target[link.name+'__updating']
                    }
                })
    
                link.target.__depends[link.name].push(connectionObj)
            }
            
        }
        
        return target[name].getter.call(target)
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static set(target, name, value, meta){
        let oldValue = target[name].getter.call(target)
        let currentValue = oldValue

        if(typeof value === 'function'){
            try {
                global.queueFlag.push(true)
                this.queueLink.push({
                    target: target,
                    name: name,
                    meta: meta,
                    func: value,
                })
                currentValue = value.call(target)
            } finally {
                global.queueFlag.pop()
                this.queueLink.pop()
            }
        } else {
            currentValue = value
        }  

        if(oldValue !== currentValue){
            target[name].setter.call(target, value)
            // Signal.get(target, name + 'Changed')(oldValue, currentValue)
        }

        return true
    }
}

module.exports = Alias