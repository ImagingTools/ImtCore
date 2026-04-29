const { Node3D } = require('./Node3D')
const { QReal, QVar } = require('../utils/properties')

/**
 * Repeater3D - mirrors Qt Quick 3D's Repeater3D.
 *
 * Creates multiple instances of a delegate component inside the 3D
 * scene graph, similar to the 2D Repeater but for Node3D children.
 */
class Repeater3D extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        model:    { type: QVar,  value: 0,         changed: '$modelChanged' },
        delegate: { type: QVar,  value: undefined, changed: '$modelChanged' },
        count:    { type: QReal, value: 0 },
    }

    $createObject3D(THREE){
        return new THREE.Group()
    }

    $modelChanged(){
        if(this.$host3D) this.$host3D.$requestRender()
    }
}

module.exports.Repeater3D = Repeater3D
