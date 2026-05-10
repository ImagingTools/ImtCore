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
        this.$applyLight(THREE, l)
        return l
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

    $lightChanged(){
        this.$applyLight(this.$host3D && this.$host3D.$THREE)
        if(this.$host3D) this.$host3D.$requestRender()
    }

    $shadowChanged(){
        if(this.$three3D) this.$three3D.castShadow = !!this.castsShadow
        if(this.$host3D) this.$host3D.$requestRender()
    }
}

module.exports.DirectionalLight = DirectionalLight
