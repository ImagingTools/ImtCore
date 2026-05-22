const { QtObject } = require('./QtObject')
const { QColor, QReal, QVar } = require('../utils/properties')

/**
 * DefaultMaterial - non-PBR (classic Phong-style), mirrors Qt Quick 3D's
 * DefaultMaterial. Maps to THREE.MeshPhongMaterial for similar look.
 */
class DefaultMaterial extends QtObject {
    static defaultProperties = {
        diffuseColor:  { type: QColor, value: '#ffffffff', changed: '$materialChanged' },
        specularTint:  { type: QColor, value: '#ffffffff', changed: '$materialChanged' },
        shininess:     { type: QReal,  value: 50,          changed: '$materialChanged' },
        opacity:       { type: QReal,  value: 1,           changed: '$materialChanged' },
        emissiveColor: { type: QColor, value: '#000000ff', changed: '$materialChanged' },
        diffuseMap:    { type: QVar,   value: undefined,   changed: '$materialChanged' },
        normalMap:     { type: QVar,   value: undefined,   changed: '$materialChanged' },
    }

    $build(THREE){
        if(!this.$threeMat){
            this.$threeMat = new THREE.MeshPhongMaterial()
        }
        let m = this.$threeMat
        m.color = new THREE.Color(this.diffuseColor || '#ffffff')
        m.specular = new THREE.Color(this.specularTint || '#ffffff')
        m.shininess = this.shininess
        m.opacity = this.opacity
        m.transparent = (this.opacity < 1)
        m.emissive = new THREE.Color(this.emissiveColor || '#000000')
        m.map = this.diffuseMap && this.diffuseMap.$build ? this.diffuseMap.$build(THREE) : null
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

module.exports.DefaultMaterial = DefaultMaterial
