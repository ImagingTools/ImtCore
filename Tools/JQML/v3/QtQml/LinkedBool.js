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
                if(location.hash === '#jqdebugdetail')console.error(error)
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
                if(location.hash === '#jqdebugdetail')console.error(error)
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
                if(connectionObj.name === name + 'Changed' && connectionObj.target.__self === target.__self){
                    found = true
                    break
                }
            }

            if(!found){
                const sourceIsSameProperty = link.target.__self === target.__self && link.name === name
                if (sourceIsSameProperty) {
                    return target.__self[name].value && target.__self[name].parentValue
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
        
        return target.__self[name].value && target.__self[name].parentValue
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static set(target, name, value, meta){
        let oldRawValue = target.__self[name].value
        let middleRawValue = oldRawValue

        if(typeof value === 'function'){
            try {
                global.queueFlag.push(true)
                this.queueLink.push({
                    target: target,
                    name: name,
                    meta: meta,
                    func: value,
                })
                middleRawValue = value.call(target)
            } catch(error) {
                if(location.hash === '#jqdebugdetail')console.error(error)
            } finally {
                global.queueFlag.pop()
                this.queueLink.pop()
            }
        } else {
            middleRawValue = value
        }

        // Состояние сразу перед возможным коммитом (после реэнтрантных эффектов)
        let beforeCommitValue = target.__self[name].value && target.__self[name].parentValue

        // Не перетираем, если raw уже был изменен реэнтрантно в процессе вычисления
        let rawWasNotReentered = target.__self[name].value === oldRawValue
        if(rawWasNotReentered){
            target.__self[name].value = middleRawValue
        }

        let afterCommitValue = target.__self[name].value && target.__self[name].parentValue

        if(beforeCommitValue !== afterCommitValue){
            Signal.get(target, name + 'Changed')(beforeCommitValue, afterCommitValue)
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