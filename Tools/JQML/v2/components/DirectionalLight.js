const { Node3D } = require('./Node3D')
const { QColor, QReal, QBool } = require('../utils/properties')

/**
 * Common base for the four light types we support. Mirrors the property
 * naming used by Qt Quick 3D's *Light types.
 */
function makeLightDefaults(){
    return {
        ...Node3D.defaultProperties,
        color:        { type: QColor, value: '#ffffff', changed: '$lightChanged' },
        brightness:   { type: QReal,  value: 1,         changed: '$lightChanged' },
        castsShadow:  { type: QBool,  value: false,     changed: '$shadowChanged' },
    }
}

class DirectionalLight extends Node3D {
    static defaultProperties = makeLightDefaults()

    $createObject3D(THREE){
        let l = new THREE.DirectionalLight(0xffffff, 1)
        // three.js DirectionalLight direction is defined by position->target,
        // not by the light's own rotation. We emulate QtQuick3D semantics where
        // eulerRotation defines light direction.
        this.$target3D = new THREE.Object3D()
        l.target = this.$target3D
        this.$applyLight(THREE, l)
        this.$applyDirection(THREE, l)
        return l
    }

    $attach3D(THREE, parentObject3D){
        const obj = super.$attach3D(THREE, parentObject3D)
        if(this.$target3D && parentObject3D && !this.$target3D.parent){
            parentObject3D.add(this.$target3D)
        }
        this.$applyDirection(THREE, obj)
        return obj
    }

    $applyLight(THREE, l){
        if(!l) l = this.$three3D
        if(!l) return
        if(THREE){
            l.color = new THREE.Color(this.color || '#ffffff')
        }
        l.intensity = this.brightness
        l.castShadow = !!this.castsShadow
    }

    $applyDirection(THREE, l){
        if(!l) l = this.$three3D
        if(!l || !THREE) return

        const e = this.eulerRotation || { x: 0, y: 0, z: 0 }
        const rad = Math.PI / 180
        const ex = (e.x || 0) * rad
        const ey = (e.y || 0) * rad
        const ez = (e.z || 0) * rad

        // Base forward vector pointing -Z, then apply Euler ZYX (same as Node3D)
        const dir = new THREE.Vector3(0, 0, -1)
        dir.applyEuler(new THREE.Euler(ex, ey, ez, 'ZYX')).normalize()

        const p = this.position || { x: 0, y: 0, z: 0 }
        const origin = new THREE.Vector3(p.x || 0, p.y || 0, p.z || 0)
        const distance = 1000

        if(this.$target3D){
            this.$target3D.position.copy(origin)
        }
        // Light comes from opposite side along direction.
        l.position.copy(origin.clone().sub(dir.multiplyScalar(distance)))
        l.updateMatrixWorld()
        if(this.$target3D) this.$target3D.updateMatrixWorld()
    }

    $lightChanged(){
        this.$applyLight(this.$host3D && this.$host3D.$THREE)
        this.$applyDirection(this.$host3D && this.$host3D.$THREE)
        if(this.$host3D) this.$host3D.$requestRender()
    }

    $transformChanged(){
        this.$applyDirection(this.$host3D && this.$host3D.$THREE)
        if(this.$host3D) this.$host3D.$requestRender()
    }

    $rotationChanged(){
        this.$applyDirection(this.$host3D && this.$host3D.$THREE)
        if(this.$host3D) this.$host3D.$requestRender()
    }

    $shadowChanged(){
        if(this.$three3D) this.$three3D.castShadow = !!this.castsShadow
        if(this.$host3D) this.$host3D.$requestRender()
    }

    destroy(){
        if(this.$target3D && this.$target3D.parent){
            this.$target3D.parent.remove(this.$target3D)
        }
        this.$target3D = null
        super.destroy()
    }
}

module.exports.DirectionalLight = DirectionalLight
