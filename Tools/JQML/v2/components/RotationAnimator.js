const { NumberAnimation } = require('./NumberAnimation')

class RotationAnimator extends NumberAnimation {
    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.getProperty('property').reset('rotation')
    }
}

module.exports.RotationAnimator = RotationAnimator
