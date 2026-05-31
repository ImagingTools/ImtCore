const { QtObject } = require('./QtObject')
const { QReal, QBool } = require('../utils/properties')

/**
 * GridGeometry - mirrors Qt Quick 3D Helpers' GridGeometry.
 *
 * Generates a flat grid geometry (line-based) useful for ground-plane
 * visualization. On the web this builds a THREE.BufferGeometry of
 * grid lines.
 */
class GridGeometry extends QtObject {
    static defaultProperties = {
        horizontalLines: { type: QReal, value: 20,   changed: '$geometryChanged' },
        verticalLines:   { type: QReal, value: 20,   changed: '$geometryChanged' },
        horizontalStep:  { type: QReal, value: 100,  changed: '$geometryChanged' },
        verticalStep:    { type: QReal, value: 100,  changed: '$geometryChanged' },
    }

    $build(THREE){
        if(!THREE) return null
        let hLines = this.horizontalLines || 1
        let vLines = this.verticalLines || 1
        let hStep = this.horizontalStep || 100
        let vStep = this.verticalStep || 100
        let points = []
        let halfW = (vLines * vStep) / 2
        let halfH = (hLines * hStep) / 2
        for(let i = 0; i <= hLines; i++){
            let z = -halfH + i * hStep
            points.push(-halfW, 0, z, halfW, 0, z)
        }
        for(let i = 0; i <= vLines; i++){
            let x = -halfW + i * vStep
            points.push(x, 0, -halfH, x, 0, halfH)
        }
        if(this.$threeGeom){
            try { this.$threeGeom.dispose() } catch(e) {}
        }
        this.$threeGeom = new THREE.BufferGeometry()
        this.$threeGeom.setAttribute('position', new THREE.Float32BufferAttribute(points, 3))
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

module.exports.GridGeometry = GridGeometry
