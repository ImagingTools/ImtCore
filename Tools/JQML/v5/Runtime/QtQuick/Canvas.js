// Canvas on a native <canvas>, after qquickcanvasitem.cpp: available once the item is in a window
// (queued), paint(region) is emitted at polish for the dirty rect, painted after it.
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { Item } = require('./Item')
const { toCss } = require('../Core/color')
const { QSizeF, QRectF } = require('../Core/ValueTypes')

const PROTO = typeof CanvasRenderingContext2D !== 'undefined' ? CanvasRenderingContext2D.prototype : null

// Qt colors are #AARRGGBB; CSS wants rgba() for them
function styleSetter(name){
    let native = PROTO && Object.getOwnPropertyDescriptor(PROTO, name)
    return {
        get(){ return native.get.call(this) },
        set(value){ native.set.call(this, typeof value === 'string' && value[0] === '#' ? toCss(value) : value) },
        configurable: true,
    }
}

class Canvas extends Item {
    constructor(parent = null){
        super(null)
        this.$canvas = null
        this.$context = null
        this.$dirtyRect = null
        this.$hasCanvasSize = false
        this.$images = new Map()        // url -> { image, status }
        this.$callbacks = new Map()
        this.$nextCallback = 1
        this.$v[Canvas.CANVASSIZE] = new QSizeF(0, 0)
        if(this.$dom){
            let canvas = document.createElement('canvas')
            canvas.style.cssText = 'position:absolute;left:0;top:0;'
            this.$dom.appendChild(canvas)
            this.$canvas = canvas
        }
        if(parent) this.$setParent(parent)
        // "sceneGraphInitialized" comes queued after the item enters a window
        setTimeout(() => this.$sceneGraphInitialized(), 0)
    }

    $sceneGraphInitialized(){
        if(this.$f & engine.DESTROYED || engine.isStopped()) return
        engine.batch(() => {
            this.$write(Canvas.AVAILABLE, true)
            this.$resizeCanvas()
            let type = this.$v[Canvas.CONTEXTTYPE]
            if(type) this.getContext(type)
            if(type || this.$isPaintConnected()) this.requestPaint()
        })
    }

    $isPaintConnected(){
        return engine.hasSubscribers(this, Canvas.SIGNAL_paint)
    }

    $write(id, value){
        if(id === Canvas.CANVASSIZE) this.$hasCanvasSize = true
        super.$write(id, value)
        if(id === Item.WIDTH || id === Item.HEIGHT) this.$geometryChanged()
        if(id === Canvas.CANVASSIZE) this.$resizeCanvas()
    }

    $geometryChanged(){
        let size = this.$v[Canvas.CANVASSIZE]
        let w = this.$v[Item.WIDTH], h = this.$v[Item.HEIGHT]
        if(!this.$hasCanvasSize && (!size || size.width !== w || size.height !== h)){
            this.$v[Canvas.CANVASSIZE] = new QSizeF(w, h)
            engine.emit(this, Canvas.SIGNAL_canvasSizeChanged)
            this.$resizeCanvas()
        }
        if(this.$v[Canvas.AVAILABLE] && this.$v[Item.VISIBLE]) this.requestPaint()
    }

    $resizeCanvas(){
        let canvas = this.$canvas
        if(!canvas) return
        let size = this.$v[Canvas.CANVASSIZE]
        let ratio = (typeof window !== 'undefined' && window.devicePixelRatio) || 1
        let w = Math.max(0, Math.round(size.width * ratio)), h = Math.max(0, Math.round(size.height * ratio))
        if(canvas.width === w && canvas.height === h) return
        canvas.width = w
        canvas.height = h
        canvas.style.width = size.width + 'px'
        canvas.style.height = size.height + 'px'
        if(this.$context) this.$context.setTransform(ratio, 0, 0, ratio, 0, 0)
    }

    getContext(type){
        if(!this.$v[Canvas.AVAILABLE]){
            console.warn('Unable to use getContext() at this time, please wait for available: true')
            return null
        }
        if(String(type).toLowerCase() !== '2d') return null
        if(!this.$context && this.$canvas){
            let ctx = this.$canvas.getContext('2d')
            let ratio = (typeof window !== 'undefined' && window.devicePixelRatio) || 1
            ctx.setTransform(ratio, 0, 0, ratio, 0, 0)
            this.$extend(ctx)
            this.$context = ctx
            this.$write(Canvas.CONTEXT, ctx)
        }
        return this.$context
    }

    // Context2D differences from the DOM context
    $extend(ctx){
        let item = this
        for(let name of ['fillStyle', 'strokeStyle', 'shadowColor']) Object.defineProperty(ctx, name, styleSetter(name))
        Object.defineProperty(ctx, 'canvas', { get(){ return item }, configurable: true })
        let ellipse = ctx.ellipse, drawImage = ctx.drawImage, createPattern = ctx.createPattern
        // ellipse(x, y, w, h): "an ellipse within the bounding rectangle", a closed subpath
        ctx.ellipse = function(...args){
            if(args.length === 4){
                let [x, y, w, h] = args
                this.moveTo(x + w, y + h / 2)
                ellipse.call(this, x + w / 2, y + h / 2, Math.abs(w / 2), Math.abs(h / 2), 0, 0, Math.PI * 2)
                this.closePath()
                return
            }
            ellipse.apply(this, args)
        }
        ctx.roundedRect = function(x, y, w, h, xr, yr){
            this.roundRect(x, y, w, h, [{ x: xr, y: yr === undefined ? xr : yr }])
        }
        // a url draws once loadImage() has it; an unloaded url is loaded
        let resolve = (source) => {
            if(typeof source === 'string'){
                let entry = item.$images.get(source)
                if(!entry){
                    item.loadImage(source)
                    return null
                }
                return entry.status === 1 ? entry.image : null
            }
            if(source instanceof Canvas) return source.$canvas
            if(source && source.$dom && source.$dom.querySelector){
                let img = source.$dom.querySelector('img')
                return img || null
            }
            return source
        }
        ctx.drawImage = function(source, ...args){
            let image = resolve(source)
            if(image) drawImage.call(this, image, ...args)
        }
        ctx.createPattern = function(source, repetition){
            let image = resolve(source)
            return image ? createPattern.call(this, image, repetition || 'repeat') : null
        }
    }

    requestPaint(){
        this.markDirty(new QRectF(0, 0, this.$v[Canvas.CANVASSIZE].width, this.$v[Canvas.CANVASSIZE].height))
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

    $polish(){
        if(this.$callbacks.size && this.$v[Item.VISIBLE]){
            let callbacks = this.$callbacks
            this.$callbacks = new Map()
            for(let fn of callbacks.values()) fn.call(this, Date.now())
        } else if(this.$dirtyRect){
            let region = this.$dirtyRect
            this.$dirtyRect = null
            engine.emitArgs(this, Canvas.SIGNAL_paint, [region])
            engine.emit(this, Canvas.SIGNAL_painted)
        }
    }

    requestAnimationFrame(callback){
        let id = this.$nextCallback++
        this.$callbacks.set(id, callback)
        engine.queuePolish(this)
        return id
    }

    cancelRequestAnimationFrame(id){
        this.$callbacks.delete(id)
    }

    loadImage(url){
        url = String(url)
        if(this.$images.has(url) || typeof Image === 'undefined') return
        let image = new Image()
        let entry = { image, status: 2 }
        this.$images.set(url, entry)
        image.onload = () => {
            entry.status = 1
            if(!(this.$f & engine.DESTROYED) && !engine.isStopped()) engine.batch(() => engine.emit(this, Canvas.SIGNAL_imageLoaded))
        }
        image.onerror = () => {
            entry.status = 3
            if(!(this.$f & engine.DESTROYED) && !engine.isStopped()) engine.batch(() => engine.emit(this, Canvas.SIGNAL_imageLoaded))
        }
        image.src = url
    }

    unloadImage(url){ this.$images.delete(String(url)) }
    isImageLoaded(url){ let e = this.$images.get(String(url)); return !!e && e.status === 1 }
    isImageLoading(url){ let e = this.$images.get(String(url)); return !!e && e.status === 2 }
    isImageError(url){ let e = this.$images.get(String(url)); return !!e && e.status === 3 }

    toDataURL(type){
        return this.$canvas && this.$v[Canvas.AVAILABLE] ? this.$canvas.toDataURL(type || 'image/png') : 'data:,'
    }

    save(){ return false }
}

defineType(Canvas, {
    props: {
        available: { kind: 'bool', readonly: true },
        canvasSize: 'var',
        canvasWindow: 'var',
        tileSize: 'var',
        context: { kind: 'var', readonly: true },
        contextType: 'string',
        renderStrategy: 'int',
        renderTarget: 'int',
    },
    signals: ['paint(region)', 'painted', 'imageLoaded'],
})
Object.assign(Canvas, { Immediate: 0, Threaded: 1, Cooperative: 2, Image: 0, FramebufferObject: 1 })

module.exports = { Canvas }
