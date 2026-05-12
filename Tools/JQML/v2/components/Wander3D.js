const { Node3D } = require('./Node3D')
const { QReal, QVector3D, QList } = require('../utils/properties')

/**
 * Wander3D - mirrors Qt Quick 3D Particles' Wander3D.
 *
 * Applies a random wandering force to particles, creating organic
 * drifting motion.
 */
class Wander3D extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        particles:      { type: QList,     value: [],                  changed: '$affectorChanged' },
        uniqueAmount:   { type: QVector3D, value: { x: 0, y: 0, z: 0 }, changed: '$affectorChanged' },
        uniquePace:     { type: QVector3D, value: { x: 0, y: 0, z: 0 }, changed: '$affectorChanged' },
        uniqueAmountVariation: { type: QReal, value: 0,                changed: '$affectorChanged' },
        uniquePaceVariation:   { type: QReal, value: 0,                changed: '$affectorChanged' },
        globalAmount:   { type: QVector3D, value: { x: 0, y: 0, z: 0 }, changed: '$affectorChanged' },
        globalPace:     { type: QVector3D, value: { x: 0, y: 0, z: 0 }, changed: '$affectorChanged' },
    }

    $createObject3D(THREE){
        return new THREE.Group()
    }

    $affectorChanged(){
        if(this.$host3D) this.$host3D.$requestRender()
    }
}

module.exports.Wander3D = Wander3D
