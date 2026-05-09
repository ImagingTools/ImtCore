const { Node3D } = require('./Node3D')
const { QReal } = require('../utils/properties')

/**
 * PerspectiveCamera - mirrors Qt Quick 3D's PerspectiveCamera.
 *
 * Properties:
 *   fieldOfView : vertical FOV in degrees (default 60)
 *   clipNear    : near plane distance     (default 10)
 *   clipFar     : far  plane distance     (default 10000)
 *
 * The actual aspect ratio is driven by View3D's size and updated by
 * View3D.$handleResize().
 */
class PerspectiveCamera extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        fieldOfView: { type: QReal, value: 60,    changed: '$projectionChanged' },
        clipNear:    { type: QReal, value: 10,    changed: '$projectionChanged' },
        clipFar:     { type: QReal, value: 10000, changed: '$projectionChanged' },
    }

    $createObject3D(THREE){
        let cam = new THREE.PerspectiveCamera(
            this.fieldOfView || 60,
            1, // aspect; will be set by View3D.$handleResize
            this.clipNear || 0.1,
            this.clipFar || 10000
        )
        return cam
    }

    $projectionChanged(){
        if(!this.$three3D) return
        this.$three3D.fov = this.fieldOfView
        this.$three3D.near = this.clipNear
        this.$three3D.far = this.clipFar
        this.$three3D.updateProjectionMatrix()
        if(this.$host3D) this.$host3D.$requestRender()
    }
}

module.exports.PerspectiveCamera = PerspectiveCamera
