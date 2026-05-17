const { Node3D } = require('./Node3D')
const { QString, QReal, QVar, QList } = require('../utils/properties')

/**
 * InstanceModel - mirrors Qt Quick 3D's Model with instancing enabled.
 *
 * Renders multiple instances of the same geometry using
 * THREE.InstancedMesh for efficient batched draw calls.
 */
class InstanceModel extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        source:        { type: QString, value: '',        changed: '$rebuild' },
        materials:     { type: QList,   value: [],        changed: '$rebuild' },
        instancing:    { type: QVar,    value: undefined, changed: '$rebuild' },
        instanceCount: { type: QReal,   value: 0,         changed: '$rebuild' },
    }

    $createObject3D(THREE){
        this.$group = new THREE.Group()
        this.$rebuild()
        return this.$group
    }

    $rebuild(){
        if(!this.$host3D || !this.$host3D.$THREE) return
        let THREE = this.$host3D.$THREE
        if(this.$mesh){
            this.$group.remove(this.$mesh)
            if(this.$mesh.geometry) this.$mesh.geometry.dispose()
            if(this.$mesh.material && typeof this.$mesh.material.dispose === 'function') this.$mesh.material.dispose()
            this.$mesh = null
        }
        let count = this.instanceCount || 0
        if(count <= 0) return
        let geom = this.$buildGeometry(THREE)
        if(!geom) return
        let mat = this.$buildMaterial(THREE)
        this.$mesh = new THREE.InstancedMesh(geom, mat, count)
        this.$mesh.userData.jqmlNode = this
        this.$group.add(this.$mesh)
        if(this.$host3D) this.$host3D.$requestRender()
    }

    $buildGeometry(THREE){
        let src = this.source
        if(typeof src !== 'string') return null
        switch(src){
            case '#Cube':      return new THREE.BoxGeometry(100, 100, 100)
            case '#Sphere':    return new THREE.SphereGeometry(50, 32, 16)
            case '#Cylinder':  return new THREE.CylinderGeometry(50, 50, 100, 32)
            case '#Cone':      return new THREE.ConeGeometry(50, 100, 32)
            case '#Plane':     return new THREE.PlaneGeometry(100, 100)
            default:           return null
        }
    }

    $buildMaterial(THREE){
        let mats = this.materials
        if(mats && mats.length && mats[0] && typeof mats[0].$build === 'function'){
            return mats[0].$build(THREE)
        }
        return new THREE.MeshStandardMaterial({ color: 0xffffff })
    }

    $disposeThree(){
        if(this.$mesh){
            if(this.$mesh.geometry) this.$mesh.geometry.dispose()
            if(this.$mesh.material && typeof this.$mesh.material.dispose === 'function') this.$mesh.material.dispose()
            this.$mesh = null
        }
        this.$group = null
    }
}

module.exports.InstanceModel = InstanceModel
