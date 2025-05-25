const BaseObject = require("../QtBase/BaseObject")

class AnchorLine extends BaseObject{
    static Left = 0
    static Right = 1
    static Top = 2
    static Bottom = 3
    static HorizontalCenter = 4
    static VerticalCenter = 5

    /**
     * 
     * @param {Object} target 
     * @param {String} name
     * @param {Object} meta
     * @returns {Object}
     */
    static get(target, name, meta){
        if(!target[name]) target[name] = this.create(target, meta)
        return target[name]
    }

    /**
     */
    static set(){
        throw 'Invalid left-hand side in assignment'
    }

    static create(parent, meta){
        let obj = new this()

        obj.__parent = parent
        obj.__float = meta.value

        return obj
    }

    destroy(){
        delete this.__parent
        delete this.__float
    }
}

module.exports = AnchorLine