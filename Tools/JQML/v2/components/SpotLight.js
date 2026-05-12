const { Node3D } = require('./Node3D')
const { QColor, QReal, QBool } = require('../utils/properties')

/**
 * SpotLight - cone-shaped light. The cone direction is +Z in local space
 * (Qt Quick 3D convention). On Three.js a SpotLight points at its target;
 * we keep a hidden child target offset along +Z.
 */
class SpotLight extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        color:       { type: QColor, value: '#ffffff', changed: '$lightChanged' },
        brightness:  { type: QReal,  value: 1,         changed: '$lightChanged' },
        coneAngle:   { type: QReal,  value: 40,        changed: '$lightChanged' }, // degrees, full cone
        innerConeAngle:{ type: QReal,value: 30,        changed: '$lightChanged' }, // degrees
        castsShadow: { type: QBool,  value: false,     changed: '$shadowChanged' },
    }

    $createObject3D(THREE){
        let l = new THREE.SpotLight(0xffffff, 1)
        // point along +Z by parking target at (0,0,1) in local space
        l.target.position.set(0, 0, 1)
        l.add(l.target)
        this.$applyLight(THREE, l)
        return l
    }

    $applyLight(THREE, l){
        if(!l) l = this.$three3D
        if(!l) return
        const DEG = Math.PI / 180
        if(THREE) l.color = new THREE.Color(this.color || '#ffffff')
        l.intensity = this.brightness
        l.angle = (this.coneAngle * 0.5) * DEG
        let inner = Math.min(this.innerConeAngle, this.coneAngle)
        l.penumbra = (this.coneAngle > 0) ? Math.max(0, 1 - inner / this.coneAngle) : 0
        l.castShadow = !!this.castsShadow
    }

    $lightChanged(){ this.$applyLight(this.$host3D && this.$host3D.$THREE); if(this.$host3D) this.$host3D.$requestRender() }
    $shadowChanged(){ if(this.$three3D) this.$three3D.castShadow = !!this.castsShadow; if(this.$host3D) this.$host3D.$requestRender() }
}

module.exports.SpotLight = SpotLight
