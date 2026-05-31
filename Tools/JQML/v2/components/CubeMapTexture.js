const { QtObject } = require('./QtObject')
const { QString } = require('../utils/properties')

/**
 * CubeMapTexture - 6-image cube map for skyboxes / IBL.
 *
 * `source` is a comma-separated list of 6 URLs (px, nx, py, ny, pz, nz)
 * matching Qt Quick 3D's convention.
 */
class CubeMapTexture extends QtObject {
    static defaultProperties = {
        source: { type: QString, value: '', changed: '$reload' },
    }

    $build(THREE){
        if(!THREE) return null
        let url = this.source || ''
        if(this.$loadedUrl === url && this.$threeTex) return this.$threeTex
        if(!url) return null
        let parts = url.split(',').map(s => s.trim()).filter(Boolean)
        if(parts.length !== 6){
            console.warn('[CubeMapTexture] source must be 6 comma-separated URLs (px,nx,py,ny,pz,nz), got', parts.length)
            return null
        }
        if(this.$threeTex){
            try { this.$threeTex.dispose() } catch(e) {}
        }
        this.$threeTex = new THREE.CubeTextureLoader().load(parts, ()=>{
            if(this.$onTextureLoaded) this.$onTextureLoaded()
        })
        this.$loadedUrl = url
        return this.$threeTex
    }

    $reload(){
        this.$loadedUrl = null
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

module.exports.CubeMapTexture = CubeMapTexture
