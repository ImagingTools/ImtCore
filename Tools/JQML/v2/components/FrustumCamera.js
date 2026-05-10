const { Node3D } = require('./Node3D')
const { QReal } = require('../utils/properties')

/**
 * FrustumCamera - mirrors Qt Quick 3D's FrustumCamera.
 *
 * Provides a camera defined by explicit frustum planes rather than
 * field-of-view. Maps to a THREE.PerspectiveCamera whose projection
 * matrix is overridden via makeFrustum().
 */
class FrustumCamera extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        left:     { type: QReal, value: -1,    changed: '$projectionChanged' },
        right:    { type: QReal, value: 1,     changed: '$projectionChanged' },
        bottom:   { type: QReal, value: -1,    changed: '$projectionChanged' },
        top:      { type: QReal, value: 1,     changed: '$projectionChanged' },
        clipNear: { type: QReal, value: 10,    changed: '$projectionChanged' },
        clipFar:  { type: QReal, value: 10000, changed: '$projectionChanged' },
    }

    $createObject3D(THREE){
        let cam = new THREE.PerspectiveCamera()
        this.$applyFrustum(cam)
        return cam
    }

    $applyFrustum(cam){
        if(!cam) cam = this.$three3D
        if(!cam) return
        cam.projectionMatrix.makePerspective(
            this.left, this.right, this.top, this.bottom,
            this.clipNear, this.clipFar
        )
        cam.projectionMatrixInverse.copy(cam.projectionMatrix).invert()
    }

    $projectionChanged(){
        this.$applyFrustum()
        if(this.$host3D) this.$host3D.$requestRender()
    }
}

module.exports.FrustumCamera = FrustumCamera
