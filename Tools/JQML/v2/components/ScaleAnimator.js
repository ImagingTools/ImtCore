const { NumberAnimation } = require('./NumberAnimation')

class ScaleAnimator extends NumberAnimation {
    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.getProperty('property').reset('scale')
    }
}

module.exports.ScaleAnimator = ScaleAnimator
