const { QtObject } = require('./QtObject')
const { QString, QReal, QVar } = require('../utils/properties')

/**
 * CustomMaterial - mirrors Qt Quick 3D's CustomMaterial.
 *
 * Allows supplying raw vertex/fragment shaders. On the web this maps
 * to THREE.ShaderMaterial.
 */
class CustomMaterial extends QtObject {
    static defaultProperties = {
        vertexShader:   { type: QString, value: '',        changed: '$materialChanged' },
        fragmentShader: { type: QString, value: '',        changed: '$materialChanged' },
        uniforms:       { type: QVar,    value: undefined, changed: '$materialChanged' },
        opacity:        { type: QReal,   value: 1,         changed: '$materialChanged' },
    }

    $build(THREE){
        if(!this.$threeMat){
            this.$threeMat = new THREE.ShaderMaterial()
        }
        let m = this.$threeMat
        if(this.vertexShader) m.vertexShader = this.vertexShader
        if(this.fragmentShader) m.fragmentShader = this.fragmentShader
        if(this.uniforms && typeof this.uniforms === 'object'){
            m.uniforms = this.uniforms
        }
        m.opacity = this.opacity
        m.transparent = (this.opacity < 1)
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

module.exports.CustomMaterial = CustomMaterial
