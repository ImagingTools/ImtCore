const { Node3D } = require('./Node3D')
const { QBool, QReal, QVar, QList } = require('../utils/properties')

/**
 * ParticleSystem3D - mirrors Qt Quick 3D Particles' ParticleSystem3D.
 *
 * Hosts particle emitters, affectors, and particle definitions.
 * On the web this creates a simple THREE.Points-based particle system.
 */
class ParticleSystem3D extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        running:    { type: QBool, value: true,      changed: '$systemChanged' },
        paused:     { type: QBool, value: false,     changed: '$systemChanged' },
        startTime:  { type: QReal, value: 0 },
        time:       { type: QReal, value: 0 },
        particles:  { type: QList, value: [] },
        emitters:   { type: QList, value: [] },
        affectors:  { type: QList, value: [] },
    }

    $createObject3D(THREE){
        return new THREE.Group()
    }

    $systemChanged(){
        if(this.$host3D) this.$host3D.$requestRender()
    }
}

module.exports.ParticleSystem3D = ParticleSystem3D
