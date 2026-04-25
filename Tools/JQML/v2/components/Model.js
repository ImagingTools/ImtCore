const { Node3D } = require('./Node3D')
const { QString, QBool, QVar, QList } = require('../utils/properties')

/**
 * Model - mirrors Qt Quick 3D's Model.
 *
 * `source` may be:
 *   - a built-in primitive token: "#Cube", "#Sphere", "#Cylinder", "#Cone",
 *     "#Plane", "#Rectangle"
 *   - a glTF / GLB URL (loaded lazily via three/examples/jsm/loaders/GLTFLoader)
 *
 * `materials` is a list of material wrappers (PrincipledMaterial /
 * DefaultMaterial). When the list has more than one entry, only the first
 * is currently applied (Qt Quick 3D supports per-submesh material assignment
 * which we leave to a future iteration).
 */
class Model extends Node3D {
    static defaultProperties = {
        ...Node3D.defaultProperties,
        source:          { type: QString, value: '',   changed: '$sourceChanged' },
        materials:       { type: QList,   value: [],   changed: '$materialsChanged' },
        castsShadows:    { type: QBool,   value: true, changed: '$shadowChanged' },
        receivesShadows: { type: QBool,   value: true, changed: '$shadowChanged' },
        // optional explicit geometry (advanced; uses raw THREE.BufferGeometry)
        geometry:        { type: QVar,    value: undefined, changed: '$sourceChanged' },
    }

    $createObject3D(THREE){
        // Start with an empty Group; geometry/material are attached
        // asynchronously by $rebuild().
        this.$group = new THREE.Group()
        this.$rebuild()
        return this.$group
    }

    $rebuild(){
        if(!this.$host3D || !this.$host3D.$THREE) return
        let THREE = this.$host3D.$THREE
        // remove previous mesh
        if(this.$mesh){
            this.$group.remove(this.$mesh)
            this.$disposeMesh(this.$mesh)
            this.$mesh = null
        }

        let geom = null
        let src = this.source
        if(this.geometry && this.geometry.isBufferGeometry){
            geom = this.geometry
        } else if(typeof src === 'string' && src.startsWith('#')){
            geom = this.$buildPrimitive(THREE, src)
        }

        if(geom){
            let mat = this.$buildMaterial(THREE)
            this.$mesh = new THREE.Mesh(geom, mat)
            this.$mesh.castShadow = !!this.castsShadows
            this.$mesh.receiveShadow = !!this.receivesShadows
            this.$mesh.userData.jqmlNode = this
            this.$group.add(this.$mesh)
            if(this.$host3D) this.$host3D.$requestRender()
            return
        }

        // glTF path: lazy-load the loader on demand
        if(typeof src === 'string' && (src.endsWith('.glb') || src.endsWith('.gltf'))){
            this.$loadGltf(THREE, src)
        }
    }

    $buildPrimitive(THREE, token){
        switch(token){
            case '#Cube':      return new THREE.BoxGeometry(100, 100, 100)
            case '#Sphere':    return new THREE.SphereGeometry(50, 32, 16)
            case '#Cylinder':  return new THREE.CylinderGeometry(50, 50, 100, 32)
            case '#Cone':      return new THREE.ConeGeometry(50, 100, 32)
            case '#Plane':     return new THREE.PlaneGeometry(100, 100)
            case '#Rectangle': return new THREE.PlaneGeometry(100, 100)
            default:
                console.warn('[Model] unknown primitive token:', token)
                return null
        }
    }

    $buildMaterial(THREE){
        let mats = this.materials
        if(mats && mats.length){
            let m = mats[0]
            if(m && typeof m.$build === 'function'){
                // wire callback so material edits trigger rebuild
                m.$onMaterialChanged = () => {
                    if(this.$mesh){
                        this.$mesh.material = m.$build(THREE)
                        if(this.$host3D) this.$host3D.$requestRender()
                    }
                }
                return m.$build(THREE)
            }
        }
        // fallback: white standard material
        return new THREE.MeshStandardMaterial({ color: 0xffffff, metalness: 0, roughness: 1 })
    }

    $loadGltf(THREE, src){
        let url = this.$resolveUrl(src)
        // import the GLTFLoader lazily; cache the module promise on the class
        if(!Model.$gltfLoaderPromise){
            Model.$gltfLoaderPromise = import('three/examples/jsm/loaders/GLTFLoader.js')
                .then(mod => mod.GLTFLoader || (mod.default && mod.default.GLTFLoader))
                .catch(err => { console.warn('[Model] GLTFLoader unavailable:', err); return null })
        }
        Model.$gltfLoaderPromise.then((Loader)=>{
            if(!Loader || this.$disposed3D) return
            let loader = new Loader()
            loader.load(url, (gltf)=>{
                if(this.$disposed3D) return
                if(this.$mesh){ this.$group.remove(this.$mesh); this.$disposeMesh(this.$mesh) }
                this.$mesh = gltf.scene
                this.$mesh.userData.jqmlNode = this
                // apply user material if provided (overrides glTF embedded material)
                let mats = this.materials
                if(mats && mats.length && mats[0] && typeof mats[0].$build === 'function'){
                    let mat = mats[0].$build(THREE)
                    this.$mesh.traverse((o)=>{ if(o.isMesh) o.material = mat })
                }
                this.$mesh.traverse((o)=>{
                    if(o.isMesh){
                        o.castShadow = !!this.castsShadows
                        o.receiveShadow = !!this.receivesShadows
                    }
                })
                this.$group.add(this.$mesh)
                if(this.$host3D) this.$host3D.$requestRender()
            }, undefined, (err)=>{
                console.warn('[Model] failed to load', url, err)
            })
        })
    }

    $resolveUrl(src){
        if(!src) return ''
        if(src.startsWith('data:') || src.startsWith('http://') || src.startsWith('https://')) return src
        let root = (typeof rootPath !== 'undefined') ? rootPath : ''
        let safeSrc = src
        let prev
        do {
            prev = safeSrc
            safeSrc = safeSrc.replace(/\.\.\//g, '')
        } while (safeSrc !== prev)
        return (root + '/' + safeSrc).replace(/\/+/g, '/')
    }

    $disposeMesh(m){
        if(!m) return
        m.traverse((o)=>{
            if(o.geometry && typeof o.geometry.dispose === 'function') o.geometry.dispose()
            if(o.material){
                let mats = Array.isArray(o.material) ? o.material : [o.material]
                for(let mat of mats) if(mat && typeof mat.dispose === 'function') mat.dispose()
            }
        })
    }

    $sourceChanged(){ this.$rebuild() }
    $materialsChanged(){ this.$rebuild() }
    $shadowChanged(){
        if(this.$mesh){
            this.$mesh.traverse((o)=>{
                if(o.isMesh){
                    o.castShadow = !!this.castsShadows
                    o.receiveShadow = !!this.receivesShadows
                }
            })
        }
        if(this.$host3D) this.$host3D.$requestRender()
    }

    $disposeThree(){
        this.$disposed3D = true
        if(this.$mesh){
            this.$disposeMesh(this.$mesh)
            this.$mesh = null
        }
        this.$group = null
    }
}

Model.$gltfLoaderPromise = null

module.exports.Model = Model
