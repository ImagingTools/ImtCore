const NumberAnimation = require("./NumberAnimation")

class RotationAnimator extends NumberAnimation {
    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.property = 'rotation'
        return obj
    }
}

module.exports = RotationAnimator
