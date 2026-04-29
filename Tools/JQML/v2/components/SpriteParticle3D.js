const { QtObject } = require('./QtObject')
const { QReal, QVar, QColor, QBool } = require('../utils/properties')

/**
 * SpriteParticle3D - mirrors Qt Quick 3D Particles' SpriteParticle3D.
 *
 * Defines a billboard-sprite particle type with size, color, and
 * optional sprite-sheet animation.
 */
class SpriteParticle3D extends QtObject {
    static defaultProperties = {
        maxAmount:       { type: QReal,  value: 100,        changed: '$particleChanged' },
        color:           { type: QColor, value: '#ffffffff', changed: '$particleChanged' },
        colorVariation:  { type: QVar,   value: { x: 0, y: 0, z: 0, w: 0 }, changed: '$particleChanged' },
        billboard:       { type: QBool,  value: false,      changed: '$particleChanged' },
        particleScale:   { type: QReal,  value: 1,          changed: '$particleChanged' },
        sprite:          { type: QVar,   value: undefined,  changed: '$particleChanged' },
    }

    $particleChanged(){
        if(this.$onParticleChanged) this.$onParticleChanged()
    }
}

module.exports.SpriteParticle3D = SpriteParticle3D
