const QBaseObject = require("../QtBase/QBaseObject")
const Signal = require("../QtQml/Signal")
const Property = require("./Property")



class GroupProperty extends QBaseObject {
    /**
     * 
     * @param {Object} target 
     * @param {String} name
     * @returns {Object}
     */
    static get(target, name){
        if(!target[name]) target[name] = this.create(target)
        return target[name]
    }

    /**
     */
    /**
     * @param {Object} target 
     * @param {String} name
     * @param {*} value
     * @param {Object} meta
     */
    static set(target, name, value, meta){
        let _value = typeof value === 'function' ? value() : value

        for(let key in _value){
            target.__proxy[name][key] = _value[key]
        }

        // if(target['SLOT_' + name + 'Changed']) target['SLOT_' + name + 'Changed'](target[name], target[name])
        
        return true
    }

    static create(parent = null){
        let obj = new this()
        let proxy = new Proxy(obj, this.handle)
        obj.__parent = parent ? parent.__proxy : null
        obj.__proxy = proxy
        return proxy
    }
}





module.exports = GroupProperty