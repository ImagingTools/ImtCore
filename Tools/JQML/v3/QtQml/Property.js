const BaseObject = require("../QtBase/BaseObject")
const Signal = require("./Signal")

class Property extends BaseObject {
    static queueLink = []

    /**
     * 
     * @param {Object} target 
     * @param {String} name
     * @param {Object} meta
     * @returns {Object}
     */
    static simpleGet(target, name, meta){
        return name in target ? target[name] : ('value' in meta ? meta.value : meta.type.getDefaultValue())
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static simpleSet(target, name, value, meta){
        let oldValue = name in target ? target[name] : ('value' in meta ? meta.value : meta.type.getDefaultValue())

        if(typeof value === 'function'){
            try {
                target[name] = this.typeCasting(value.call(target))
            } catch(error) {
                if(location.hash === '#jqdebugdetail')console.error(error)
            }
        } else {
            try {
                target[name] = this.typeCasting(value)
            } catch (error) {
                if(location.hash === '#jqdebugdetail')console.error(error)
            }
        }  

        let currentValue = name in target ? target[name] : ('value' in meta ? meta.value : meta.type.getDefaultValue())

        if(oldValue !== currentValue){
            Signal.get(target, name + 'Changed')(oldValue, currentValue)
        }

        return true
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

        return this.simpleSet(target, name, value, meta)
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
            // let func = target.__properties[name]
            // this.set(target, name, func, meta)
        }
        
        let flag = global.queueFlag[global.queueFlag.length - 1]
        let link = this.queueLink[this.queueLink.length - 1]
        if(link && flag){
            if(!link.target.__depends[link.name]) link.target.__depends[link.name] = []

            let found = false
            for(let connectionObj of link.target.__depends[link.name]){
                if(connectionObj.name === name + 'Changed' && connectionObj.target.__self === target.__self){
                    found = true
                    break
                }
            }

            if(!found){
                const sourceIsSameProperty = link.target.__self === target.__self && link.name === name
                if (sourceIsSameProperty) {
                    return name in target ? target[name] : ("value" in meta ? meta.value : meta.type.getDefaultValue())
                }

                const signalFunc = target.constructor.meta[name + "Changed"].type.get(target, name + "Changed")
                const isCrossItem = link.target !== target
                const connectFn = isCrossItem && signalFunc.connectBefore ? signalFunc.connectBefore : signalFunc.connect

                let connectionObj = connectFn(() => {
                    const updatingKey = link.name + "__updating"
                    const pendingKey = link.name + "__pendingRecalc"
                    const maxFlushPasses = 16

                    if (link.target[updatingKey]) {
                        link.target[pendingKey] = true
                        return
                    }

                    link.target[updatingKey] = true
                    let flushPass = 0
                    try {
                        while (true) {
                            link.target[pendingKey] = false
                            link.meta.type.set(link.target, link.name, link.func, link.meta)

                            if (!link.target[pendingKey]) {
                                break
                            }

                            flushPass++
                            if (flushPass >= maxFlushPasses) {
                                if (location.hash === "#jqdebugdetail") {
                                    console.warn("Binding flush limit reached for " + link.name)
                                }
                                break
                            }
                        }
                    } finally {
                        delete link.target[updatingKey]
                        delete link.target[pendingKey]
                    }
                })

                link.target.__depends[link.name].push(connectionObj)
            }
            
        }

        

        return name in target ? target[name] : ('value' in meta ? meta.value : meta.type.getDefaultValue())
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static set(target, name, value, meta){
        if(target.constructor.meta[name].modifiers && target.constructor.meta[name].modifiers.readonly){
            target[`__${name}__init`] = true
        }
        // if(target.constructor.meta[name].modifiers && target.constructor.meta[name].modifiers.readonly && (!target.__properties || !target.__properties[name]) && typeof value !== 'function') {
        //     throw new Error(`Cannot assign to read-only property "${name}"`)
        // }

        let oldValue = name in target ? target[name] : ('value' in meta ? meta.value : meta.type.getDefaultValue())
        let middleValue = oldValue

        if(typeof value === 'function'){
            try {
                global.queueFlag.push(true)
                this.queueLink.push({
                    target: target,
                    name: name,
                    meta: meta,
                    func: value,
                })
                middleValue = this.typeCasting(value.call(target))
            } catch(error) {
                if(location.hash === '#jqdebugdetail')console.error(error)
            } finally {
                global.queueFlag.pop()
                this.queueLink.pop()
            }
        } else {
            try {
                middleValue = this.typeCasting(value)
            } catch (error) {
                if(location.hash === '#jqdebugdetail')console.error(error)
            }
        }  

        let currentValue = name in target ? target[name] : ('value' in meta ? meta.value : meta.type.getDefaultValue())

        if(oldValue !== middleValue && oldValue === currentValue){
            target[name] = middleValue
            target.constructor.meta[name + 'Changed'].type.get(target, name + 'Changed')(oldValue, middleValue)
        }

        return true
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static reset(target, name, value, meta){
        if(target.constructor.meta[name].modifiers && target.constructor.meta[name].modifiers.readonly && target[`__${name}__init`]) {
            throw new Error(`Cannot assign to read-only property "${name}"`)
        }
            
        if(!(global.signalTargets[global.signalTargets.length-1] === target)){
            if(target.__depends[name]){
                for(let connectionObj of target.__depends[name]){
                    target.constructor.meta[name + 'Changed'].type.removeConnection(connectionObj)
                }
                delete target.__depends[name]
            }

            if(target.__properties) delete target.__properties[name]
        }
        return this.set(target, name, value, meta)
    }

    static typeCasting(value){
        return value
    }

    static getDefaultValue(){
        return null
    }

    static error(target, key, meta){
        
    }
}

module.exports = Property