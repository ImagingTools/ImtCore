const NumberAnimation = require("./NumberAnimation")

class YAnimator extends NumberAnimation {
    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.property = 'y'
        return obj
    }
}

module.exports = YAnimator
