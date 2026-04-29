const { Node3D } = require('./Node3D')
const { QReal, QVector3D, QList } = require('../utils/properties')

/**
 * Gravity3D - mirrors Qt Quick 3D Particles' Gravity3D.
 *
 * Applies a constant directional gravity force to particles.
 */
class Gravity3D extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        particles:  { type: QList,     value: [],                      changed: '$affectorChanged' },
        direction:  { type: QVector3D, value: { x: 0, y: -100, z: 0 }, changed: '$affectorChanged' },
        magnitude:  { type: QReal,     value: 100,                     changed: '$affectorChanged' },
    }

    $createObject3D(THREE){
        return new THREE.Group()
    }

    $affectorChanged(){
        if(this.$host3D) this.$host3D.$requestRender()
    }
}

module.exports.Gravity3D = Gravity3D
