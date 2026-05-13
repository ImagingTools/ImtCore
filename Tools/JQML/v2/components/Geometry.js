const { QtObject } = require('./QtObject')
const { QReal, QVar } = require('../utils/properties')

/**
 * Geometry - mirrors Qt Quick 3D's Geometry.
 *
 * Wraps a custom vertex/index buffer configuration. On the web this
 * creates a THREE.BufferGeometry from the supplied attributes.
 */
class Geometry extends QtObject {
    static defaultProperties = {
        primitiveType: { type: QReal, value: 0,         changed: '$geometryChanged' },
        attributes:    { type: QVar,  value: undefined, changed: '$geometryChanged' },
    }

    $build(THREE){
        if(!this.$threeGeom){
            this.$threeGeom = new THREE.BufferGeometry()
        }
        let g = this.$threeGeom
        if(this.attributes && typeof this.attributes === 'object'){
            for(let name in this.attributes){
                let attr = this.attributes[name]
                if(attr && attr.array && attr.itemSize){
                    g.setAttribute(name, new THREE.BufferAttribute(attr.array, attr.itemSize))
                }
            }
        }
        return g
    }

    $geometryChanged(){
        if(this.$onGeometryChanged) this.$onGeometryChanged()
    }

    destroy(){
        if(this.$threeGeom){
            try { this.$threeGeom.dispose() } catch(e) {}
            this.$threeGeom = null
        }
        super.destroy()
    }
}

module.exports.Geometry = Geometry
