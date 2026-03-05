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

    __updateProperty(propName){
        let path = propName.split('.')
        let value = this.__properties[propName]

        if(value instanceof JQModules.QtQml.QObject){
            value.__updatePrimaryProperties()
            value.__updateProperties()
            value.__complete()
        }

        if(path.length === 2){
            this.__proxy[path[0]][path[1]] = value
        } else {
            this.__proxy[path[0]] = value
        }
    }
  
    __updateProperties(){
        for(let propName in this.__properties){
            this.__updateProperty(propName)
        }
    }
}





module.exports = GroupProperty