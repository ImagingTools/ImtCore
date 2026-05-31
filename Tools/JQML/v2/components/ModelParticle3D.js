const { QtObject } = require('./QtObject')
const { QReal, QVar, QBool } = require('../utils/properties')

/**
 * ModelParticle3D - mirrors Qt Quick 3D Particles' ModelParticle3D.
 *
 * Defines a 3D-model-based particle type. Each particle is rendered
 * as a small mesh instance.
 */
class ModelParticle3D extends QtObject {
    static defaultProperties = {
        maxAmount:  { type: QReal, value: 100,       changed: '$particleChanged' },
        delegate:   { type: QVar,  value: undefined, changed: '$particleChanged' },
        alignMode:  { type: QReal, value: 0,         changed: '$particleChanged' },
        hasTransparency: { type: QBool, value: false, changed: '$particleChanged' },
    }

    $particleChanged(){
        if(this.$onParticleChanged) this.$onParticleChanged()
    }
}

module.exports.ModelParticle3D = ModelParticle3D
