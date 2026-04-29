const { QtObject } = require('./QtObject')
const { QReal, QVar, QString } = require('../utils/properties')

/**
 * HeightFieldGeometry - mirrors Qt Quick 3D Helpers' HeightFieldGeometry.
 *
 * Generates a terrain mesh from a heightmap image. On the web this
 * creates a subdivided plane whose vertex Y values are displaced by
 * the heightmap pixel values.
 */
class HeightFieldGeometry extends QtObject {
    static defaultProperties = {
        source:        { type: QString, value: '',  changed: '$geometryChanged' },
        extents:       { type: QVar,    value: { x: 100, y: 100, z: 100 }, changed: '$geometryChanged' },
        heightAmount:  { type: QReal,   value: 1,   changed: '$geometryChanged' },
        smoothShading: { type: QVar,    value: true, changed: '$geometryChanged' },
    }

    $build(THREE){
        if(!THREE) return null
        let ex = this.extents || { x: 100, y: 100, z: 100 }
        let segs = 64
        if(this.$threeGeom){
            try { this.$threeGeom.dispose() } catch(e) {}
        }
        this.$threeGeom = new THREE.PlaneGeometry(ex.x, ex.z, segs, segs)
        this.$threeGeom.rotateX(-Math.PI / 2)
        return this.$threeGeom
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

module.exports.HeightFieldGeometry = HeightFieldGeometry
