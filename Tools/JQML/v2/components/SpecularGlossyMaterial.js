const { QtObject } = require('./QtObject')
const { QColor, QReal, QVar } = require('../utils/properties')

/**
 * SpecularGlossyMaterial - mirrors Qt Quick 3D's SpecularGlossyMaterial.
 *
 * Implements the specular/glossiness PBR workflow. On the web this maps
 * to THREE.MeshPhongMaterial which provides a reasonable approximation.
 */
class SpecularGlossyMaterial extends QtObject {
    static defaultProperties = {
        diffuseColor:   { type: QColor, value: '#ffffffff', changed: '$materialChanged' },
        specularColor:  { type: QColor, value: '#ffffffff', changed: '$materialChanged' },
        glossiness:     { type: QReal,  value: 1,           changed: '$materialChanged' },
        opacity:        { type: QReal,  value: 1,           changed: '$materialChanged' },
        emissiveColor:  { type: QColor, value: '#000000ff', changed: '$materialChanged' },
        diffuseMap:     { type: QVar,   value: undefined,   changed: '$materialChanged' },
        specularMap:    { type: QVar,   value: undefined,   changed: '$materialChanged' },
        normalMap:      { type: QVar,   value: undefined,   changed: '$materialChanged' },
    }

    $build(THREE){
        if(!this.$threeMat){
            this.$threeMat = new THREE.MeshPhongMaterial()
        }
        let m = this.$threeMat
        m.color = new THREE.Color(this.diffuseColor || '#ffffff')
        m.specular = new THREE.Color(this.specularColor || '#ffffff')
        m.shininess = this.glossiness * 100
        m.opacity = this.opacity
        m.transparent = (this.opacity < 1)
        m.emissive = new THREE.Color(this.emissiveColor || '#000000')
        m.map = this.diffuseMap && this.diffuseMap.$build ? this.diffuseMap.$build(THREE) : null
        m.specularMap = this.specularMap && this.specularMap.$build ? this.specularMap.$build(THREE) : null
        m.normalMap = this.normalMap && this.normalMap.$build ? this.normalMap.$build(THREE) : null
        m.needsUpdate = true
        return m
    }

    $materialChanged(){
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

module.exports.SpecularGlossyMaterial = SpecularGlossyMaterial
