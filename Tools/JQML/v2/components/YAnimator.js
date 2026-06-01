const { NumberAnimation } = require('./NumberAnimation')

class YAnimator extends NumberAnimation {
    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.getProperty('property').reset('y')
    }
}

module.exports.YAnimator = YAnimator
