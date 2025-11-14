const BaseObject = require("../QtBase/BaseObject")
const Signal = require("./Signal")

class SDLProperty extends BaseObject {
    /**
     * 
     * @param {Object} target 
     * @param {String} name
     * @param {Object} meta
     * @returns {Object}
     */
    static get(target, name, meta){
        return target['PROXY__get__'+name]()
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static set(target, name, value, meta){
        let oldValue = target[name]
        target['PROXY__set__'+name](value)

        if(oldValue !== value){
            Signal.get(target, name + 'Changed')(oldValue, value)
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

module.exports = SDLProperty