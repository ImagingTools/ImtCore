const { QtObject } = require('./QtObject')
const { QReal, QVar } = require('../utils/properties')

/**
 * ProceduralTexture - mirrors Qt Quick 3D's ProceduralTexture.
 *
 * Generates texture data from a callback function rather than
 * loading an image. On the web this creates a THREE.DataTexture
 * from user-supplied pixel data.
 */
class ProceduralTexture extends QtObject {
    static defaultProperties = {
        width:       { type: QReal, value: 256,       changed: '$textureChanged' },
        height:      { type: QReal, value: 256,       changed: '$textureChanged' },
        textureData: { type: QVar,  value: undefined, changed: '$textureChanged' },
    }

    $build(THREE){
        if(!THREE) return null
        let w = this.width || 256
        let h = this.height || 256
        let data = this.textureData
        if(!data){
            data = new Uint8Array(w * h * 4)
            data.fill(255)
        }
        if(this.$threeTex){
            try { this.$threeTex.dispose() } catch(e) {}
        }
        this.$threeTex = new THREE.DataTexture(data, w, h)
        this.$threeTex.needsUpdate = true
        return this.$threeTex
    }

    $textureChanged(){
        if(this.$onTextureLoaded) this.$onTextureLoaded()
    }

    destroy(){
        if(this.$threeTex){
            try { this.$threeTex.dispose() } catch(e) {}
            this.$threeTex = null
        }
        super.destroy()
    }
}

module.exports.ProceduralTexture = ProceduralTexture
