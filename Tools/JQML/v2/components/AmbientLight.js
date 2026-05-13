const { Node3D } = require('./Node3D')
const { QColor, QReal } = require('../utils/properties')

/**
 * AmbientLight - non-directional fill light. There is no direct equivalent
 * in Qt Quick 3D (which uses Image-Based Lighting via SceneEnvironment);
 * we provide it as a convenience for simple scenes.
 */
class AmbientLight extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        color:      { type: QColor, value: '#ffffff', changed: '$lightChanged' },
        brightness: { type: QReal,  value: 0.2,       changed: '$lightChanged' },
    }

    $createObject3D(THREE){
        let l = new THREE.AmbientLight(0xffffff, 0.2)
        this.$applyLight(THREE, l)
        return l
    }

    $applyLight(THREE, l){
        if(!l) l = this.$three3D
        if(!l) return
        if(THREE) l.color = new THREE.Color(this.color || '#ffffff')
        l.intensity = this.brightness
    }

    $lightChanged(){ this.$applyLight(this.$host3D && this.$host3D.$THREE); if(this.$host3D) this.$host3D.$requestRender() }
}

module.exports.AmbientLight = AmbientLight
