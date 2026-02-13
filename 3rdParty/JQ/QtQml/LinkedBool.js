const Property = require("./Property")
const Signal = require("./Signal")

class LinkedBool extends Property {

    /**
     * 
     * @param {Object} target 
     * @param {String} name
     * @param {Object} meta
     * @returns {Object}
     */
    static simpleGet(target, name, meta){
        return target.__self[name].value && target.__self[name].parentValue
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static simpleSet(target, name, value, meta){
        let oldValue = target.__self[name].value && target.__self[name].parentValue

        if(typeof value === 'function'){
            try {
                target.__self[name].value = value.call(target)
            } catch(error) {
                if(location.hash === '#jqdebug')console.error(error)
            }
        } else {
            target.__self[name].value = value
        }  

        let currentValue = target.__self[name].value && target.__self[name].parentValue

        if(oldValue !== currentValue){
            Signal.get(target, name + 'Changed')(oldValue, currentValue)
        }
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static simpleReset(target, name, value, meta){
        if(target.__depends[name]){
            for(let connectionObj of target.__depends[name]){
                Signal.removeConnection(connectionObj)
            }
            delete target.__depends[name]
        }

        if(target.__properties) delete target.__properties[name]
        
        let oldValue = target.__self[name].value && target.__self[name].parentValue

        if(typeof value === 'function'){
            try {
                target.__self[name].value = value.call(target)
            } catch(error) {
                if(location.hash === '#jqdebug')console.error(error)
            }
        } else {
            target.__self[name].value = value
        }  

        let currentValue = target.__self[name].value && target.__self[name].parentValue

        if(oldValue !== currentValue){
            Signal.get(target, name + 'Changed')(oldValue, currentValue)
        }
    }

    /**
     * 
     * @param {Object} target 
     * @param {String} name
     * @param {Object} meta
     */
    static init(target, name, value, parentValue){
        if(!(name in target)) {
            target.__self[name] = {
                value: value,
                parentValue: parentValue,
            }
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
        if(target.__properties && target.__properties[name]) {
            target.__updateProperty(name)
        }

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
        
        return target.__self[name].value && target.__self[name].parentValue
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static set(target, name, value, meta){
        let oldValue = target.__self[name].value && target.__self[name].parentValue

        if(typeof value === 'function'){
            try {
                global.queueFlag.push(true)
                this.queueLink.push({
                    target: target,
                    name: name,
                    meta: meta,
                    func: value,
                })
                target.__self[name].value = value.call(target)
            } catch(error) {
                if(location.hash === '#jqdebug')console.error(error)
            } finally {
                global.queueFlag.pop()
                this.queueLink.pop()
            }
        } else {
            target.__self[name].value = value
        }  

        let currentValue = target.__self[name].value && target.__self[name].parentValue

        if(oldValue !== currentValue){
            Signal.get(target, name + 'Changed')(oldValue, currentValue)
        }

        return true
    }

    static parentSet(target, name, value){
        let oldValue = target.__self[name].value && target.__self[name].parentValue

        target.__self[name].parentValue = value

        let currentValue = target.__self[name].value && target.__self[name].parentValue

        if(oldValue !== currentValue){
            Signal.get(target, name + 'Changed')(oldValue, currentValue)
        }

        return true
    }

    static getDefaultValue(){
        return true
    }

}

module.exports = LinkedBool