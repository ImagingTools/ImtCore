// Canvas on a native <canvas>, after qquickcanvasitem.cpp (Qt 6.8): available once the item is in a window (queued,
// QQuickCanvasItem::itemChange ItemSceneChange -> sceneGraphInitialized); paint(region) is emitted at polish for the
// dirty rect, painted() once the commands are on the canvas. The context is the DOM one with Qt's behaviour
// (Context2D.js).
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { Item } = require('./Item')
const { QtObject } = require('../Core/QtObject')
const { QSizeF, QRectF } = require('../Core/ValueTypes')
const { extendContext } = require('./Context2D')
const { resolveUrl } = require('../Core/Resources')

// QQuickPixmap cache: a url loaded once is ready for every canvas (loadImage of a ready url emits nothing)
const pixmaps = new Map()       // resolved url -> { image, status, waiting: Set<Canvas> }
const READY = 1, LOADING = 2, ERROR = 3

// QQuickCanvasItem::requestAnimationFrame: one counter for every canvas (static int id)
let nextCallbackId = 0

// the methods of a context that put something on the canvas (QQuickContext2DCommandBuffer not empty)
const DRAWING = ['fill', 'stroke', 'fillRect', 'strokeRect', 'clearRect', 'fillText', 'strokeText', 'drawImage', 'putImageData', 'reset']

function inWindow(item){
    let top = item
    while(top.$parent) top = top.$parent
    let scene = require('./PointerDelivery').scene
    return scene.roots.includes(top)
}

class Canvas extends Item {
    constructor(parent = null){
        super(null)
        this.$canvas = null
        this.$context = null
        this.$dirtyRect = null
        this.$hasCanvasSize = false
        this.$hasCanvasWindow = false
        this.$hasTileSize = false
        this.$images = new Map()        // resolved url -> pixmap entry of this canvas (loadImage / unloadImage)
        this.$callbacks = new Map()
        this.$drawn = false
        this.$sceneWatch = null
        this.$v[Canvas.CANVASSIZE] = new QSizeF(0, 0)
        this.$v[Canvas.TILESIZE] = new QSizeF(0, 0)
        this.$v[Canvas.CANVASWINDOW] = new QRectF(0, 0, 0, 0)
        if(this.$dom){
            let canvas = document.createElement('canvas')
            canvas.style.cssText = 'position:absolute;left:0;top:0;'
            this.$dom.appendChild(canvas)
            this.$canvas = canvas
        }
        if(parent) this.$setParent(parent)
        // ItemSceneChange: "sceneGraphInitialized" is invoked queued once the item is in a window
        setTimeout(() => this.$checkWindow(), 0)
    }

    $checkWindow(){
        if(this.$f & engine.DESTROYED || engine.isStopped() || this.$v[Canvas.AVAILABLE]) return
        if(inWindow(this)){
            this.$sceneGraphInitialized()
            return
        }
        // not in a window yet: the topmost ancestor going into one brings the scene change
        let top = this
        while(top.$parent) top = top.$parent
        if(this.$sceneWatch && this.$sceneWatch.item === top) return
        if(this.$sceneWatch) engine.unsubscribe(this.$sceneWatch.link)
        let link = engine.connect(top, QtObject.SIGNAL_parentChanged, () => setTimeout(() => this.$checkWindow(), 0))
        this.$sceneWatch = { item: top, link }
    }

    // QQuickCanvasItem::sceneGraphInitialized: available; the context of contextType is made (delayedCreate) and
    // painted, or a canvas with a paint handler is painted
    $sceneGraphInitialized(){
        if(this.$sceneWatch){
            engine.unsubscribe(this.$sceneWatch.link)
            this.$sceneWatch = null
        }
        engine.batch(() => {
            this.$write(Canvas.AVAILABLE, true)
            this.$resizeCanvas()
            let type = this.$v[Canvas.CONTEXTTYPE]
            if(type){
                if(!this.$context) this.$createContext(type)
                this.requestPaint()
            } else if(this.$isPaintConnected()){
                this.requestPaint()
            }
        })
    }

    $isPaintConnected(){
        return engine.hasSubscribers(this, Canvas.SIGNAL_paint)
    }

    $write(id, value){
        if(id === Canvas.CANVASSIZE && !this.$syncing){
            let old = this.$v[id]
            if(old && value && old.width === value.width && old.height === value.height) return
            this.$hasCanvasSize = true
        }
        if(id === Canvas.CANVASWINDOW && !this.$syncing) this.$hasCanvasWindow = true
        if(id === Canvas.TILESIZE && !this.$syncing) this.$hasTileSize = true
        let old = this.$v[id]
        super.$write(id, value)
        if((id === Item.WIDTH || id === Item.HEIGHT) && old !== this.$v[id]) this.$geometryChanged()
        if(id === Canvas.CANVASSIZE || id === Canvas.CANVASWINDOW || id === Canvas.TILESIZE){
            this.$resizeCanvas()
            if(this.$context) engine.queuePolish(this)
        }
        if(id === Item.VISIBLE && this.$v[id] && this.$callbacks.size) engine.queuePolish(this)
    }

    // QQuickCanvasItem::geometryChange: canvasSize, tileSize and canvasWindow follow the item unless set; a visible
    // available canvas of a new size is painted again
    $geometryChanged(){
        let w = this.$v[Item.WIDTH], h = this.$v[Item.HEIGHT]
        this.$syncing = true
        try {
            let size = this.$v[Canvas.CANVASSIZE]
            if(!this.$hasCanvasSize && (size.width !== w || size.height !== h)) this.$write(Canvas.CANVASSIZE, new QSizeF(w, h))
            let tile = this.$v[Canvas.TILESIZE]
            if(!this.$hasTileSize && (tile.width !== Math.trunc(w) || tile.height !== Math.trunc(h))) this.$write(Canvas.TILESIZE, new QSizeF(Math.trunc(w), Math.trunc(h)))
            let win = this.$v[Canvas.CANVASWINDOW]
            if(!this.$hasCanvasWindow && (win.x !== 0 || win.y !== 0 || win.width !== w || win.height !== h)) this.$write(Canvas.CANVASWINDOW, new QRectF(0, 0, w, h))
        } finally {
            this.$syncing = false
        }
        if(this.$v[Canvas.AVAILABLE] && this.$v[Item.VISIBLE]) this.requestPaint()
    }

    $canvasSizeValue(){ return this.$v[Canvas.CANVASSIZE] }

    $ratio(){ return (typeof window !== 'undefined' && window.devicePixelRatio) || 1 }

    // a new backing store (QQuickContext2DTexture::canvasChanged: a new image); the state of the context stays
    $resizeCanvas(){
        let canvas = this.$canvas
        if(!canvas) return
        let size = this.$v[Canvas.CANVASSIZE]
        let ratio = this.$ratio()
        let w = Math.max(0, Math.round(size.width * ratio)), h = Math.max(0, Math.round(size.height * ratio))
        if(canvas.width === w && canvas.height === h) return
        let ctx = this.$context
        let current = ctx ? ctx.$snapshot() : null
        canvas.width = w
        canvas.height = h
        canvas.style.width = size.width + 'px'
        canvas.style.height = size.height + 'px'
        if(!ctx) return
        // the DOM state went with the old backing store: the saved states, then the current one
        let save = Object.getPrototypeOf(ctx).save
        for(let state of ctx.$stack){
            ctx.$apply(state)
            save.call(ctx)
        }
        ctx.$apply(current)
    }

    // QQuickCanvasItem::getContext: the context of the type, made if there is none; another type: a warning
    getContext(type){
        if(typeof type !== 'string'){
            console.warn('JQML: Canvas: getContext should be called with a string naming the required context type')
            return null
        }
        if(!this.$v[Canvas.AVAILABLE]){
            console.warn('JQML: Canvas: Unable to use getContext() at this time, please wait for available: true')
            return null
        }
        if(this.$context){
            if(type.toLowerCase() === '2d') return this.$context
            console.warn('JQML: Canvas: Canvas already initialized with a different context type')
            return null
        }
        return this.$createContext(type) ? this.$context : null
    }

    // QQuickCanvasItem::createContext: "2d" only (case-sensitive), in a window
    $createContext(type){
        if(type !== '2d' || !this.$canvas) return false
        if(String(this.$v[Canvas.CONTEXTTYPE]).toLowerCase() !== '2d'){
            this.$settingType = true
            this.$write(Canvas.CONTEXTTYPE, '2d')
            this.$settingType = false
        }
        let ctx = this.$canvas.getContext('2d')
        let ratio = this.$ratio()
        ctx.setTransform(ratio, 0, 0, ratio, 0, 0)
        extendContext(ctx, this, () => this.$ratio())
        // the commands put on the canvas: painted() after the polish (textureChanged)
        for(let name of DRAWING){
            let fn = ctx[name]
            if(typeof fn !== 'function') continue
            let item = this
            ctx[name] = function(...args){
                item.$drawn = true
                return fn.apply(this, args)
            }
        }
        this.$context = ctx
        this.$write(Canvas.CONTEXT, ctx)
        return true
    }

    // an image of drawImage / createPattern: a url (loaded by loadImage, an unloaded one starts loading), an Image item,
    // another Canvas, a CanvasImageData
    $imageSource(source){
        if(source == null) return null
        if(typeof source === 'string' || (typeof source === 'object' && source.constructor && source.constructor.name === 'URL')){
            let url = resolveUrl(String(source))
            let entry = this.$images.get(url)
            if(!entry){
                this.loadImage(String(source))
                entry = this.$images.get(url)
            }
            return entry && entry.status === READY ? entry.image : null
        }
        if(source instanceof Canvas) return source.$canvas
        if(source instanceof Item){
            // QQuickImage: its pixmap, by its url
            let url = source.$url
            return url ? this.$imageSource(url) : null
        }
        if(typeof ImageData !== 'undefined' && source instanceof ImageData && typeof document !== 'undefined'){
            let scratch = document.createElement('canvas')
            scratch.width = source.width
            scratch.height = source.height
            scratch.getContext('2d').putImageData(source, 0, 0)
            return scratch
        }
        return source
    }

    // QQuickCanvasItem::requestPaint: the canvas window is dirty
    requestPaint(){
        let w = this.$v[Canvas.CANVASWINDOW]
        this.markDirty(new QRectF(w.x, w.y, w.width, w.height))
    }

    markDirty(rect){
        if(!this.$v[Canvas.AVAILABLE]) return
        let d = this.$dirtyRect
        if(!d) this.$dirtyRect = new QRectF(rect.x, rect.y, rect.width, rect.height)
        else {
            let x = Math.min(d.x, rect.x), y = Math.min(d.y, rect.y)
            this.$dirtyRect = new QRectF(x, y, Math.max(d.x + d.width, rect.x + rect.width) - x, Math.max(d.y + d.height, rect.y + rect.height) - y)
        }
        engine.queuePolish(this)
    }

    // QQuickCanvasItem::updatePolish: the animation callbacks of a visible canvas, or else paint(dirty rect); then the
    // context flushes, and painted() follows when there were commands
    $polish(){
        if(this.$callbacks.size && this.$v[Item.VISIBLE]){
            let callbacks = this.$callbacks
            this.$callbacks = new Map()
            for(let fn of callbacks.values()) fn.call(this, Date.now())
        } else if(this.$dirtyRect){
            let region = this.$dirtyRect
            this.$dirtyRect = null
            let r = new QRectF(Math.trunc(region.x), Math.trunc(region.y), Math.trunc(region.width), Math.trunc(region.height))
            engine.emitArgs(this, Canvas.SIGNAL_paint, [r])
        }
        if(this.$context && this.$drawn){
            this.$drawn = false
            engine.emit(this, Canvas.SIGNAL_painted)
        }
    }

    requestAnimationFrame(callback){
        if(typeof callback !== 'function'){
            console.warn('JQML: Canvas: requestAnimationFrame should be called with an animation callback function')
            return null
        }
        let id = ++nextCallbackId
        this.$callbacks.set(id, callback)
        // QTBUG-55778: posted, not a polish from here; a hidden canvas waits for visibleChanged
        if(this.$v[Item.VISIBLE]) setTimeout(() => { if(!(this.$f & engine.DESTROYED)) engine.batch(() => engine.queuePolish(this)) }, 0)
        return id
    }

    cancelRequestAnimationFrame(id){
        if(!Number.isInteger(id)){
            console.warn('JQML: Canvas: cancelRequestAnimationFrame should be called with an animation callback id')
            return null
        }
        this.$callbacks.delete(id)
    }

    // QQuickCanvasItem::loadImage: asynchronous, through the pixmap cache; imageLoaded() when one was loading
    loadImage(url){
        let resolved = resolveUrl(String(url))
        if(this.$images.has(resolved)) return
        let entry = pixmaps.get(resolved)
        if(!entry){
            entry = { image: null, status: LOADING, waiting: new Set() }
            pixmaps.set(resolved, entry)
            if(typeof Image !== 'undefined'){
                let image = new Image()
                entry.image = image
                let done = (status) => {
                    entry.status = status
                    let waiting = Array.from(entry.waiting)
                    entry.waiting.clear()
                    if(engine.isStopped()) return
                    engine.batch(() => {
                        for(let canvas of waiting){
                            if(!(canvas.$f & engine.DESTROYED) && canvas.$images.get(resolved) === entry) engine.emit(canvas, Canvas.SIGNAL_imageLoaded)
                        }
                    })
                }
                image.onload = () => done(READY)
                image.onerror = () => done(ERROR)
                image.src = resolved
            } else {
                entry.status = ERROR
            }
        }
        this.$images.set(resolved, entry)
        if(entry.status === LOADING) entry.waiting.add(this)
    }

    unloadImage(url){ this.$images.delete(resolveUrl(String(url))) }
    isImageLoaded(url){ let e = this.$images.get(resolveUrl(String(url))); return !!e && e.status === READY }
    isImageLoading(url){ let e = this.$images.get(resolveUrl(String(url))); return !!e && e.status === LOADING }
    isImageError(url){ let e = this.$images.get(resolveUrl(String(url))); return !!e && e.status === ERROR }

    // QQuickCanvasItem::toDataURL: image/png by default; another image/ type the browser cannot write, or no context: "data:,"
    toDataURL(mimeType){
        let mime = String(mimeType === undefined ? 'image/png' : mimeType).toLowerCase()
        if(!this.$context || !this.$canvas || !mime.startsWith('image/')) return 'data:,'
        let ext = mime.slice(6)
        if(ext !== 'png' && ext !== 'jpeg') return 'data:,'
        return this.$canvas.toDataURL(mime)
    }

    // QQuickCanvasItem::save writes a file: a page has no file system
    save(){ return false }

    $disposeNative(){
        if(this.$sceneWatch){
            engine.unsubscribe(this.$sceneWatch.link)
            this.$sceneWatch = null
        }
        for(let entry of this.$images.values()) entry.waiting && entry.waiting.delete(this)
        super.$disposeNative()
    }
}

defineType(Canvas, {
    props: {
        available: { kind: 'bool', readonly: true },
        canvasSize: 'var',
        canvasWindow: 'var',
        tileSize: 'var',
        context: { kind: 'var', readonly: true },
        contextType: {
            kind: 'string',
            // QQuickCanvasItem::setContextType: made at once when available; a canvas with another context keeps it
            effect(canvas, value, old){
                if(canvas.$settingType) return
                if(canvas.$context){
                    console.warn('JQML: Canvas: Canvas already initialized with a different context type')
                    canvas.$v[Canvas.CONTEXTTYPE] = old
                    return
                }
                if(canvas.$v[Canvas.AVAILABLE] && value) canvas.$createContext(value)
            },
        },
        renderStrategy: 'int',
        renderTarget: 'int',
    },
    signals: ['paint(region)', 'painted', 'imageLoaded'],
})
Object.assign(Canvas, { Immediate: 0, Threaded: 1, Cooperative: 2, Image: 0, FramebufferObject: 1 })

module.exports = { Canvas, pixmaps }
