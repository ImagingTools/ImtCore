// Path of QtQuick and its elements (qquickpath.cpp, Qt 6.12): a path is a start point and the elements declared in it
// (its default property pathElements), each going on from where the one before it ended. A path gives its outline as
// SVG path data ($svg) and the points along it ($points) for its bounds; a change of an element is a change of the
// path (changed), which the item drawing it (Shape) follows.
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { QtObject } = require('../Core/QtObject')

// a change of an element or a path: the path it is in, then what uses the path
function pathChanged(object){
    for(let it = object; it; it = it.$parent){
        if(it instanceof Path){
            it.$changed()
            return
        }
    }
}

class PathElement extends QtObject {
    $parentChanged(old, parent){
        super.$parentChanged(old, parent)
        if(old) pathChanged(old)
        if(parent) pathChanged(parent)
    }
}
defineType(PathElement)

const num = (value) => (Number.isFinite(value) ? value : 0)
const fmt = (value) => String(Math.round(num(value) * 1000) / 1000)

// the end point of an element with x / y / relativeX / relativeY (QQuickCurve): a coordinate not set (NaN) is the
// current one, a relative one is from the current one
function endPoint(element, at){
    let v = element.$v, C = element.constructor
    let rx = v[C.RELATIVEX], ry = v[C.RELATIVEY]
    let x = rx === rx ? at.x + rx : (v[C.X] === v[C.X] ? v[C.X] : at.x)
    let y = ry === ry ? at.y + ry : (v[C.Y] === v[C.Y] ? v[C.Y] : at.y)
    return { x, y }
}
const curveProps = {
    x: { kind: 'real', initial: NaN, effect: pathChanged },
    y: { kind: 'real', initial: NaN, effect: pathChanged },
    relativeX: { kind: 'real', initial: NaN, effect: pathChanged },
    relativeY: { kind: 'real', initial: NaN, effect: pathChanged },
}

class PathLine extends PathElement {
    $add(out, at){
        let p = endPoint(this, at)
        out.d.push('L', fmt(p.x), fmt(p.y))
        out.points.push(p)
        return p
    }
}
defineType(PathLine, { props: curveProps })

class PathMove extends PathElement {
    $add(out, at){
        let p = endPoint(this, at)
        out.d.push('M', fmt(p.x), fmt(p.y))
        out.points.push(p)
        return p
    }
}
defineType(PathMove, { props: curveProps })

// a point of a curve at t, for the bounds
const bezier = (pts, t) => {
    let list = pts
    while(list.length > 1){
        let next = []
        for(let i = 0; i + 1 < list.length; i++) next.push({ x: list[i].x + (list[i + 1].x - list[i].x) * t, y: list[i].y + (list[i + 1].y - list[i].y) * t })
        list = next
    }
    return list[0]
}

class PathQuad extends PathElement {
    $add(out, at){
        let v = this.$v
        let p = endPoint(this, at)
        let cx = v[PathQuad.RELATIVECONTROLX] === v[PathQuad.RELATIVECONTROLX] ? at.x + v[PathQuad.RELATIVECONTROLX] : num(v[PathQuad.CONTROLX])
        let cy = v[PathQuad.RELATIVECONTROLY] === v[PathQuad.RELATIVECONTROLY] ? at.y + v[PathQuad.RELATIVECONTROLY] : num(v[PathQuad.CONTROLY])
        out.d.push('Q', fmt(cx), fmt(cy), fmt(p.x), fmt(p.y))
        for(let i = 1; i <= 8; i++) out.points.push(bezier([at, { x: cx, y: cy }, p], i / 8))
        return p
    }
}
defineType(PathQuad, {
    props: Object.assign({
        controlX: { kind: 'real', effect: pathChanged }, controlY: { kind: 'real', effect: pathChanged },
        relativeControlX: { kind: 'real', initial: NaN, effect: pathChanged }, relativeControlY: { kind: 'real', initial: NaN, effect: pathChanged },
    }, curveProps),
})

class PathCubic extends PathElement {
    $add(out, at){
        let v = this.$v, C = PathCubic
        let p = endPoint(this, at)
        let ctl = (abs, rel, from) => (v[rel] === v[rel] ? from + v[rel] : num(v[abs]))
        let c1 = { x: ctl(C.CONTROL1X, C.RELATIVECONTROL1X, at.x), y: ctl(C.CONTROL1Y, C.RELATIVECONTROL1Y, at.y) }
        let c2 = { x: ctl(C.CONTROL2X, C.RELATIVECONTROL2X, at.x), y: ctl(C.CONTROL2Y, C.RELATIVECONTROL2Y, at.y) }
        out.d.push('C', fmt(c1.x), fmt(c1.y), fmt(c2.x), fmt(c2.y), fmt(p.x), fmt(p.y))
        for(let i = 1; i <= 12; i++) out.points.push(bezier([at, c1, c2, p], i / 12))
        return p
    }
}
defineType(PathCubic, {
    props: Object.assign({
        control1X: { kind: 'real', effect: pathChanged }, control1Y: { kind: 'real', effect: pathChanged },
        control2X: { kind: 'real', effect: pathChanged }, control2Y: { kind: 'real', effect: pathChanged },
        relativeControl1X: { kind: 'real', initial: NaN, effect: pathChanged }, relativeControl1Y: { kind: 'real', initial: NaN, effect: pathChanged },
        relativeControl2X: { kind: 'real', initial: NaN, effect: pathChanged }, relativeControl2Y: { kind: 'real', initial: NaN, effect: pathChanged },
    }, curveProps),
})

// the points of an elliptic arc from angle a0 sweeping by sweep (degrees, clockwise on the screen)
function arcPoints(out, cx, cy, rx, ry, a0, sweep){
    let steps = Math.max(2, Math.ceil(Math.abs(sweep) / 15))
    for(let i = 0; i <= steps; i++){
        let a = (a0 + sweep * i / steps) * Math.PI / 180
        out.points.push({ x: cx + rx * Math.cos(a), y: cy + ry * Math.sin(a) })
    }
}

// SVG arcs from the start angle by the sweep (an arc of 360 degrees or more is two arcs: SVG draws no full circle)
function svgArc(out, cx, cy, rx, ry, a0, sweep, rotation = 0){
    let parts = Math.abs(sweep) >= 360 ? 2 : 1
    let step = sweep / parts
    for(let i = 1; i <= parts; i++){
        let a = (a0 + step * i) * Math.PI / 180
        out.d.push('A', fmt(rx), fmt(ry), fmt(rotation), Math.abs(step) > 180 ? 1 : 0, step > 0 ? 1 : 0, fmt(cx + rx * Math.cos(a)), fmt(cy + ry * Math.sin(a)))
    }
}

// PathArc: an arc from the current point to x, y of the radii (QQuickPathArc: Clockwise the default direction)
class PathArc extends PathElement {
    $add(out, at){
        let v = this.$v
        let p = endPoint(this, at)
        let clockwise = v[PathArc.DIRECTION] === PathArc.Clockwise
        out.d.push('A', fmt(v[PathArc.RADIUSX]), fmt(v[PathArc.RADIUSY]), fmt(v[PathArc.XAXISROTATION]), v[PathArc.USELARGEARC] ? 1 : 0, clockwise ? 1 : 0, fmt(p.x), fmt(p.y))
        out.points.push(p)
        return p
    }
}
defineType(PathArc, {
    props: Object.assign({
        radiusX: { kind: 'real', effect: pathChanged }, radiusY: { kind: 'real', effect: pathChanged },
        useLargeArc: { kind: 'bool', effect: pathChanged }, direction: { kind: 'int', effect: pathChanged },
        xAxisRotation: { kind: 'real', effect: pathChanged },
    }, curveProps),
})
Object.assign(PathArc, { Clockwise: 0, Counterclockwise: 1 })

// PathAngleArc: an arc of an ellipse by its center, from startAngle by sweepAngle (degrees, clockwise); moveToStart
// moves to its start, else a line goes there
class PathAngleArc extends PathElement {
    $add(out, at){
        let v = this.$v, C = PathAngleArc
        let cx = v[C.CENTERX], cy = v[C.CENTERY], rx = v[C.RADIUSX], ry = v[C.RADIUSY]
        let a0 = v[C.STARTANGLE], sweep = Math.max(-360, Math.min(360, v[C.SWEEPANGLE]))
        let start = { x: cx + rx * Math.cos(a0 * Math.PI / 180), y: cy + ry * Math.sin(a0 * Math.PI / 180) }
        out.d.push(v[C.MOVETOSTART] ? 'M' : 'L', fmt(start.x), fmt(start.y))
        if(sweep) svgArc(out, cx, cy, rx, ry, a0, sweep)
        arcPoints(out, cx, cy, rx, ry, a0, sweep)
        let a = (a0 + sweep) * Math.PI / 180
        return { x: cx + rx * Math.cos(a), y: cy + ry * Math.sin(a) }
    }
}
defineType(PathAngleArc, {
    props: {
        centerX: { kind: 'real', effect: pathChanged }, centerY: { kind: 'real', effect: pathChanged },
        radiusX: { kind: 'real', effect: pathChanged }, radiusY: { kind: 'real', effect: pathChanged },
        startAngle: { kind: 'real', effect: pathChanged }, sweepAngle: { kind: 'real', effect: pathChanged },
        moveToStart: { kind: 'bool', initial: true, effect: pathChanged },
    },
})

// PathRectangle (Qt 6.8): a closed rectangle with rounded corners (a corner not set is the radius, each at most half
// the shorter side); the path goes on from its top left
class PathRectangle extends PathElement {
    $corner(name){
        let value = this.$v[PathRectangle[name.toUpperCase()]]
        return value === value ? value : this.$v[PathRectangle.RADIUS]
    }

    $add(out, at){
        let v = this.$v, C = PathRectangle
        let x = v[C.X], y = v[C.Y], w = v[C.WIDTH], h = v[C.HEIGHT]
        let max = Math.max(0, Math.min(w, h) / 2)
        let r = (name) => Math.min(max, Math.max(0, num(this.$corner(name))))
        let tl = r('topLeftRadius'), tr = r('topRightRadius'), br = r('bottomRightRadius'), bl = r('bottomLeftRadius')
        out.d.push('M', fmt(x + tl), fmt(y), 'L', fmt(x + w - tr), fmt(y))
        if(tr) out.d.push('A', fmt(tr), fmt(tr), 0, 0, 1, fmt(x + w), fmt(y + tr))
        out.d.push('L', fmt(x + w), fmt(y + h - br))
        if(br) out.d.push('A', fmt(br), fmt(br), 0, 0, 1, fmt(x + w - br), fmt(y + h))
        out.d.push('L', fmt(x + bl), fmt(y + h))
        if(bl) out.d.push('A', fmt(bl), fmt(bl), 0, 0, 1, fmt(x), fmt(y + h - bl))
        out.d.push('L', fmt(x), fmt(y + tl))
        if(tl) out.d.push('A', fmt(tl), fmt(tl), 0, 0, 1, fmt(x + tl), fmt(y))
        out.d.push('Z')
        out.points.push({ x, y }, { x: x + w, y: y + h })
        return { x, y }
    }
}
const cornerProp = { kind: 'real', initial: NaN, effect: pathChanged }
defineType(PathRectangle, {
    props: {
        x: { kind: 'real', effect: pathChanged }, y: { kind: 'real', effect: pathChanged },
        width: { kind: 'real', effect: pathChanged }, height: { kind: 'real', effect: pathChanged },
        radius: { kind: 'real', effect: pathChanged },
        topLeftRadius: cornerProp, topRightRadius: cornerProp, bottomLeftRadius: cornerProp, bottomRightRadius: cornerProp,
        strokeAdjustment: { kind: 'real', effect: pathChanged },
    },
})
for(let name of ['topLeftRadius', 'topRightRadius', 'bottomLeftRadius', 'bottomRightRadius']){
    let id = PathRectangle[name.toUpperCase()]
    Object.defineProperty(PathRectangle.prototype, name, {
        get(){
            engine.track(this, PathRectangle.$type.props[id].signal)
            engine.track(this, PathRectangle.SIGNAL_radiusChanged)
            return this.$corner(name)
        },
        set(value){ this.$assign(id, value === undefined ? NaN : value) },
        configurable: true,
    })
}

// PathSvg: SVG path data as it is (its last point is not followed)
class PathSvg extends PathElement {
    $add(out, at){
        let path = String(this.$v[PathSvg.PATH] || '')
        out.d.push(path)
        let numbers = path.match(/-?\d*\.?\d+(?:e[-+]?\d+)?/gi) || []
        for(let i = 0; i + 1 < numbers.length; i += 2) out.points.push({ x: Number(numbers[i]), y: Number(numbers[i + 1]) })
        let last = numbers.length >= 2 ? { x: Number(numbers[numbers.length - 2]), y: Number(numbers[numbers.length - 1]) } : at
        return last
    }
}
defineType(PathSvg, { props: { path: { kind: 'string', effect: pathChanged } } })

// PathPolyline / PathMultiline: lines through a list of points (each a point or an { x, y })
function polyline(out, list){
    let first = true, last = null
    for(let p of list || []){
        let x = num(p && p.x), y = num(p && p.y)
        out.d.push(first ? 'M' : 'L', fmt(x), fmt(y))
        out.points.push({ x, y })
        first = false
        last = { x, y }
    }
    return last
}
class PathPolyline extends PathElement {
    $add(out, at){ return polyline(out, this.$v[PathPolyline.PATH]) || at }
}
defineType(PathPolyline, { props: { path: { kind: 'var', initial: null, effect: pathChanged }, start: { kind: 'var', readonly: true } } })
class PathMultiline extends PathElement {
    $add(out, at){
        let last = at
        for(let list of this.$v[PathMultiline.PATHS] || []) last = polyline(out, list) || last
        return last
    }
}
defineType(PathMultiline, { props: { paths: { kind: 'var', initial: null, effect: pathChanged }, start: { kind: 'var', readonly: true } } })

// PathAttribute / PathPercent: values along the path for a PathView, no outline
class PathAttribute extends PathElement {}
defineType(PathAttribute, { props: { name: { kind: 'string', effect: pathChanged }, value: { kind: 'real', effect: pathChanged } } })
class PathPercent extends PathElement {}
defineType(PathPercent, { props: { value: { kind: 'real', effect: pathChanged } } })

class Path extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$users = new Set()     // what draws or follows the path: told of its changes ($pathChanged)
    }

    $elements(){
        let list = this.$v[Path.PATHELEMENTS]
        if(Array.isArray(list)) return list
        return (this.$children || []).filter(child => child instanceof PathElement)
    }

    $changed(){
        engine.emit(this, Path.SIGNAL_changed)
        for(let user of this.$users) user.$pathChanged(this)
        if(this.$parent && this.$parent.$pathChanged) this.$parent.$pathChanged(this)
    }

    // { d: SVG path data, points: the points along it, closed }
    $outline(){
        let v = this.$v
        let at = { x: num(v[Path.STARTX]), y: num(v[Path.STARTY]) }
        let start = at
        let out = { d: ['M', fmt(at.x), fmt(at.y)], points: [at] }
        for(let element of this.$elements()){
            if(element.$add) at = element.$add(out, at) || at
        }
        let closed = Math.abs(at.x - start.x) < 1e-9 && Math.abs(at.y - start.y) < 1e-9 && out.points.length > 1
        return { d: out.d.join(' '), points: out.points, closed }
    }

    $svg(){ return this.$outline().d }
}
defineType(Path, {
    props: {
        startX: { kind: 'real', initial: 0, effect: (path) => path.$changed() },
        startY: { kind: 'real', initial: 0, effect: (path) => path.$changed() },
        pathElements: { kind: 'var', initial: null, effect: (path) => path.$changed() },
        closed: { kind: 'bool', readonly: true, get(){ engine.track(this, Path.SIGNAL_changed); return this.$outline().closed } },
        scale: { kind: 'var', initial: null, effect: (path) => path.$changed() },
        simplify: { kind: 'bool', effect: (path) => path.$changed() },
        asynchronous: 'bool',
    },
    signals: ['changed'],
})

module.exports = {
    Path, PathElement, PathLine, PathMove, PathQuad, PathCubic, PathArc, PathAngleArc, PathRectangle, PathSvg, PathPolyline,
    PathMultiline, PathAttribute, PathPercent,
}
