const { QtObject } = require('./QtObject')
const { QVar, QList } = require('../utils/properties')

/**
 * Effect - mirrors Qt Quick 3D's Effect.
 *
 * Base for post-processing effects applied to View3D output.
 * Each effect holds a list of render passes that define the
 * processing pipeline.
 */
class Effect extends QtObject {
    static defaultProperties = {
        passes: { type: QList, value: [], changed: '$effectChanged' },
    }

    $effectChanged(){
        if(this.$onEffectChanged) this.$onEffectChanged()
    }
}

module.exports.Effect = Effect
