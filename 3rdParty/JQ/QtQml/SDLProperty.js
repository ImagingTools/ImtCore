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
        return target.__proxy.jsData[name.replaceAll('m_','')]
    }

    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static set(target, name, value, meta){
        target.__proxy.jsData[name.replaceAll('m_','')] = value

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