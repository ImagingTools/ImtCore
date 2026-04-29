const { QtObject } = require('./QtObject')
const { QReal, QBool } = require('../utils/properties')

/**
 * Instancing - mirrors Qt Quick 3D's Instancing base type.
 *
 * Abstract base for instancing strategies. Subclasses provide the
 * actual per-instance data.
 */
class Instancing extends QtObject {
    static defaultProperties = {
        instanceCount:    { type: QReal, value: 0,     changed: '$instancingChanged' },
        hasTransparency:  { type: QBool, value: false, changed: '$instancingChanged' },
    }

    $instancingChanged(){
        if(this.$onInstancingChanged) this.$onInstancingChanged()
    }
}

module.exports.Instancing = Instancing
