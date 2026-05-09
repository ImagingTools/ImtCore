const { QtObject } = require('./QtObject')
const { QReal, QVar } = require('../utils/properties')

/**
 * MorphTarget - mirrors Qt Quick 3D's MorphTarget.
 *
 * Describes a single morph-target (blend shape) with a weight that
 * controls how much of the target displacement is applied.
 */
class MorphTarget extends QtObject {
    static defaultProperties = {
        weight:     { type: QReal, value: 0,         changed: '$morphChanged' },
        attributes: { type: QVar,  value: undefined, changed: '$morphChanged' },
    }

    $morphChanged(){
        if(this.$onMorphChanged) this.$onMorphChanged()
    }
}

module.exports.MorphTarget = MorphTarget
