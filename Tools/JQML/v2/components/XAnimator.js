const { NumberAnimation } = require('./NumberAnimation')

class XAnimator extends NumberAnimation {
    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.getProperty('property').reset('x')
    }
}

module.exports.XAnimator = XAnimator
