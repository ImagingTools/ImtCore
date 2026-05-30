const NumberAnimation = require("./NumberAnimation")

class OpacityAnimator extends NumberAnimation {
    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.property = 'opacity'
        return obj
    }
}

module.exports = OpacityAnimator
