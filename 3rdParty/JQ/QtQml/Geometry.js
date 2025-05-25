const Real = require("./Real")

class Geometry extends Real {
    static set(target, name, value, meta){
        target[name+'__prevent'] = true
        return super.set(target, name, value, meta)
    }

    static setAuto(target, name, value, meta){
        if(!target[name+'__prevent']) return super.set(target, name, value, meta)
    }
}

module.exports = Geometry