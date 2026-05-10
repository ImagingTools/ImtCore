const { Node3D } = require('./Node3D')
const { QReal, QVar } = require('../utils/properties')

/**
 * CustomCamera - mirrors Qt Quick 3D's CustomCamera.
 *
 * Allows supplying an arbitrary 4×4 projection matrix. On the web
 * this wraps a bare THREE.Camera whose projection is set manually.
 */
class CustomCamera extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        projection: { type: QVar,  value: undefined, changed: '$projectionChanged' },
        clipNear:   { type: QReal, value: 10,        changed: '$projectionChanged' },
        clipFar:    { type: QReal, value: 10000,     changed: '$projectionChanged' },
    }

    $createObject3D(THREE){
        let cam = new THREE.Camera()
        this.$applyProjection(cam)
        return cam
    }

    $applyProjection(cam){
        if(!cam) cam = this.$three3D
        if(!cam) return
        if(this.projection && Array.isArray(this.projection) && this.projection.length === 16){
            cam.projectionMatrix.fromArray(this.projection)
            cam.projectionMatrixInverse.copy(cam.projectionMatrix).invert()
        }
    }

    $projectionChanged(){
        this.$applyProjection()
        if(this.$host3D) this.$host3D.$requestRender()
    }
}

module.exports.CustomCamera = CustomCamera
