const { NumberAnimation } = require('./NumberAnimation')

class OpacityAnimator extends NumberAnimation {
    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.getProperty('property').reset('opacity')
    }
}

module.exports.OpacityAnimator = OpacityAnimator
