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
        renderMode:    { type: QReal,  value: 0 /* View3D.InlineMode */ },
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
     *
     * Strategy (in priority order):
     *  1. Already loaded as window.THREE  → use it immediately.
     *  2. Already injected <script> tag   → wait for it to finish.
     *  3. Inject a <script> tag pointing at View3D.threeUrl and wait for onload.
     *
     * This approach works with webpack externals because we never call
     * import('three') (which webpack can't resolve for externals at runtime).
     *
     * Set View3D.threeUrl BEFORE the first View3D is constructed if you want
     * to use a different CDN or a local file, e.g.:
     *   View3D.threeUrl = '/js/three.min.js'
     */
    $loadThree(){
        if(this.$THREE) return Promise.resolve(this.$THREE)

        // Already available as a global (e.g. loaded by the page via <script>)
        if(typeof window !== 'undefined' && window.THREE){
            return Promise.resolve(window.THREE)
                .then((THREE)=>{ this.$onThreeLoaded(THREE); return THREE })
        }

        if(View3D.$threePromise) {
            return View3D.$threePromise.then((THREE)=>{ if(THREE) this.$onThreeLoaded(THREE); return THREE })
        }

        // Inject a <script> tag to load three.js from CDN / local path
        View3D.$threePromise = new Promise((resolve, reject)=>{
            const urls = View3D.threeUrl
                ? [View3D.threeUrl]
                : [
                    // Global UMD builds (window.THREE)
                    'https://cdnjs.cloudflare.com/ajax/libs/three.js/r170/three.min.js',
                    'https://cdn.jsdelivr.net/npm/three@0.146.0/build/three.min.js',
                    'https://unpkg.com/three@0.146.0/build/three.min.js',
                ]

            const tryLoad = (index)=>{
                if(index >= urls.length){
                    reject(new Error('[View3D] Failed to load three.js from all configured CDNs'))
                    return
                }

                const url = urls[index]
                let script = document.querySelector(`script[src="${url}"]`)
                if(!script){
                    script = document.createElement('script')
                    script.src = url
                    script.async = true
                    document.head.appendChild(script)
                }

                const onLoad = ()=>{
                    const THREE = window.THREE
                    if(THREE && (THREE.Scene || THREE.WebGLRenderer)){
                        cleanup()
                        resolve(THREE)
                    } else {
                        cleanup()
                        tryLoad(index + 1)
                    }
                }
                const onError = ()=>{
                    cleanup()
                    tryLoad(index + 1)
                }
                const cleanup = ()=>{
                    script.removeEventListener('load', onLoad)
                    script.removeEventListener('error', onError)
                }

                script.addEventListener('load', onLoad)
                script.addEventListener('error', onError)

                // If already loaded, resolve immediately.
                if(script.readyState === 'complete' && window.THREE){
                    cleanup()
                    resolve(window.THREE)
                }
            }

            tryLoad(0)
        }).catch((err)=>{
            console.warn('[View3D] three.js load failed:', err)
            View3D.$threePromise = null // allow retry
            return null
        })

        return View3D.$threePromise.then((THREE)=>{ if(THREE) this.$onThreeLoaded(THREE); return THREE })
    }

    /**
     * JQML registers component classes on `window` using their QML names.
     * That can overwrite native globals like `Map` / `Set`.
     * Three.js relies on native constructors, so we restore them lazily here.
     */
    $ensureNativeGlobalsForThree(){
        if(typeof window === 'undefined') return

        if(!View3D.$nativeGlobals){
            View3D.$nativeGlobals = {}
            try {
                const frame = document.createElement('iframe')
                frame.style.display = 'none'
                document.documentElement.appendChild(frame)
                const w = frame.contentWindow
                if(w){
                    View3D.$nativeGlobals.Map = w.Map
                    View3D.$nativeGlobals.Set = w.Set
                    View3D.$nativeGlobals.Image = w.Image
                    View3D.$nativeGlobals.Text = w.Text
                    View3D.$nativeGlobals.Animation = w.Animation
                }
                frame.remove()
            } catch (_e) {
                // ignore; we'll just keep current globals
            }
        }

        const ng = View3D.$nativeGlobals || {}
        if(ng.Map && (!window.Map || window.Map.defaultProperties !== undefined)) window.Map = ng.Map
        if(ng.Set && (!window.Set || window.Set.defaultProperties !== undefined)) window.Set = ng.Set
        if(ng.Image && (!window.Image || window.Image.defaultProperties !== undefined)) window.Image = ng.Image
        if(ng.Text && (!window.Text || window.Text.defaultProperties !== undefined)) window.Text = ng.Text
        if(ng.Animation && (!window.Animation || window.Animation.defaultProperties !== undefined)) window.Animation = ng.Animation
    }

    $onThreeLoaded(THREE){
        if(this.$disposed || this.$THREE) return
        this.$ensureNativeGlobalsForThree()
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
        this.$environmentChanged()
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
        if(this.$boundEnvironment && this.$boundEnvironment !== env){
            this.$boundEnvironment.$onEnvChanged = null
            this.$boundEnvironment = null
        }
        if(env && typeof env.$applyToScene === 'function'){
            if(env !== this.$boundEnvironment){
                env.$onEnvChanged = ()=>{
                    if(this.$disposed || !this.$THREE || !this.$scene) return
                    env.$applyToScene(this.$THREE, this.$scene, this.$renderer)
                    this.$requestRender()
                }
                this.$boundEnvironment = env
            }
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

/**
 * Override this BEFORE the first View3D is created to use a local file
 * or a specific CDN version instead of the default jsdelivr CDN, e.g.:
 *   View3D.threeUrl = '/Resources/three.min.js'
 */
View3D.threeUrl = null

module.exports.View3D = View3D
