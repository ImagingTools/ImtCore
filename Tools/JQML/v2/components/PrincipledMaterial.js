const { QtObject } = require('./QtObject')
const { QColor, QReal, QVar } = require('../utils/properties')

/**
 * PrincipledMaterial - PBR (metalness/roughness) workflow, mirrors Qt
 * Quick 3D's PrincipledMaterial. Maps to THREE.MeshStandardMaterial.
 *
 * NOTE: A material is a *value*, not a Node3D — it doesn't live in the
 * scene graph. The owning Model picks it up via its `materials` list.
 */
class PrincipledMaterial extends QtObject {
    static defaultProperties = {
        baseColor:        { type: QColor, value: '#ffffffff', changed: '$materialChanged' },
        metalness:        { type: QReal,  value: 0,           changed: '$materialChanged' },
        roughness:        { type: QReal,  value: 1,           changed: '$materialChanged' },
        opacity:          { type: QReal,  value: 1,           changed: '$materialChanged' },
        emissiveColor:    { type: QColor, value: '#000000ff', changed: '$materialChanged' },
        // texture maps (reference Texture objects)
        baseColorMap:     { type: QVar,   value: undefined,   changed: '$materialChanged' },
        metalnessMap:     { type: QVar,   value: undefined,   changed: '$materialChanged' },
        roughnessMap:     { type: QVar,   value: undefined,   changed: '$materialChanged' },
        normalMap:        { type: QVar,   value: undefined,   changed: '$materialChanged' },
        emissiveMap:      { type: QVar,   value: undefined,   changed: '$materialChanged' },
    }

    /**
     * Build (or update) the underlying THREE.Material.
     * @param {object} THREE the lazily-loaded three.js module
     * @returns {THREE.Material}
     */
    $toThreeColor(THREE, colorValue, fallback = '#ffffff'){
        let c = (colorValue === undefined || colorValue === null) ? fallback : ('' + colorValue)

        // In this runtime QColor uses #RRGGBBAA / #RGBA, while THREE.Color
        // expects #RRGGBB / #RGB (no alpha channel).
        if(/^#([0-9a-f]{8})$/i.test(c)){
            // #RRGGBBAA -> #RRGGBB
            c = '#' + c.substring(1, 7)
        } else if(/^#([0-9a-f]{4})$/i.test(c)){
            // #RGBA -> #RGB
            c = '#' + c[1] + c[2] + c[3]
        }

        try {
            return new THREE.Color(c)
        } catch (_e) {
            return new THREE.Color(fallback)
        }
    }

    $build(THREE){
        if(!this.$threeMat){
            this.$threeMat = new THREE.MeshStandardMaterial()
        }
        let m = this.$threeMat
        m.color = this.$toThreeColor(THREE, this.baseColor, '#ffffff')
        m.metalness = this.metalness
        m.roughness = this.roughness
        m.opacity = this.opacity
        m.transparent = (this.opacity < 1)
        m.emissive = this.$toThreeColor(THREE, this.emissiveColor, '#000000')

        m.map = this.baseColorMap && this.baseColorMap.$build ? this.baseColorMap.$build(THREE) : null
        m.metalnessMap = this.metalnessMap && this.metalnessMap.$build ? this.metalnessMap.$build(THREE) : null
        m.roughnessMap = this.roughnessMap && this.roughnessMap.$build ? this.roughnessMap.$build(THREE) : null
        m.normalMap = this.normalMap && this.normalMap.$build ? this.normalMap.$build(THREE) : null
        m.emissiveMap = this.emissiveMap && this.emissiveMap.$build ? this.emissiveMap.$build(THREE) : null

        m.needsUpdate = true
        return m
    }

    $materialChanged(){
        // notify any owning Model to rebuild
        if(this.$onMaterialChanged) this.$onMaterialChanged()
    }

    destroy(){
        if(this.$threeMat){
            try { this.$threeMat.dispose() } catch(e) {}
            this.$threeMat = null
        }
        super.destroy()
    }
}

module.exports.PrincipledMaterial = PrincipledMaterial
