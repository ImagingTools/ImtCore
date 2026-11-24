// QtQuick.Shapes (qquickshape.cpp, Qt 6.12): a Shape draws its ShapePaths, each a Path filled and stroked. Here the
// Shape is an SVG element in its item: a ShapePath is an SVG path, its gradient an SVG gradient in the coordinates of
// the shape (a ConicalGradient, which SVG has not, a CSS conic-gradient clipped to the path). The implicit size of the
// Shape is the bottom right of what its paths cover (QQuickShape: setImplicitSize(br.right(), br.bottom())).
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { QtObject } = require('../Core/QtObject')
const { toCss, parseColor } = require('../Core/color')
const { QRectF } = require('../Core/ValueTypes')
const alphaOf = (color) => { let c = parseColor(color); return c ? c.a : 0 }
const { Item, bits } = require('./Item')
const { Path } = require('./Path')
const { GradientStop } = require('./Gradient')

const STYLE = bits.STYLE
let ids = 0

// the shapes drawing a path or a gradient are drawn again
function shapesOf(object){
    let out = []
    for(let it = object; it; it = it.$parent){
        if(it instanceof Shape){
            out.push(it)
            break
        }
    }
    return out
}
const redraw = (object) => {
    for(let shape of shapesOf(object)) shape.$invalidate()
}

// ---- gradients ----

class ShapeGradient extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$users = new Set()     // the ShapePaths filled with it
        this.$id = 'jq-sg-' + (++ids)
    }

    $stops(){
        let list = this.$v[ShapeGradient.STOPS]
        let stops = Array.isArray(list) ? list : (this.$children || []).filter(child => child instanceof GradientStop)
        return stops.slice().sort((a, b) => a.$v[GradientStop.POSITION] - b.$v[GradientStop.POSITION])
    }

    $changed(){
        for(let path of this.$users) redraw(path)
    }

    $stopsSvg(){
        return this.$stops().map(stop => '<stop offset="' + Math.max(0, Math.min(1, stop.$v[GradientStop.POSITION])) + '" stop-color="' + toCss(stop.$v[GradientStop.COLOR]) + '"/>').join('')
    }

    $spread(){
        return ['pad', 'reflect', 'repeat'][this.$v[ShapeGradient.SPREAD]] || 'pad'
    }
}
const gradientChanged = (gradient) => gradient.$changed()
defineType(ShapeGradient, {
    props: {
        spread: { kind: 'int', effect: gradientChanged },
        stops: { kind: 'var', initial: null, effect: gradientChanged },
    },
})
Object.assign(ShapeGradient, { PadSpread: 0, ReflectSpread: 1, RepeatSpread: 2 })

class LinearGradient extends ShapeGradient {
    $def(){
        let v = this.$v, C = LinearGradient
        return '<linearGradient id="' + this.$id + '" gradientUnits="userSpaceOnUse" spreadMethod="' + this.$spread() + '" x1="' + v[C.X1] + '" y1="' + v[C.Y1]
            + '" x2="' + v[C.X2] + '" y2="' + v[C.Y2] + '">' + this.$stopsSvg() + '</linearGradient>'
    }
}
defineType(LinearGradient, {
    props: {
        x1: { kind: 'real', effect: gradientChanged }, y1: { kind: 'real', effect: gradientChanged },
        x2: { kind: 'real', effect: gradientChanged }, y2: { kind: 'real', effect: gradientChanged },
    },
})

class RadialGradient extends ShapeGradient {
    $def(){
        let v = this.$v, C = RadialGradient
        return '<radialGradient id="' + this.$id + '" gradientUnits="userSpaceOnUse" spreadMethod="' + this.$spread() + '" cx="' + v[C.CENTERX] + '" cy="' + v[C.CENTERY]
            + '" r="' + Math.max(0, v[C.CENTERRADIUS]) + '" fx="' + v[C.FOCALX] + '" fy="' + v[C.FOCALY] + '" fr="' + Math.max(0, v[C.FOCALRADIUS]) + '">' + this.$stopsSvg() + '</radialGradient>'
    }
}
defineType(RadialGradient, {
    props: {
        centerX: { kind: 'real', effect: gradientChanged }, centerY: { kind: 'real', effect: gradientChanged },
        centerRadius: { kind: 'real', effect: gradientChanged },
        focalX: { kind: 'real', effect: gradientChanged }, focalY: { kind: 'real', effect: gradientChanged },
        focalRadius: { kind: 'real', effect: gradientChanged },
    },
})

// QConicalGradient: from angle (degrees, counterclockwise from 3 o'clock) the colors go round counterclockwise; a CSS
// conic gradient goes clockwise from 12 o'clock: it starts at 90 - angle with the positions reversed
class ConicalGradient extends ShapeGradient {
    $css(){
        let v = this.$v
        let stops = this.$stops().slice().reverse().map(stop => toCss(stop.$v[GradientStop.COLOR]) + ' ' + (1 - stop.$v[GradientStop.POSITION]) * 360 + 'deg')
        if(stops.length === 1) stops.push(stops[0])
        return 'conic-gradient(from ' + (90 - v[ConicalGradient.ANGLE]) + 'deg at ' + v[ConicalGradient.CENTERX] + 'px ' + v[ConicalGradient.CENTERY] + 'px, ' + stops.join(', ') + ')'
    }
}
defineType(ConicalGradient, {
    props: {
        centerX: { kind: 'real', effect: gradientChanged }, centerY: { kind: 'real', effect: gradientChanged },
        angle: { kind: 'real', effect: gradientChanged },
    },
})

// ---- ShapePath ----

const JOIN = { 0: 'miter', 0x40: 'bevel', 0x80: 'round', 0x100: 'miter' }
const CAP = { 0: 'butt', 0x10: 'square', 0x20: 'round' }

class ShapePath extends Path {
    $pathChanged(){ redraw(this) }

    $changed(){
        super.$changed()
        redraw(this)
    }

    // the SVG of the path: its gradient (defs) and its path
    $draw(width, height){
        let v = this.$v, C = ShapePath
        let outline = this.$outline()
        let defs = ''
        let attrs = ' d="' + outline.d + '"'
        let gradient = v[C.FILLGRADIENT]
        let conical = null
        if(gradient instanceof ConicalGradient){
            conical = gradient
            attrs += ' fill="none"'
        } else if(gradient instanceof ShapeGradient){
            defs += gradient.$def()
            attrs += ' fill="url(#' + gradient.$id + ')"'
        } else {
            attrs += ' fill="' + toCss(v[C.FILLCOLOR]) + '"'
        }
        attrs += ' fill-rule="' + (v[C.FILLRULE] === ShapePath.WindingFill ? 'nonzero' : 'evenodd') + '"'
        let strokeWidth = v[C.STROKEWIDTH]
        if(strokeWidth < 0 || v[C.STROKESTYLE] === 0){
            attrs += ' stroke="none"'
        } else {
            attrs += ' stroke="' + toCss(v[C.STROKECOLOR]) + '" stroke-width="' + strokeWidth + '" stroke-linejoin="' + (JOIN[v[C.JOINSTYLE]] || 'bevel')
                + '" stroke-linecap="' + (CAP[v[C.CAPSTYLE]] || 'square') + '" stroke-miterlimit="' + v[C.MITERLIMIT] + '"'
            if(v[C.STROKESTYLE] === ShapePath.DashLine){
                let pattern = (v[C.DASHPATTERN] || []).map(n => Number(n) * Math.max(strokeWidth, 1))
                attrs += ' stroke-dasharray="' + pattern.join(' ') + '" stroke-dashoffset="' + v[C.DASHOFFSET] * Math.max(strokeWidth, 1) + '"'
            }
        }
        let scale = v[C.SCALE]
        let transform = scale && (scale.width !== 1 || scale.height !== 1) ? ' transform="scale(' + scale.width + ' ' + scale.height + ')"' : ''
        let svg = '<path' + attrs + transform + '/>'
        if(conical){
            let clip = 'jq-sc-' + (++ids)
            defs += '<clipPath id="' + clip + '"><path d="' + outline.d + '"' + transform + '/></clipPath>'
            svg = '<foreignObject x="0" y="0" width="' + Math.max(width, 1) + '" height="' + Math.max(height, 1) + '" clip-path="url(#' + clip + ')"><div xmlns="http://www.w3.org/1999/xhtml" style="width:100%;height:100%;background:'
                + conical.$css() + '"></div></foreignObject>' + svg
        }
        return { defs, svg, points: outline.points }
    }
}
const shapePathChanged = (path) => redraw(path)
defineType(ShapePath, {
    props: {
        strokeColor: { kind: 'color', initial: '#ffffff', effect: shapePathChanged },
        strokeWidth: { kind: 'real', initial: 1, effect: shapePathChanged },
        fillColor: { kind: 'color', initial: '#ffffff', effect: shapePathChanged },
        fillRule: { kind: 'int', initial: 0, effect: shapePathChanged },
        joinStyle: { kind: 'int', initial: 0x40, effect: shapePathChanged },
        miterLimit: { kind: 'int', initial: 2, effect: shapePathChanged },
        capStyle: { kind: 'int', initial: 0x10, effect: shapePathChanged },
        strokeStyle: { kind: 'int', initial: 1, effect: shapePathChanged },
        dashOffset: { kind: 'real', effect: shapePathChanged },
        dashPattern: { kind: 'var', initial: [4, 2], effect: shapePathChanged },
        fillGradient: {
            kind: 'var',
            initial: null,
            effect(path, value, old){
                if(old && old.$users) old.$users.delete(path)
                if(value && value.$users) value.$users.add(path)
                redraw(path)
            },
        },
        fillItem: { kind: 'var', initial: null, effect: shapePathChanged },
        fillTransform: { kind: 'var', initial: null, effect: shapePathChanged },
        pathHints: { kind: 'int', effect: shapePathChanged },
    },
})
Object.assign(ShapePath, {
    OddEvenFill: 0, WindingFill: 1, MiterJoin: 0, BevelJoin: 0x40, RoundJoin: 0x80, FlatCap: 0, SquareCap: 0x10, RoundCap: 0x20,
    NoPen: 0, SolidLine: 1, DashLine: 2, PathLinear: 0x1, PathQuadratic: 0x2, PathConvex: 0x4, PathFillOnRight: 0x8,
    PathSolid: 0x10, PathNonIntersecting: 0x20, PathNonOverlappingControlPointTriangles: 0x40,
})

// ---- Shape ----

class Shape extends Item {
    $paths(){
        return (this.$children || []).filter(child => child instanceof ShapePath)
    }

    $invalidate(){
        // the implicit size follows the paths now, not when the page is drawn
        this.$updateBounds()
        if(this.$redrawQueued) return
        this.$redrawQueued = true
        engine.markDirty(this, STYLE)
    }

    $childObjectAdded(child){
        if(child instanceof ShapePath) this.$invalidate()
    }

    // QQuickShape::boundingRect: the bounds of the paths, each widened by its pen (a square cap by width / sqrt 2, else
    // by half the width; the width of a pen with no alpha is 0, and a negative one narrows it); the implicit size is
    // its bottom right
    $bounds(){
        let rect = null
        for(let path of this.$paths()){
            let v = path.$v
            let pw = alphaOf(v[ShapePath.STROKECOLOR]) ? v[ShapePath.STROKEWIDTH] : 0
            let d = v[ShapePath.CAPSTYLE] === ShapePath.SquareCap ? pw * Math.SQRT1_2 : pw / 2
            let x0 = Infinity, y0 = Infinity, x1 = -Infinity, y1 = -Infinity
            for(let p of path.$outline().points){
                if(!Number.isFinite(p.x) || !Number.isFinite(p.y)) continue
                x0 = Math.min(x0, p.x); y0 = Math.min(y0, p.y); x1 = Math.max(x1, p.x); y1 = Math.max(y1, p.y)
            }
            if(x0 === Infinity) continue
            let r = { x0: x0 - d, y0: y0 - d, x1: x1 + d, y1: y1 + d }
            rect = rect ? { x0: Math.min(rect.x0, r.x0), y0: Math.min(rect.y0, r.y0), x1: Math.max(rect.x1, r.x1), y1: Math.max(rect.y1, r.y1) } : r
        }
        return rect || { x0: 0, y0: 0, x1: 0, y1: 0 }
    }

    $updateBounds(){
        let r = this.$bounds()
        this.$write(Item.IMPLICITWIDTH, r.x1)
        this.$write(Item.IMPLICITHEIGHT, r.y1)
        engine.emit(this, Shape.SIGNAL_boundingRectChanged)
    }

    $paint(dirty){
        super.$paint(dirty)
        this.$redrawQueued = false
        if(!this.$dom || !(dirty & STYLE)) return
        if(!this.$svg){
            this.$svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg')
            this.$svg.style.cssText = 'position:absolute;left:0;top:0;overflow:visible;pointer-events:none'
            this.$dom.insertBefore(this.$svg, this.$dom.firstChild)
        }
        let width = this.$v[Item.WIDTH], height = this.$v[Item.HEIGHT]
        this.$svg.setAttribute('width', Math.max(width, 1))
        this.$svg.setAttribute('height', Math.max(height, 1))
        let defs = '', body = ''
        for(let path of this.$paths()){
            let drawn = path.$draw(width, height)
            defs += drawn.defs
            body += drawn.svg
        }
        this.$svg.innerHTML = (defs ? '<defs>' + defs + '</defs>' : '') + body
        // drawn: the renderer is chosen and the shape is ready (Null and UnknownRenderer before, as in Qt)
        this.$write(Shape.RENDERERTYPE, this.$v[Shape.PREFERREDRENDERERTYPE] === Shape.CurveRenderer ? Shape.CurveRenderer : Shape.GeometryRenderer)
        this.$write(Shape.STATUS, Shape.Ready)
    }
}
defineType(Shape, {
    props: {
        rendererType: { kind: 'int', readonly: true },
        boundingRect: {
            kind: 'var',
            readonly: true,
            get(){
                engine.track(this, Shape.SIGNAL_boundingRectChanged)
                let r = this.$bounds()
                return new QRectF(r.x0, r.y0, r.x1 - r.x0, r.y1 - r.y0)
            },
        },
        preferredRendererType: 'int',
        asynchronous: 'bool',
        vendorExtensionsEnabled: { kind: 'bool', initial: true },
        status: { kind: 'int', readonly: true },
        containsMode: 'int',
        fillMode: { kind: 'int', effect: (shape) => shape.$invalidate() },
        horizontalAlignment: { kind: 'int', initial: 1, effect: (shape) => shape.$invalidate() },
        verticalAlignment: { kind: 'int', initial: 0x20, effect: (shape) => shape.$invalidate() },
    },
})
Object.assign(Shape, {
    UnknownRenderer: 0, GeometryRenderer: 1, NvprRenderer: 2, SoftwareRenderer: 3, CurveRenderer: 4,
    Null: 0, Ready: 1, Processing: 2, BoundingRectContains: 0, FillContains: 1,
    NoResize: 0, PreserveAspectFit: 1, PreserveAspectCrop: 2, Stretch: 3,
})
// the size draws the conical gradients again
{
    let base = Shape.prototype.$write
    Shape.prototype.$write = function(id, value){
        base.call(this, id, value)
        if((id === Item.WIDTH || id === Item.HEIGHT) && this.$svg) engine.markDirty(this, STYLE)
    }
}

module.exports = { Shape, ShapePath, ShapeGradient, LinearGradient, RadialGradient, ConicalGradient }
