const BaseObject = require("../QtBase/BaseObject")

class Method extends BaseObject {
    /**
     * 
     * @param {Object} target 
     * @param {String} name
     * @returns {Object}
     */
    static get(target, name){
        if(!target[`CACHED_FUNC_${name}`]){
            let f = target[name].bind(target.__proxy)
            f.meta = {
                parent: target,
                name: name
            }
            target[`CACHED_FUNC_${name}`] = f
        }

        return target[`CACHED_FUNC_${name}`]
    }

    /**
     */
    static set(){
        throw 'Invalid left-hand side in assignment'
    }
}

module.exports = Method