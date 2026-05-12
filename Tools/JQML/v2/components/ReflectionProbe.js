const { Node3D } = require('./Node3D')
const { QReal, QBool, QVar } = require('../utils/properties')

/**
 * ReflectionProbe - mirrors Qt Quick 3D's ReflectionProbe.
 *
 * Captures the surrounding environment from a point in the scene to
 * produce a cube-map used for reflections. On the web this wraps a
 * THREE.CubeCamera.
 */
class ReflectionProbe extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        quality:       { type: QReal, value: 0,     changed: '$probeChanged' },
        refreshMode:   { type: QReal, value: 0,     changed: '$probeChanged' },
        timeSlicing:   { type: QReal, value: 0,     changed: '$probeChanged' },
        boxSize:       { type: QVar,  value: { x: 0, y: 0, z: 0 }, changed: '$probeChanged' },
        parallaxCorrection: { type: QBool, value: false, changed: '$probeChanged' },
    }

    $createObject3D(THREE){
        return new THREE.Group()
    }

    $probeChanged(){
        if(this.$host3D) this.$host3D.$requestRender()
    }
}

module.exports.ReflectionProbe = ReflectionProbe
