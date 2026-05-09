const { Node3D } = require('./Node3D')
const { QReal } = require('../utils/properties')

/**
 * AxisHelper - mirrors Qt Quick 3D Helpers' AxisHelper.
 *
 * Renders RGB axis lines (X=red, Y=green, Z=blue) for visual
 * orientation inside a 3D scene.
 */
class AxisHelper extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        axisLength: { type: QReal, value: 100, changed: '$axisChanged' },
    }

    $createObject3D(THREE){
        let group = new THREE.Group()
        this.$buildAxes(THREE, group)
        return group
    }

    $buildAxes(THREE, group){
        if(!group) group = this.$three3D
        if(!group || !THREE) return
        while(group.children.length) group.remove(group.children[0])
        let len = this.axisLength || 100
        let colors = [0xff0000, 0x00ff00, 0x0000ff]
        let dirs = [
            [len, 0, 0],
            [0, len, 0],
            [0, 0, len],
        ]
        for(let i = 0; i < 3; i++){
            let points = [
                new THREE.Vector3(0, 0, 0),
                new THREE.Vector3(dirs[i][0], dirs[i][1], dirs[i][2]),
            ]
            let geom = new THREE.BufferGeometry().setFromPoints(points)
            let mat = new THREE.LineBasicMaterial({ color: colors[i] })
            group.add(new THREE.Line(geom, mat))
        }
    }

    $axisChanged(){
        if(this.$three3D && this.$host3D && this.$host3D.$THREE){
            this.$buildAxes(this.$host3D.$THREE)
        }
        if(this.$host3D) this.$host3D.$requestRender()
    }
}

module.exports.AxisHelper = AxisHelper
