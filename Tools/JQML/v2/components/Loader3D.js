const { Node3D } = require('./Node3D')
const { QString, QBool, QVar } = require('../utils/properties')

/**
 * Loader3D - mirrors Qt Quick 3D's Loader3D.
 *
 * Dynamically loads 3D content from a URL or component. On the web
 * this uses GLTFLoader for .glb/.gltf sources.
 */
class Loader3D extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        source: { type: QString, value: '',        changed: '$sourceChanged' },
        active: { type: QBool,   value: true,      changed: '$sourceChanged' },
        item:   { type: QVar,    value: undefined },
    }

    $createObject3D(THREE){
        this.$group = new THREE.Group()
        if(this.active && this.source) this.$loadSource(THREE)
        return this.$group
    }

    $loadSource(THREE){
        if(!THREE) THREE = this.$host3D && this.$host3D.$THREE
        if(!THREE || !this.source || !this.active) return
        let url = this.source
        if(!Loader3D.$gltfPromise){
            Loader3D.$gltfPromise = import('three/examples/jsm/loaders/GLTFLoader.js')
                .then(mod => mod.GLTFLoader || (mod.default && mod.default.GLTFLoader))
                .catch(() => null)
        }
        Loader3D.$gltfPromise.then((Loader) => {
            if(!Loader || !this.$group) return
            let loader = new Loader()
            loader.load(url, (gltf) => {
                if(!this.$group) return
                while(this.$group.children.length) this.$group.remove(this.$group.children[0])
                this.$group.add(gltf.scene)
                this.item = gltf.scene
                if(this.$host3D) this.$host3D.$requestRender()
            }, undefined, (err) => {
                console.warn('[Loader3D] failed to load', url, err)
            })
        })
    }

    $sourceChanged(){
        if(this.$host3D && this.$host3D.$THREE) this.$loadSource(this.$host3D.$THREE)
    }

    $disposeThree(){
        this.$group = null
    }
}

Loader3D.$gltfPromise = null

module.exports.Loader3D = Loader3D
