const NumberAnimation = require("./NumberAnimation")

class XAnimator extends NumberAnimation {
    static create(parent, ...args){
        let obj = super.create(parent, ...args)
        obj.property = 'x'
        return obj
    }
}

module.exports = XAnimator
