const { Node3D } = require('./Node3D')
const { QReal } = require('../utils/properties')

/**
 * OrthographicCamera - mirrors Qt Quick 3D's OrthographicCamera.
 *
 * The frustum is symmetric around the camera position. Width/height of
 * the viewing volume are driven by View3D size (1 world-unit per pixel
 * by default); use `scale` to zoom.
 */
class OrthographicCamera extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        clipNear: { type: QReal, value: -10000, changed: '$projectionChanged' },
        clipFar:  { type: QReal, value:  10000, changed: '$projectionChanged' },
    }

    $createObject3D(THREE){
        // initial frustum is unit; View3D.$handleResize will resize.
        let cam = new THREE.OrthographicCamera(-1, 1, 1, -1, this.clipNear, this.clipFar)
        return cam
    }

    $projectionChanged(){
        if(!this.$three3D) return
        this.$three3D.near = this.clipNear
        this.$three3D.far = this.clipFar
        this.$three3D.updateProjectionMatrix()
        if(this.$host3D) this.$host3D.$requestRender()
    }
}

module.exports.OrthographicCamera = OrthographicCamera
