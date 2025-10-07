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
        return this.getter(target, name)
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static simpleSet(target, name, value, meta){
        let oldValue = this.getter(target, name)
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
     * @param {Object} func 
     * @param {String} propName
     */
    static init(target, name, func, propName){
        target.__self[name] = {

            // getter: function(){   
            //     let obj = func()
            //     if(!target.__self[name].connected){
            //         target.__self[name].connected = true
            //         if(obj instanceof QBaseObject){
            //             Signal.get(obj, propName+'Changed').connect((oldValue, newValue)=>{
            //                 Signal.get(target, name+'Changed')(oldValue, newValue)
            //             })
            //         }
            //     }       
            //     return obj[propName]
            // },
            // setter: function(val){
            //     let obj = func()
            //     if(!target.__self[name].connected){
            //         target.__self[name].connected = true
            //         if(obj instanceof QBaseObject){
            //             Signal.get(obj, propName+'Changed').connect((oldValue, newValue)=>{
            //                 Signal.get(target, name+'Changed')(oldValue, newValue)
            //             })
            //         }
            //     } 
            //     obj[propName] = val
            // },
            func: func,
            propName: propName,
            connected: false,
        }
    }

    static prepare(target, name, obj, propName){
        if(!target.__self[name].connected){
            target.__self[name].connected = true
            if(obj instanceof QBaseObject){
                Signal.get(obj, propName+'Changed').connect((oldValue, newValue)=>{
                    Signal.get(target, name+'Changed')(oldValue, newValue)
                })
            }
        } 
    }
    static getter(target, name){
        let obj = target.__self[name].func()
        this.prepare(target, name, obj, target.__self[name].propName)
        return obj[target.__self[name].propName]
    }
    static setter(target, name, value){
        let obj = target.__self[name].func()
        this.prepare(target, name, obj, target.__self[name].propName)
        obj[target.__self[name].propName] = value
    }
    static resetter(target, name){
        let obj = target.__self[name].func()
        this.prepare(target, name, obj, target.__self[name].propName)
        obj[target.__self[name].propName] = obj[target.__self[name].propName]
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
        
        return this.getter(target, name)
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static reset(target, name, value, meta){
        this.resetter(target, name)
        return super.reset(target, name, value, meta)
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static set(target, name, value, meta){
        let oldValue = this.getter(target, name)
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
            this.setter(target, name, value)
            // Signal.get(target, name + 'Changed')(oldValue, currentValue)
        }

        return true
    }
}

module.exports = Alias