const { Node3D } = require('./Node3D')
const { QReal, QVar, QVector3D, QList } = require('../utils/properties')

/**
 * Attractor3D - mirrors Qt Quick 3D Particles' Attractor3D.
 *
 * Attracts particles towards the attractor position. Strength and
 * shape determine how the force field behaves.
 */
class Attractor3D extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        particles:   { type: QList,     value: [],                  changed: '$affectorChanged' },
        positionVariation: { type: QVector3D, value: { x: 0, y: 0, z: 0 }, changed: '$affectorChanged' },
        duration:    { type: QReal,     value: -1,                  changed: '$affectorChanged' },
        durationVariation: { type: QReal, value: 0,                 changed: '$affectorChanged' },
        hideAtEnd:   { type: QVar,      value: false,               changed: '$affectorChanged' },
    }

    $createObject3D(THREE){
        return new THREE.Group()
    }

    $affectorChanged(){
        if(this.$host3D) this.$host3D.$requestRender()
    }
}

module.exports.Attractor3D = Attractor3D
