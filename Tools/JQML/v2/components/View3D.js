const { Item } = require('./Item')
const { QBool, QReal, QString, QVar, QColor } = require('../utils/properties')

/**
 * View3D - hosts a 3D scene inside a 2D Item rectangle.
 *
 * Mirrors Qt Quick 3D's View3D type. On the web we render via Three.js'
 * WebGLRenderer into a <canvas> element that owns the Item's $dom area.
 *
 * Three.js (and the GLTFLoader, used by Model) is loaded LAZILY: the very
 * first time a View3D is constructed, we kick off `import('three')`. While
 * the import is in flight, child Node3D instances queue themselves and get
 * attached to the scene as soon as THREE is available. This keeps the 2D
 * web bundle free of any 3D code for apps that don't use View3D.
 *
 * The render loop is on-demand: $requestRender() schedules one frame on
 * the next requestAnimationFrame; nothing renders unless something changed.
 */
class View3D extends Item {
    static InlineMode = 0
    static OffscreenMode = 1

    static defaultProperties = {
        camera:        { type: QVar,   value: undefined, changed: '$cameraChanged' },
        environment:   { type: QVar,   value: undefined, changed: '$environmentChanged' },
        importScene:   { type: QVar,   value: undefined },
        renderMode:    { type: QReal,  value: View3D.InlineMode },
        // diagnostics
        ready3D:       { type: QBool,  value: false, changed: '$ready3DChanged' },
        // background fallback when no environment is set
        clearColor:    { type: QColor, value: '#00000000', changed: '$clearColorChanged' },
    }

    static defaultSignals = {
        ready: { params: [] },
        sceneGraphChanged: { params: [] },
    }

    // marker used by Node3D.$findHost3D() to recognize us
    $isView3D = true

    constructor(parent, exCtx, exModel){
        super(parent, exCtx, exModel)

        this.$THREE = null
        this.$renderer = null
        this.$scene = null
        this.$activeCamera = null
        this.$pendingNodes = []
        this.$renderRequested = false
        this.$disposed = false

        this.setStyle({
            pointerEvents: 'auto',
            touchAction: 'auto',
            position: 'absolute',
            overflow: 'hidden',
        })

        // create the <canvas> child element manually because <div> default
        // from Item already exists; the canvas fills the Item.
        this.$canvas = document.createElement('canvas')
        this.$canvas.style.position = 'absolute'
        this.$canvas.style.left = '0'
        this.$canvas.style.top = '0'
        this.$canvas.style.width = '100%'
        this.$canvas.style.height = '100%'
        this.$canvas.style.display = 'block'
        this.$dom.appendChild(this.$canvas)

        // size tracking
        if(typeof ResizeObserver !== 'undefined'){
            this.$resizeObserver = new ResizeObserver(()=>{ this.$handleResize() })
            this.$resizeObserver.observe(this.$dom)
        } else {
            // fallback: react to width/height property changes
            this.getProperty('width').connect(this, this.$handleResize)
            this.getProperty('height').connect(this, this.$handleResize)
        }

        this.$loadThree()
    }

    /**
     * Lazy-load three.js. Resolves silently if it's already loaded.
     */
    $loadThree(){
        if(this.$THREE) return Promise.resolve(this.$THREE)
        if(View3D.$threePromise) {
            return View3D.$threePromise.then((THREE)=>{ this.$onThreeLoaded(THREE); return THREE })
        }
        let p
        try {
            // dynamic import keeps this out of the main bundle
            p = import('three')
        } catch (e) {
            // some bundlers (CommonJS) won't honor `import()`; fallback
            return new Promise((_resolve, reject)=>{
                console.warn('[View3D] three.js could not be loaded:', e)
                reject(e)
            })
        }
        View3D.$threePromise = p.then((mod)=>{
            // both `import('three')` (ESM) and webpack interop produce { ... } where
            // the named exports live; some CDNs put them on .default
            let THREE = (mod && (mod.Scene || mod.WebGLRenderer)) ? mod : (mod && mod.default ? mod.default : mod)
            return THREE
        }).catch((err)=>{
            console.warn('[View3D] three.js dynamic import failed:', err)
            return null
        })
        return View3D.$threePromise.then((THREE)=>{ if(THREE) this.$onThreeLoaded(THREE); return THREE })
    }

    $onThreeLoaded(THREE){
        if(this.$disposed || this.$THREE) return
        this.$THREE = THREE
        this.$scene = new THREE.Scene()

        try {
            this.$renderer = new THREE.WebGLRenderer({
                canvas: this.$canvas,
                antialias: true,
                alpha: true,
                preserveDrawingBuffer: false,
            })
        } catch (e) {
            console.warn('[View3D] WebGL renderer creation failed:', e)
            return
        }

        if(typeof window !== 'undefined' && window.devicePixelRatio){
            this.$renderer.setPixelRatio(window.devicePixelRatio)
        }
        this.$applyClearColor()
        this.$handleResize()

        // Attach all queued Node3D children (recursively via their own $registerNode3D)
        for(let node of this.$pendingNodes.splice(0, this.$pendingNodes.length)){
            node.$attach3D(THREE, this.$scene)
        }
        // Also re-walk QML children in case some are Node3D and weren't caught
        // (they should have been via Node3D.$findHost3D, but be safe).
        let kids = this.getProperty('children').get()
        if(kids){
            for(let k of kids){
                if(k && k.$attach3D && !k.$three3D){
                    k.$attach3D(THREE, this.$scene)
                }
            }
        }

        // Bind camera if one was set declaratively before THREE loaded
        this.$cameraChanged()

        this.ready3D = true
        let sig = this.getSignal && this.getSignal('ready')
        if(sig) sig()
        this.$requestRender()
    }

    /**
     * Called by Node3D constructors. Either attaches immediately (if THREE
     * is loaded) or queues for $onThreeLoaded.
     */
    $registerNode3D(node){
        if(this.$THREE && this.$scene){
            node.$attach3D(this.$THREE, this.$scene)
            this.$requestRender()
        } else {
            if(this.$pendingNodes.indexOf(node) < 0) this.$pendingNodes.push(node)
        }
    }

    $unregisterNode3D(node){
        let idx = this.$pendingNodes.indexOf(node)
        if(idx >= 0) this.$pendingNodes.splice(idx, 1)
        this.$requestRender()
    }

    /**
     * Schedule a render frame. Coalesces multiple requests in a single tick.
     */
    $requestRender(){
        if(this.$renderRequested || this.$disposed) return
        if(!this.$renderer || !this.$scene || !this.$activeCamera) return
        this.$renderRequested = true
        let raf = (typeof requestAnimationFrame !== 'undefined')
            ? requestAnimationFrame
            : (cb)=>setTimeout(cb, 16)
        raf(()=>{
            this.$renderRequested = false
            if(this.$disposed || !this.$renderer || !this.$scene || !this.$activeCamera) return
            try {
                this.$renderer.render(this.$scene, this.$activeCamera)
            } catch (e) {
                console.warn('[View3D] render failed:', e)
            }
        })
    }

    $handleResize(){
        if(!this.$renderer) return
        let rect = this.$dom.getBoundingClientRect()
        let w = Math.max(1, Math.floor(rect.width))
        let h = Math.max(1, Math.floor(rect.height))
        this.$renderer.setSize(w, h, false)
        // update active camera aspect if it's a perspective camera
        let cam = this.$activeCamera
        if(cam){
            if(cam.isPerspectiveCamera){
                cam.aspect = w / h
                cam.updateProjectionMatrix()
            } else if(cam.isOrthographicCamera){
                // keep symmetric frustum sized to viewport
                let halfW = w / 2
                let halfH = h / 2
                cam.left = -halfW; cam.right = halfW
                cam.top = halfH;  cam.bottom = -halfH
                cam.updateProjectionMatrix()
            }
        }
        this.$requestRender()
    }

    $cameraChanged(){
        if(!this.$THREE) return
        let camNode = this.camera
        if(camNode && camNode.$attach3D && !camNode.$three3D){
            // attach orphan camera (camera declared outside the visual tree)
            camNode.$attach3D(this.$THREE, this.$scene)
        }
        this.$activeCamera = (camNode && camNode.$three3D) ? camNode.$three3D : null
        this.$handleResize()
        this.$requestRender()
    }

    $environmentChanged(){
        if(!this.$THREE || !this.$scene) return
        let env = this.environment
        if(env && typeof env.$applyToScene === 'function'){
            env.$applyToScene(this.$THREE, this.$scene, this.$renderer)
        }
        this.$requestRender()
    }

    $clearColorChanged(){ this.$applyClearColor(); this.$requestRender() }

    $applyClearColor(){
        if(!this.$renderer || !this.$THREE) return
        let c = this.clearColor
        // QColor uses #rrggbbaa or named CSS colors; THREE.Color parses css.
        try {
            let col = new this.$THREE.Color(c)
            // alpha extraction from #rrggbbaa
            let alpha = 1
            if(typeof c === 'string' && /^#([0-9a-f]{8})$/i.test(c)){
                alpha = parseInt(c.substring(7, 9), 16) / 255
            }
            this.$renderer.setClearColor(col, alpha)
        } catch (e){
            // ignore invalid color
        }
    }

    $ready3DChanged(){}

    /**
     * Pick a 3D object at a 2D screen position (relative to the View3D).
     * Returns the closest Node3D under the cursor or null.
     * @param {number} x
     * @param {number} y
     */
    pick(x, y){
        if(!this.$THREE || !this.$scene || !this.$activeCamera) return null
        let rect = this.$dom.getBoundingClientRect()
        let nx = (x / rect.width) * 2 - 1
        let ny = -((y / rect.height) * 2 - 1)
        let raycaster = new this.$THREE.Raycaster()
        raycaster.setFromCamera({ x: nx, y: ny }, this.$activeCamera)
        let hits = raycaster.intersectObjects(this.$scene.children, true)
        for(let h of hits){
            let obj = h.object
            while(obj){
                if(obj.userData && obj.userData.jqmlNode) return obj.userData.jqmlNode
                obj = obj.parent
            }
        }
        return null
    }

    /**
     * Map a 3D world position to a 2D screen position inside this View3D.
     * @param {object} pos {x,y,z}
     */
    mapFrom3DScene(pos){
        if(!this.$THREE || !this.$activeCamera) return { x: 0, y: 0, z: 0 }
        let v = new this.$THREE.Vector3(pos.x, pos.y, pos.z)
        v.project(this.$activeCamera)
        let rect = this.$dom.getBoundingClientRect()
        return {
            x: (v.x + 1) * 0.5 * rect.width,
            y: (1 - (v.y + 1) * 0.5) * rect.height,
            z: v.z,
        }
    }

    /**
     * Inverse of mapFrom3DScene: 2D screen position to 3D world position
     * (z is the normalised depth in [-1,1]).
     * @param {object} pos {x,y,z}
     */
    mapTo3DScene(pos){
        if(!this.$THREE || !this.$activeCamera) return { x: 0, y: 0, z: 0 }
        let rect = this.$dom.getBoundingClientRect()
        let nx = (pos.x / rect.width) * 2 - 1
        let ny = -((pos.y / rect.height) * 2 - 1)
        let nz = (pos.z !== undefined) ? pos.z : 0
        let v = new this.$THREE.Vector3(nx, ny, nz)
        v.unproject(this.$activeCamera)
        return { x: v.x, y: v.y, z: v.z }
    }

    addDomChild(child){
        // 3D Node children must NOT live in the DOM tree; they live in the
        // THREE scene graph. If a child is a Node3D and it accidentally got
        // inserted as a DOM child by Item.createDom(), pull it back out.
        if(child && child.$attach3D){
            if(child.$dom && child.$dom.parentNode === this.$dom){
                this.$dom.removeChild(child.$dom)
            }
        }
        super.addDomChild(child)
    }

    destroy(){
        this.$disposed = true
        if(this.$resizeObserver){
            try { this.$resizeObserver.disconnect() } catch(e) {}
            this.$resizeObserver = null
        }
        if(this.$renderer){
            try { this.$renderer.dispose() } catch(e) {}
            this.$renderer = null
        }
        // dispose scene resources
        if(this.$scene && this.$THREE){
            this.$scene.traverse((obj)=>{
                if(obj.geometry && typeof obj.geometry.dispose === 'function') obj.geometry.dispose()
                if(obj.material){
                    let mats = Array.isArray(obj.material) ? obj.material : [obj.material]
                    for(let m of mats) if(m && typeof m.dispose === 'function') m.dispose()
                }
            })
        }
        this.$scene = null
        this.$activeCamera = null
        this.$pendingNodes.length = 0
        super.destroy()
    }
}

// shared promise so multiple View3Ds share a single THREE module load
View3D.$threePromise = null

module.exports.View3D = View3D
