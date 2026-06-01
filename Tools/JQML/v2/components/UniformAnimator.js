const { NumberAnimation } = require('./NumberAnimation')
const { QString } = require('../utils/properties')

class UniformAnimator extends NumberAnimation {

    static defaultProperties = {
        uniform: { type: QString, value: '', changed: '$uniformChanged' },
    }

    $uniformChanged(){
        this.getProperty('property').reset(this.getPropertyValue('uniform'))
    }
}

module.exports.UniformAnimator = UniformAnimator
