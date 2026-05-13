const { Node3D } = require('./Node3D')
const { QReal, QVar, QVector3D } = require('../utils/properties')

/**
 * ParticleEmitter3D - mirrors Qt Quick 3D Particles' ParticleEmitter3D.
 *
 * Emits particles from the emitter position with configurable rate,
 * velocity, and life span.
 */
class ParticleEmitter3D extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        particle:         { type: QVar,      value: undefined, changed: '$emitterChanged' },
        emitRate:         { type: QReal,     value: 0,         changed: '$emitterChanged' },
        lifeSpan:         { type: QReal,     value: 1000,      changed: '$emitterChanged' },
        lifeSpanVariation:{ type: QReal,     value: 0,         changed: '$emitterChanged' },
        particleScale:    { type: QReal,     value: 1,         changed: '$emitterChanged' },
        velocity:         { type: QVar,      value: undefined, changed: '$emitterChanged' },
        particleRotation: { type: QVector3D, value: { x: 0, y: 0, z: 0 }, changed: '$emitterChanged' },
    }

    $createObject3D(THREE){
        return new THREE.Group()
    }

    $emitterChanged(){
        if(this.$host3D) this.$host3D.$requestRender()
    }
}

module.exports.ParticleEmitter3D = ParticleEmitter3D
