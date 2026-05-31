const { QtObject } = require('./QtObject')
const { QColor, QReal, QBool } = require('../utils/properties')

/**
 * Fog - mirrors Qt Quick 3D's Fog.
 *
 * Configures distance-based fog for a scene. On the web this creates
 * a THREE.Fog or THREE.FogExp2 applied to the scene.
 */
class Fog extends QtObject {
    static defaultProperties = {
        enabled:     { type: QBool,  value: true,      changed: '$fogChanged' },
        color:       { type: QColor, value: '#ffffff',  changed: '$fogChanged' },
        density:     { type: QReal,  value: 1,          changed: '$fogChanged' },
        depthNear:   { type: QReal,  value: 10,         changed: '$fogChanged' },
        depthFar:    { type: QReal,  value: 1000,       changed: '$fogChanged' },
        depthEnabled:{ type: QBool,  value: true,       changed: '$fogChanged' },
    }

    $applyToScene(THREE, scene){
        if(!THREE || !scene) return
        if(!this.enabled){
            scene.fog = null
            return
        }
        if(this.depthEnabled){
            scene.fog = new THREE.Fog(
                new THREE.Color(this.color || '#ffffff'),
                this.depthNear,
                this.depthFar
            )
        } else {
            scene.fog = new THREE.FogExp2(
                new THREE.Color(this.color || '#ffffff'),
                this.density * 0.001
            )
        }
    }

    $fogChanged(){
        if(this.$onFogChanged) this.$onFogChanged()
    }
}

module.exports.Fog = Fog
