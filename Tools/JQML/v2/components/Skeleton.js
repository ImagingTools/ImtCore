const { Node3D } = require('./Node3D')
const { QList } = require('../utils/properties')

/**
 * Skeleton - mirrors Qt Quick 3D's Skeleton.
 *
 * Container for a hierarchy of Joint nodes used for skeletal animation.
 * On the web this wraps a THREE.Skeleton built from child Joint objects.
 */
class Skeleton extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        joints: { type: QList, value: [], changed: '$skeletonChanged' },
    }

    $createObject3D(THREE){
        return new THREE.Group()
    }

    $buildSkeleton(THREE){
        let bones = []
        let joints = this.joints || []
        for(let j of joints){
            if(j && j.$three3D && j.$three3D.isBone) bones.push(j.$three3D)
        }
        if(bones.length === 0) return null
        this.$threeSkeleton = new THREE.Skeleton(bones)
        return this.$threeSkeleton
    }

    $skeletonChanged(){
        if(this.$host3D && this.$host3D.$THREE){
            this.$buildSkeleton(this.$host3D.$THREE)
        }
        if(this.$host3D) this.$host3D.$requestRender()
    }
}

module.exports.Skeleton = Skeleton
