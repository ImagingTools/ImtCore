// The Context2D of a Canvas, after qquickcontext2d.cpp (Qt 6.8), on the DOM CanvasRenderingContext2D of the canvas.
// What Qt does differently from the DOM context:
// - the methods that draw, build the path or change the state return the context (RETURN_RESULT(*thisObject));
// - colors are Qt colors (#AARRGGBB), in the styles and in the stops of a gradient (qt_color_from_string);
// - the coordinates are the canvas item's: the device pixel ratio of the window is applied under the transform
//   (setTransform, resetTransform, reset, isPointInPath work in item coordinates);
// - fillRule is part of the state (Qt.WindingFill by default), used by fill(), clip() and isPointInPath();
// - shear(), createConicalGradient(), roundedRect(), ellipse(x, y, w, h), text(), the path property;
// - drawImage() and createImageData() take a url, an Image item, another Canvas, a CanvasImageData;
// - drawFocusRing(), setCaretSelectionRect(), caretBlinkRate() throw NOT_SUPPORTED_ERR;
// - the state survives a new canvas size: a new DOM backing store resets the DOM state, Qt keeps it
//   (QQuickContext2DTexture makes a new image, the QQuickContext2D state stays).
const { toCss } = require('../Core/color')

const ODD_EVEN_FILL = 0, WINDING_FILL = 1

// QQuickJSContext2DPrototype methods ending with RETURN_RESULT(*thisObject)
const RETURNS_CONTEXT = ['arc', 'arcTo', 'beginPath', 'bezierCurveTo', 'clearRect', 'closePath', 'fillRect', 'fillText',
    'lineTo', 'moveTo', 'putImageData', 'quadraticCurveTo', 'rect', 'rotate', 'scale', 'stroke', 'strokeRect',
    'strokeText', 'transform', 'translate']

// QQuickContext2D::State, besides the transform, the clip and fillRule
const STATE = ['fillStyle', 'strokeStyle', 'globalAlpha', 'globalCompositeOperation', 'lineWidth', 'lineCap', 'lineJoin',
    'miterLimit', 'lineDashOffset', 'shadowOffsetX', 'shadowOffsetY', 'shadowBlur', 'shadowColor', 'font', 'textAlign',
    'textBaseline']
const DEFAULTS = { fillStyle: '#000000', strokeStyle: '#000000', globalAlpha: 1, globalCompositeOperation: 'source-over',
    lineWidth: 1, lineCap: 'butt', lineJoin: 'miter', miterLimit: 10, lineDashOffset: 0, shadowOffsetX: 0,
    shadowOffsetY: 0, shadowBlur: 0, shadowColor: 'rgba(0, 0, 0, 0)', font: '10px sans-serif', textAlign: 'start',
    textBaseline: 'alphabetic' }

// THROW_DOM of qquickcontext2d.cpp: an Error (newErrorObject) with the DOM exception code
function domException(code, name, message){
    let error = new Error(message)
    error.code = code
    return error
}
const NOT_SUPPORTED_ERR = 9, SYNTAX_ERR = 12, INDEX_SIZE_ERR = 1

// a Qt color string for the DOM: #AARRGGBB to rgba(); a QML color value is a string already
function cssColor(value){
    return typeof value === 'string' && value[0] === '#' ? toCss(value) : value
}

function isFiniteNumber(v){ return typeof v === 'number' && Number.isFinite(v) }

function nativeDescriptor(proto, name){
    for(let p = proto; p; p = Object.getPrototypeOf(p)){
        let d = Object.getOwnPropertyDescriptor(p, name)
        if(d) return d
    }
    return null
}

// gradient.addColorStop with a Qt color (QQuickContext2DStyle::gradient_proto_addColorStop); a conical gradient
// turns counter-clockwise, the DOM conic one clockwise: its stops are mirrored
function qtGradient(gradient, mirrored){
    if(!gradient) return gradient
    let add = gradient.addColorStop
    gradient.addColorStop = function(offset, color){
        if(!(offset >= 0 && offset <= 1)) throw domException(INDEX_SIZE_ERR, 'IndexSizeError', 'CanvasGradient: parameter offset out of range')
        try {
            add.call(this, mirrored ? 1 - offset : offset, cssColor(color))
        } catch(e) {
            throw domException(SYNTAX_ERR, 'SyntaxError', 'CanvasGradient: parameter color is not a valid color string')
        }
    }
    return gradient
}

function extendContext(ctx, item, ratioOf){
    let proto = Object.getPrototypeOf(ctx)
    let native = {}
    for(let name of ['save', 'restore', 'reset', 'setTransform', 'resetTransform', 'fill', 'clip', 'stroke', 'beginPath',
        'isPointInPath', 'drawImage', 'createPattern', 'createImageData', 'ellipse', 'roundRect', 'createLinearGradient',
        'createRadialGradient', 'createConicGradient', 'transform', 'getTransform', 'setLineDash', 'getLineDash',
        'fillText', 'strokeText', 'clearRect']){
        if(typeof ctx[name] === 'function') native[name] = ctx[name]
    }
    let props = {}
    for(let name of STATE) props[name] = nativeDescriptor(proto, name)

    // the state Qt keeps besides the DOM's: fillRule; the stack, for a new backing store
    ctx.$fillRule = WINDING_FILL
    ctx.$stack = []
    ctx.$texts = []         // text() of the current path: filled and stroked as text
    ctx.$path = null        // the path property: a Path2D the current path starts with
    ctx.$pathValue = null

    let rule = () => ctx.$fillRule === ODD_EVEN_FILL ? 'evenodd' : 'nonzero'
    let returnsContext = (name, fn) => { ctx[name] = function(...args){ fn.apply(this, args); return this } }

    for(let name of RETURNS_CONTEXT){
        let fn = ctx[name]
        if(typeof fn === 'function') returnsContext(name, fn)
    }

    // QQuickJSContext2DPrototype::method_arc / method_arcTo: a finite negative radius is refused
    for(let [name, at] of [['arc', 2], ['arcTo', 4]]){
        let fn = ctx[name]
        if(typeof fn !== 'function') continue
        ctx[name] = function(...args){
            let radius = Number(args[at])
            if(Number.isFinite(radius) && radius < 0) throw domException(INDEX_SIZE_ERR, 'IndexSizeError', 'Incorrect argument radius')
            return fn.apply(this, args)
        }
    }

    // ---- state ----
    Object.defineProperty(ctx, 'canvas', { get(){ return item }, configurable: true })
    for(let name of ['fillStyle', 'strokeStyle', 'shadowColor']){
        let d = props[name]
        Object.defineProperty(ctx, name, {
            get(){ return d.get.call(this) },
            set(value){ d.set.call(this, cssColor(value)) },
            configurable: true,
        })
    }
    Object.defineProperty(ctx, 'fillRule', {
        get(){ return this.$fillRule },
        set(value){
            if(value === 'WindingFill' || value === WINDING_FILL) this.$fillRule = WINDING_FILL
            else if(value === 'OddEvenFill' || value === ODD_EVEN_FILL) this.$fillRule = ODD_EVEN_FILL
        },
        configurable: true,
    })

    let snapshot = () => {
        let s = {}
        for(let name of STATE) s[name] = props[name].get.call(ctx)
        s.lineDash = native.getLineDash ? native.getLineDash.call(ctx) : []
        s.transform = native.getTransform ? native.getTransform.call(ctx) : null
        s.fillRule = ctx.$fillRule
        return s
    }
    let apply = (s) => {
        for(let name of STATE) props[name].set.call(ctx, s[name])
        if(native.setLineDash) native.setLineDash.call(ctx, s.lineDash)
        if(s.transform) native.setTransform.call(ctx, s.transform)
        ctx.$fillRule = s.fillRule
    }
    ctx.$snapshot = snapshot
    ctx.$apply = apply

    returnsContext('save', function(){
        this.$stack.push(snapshot())
        native.save.call(this)
    })
    returnsContext('restore', function(){
        if(!this.$stack.length) return
        let s = this.$stack.pop()
        native.restore.call(this)
        this.$fillRule = s.fillRule
    })
    // QQuickContext2D::reset: a new state, the path and the stack cleared, the canvas cleared
    returnsContext('reset', function(){
        while(this.$stack.length){
            this.$stack.pop()
            native.restore.call(this)
        }
        let r = ratioOf()
        native.setTransform.call(this, r, 0, 0, r, 0, 0)
        for(let name of STATE) props[name].set.call(this, DEFAULTS[name])
        if(native.setLineDash) native.setLineDash.call(this, [])
        this.$fillRule = WINDING_FILL
        this.beginPath()
        let size = item.$canvasSizeValue()
        native.clearRect.call(this, 0, 0, size.width, size.height)
    })

    // ---- transforms: the item's coordinates, the device pixel ratio under them ----
    returnsContext('setTransform', function(a, b, c, d, e, f){
        if(![a, b, c, d, e, f].every(isFiniteNumber)) return
        let r = ratioOf()
        native.setTransform.call(this, r * a, r * b, r * c, r * d, r * e, r * f)
    })
    returnsContext('resetTransform', function(){
        let r = ratioOf()
        native.setTransform.call(this, r, 0, 0, r, 0, 0)
    })
    // QTransform::shear(sh, sv): m21 += sh * m11 ... as the matrix [1 sv; sh 1]
    returnsContext('shear', function(h, v){
        if(!isFiniteNumber(h) || !isFiniteNumber(v)) return
        native.transform.call(this, 1, v, h, 1, 0, 0)
    })

    // ---- the path ----
    returnsContext('beginPath', function(){
        this.$texts = []
        this.$path = null
        native.beginPath.call(this)
    })
    // QPainterPath::addEllipse: a closed subpath starting at the right end
    returnsContext('ellipse', function(...args){
        if(args.length === 4){
            let [x, y, w, h] = args
            if(![x, y, w, h].every(isFiniteNumber)) return
            if(!w && !h){
                this.moveTo(x, y)
                return
            }
            this.moveTo(x + w, y + h / 2)
            native.ellipse.call(this, x + w / 2, y + h / 2, Math.abs(w / 2), Math.abs(h / 2), 0, 0, Math.PI * 2)
            this.closePath()
            return
        }
        native.ellipse.apply(this, args)
    })
    // QPainterPath::addRoundedRect(rect, xr, yr, Qt::AbsoluteSize)
    returnsContext('roundedRect', function(x, y, w, h, xr, yr){
        if(![x, y, w, h, xr, yr].every(isFiniteNumber)) return
        if(!w && !h){
            this.moveTo(x, y)
            return
        }
        let rx = Math.min(Math.abs(xr), Math.abs(w) / 2), ry = Math.min(Math.abs(yr), Math.abs(h) / 2)
        native.roundRect.call(this, x, y, w, h, [{ x: rx, y: ry }])
    })
    // QPainterPath::addText: the glyphs at the left end of the baseline, in the state's font; filled and stroked
    // with the path
    returnsContext('text', function(text, x, y){
        if(!isFiniteNumber(x) || !isFiniteNumber(y)) return
        this.$texts.push({ text: String(text), x, y, transform: native.getTransform.call(this) })
    })
    let drawTexts = (method) => {
        if(!ctx.$texts.length) return
        let transform = native.getTransform.call(ctx)
        let align = props.textAlign.get.call(ctx), baseline = props.textBaseline.get.call(ctx)
        props.textAlign.set.call(ctx, 'left')
        props.textBaseline.set.call(ctx, 'alphabetic')
        for(let t of ctx.$texts){
            native.setTransform.call(ctx, t.transform)
            method.call(ctx, t.text, t.x, t.y)
        }
        native.setTransform.call(ctx, transform)
        props.textAlign.set.call(ctx, align)
        props.textBaseline.set.call(ctx, baseline)
    }
    returnsContext('fill', function(){
        if(this.$path) native.fill.call(this, this.$path, rule())
        native.fill.call(this, rule())
        drawTexts(native.fillText)
    })
    returnsContext('stroke', function(){
        if(this.$path) native.stroke.call(this, this.$path)
        native.stroke.call(this)
        drawTexts(native.strokeText)
    })
    returnsContext('clip', function(){
        native.clip.call(this, rule())
    })
    // QQuickContext2D::isPointInPath: (x, y) in the item's coordinates, the path with the state's fill rule
    ctx.isPointInPath = function(x, y){
        if(!isFiniteNumber(x) || !isFiniteNumber(y)) return false
        let r = ratioOf()
        let inside = native.isPointInPath.call(this, x * r, y * r, rule())
        if(!inside && this.$path) inside = native.isPointInPath.call(this, this.$path, x * r, y * r, rule())
        return inside
    }
    // the path: an SVG path string (QQuickSvgParser::parsePathDataFast); setting it begins a new path
    Object.defineProperty(ctx, 'path', {
        get(){ return this.$pathValue },
        set(value){
            this.beginPath()
            this.$pathValue = value
            if(typeof Path2D !== 'undefined' && typeof value === 'string') this.$path = new Path2D(value)
            else if(value && typeof value.$svgPath === 'function' && typeof Path2D !== 'undefined') this.$path = new Path2D(value.$svgPath())
        },
        configurable: true,
    })

    // ---- styles ----
    ctx.createLinearGradient = function(...args){
        if(args.length < 4) return this
        return qtGradient(native.createLinearGradient.apply(this, args), false)
    }
    ctx.createRadialGradient = function(...args){
        if(args.length < 6) return this
        return qtGradient(native.createRadialGradient.apply(this, args), false)
    }
    // QConicalGradient(x, y, angle): counter-clockwise from angle
    ctx.createConicalGradient = function(x, y, angle){
        if(arguments.length < 3) return this
        if(!isFiniteNumber(x) || !isFiniteNumber(y)) throw domException(NOT_SUPPORTED_ERR, 'NotSupportedError', 'createConicalGradient(): Incorrect arguments')
        if(!isFiniteNumber(angle)) throw domException(INDEX_SIZE_ERR, 'IndexSizeError', 'createConicalGradient(): Incorrect arguments')
        if(!native.createConicGradient) return null
        return qtGradient(native.createConicGradient.call(this, -angle, x, y), true)
    }

    // ---- images ----
    let resolve = (source) => item.$imageSource(source)
    returnsContext('drawImage', function(source, ...args){
        let image = resolve(source)
        if(image) native.drawImage.call(this, image, ...args)
    })
    ctx.createPattern = function(source, repetition){
        let image = resolve(source)
        return image ? native.createPattern.call(this, image, repetition || 'repeat') : null
    }
    // createImageData(sw, sh), (imageData), (url): the last one has the pixels of the loaded image
    ctx.createImageData = function(...args){
        if(args.length === 1 && typeof args[0] === 'string'){
            let image = resolve(args[0])
            if(!image) return native.createImageData.call(this, 1, 1)
            let w = image.naturalWidth || image.width, h = image.naturalHeight || image.height
            let scratch = typeof document !== 'undefined' ? document.createElement('canvas') : null
            if(!scratch || !w || !h) return native.createImageData.call(this, Math.max(1, w), Math.max(1, h))
            scratch.width = w
            scratch.height = h
            let c = scratch.getContext('2d')
            c.drawImage(image, 0, 0)
            return c.getImageData(0, 0, w, h)
        }
        return native.createImageData.apply(this, args)
    }

    for(let name of ['drawFocusRing', 'setCaretSelectionRect', 'caretBlinkRate']){
        ctx[name] = function(){ throw domException(NOT_SUPPORTED_ERR, 'NotSupportedError', 'Context2D::' + name + ' is not supported') }
    }
    return ctx
}

module.exports = { extendContext, cssColor, ODD_EVEN_FILL, WINDING_FILL }
