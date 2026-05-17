const { Node3D } = require('./Node3D')
const { QColor, QReal, QBool } = require('../utils/properties')

class PointLight extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        color:        { type: QColor, value: '#ffffff', changed: '$lightChanged' },
        brightness:   { type: QReal,  value: 1,         changed: '$lightChanged' },
        constantFade: { type: QReal,  value: 1,         changed: '$lightChanged' },
        linearFade:   { type: QReal,  value: 0,         changed: '$lightChanged' },
        quadraticFade:{ type: QReal,  value: 1,         changed: '$lightChanged' },
        // Three.js exposes a single `distance`/`decay` model. We map
        // quadraticFade -> decay (2.0 ~ physically correct), linearFade -> 1.0
        // is implicit and constantFade is ignored (no exact analog).
        castsShadow:  { type: QBool,  value: false,     changed: '$shadowChanged' },
    }

    $createObject3D(THREE){
        let l = new THREE.PointLight(0xffffff, 1, 0, this.quadraticFade > 0 ? 2 : 1)
        this.$applyLight(THREE, l)
        return l
    }

    $applyLight(THREE, l){
        if(!l) l = this.$three3D
        if(!l) return
        if(THREE) l.color = new THREE.Color(this.color || '#ffffff')
        l.intensity = this.brightness
        l.decay = this.quadraticFade > 0 ? 2 : (this.linearFade > 0 ? 1 : 0)
        l.castShadow = !!this.castsShadow
    }

    $lightChanged(){ this.$applyLight(this.$host3D && this.$host3D.$THREE); if(this.$host3D) this.$host3D.$requestRender() }
    $shadowChanged(){ if(this.$three3D) this.$three3D.castShadow = !!this.castsShadow; if(this.$host3D) this.$host3D.$requestRender() }
}

module.exports.PointLight = PointLight
