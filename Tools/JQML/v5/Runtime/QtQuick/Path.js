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
        out.points.push({ x: p.x, y: p.y, move: true })
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

// the points of a curve; for the length of a path (fine) each takes an equal share of the length of the curve: a
// point at a share of the length of a curve is at that t (QQuickPath::createPointCache: realT linear in the length)
function curvePoints(out, controls, coarse){
    let n = out.fine ? 64 : coarse
    let pts = []
    for(let i = 1; i <= n; i++) pts.push(bezier(controls, i / n))
    if(out.fine){
        let length = 0, prev = controls[0]
        for(let i = 1; i <= 256; i++){
            let q = bezier(controls, i / 256)
            length += Math.hypot(q.x - prev.x, q.y - prev.y)
            prev = q
        }
        for(let q of pts) q.dl = length / n
    }
    out.points.push(...pts)
}

class PathQuad extends PathElement {
    $add(out, at){
        let v = this.$v
        let p = endPoint(this, at)
        let cx = v[PathQuad.RELATIVECONTROLX] === v[PathQuad.RELATIVECONTROLX] ? at.x + v[PathQuad.RELATIVECONTROLX] : num(v[PathQuad.CONTROLX])
        let cy = v[PathQuad.RELATIVECONTROLY] === v[PathQuad.RELATIVECONTROLY] ? at.y + v[PathQuad.RELATIVECONTROLY] : num(v[PathQuad.CONTROLY])
        out.d.push('Q', fmt(cx), fmt(cy), fmt(p.x), fmt(p.y))
        curvePoints(out, [at, { x: cx, y: cy }, p], 8)
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
        curvePoints(out, [at, c1, c2, p], 12)
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
    let steps = Math.max(2, Math.ceil(Math.abs(sweep) / (out.fine ? 1 : 15)))
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

// the points of an SVG arc (the endpoint parameterization of SVG 1.1 F.6.5: the center, the angles), for its length
function svgArcPoints(out, from, to, rx, ry, rotation, large, sweep){
    rx = Math.abs(rx); ry = Math.abs(ry)
    if(!rx || !ry || (from.x === to.x && from.y === to.y)){
        out.points.push(to)
        return
    }
    let phi = rotation * Math.PI / 180, cos = Math.cos(phi), sin = Math.sin(phi)
    let dx = (from.x - to.x) / 2, dy = (from.y - to.y) / 2
    let x1 = cos * dx + sin * dy, y1 = -sin * dx + cos * dy
    let lambda = (x1 * x1) / (rx * rx) + (y1 * y1) / (ry * ry)
    if(lambda > 1){ rx *= Math.sqrt(lambda); ry *= Math.sqrt(lambda) }
    let num = rx * rx * ry * ry - rx * rx * y1 * y1 - ry * ry * x1 * x1
    let k = Math.sqrt(Math.max(0, num / (rx * rx * y1 * y1 + ry * ry * x1 * x1))) * (large === sweep ? -1 : 1)
    let cx1 = k * rx * y1 / ry, cy1 = -k * ry * x1 / rx
    let cx = cos * cx1 - sin * cy1 + (from.x + to.x) / 2, cy = sin * cx1 + cos * cy1 + (from.y + to.y) / 2
    let angle = (ux, uy, vx, vy) => Math.atan2(ux * vy - uy * vx, ux * vx + uy * vy)
    let t1 = angle(1, 0, (x1 - cx1) / rx, (y1 - cy1) / ry)
    let dt = angle((x1 - cx1) / rx, (y1 - cy1) / ry, (-x1 - cx1) / rx, (-y1 - cy1) / ry)
    if(!sweep && dt > 0) dt -= 2 * Math.PI
    else if(sweep && dt < 0) dt += 2 * Math.PI
    let steps = Math.max(2, Math.ceil(Math.abs(dt) * 180 / Math.PI))
    for(let i = 1; i <= steps; i++){
        let t = t1 + dt * i / steps
        let ex = rx * Math.cos(t), ey = ry * Math.sin(t)
        out.points.push({ x: cos * ex - sin * ey + cx, y: sin * ex + cos * ey + cy })
    }
}

// PathArc: an arc from the current point to x, y of the radii (QQuickPathArc: Clockwise the default direction)
class PathArc extends PathElement {
    $add(out, at){
        let v = this.$v
        let p = endPoint(this, at)
        let clockwise = v[PathArc.DIRECTION] === PathArc.Clockwise
        out.d.push('A', fmt(v[PathArc.RADIUSX]), fmt(v[PathArc.RADIUSY]), fmt(v[PathArc.XAXISROTATION]), v[PathArc.USELARGEARC] ? 1 : 0, clockwise ? 1 : 0, fmt(p.x), fmt(p.y))
        if(out.fine) svgArcPoints(out, at, p, v[PathArc.RADIUSX], v[PathArc.RADIUSY], v[PathArc.XAXISROTATION], v[PathArc.USELARGEARC], clockwise)
        else out.points.push(p)
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
        let first = out.points.length
        arcPoints(out, cx, cy, rx, ry, a0, sweep)
        if(v[C.MOVETOSTART] && out.points[first]) out.points[first].move = true
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
        if(out.fine) out.points.push({ x, y, move: true }, { x: x + w, y }, { x: x + w, y: y + h }, { x, y: y + h }, { x, y })
        else out.points.push({ x, y }, { x: x + w, y: y + h })
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
        this.$geo = null
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

    // QQuickPath::createPath: the length of the path, and the attribute points at the ends of its curves (the values of
    // the PathAttributes and PathPercents interpolated by length between those that set them)
    $geometry(){
        if(this.$geo) return this.$geo
        let v = this.$v
        let at = { x: num(v[Path.STARTX]), y: num(v[Path.STARTY]) }
        let out = { d: [], points: [{ x: at.x, y: at.y, move: true }], fine: true }
        let elements = this.$elements()
        let names = []
        for(let e of elements) if(e instanceof PathAttribute && names.indexOf(e.$v[PathAttribute.NAME]) < 0) names.push(e.$v[PathAttribute.NAME])
        let lengths = [0]
        let lengthOf = () => {
            for(let i = lengths.length; i < out.points.length; i++){
                let p = out.points[i], q = out.points[i - 1]
                lengths.push(lengths[i - 1] + (p.move ? 0 : p.dl !== undefined ? p.dl : Math.hypot(p.x - q.x, p.y - q.y)))
            }
            return lengths[lengths.length - 1]
        }
        let first = { orig: 0, values: {}, scale: 1 }
        for(let name of names) first.values[name] = 0
        let points = [first]
        let usesPercent = false
        for(let e of elements){
            if(e instanceof PathAttribute){
                points[points.length - 1].values[e.$v[PathAttribute.NAME]] = e.$v[PathAttribute.VALUE]
                interpolate(points, points.length - 1, e.$v[PathAttribute.NAME], e.$v[PathAttribute.VALUE])
            } else if(e instanceof PathPercent){
                points[points.length - 1].values[PERCENT] = e.$v[PathPercent.VALUE]
                interpolate(points, points.length - 1, PERCENT, e.$v[PathPercent.VALUE])
                usesPercent = true
            } else if(e.$add){
                at = e.$add(out, at) || at
                points.push({ orig: lengthOf(), values: {}, scale: 1 })
            }
        }
        let last = points[points.length - 1]
        for(let name of names) if(!(name in last.values)) endpoint(points, name)
        if(usesPercent && !(PERCENT in last.values)){
            last.values[PERCENT] = 1
            interpolate(points, points.length - 1, PERCENT, 1)
        }
        let length = lengthOf()
        let prevPercent = 0, prevOrig = 0
        for(let i = 0; i < points.length; i++){
            let point = points[i]
            if(PERCENT in point.values){
                if(i > 0) point.scale = (point.orig / length - prevOrig) / (point.values[PERCENT] - prevPercent)
                point.orig /= length
                point.percent = point.values[PERCENT]
                prevOrig = point.orig
                prevPercent = point.percent
            } else {
                point.orig = length ? point.orig / length : 0
                point.percent = point.orig
            }
        }
        return this.$geo = { length, points: out.points, lengths, attributePoints: points, names }
    }

    // QQuickPath::pointAtPercent: the point at percent t of the path (QQuickPath::createPointCache: the percent of a
    // PathPercent mapped to the length)
    $pointAt(t){
        let g = this.$geometry()
        if(!(g.length > 0)) return { x: g.points[0].x, y: g.points[0].y }
        let spc = t
        let prevPercent = 0, prevOrig = 0
        let ap = g.attributePoints
        for(let i = 0; i < ap.length; i++){
            let point = ap[i]
            if(t < point.percent || i === ap.length - 1){
                spc = prevOrig + (t - prevPercent) * point.scale
                break
            }
            prevOrig = point.orig
            prevPercent = point.percent
        }
        let target = Math.max(0, Math.min(1, spc)) * g.length
        let lengths = g.lengths
        let lo = 0, hi = lengths.length - 1
        while(lo < hi){
            let mid = (lo + hi) >> 1
            if(lengths[mid] < target) lo = mid + 1
            else hi = mid
        }
        let i = Math.max(1, lo)
        let a = g.points[i - 1], b = g.points[i]
        let span = lengths[i] - lengths[i - 1]
        let f = span > 0 ? (target - lengths[i - 1]) / span : 1
        return { x: a.x + (b.x - a.x) * f, y: a.y + (b.y - a.y) * f }
    }

    // QQuickPath::attributeAt
    $attributeAt(name, percent){
        if(percent < 0 || percent > 1) return 0
        let ap = this.$geometry().attributePoints
        for(let i = 0; i < ap.length; i++){
            let point = ap[i]
            if(point.percent === percent) return point.values[name] || 0
            if(point.percent > percent){
                let lastValue = i ? ap[i - 1].values[name] || 0 : 0
                let lastPercent = i ? ap[i - 1].percent : 0
                let cur = point.values[name] || 0
                return lastValue + (cur - lastValue) * (percent - lastPercent) / (point.percent - lastPercent)
            }
        }
        return 0
    }

    $attributes(){ return this.$geometry().names }

    pointAtPercent(t){
        let p = this.$pointAt(Number(t) || 0)
        return new (require('../Core/ValueTypes').QPointF)(p.x, p.y)
    }
}

const PERCENT = '_qfx_percent'

// QQuickPath::interpolate: the points before idx back to the last that has the value get it by their length
function interpolate(points, idx, name, value){
    if(!idx) return
    let lastValue = 0, lastPercent = 0
    let search = idx - 1
    while(search >= 0){
        let point = points[search]
        if(name in point.values){
            lastValue = point.values[name]
            lastPercent = point.orig
            break
        }
        search--
    }
    search++
    let cur = points[idx]
    for(let i = search; i < idx; i++) points[i].values[name] = lastValue + (value - lastValue) * (points[i].orig - lastPercent) / (cur.orig - lastPercent)
}

// QQuickPath::endpoint: after the last point that has the value, the points take the value of the first point
function endpoint(points, name){
    let value = points[0].values[name]
    for(let i = points.length - 1; i >= 0; i--){
        if(name in points[i].values){
            for(let j = i + 1; j < points.length; j++) points[j].values[name] = value
            return
        }
    }
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
