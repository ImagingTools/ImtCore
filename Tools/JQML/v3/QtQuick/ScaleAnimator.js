const NumberAnimation = require("./NumberAnimation")

class ScaleAnimator extends NumberAnimation {
    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.property = 'scale'
        return obj
    }
}

module.exports = ScaleAnimator
