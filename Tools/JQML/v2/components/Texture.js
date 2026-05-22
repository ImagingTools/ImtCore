const { QtObject } = require('./QtObject')
const { QString, QReal } = require('../utils/properties')

/**
 * Texture - 2D image texture, mirrors Qt Quick 3D's Texture.
 *
 * The image is loaded via THREE.TextureLoader on first $build(). URLs are
 * resolved against the app rootPath (same convention as Canvas.js / Image.js).
 *
 * Tiling modes match Qt Quick 3D's TilingMode enum:
 *   ClampToEdge = 0, MirroredRepeat = 1, Repeat = 2
 */
class Texture extends QtObject {
    static ClampToEdge   = 0
    static MirroredRepeat = 1
    static Repeat         = 2

    static defaultProperties = {
        source:      { type: QString, value: '',  changed: '$reload' },
        // tilingMode constants (ClampToEdge=0, MirroredRepeat=1, Repeat=2);
        // use literal 0 here so this initializer does not depend on the
        // ordering of static field initialization.
        tilingModeHorizontal: { type: QReal, value: 0, changed: '$applyTiling' },
        tilingModeVertical:   { type: QReal, value: 0, changed: '$applyTiling' },
    }

    $resolveUrl(src){
        if(!src) return ''
        if(src.startsWith('data:') || src.startsWith('http://') || src.startsWith('https://')) return src
        let root = (typeof rootPath !== 'undefined') ? rootPath : ''
        let safeSrc = src
        let previous
        do {
            previous = safeSrc
            safeSrc = safeSrc.replace(/\.\.\//g, '')
        } while (safeSrc !== previous)
        return (root + '/' + safeSrc).replace(/\/+/g, '/')
    }

    $build(THREE){
        if(!THREE) return null
        let url = this.$resolveUrl(this.source)
        if(this.$loadedUrl === url && this.$threeTex) return this.$threeTex
        if(!url) return null
        if(!this.$loader) this.$loader = new THREE.TextureLoader()
        if(this.$threeTex){
            try { this.$threeTex.dispose() } catch(e) {}
            this.$threeTex = null
        }
        this.$threeTex = this.$loader.load(url, ()=>{
            // when loaded, ask host to redraw
            if(this.$onTextureLoaded) this.$onTextureLoaded()
        })
        this.$loadedUrl = url
        this.$applyTiling()
        return this.$threeTex
    }

    $applyTiling(){
        if(!this.$threeTex) return
        // we lazily access THREE through global namespace if available; otherwise
        // map enum values to numeric constants matching THREE.RepeatWrapping etc.
        // THREE.ClampToEdgeWrapping = 1001, RepeatWrapping = 1000, MirroredRepeatWrapping = 1002.
        const map = [1001, 1002, 1000]
        this.$threeTex.wrapS = map[this.tilingModeHorizontal] || 1001
        this.$threeTex.wrapT = map[this.tilingModeVertical] || 1001
        this.$threeTex.needsUpdate = true
    }

    $reload(){
        // force rebuild on next $build()
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

module.exports.Texture = Texture
