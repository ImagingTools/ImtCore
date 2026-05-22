const { Node3D } = require('./Node3D')
const { QReal } = require('../utils/properties')

/**
 * Joint - mirrors Qt Quick 3D's Joint.
 *
 * Represents a single bone in a Skeleton hierarchy. On the web this
 * wraps a THREE.Bone positioned by the standard Node3D transform.
 */
class Joint extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        index: { type: QReal, value: 0 },
    }

    $createObject3D(THREE){
        return new THREE.Bone()
    }
}

module.exports.Joint = Joint
