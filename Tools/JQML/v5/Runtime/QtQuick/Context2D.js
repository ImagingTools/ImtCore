// The Context2D of a Canvas: qquickcontext2d.cpp of Qt 6.12 (QQuickJSContext2D, QQuickContext2D, the command buffer's
// replay) over the DOM canvas of the item. The context keeps Qt's state (QQuickContext2D::State) and Qt's path (a
// QPainterPath: arcs and ellipses as Qt's Bézier curves), reads and checks the arguments and the values as Qt does, and
// has the DOM context draw what QPainter would: the state is put on the DOM context for each command.
// What follows Qt rather than the HTML canvas:
// - the methods return what Qt returns (the context, undefined, null), ignore missing or not finite arguments and throw
//   Qt's DOM exceptions (code and message);
// - fillStyle / strokeStyle read back as #rrggbb or rgba(r, g, b, a) (a gradient or a pattern as its color), shadowColor
//   as #rrggbb; font as QFont::toString; lineWidth, miterLimit, shadowBlur only take positive values; textBaseline has
//   no "ideographic"; globalCompositeOperation takes Qt's names (qt-multiply ...);
// - setLineDash doubles an odd list and refuses a negative entry; the dashes and their offset are in units of the line
//   width (QPen);
// - transform(a, b, c, d, e, f) multiplies the matrix after the current one (state.matrix * transform), as Qt does;
// - the compositions are bounded to what is drawn (QPainter), also copy, source-in, source-out, destination-in and
//   destination-atop, which the DOM applies to the whole canvas; qt-clear and qt-destination;
// - getImageData returns Qt's CanvasImageData: data takes integers 0..255 only; putImageData draws (with the state)
//   only with all seven arguments, as Qt 6.12; createPattern(color, Qt.XxxPattern) makes Qt's brush patterns; a
//   gradient keeps the stops it had when it became a style;
// - text(), roundedRect(), ellipse(x, y, w, h), shear(), createConicalGradient(), fillRule, the path property.
// Kept from the browser: the rasterizer (always antialiased: a QPainter of a Canvas without antialiasing is not), the
// glyphs of the fonts, the blur of shadows.
const { parseColor } = require('../Core/color')

const ODD_EVEN_FILL = 0, WINDING_FILL = 1
const NOT_SUPPORTED_ERR = 9, SYNTAX_ERR = 12, INDEX_SIZE_ERR = 1, TYPE_MISMATCH_ERR = 17
const KAPPA = 0.5522847498
const GENERIC_FAMILIES = new Set(['serif', 'sans-serif', 'cursive', 'monospace', 'fantasy'])

// THROW_DOM: an Error with the DOM exception code
function domException(code, message){
    let error = new Error(message)
    error.code = code
    return error
}

const isFinite_ = (v) => typeof v === 'number' && Number.isFinite(v)
const num = (v) => Number(v)
const fuzzyIsNull = (d) => Math.abs(d) <= 0.000000000001
const fuzzyCompare = (a, b) => Math.abs(a - b) * 1000000000000 <= Math.min(Math.abs(a), Math.abs(b))
const toInt32 = (v) => { let n = Number(v); return Number.isFinite(n) ? n | 0 : 0 }

// ---- colors: qt_color_from_string, makeColorString ----

// QColor::fromString: a name (spaces left out, any case), #rgb, #rrggbb, #aarrggbb, #rrrgggbbb, #rrrrggggbbbb
function colorFromString(s){
    if(typeof s !== 'string' || !s.length) return null
    if(s[0] === '#') return parseColor(s)
    return parseColor(s.replace(/ /g, ''))
}

// qt_color_from_string: rgb(), rgba(), hsl(), hsla() of 8 characters or more, else QColor::fromString
function qtColor(value){
    let s = String(value)
    if(s.length > 255 || s.length <= 7) return colorFromString(s)
    let i = 0
    while(/\s/.test(s[i])) i++
    let isRgb = s.startsWith('rgb', i), isHsl = s.startsWith('hsl', i)
    if(!isRgb && !isHsl) return colorFromString(s.slice(i))
    i += 3
    let hasAlpha = s[i] === 'a'
    i++
    if(hasAlpha) i++
    let component = () => {
        while(/\s/.test(s[i])) i++
        let m = /^[+-]?\d+/.exec(s.slice(i))
        let v = m ? parseInt(m[0], 10) : 0
        if(m) i += m[0].length
        if(s[i] === '%'){
            v = Math.round(v / 100 * 255)
            i++
        }
        return v
    }
    let rh = component()
    if(s[i++] !== ',') return null
    let gs = component()
    if(s[i++] !== ',') return null
    let bl = component()
    let alpha = 255
    if(hasAlpha){
        if(s[i++] !== ',') return null
        while(/\s/.test(s[i])) i++
        let m = /^[+-]?(\d+\.?\d*|\.\d+)([eE][+-]?\d+)?/.exec(s.slice(i))
        let a = m ? parseFloat(m[0]) : 0
        if(m) i += m[0].length
        alpha = Math.round(a * 255)
    }
    if(s[i] !== ')') return null
    let clamp = (v, lo, hi) => Math.min(Math.max(v, lo), hi)
    let w8 = (v) => v * 0x101
    if(isRgb) return { a: w8(clamp(alpha, 0, 255)), r: w8(clamp(rh, 0, 255)), g: w8(clamp(gs, 0, 255)), b: w8(clamp(bl, 0, 255)) }
    return hslColor(clamp(rh, 0, 359), clamp(gs, 0, 255), clamp(bl, 0, 255), clamp(alpha, 0, 255))
}

// QColor::fromHsl(h, s, l, a) as its Rgb value (16 bits a component)
function hslColor(h, s, l, a){
    let hue = h / 360, sat = s / 255, light = l / 255
    let r, g, b
    if(sat === 0){
        r = g = b = light
    } else {
        let q = light < 0.5 ? light * (1 + sat) : light + sat - light * sat
        let p = 2 * light - q
        let channel = (t) => {
            if(t < 0) t += 1
            if(t > 1) t -= 1
            if(t < 1 / 6) return p + (q - p) * 6 * t
            if(t < 1 / 2) return q
            if(t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6
            return p
        }
        r = channel(hue + 1 / 3)
        g = channel(hue)
        b = channel(hue - 1 / 3)
    }
    let w = (v) => Math.round(v * 65535)
    return { a: a * 0x101, r: w(r), g: w(g), b: w(b) }
}

const c8 = (v) => Math.round(v / 257)

// QColor::name(): #rrggbb
function colorName(c){
    let hex = (v) => c8(v).toString(16).padStart(2, '0')
    return '#' + hex(c.r) + hex(c.g) + hex(c.b)
}

// makeColorString: #rrggbb when opaque, else rgba(r, g, b, alphaF with up to 6 decimals)
function colorString(c){
    if(c8(c.a) === 255) return colorName(c)
    let alpha = (c.a / 65535).toFixed(6).replace(/0+$/, '')
    if(alpha.endsWith('.')) alpha += '0'
    return 'rgba(' + c8(c.r) + ', ' + c8(c.g) + ', ' + c8(c.b) + ', ' + alpha + ')'
}

const colorCss = (c) => 'rgba(' + c8(c.r) + ',' + c8(c.g) + ',' + c8(c.b) + ',' + (c8(c.a) / 255) + ')'
const sameColor = (a, b) => a && b && a.a === b.a && a.r === b.r && a.g === b.g && a.b === b.b

// ---- fonts: qt_font_from_string, QFont::toString ----

function makeFont(family, pixelSize, pointSize){
    return { family, pointSize, pixelSize, weight: 400, style: 0, capitalization: 0 }
}

const DEFAULT_FONT = Object.freeze(makeFont('sans-serif', 10, -1))

// QFont::toString
function fontString(f){
    return [f.family, f.pointSize > 0 ? f.pointSize : -1, f.pixelSize > 0 ? f.pixelSize : -1, 5, f.weight, f.style, 0, 0, 0, 0,
        f.capitalization, 0, 0, 0, 0, 1, '', 0, 0].join(',')
}

// the CSS font of a QFont
function fontCss(f){
    let family = GENERIC_FAMILIES.has(f.family) ? f.family : '"' + f.family.replace(/\\/g, '\\\\').replace(/"/g, '\\"') + '"'
    return (f.style === 1 ? 'italic ' : f.style === 2 ? 'oblique ' : '') + (f.capitalization === 3 ? 'small-caps ' : '') + f.weight + ' '
        + (f.pixelSize > 0 ? f.pixelSize + 'px ' : f.pointSize + 'pt ') + family
}

// QFontDatabase::hasFamily: a family the browser draws otherwise than its fallbacks
let familyProbe = null
function hasFamily(family){
    if(typeof document === 'undefined') return true
    if(!familyProbe){
        let canvas = document.createElement('canvas')
        familyProbe = { ctx: canvas.getContext('2d'), known: new Map() }
    }
    if(familyProbe.known.has(family)) return true
    // a font of the page (FontLoader, @font-face), loaded or not
    if(document.fonts && typeof document.fonts.forEach === 'function'){
        let found = false
        document.fonts.forEach((face) => { if(String(face.family).replace(/^["']|["']$/g, '') === family) found = true })
        if(found){
            familyProbe.known.set(family, true)
            return true
        }
    }
    let ctx = familyProbe.ctx
    let text = 'mmmmmmmmmmlliWW@#0O'
    let width = (font) => { ctx.font = font; return ctx.measureText(text).width }
    let quoted = '"' + family.replace(/\\/g, '\\\\').replace(/"/g, '\\"') + '"'
    let found = ['monospace', 'serif', 'sans-serif'].some(fallback => width('72px ' + quoted + ', ' + fallback) !== width('72px ' + fallback))
    // a family not there now may come later (a font loading): only the ones found are kept
    if(found) familyProbe.known.set(family, true)
    return found
}

function warn(message){
    console.warn('Context2D: ' + message)
}

// qt_font_from_string: size (px or pt) and families required, style, variant and weight before them in any order
function fontFromString(fontString, current){
    if(!fontString.length){
        warn('Font string is empty.')
        return current
    }
    let sizeEnd = fontString.indexOf('px')
    if(sizeEnd === -1) sizeEnd = fontString.indexOf('pt')
    if(sizeEnd === -1){
        warn('Invalid font size unit in font string.')
        return current
    }
    let sizeStart = fontString.lastIndexOf(' ', sizeEnd)
    sizeStart = sizeStart === -1 ? 0 : sizeStart + 1
    sizeEnd += 3
    let font = makeFont('', -1, -1)
    let token = fontString.slice(sizeStart, sizeEnd).trim()
    let unit = token.slice(-2), value = token.slice(0, -2)
    if(unit !== 'px' && unit !== 'pt'){
        warn('Invalid font size unit in font string.')
        return current
    }
    let size = Number(value.trim())
    if(!value.trim().length || !Number.isFinite(size)){
        warn('A font size of ' + value + ' is invalid.')
        return current
    }
    if(unit === 'px') font.pixelSize = Math.trunc(size)
    else font.pointSize = Math.trunc(size)
    let remaining = fontString.slice(0, sizeStart) + fontString.slice(sizeEnd)
    let familiesString = remaining.slice(sizeStart)
    remaining = remaining.slice(0, sizeStart)
    // qExtractFontFamiliesFromString
    let families = []
    let quote = -1, currentFamily = ''
    for(let i = 0; i < familiesString.length; i++){
        let ch = familiesString[i]
        if(ch === '"' || ch === '\''){
            if(quote === -1) quote = i
            else if(ch === familiesString[quote]){
                families.push(familiesString.slice(quote + 1, i))
                currentFamily = ''
                quote = -1
            } else {
                warn('Mismatched quote in font string.')
                return current
            }
        } else if(ch === ' ' && quote === -1){
            if(currentFamily){
                families.push(currentFamily)
                currentFamily = ''
            }
        } else currentFamily += ch
    }
    if(currentFamily){
        if(quote === -1) families.push(currentFamily)
        else {
            warn('Unclosed quote in font string.')
            return current
        }
    }
    if(!families.length){
        warn('Missing or misplaced font family in font string (it must come after the font size).')
        return current
    }
    // qSetFontFamilyFromTokens: the first one known; a generic family is the browser's own
    let family = families.find(f => GENERIC_FAMILIES.has(f) || hasFamily(f))
    if(family === undefined){
        warn('The font families specified are invalid: ' + families.join('').trim())
        return current
    }
    font.family = family
    let tokens = remaining.trim()
    if(!tokens) return font
    let used = 0
    const STYLE = 1, VARIANT = 2, WEIGHT = 4
    let take = (flag, name) => {
        if(used & flag){
            warn('Duplicate token ' + name + ' found in font string.')
            return false
        }
        used |= flag
        return true
    }
    for(let t of tokens.split(' ')){
        if(t === 'normal'){
            if(!(used & STYLE)) used |= STYLE
            else if(!(used & VARIANT)) used |= VARIANT
            else if(!(used & WEIGHT)) used |= WEIGHT
            else {
                warn('Duplicate token "normal" found in font string.')
                return current
            }
        } else if(t === 'bold'){
            if(!take(WEIGHT, 'bold')) return current
            font.weight = 700
        } else if(t === 'italic'){
            if(!take(STYLE, 'italic')) return current
            font.style = 1
        } else if(t === 'oblique'){
            if(!take(STYLE, 'oblique')) return current
            font.style = 2
        } else if(t === 'small-caps'){
            if(!take(VARIANT, 'small-caps')) return current
            font.capitalization = 3
        } else if(/^[+-]?\d+$/.test(t)){
            if(!take(WEIGHT, '<font-weight>')) return current
            font.weight = parseInt(t, 10)
        } else {
            warn('Invalid or misplaced token ' + t + ' found in font string.')
            return current
        }
    }
    return font
}

// ---- QTransform: [m11, m12, m21, m22, dx, dy], a point maps to (m11 x + m21 y + dx, m12 x + m22 y + dy) ----

const IDENTITY = Object.freeze([1, 0, 0, 1, 0, 0])
const mapPoint = (m, x, y) => [m[0] * x + m[2] * y + m[4], m[1] * x + m[3] * y + m[5]]
// a then b (QTransform operator*)
const multiply = (a, b) => [a[0] * b[0] + a[1] * b[2], a[0] * b[1] + a[1] * b[3], a[2] * b[0] + a[3] * b[2], a[2] * b[1] + a[3] * b[3],
    a[4] * b[0] + a[5] * b[2] + b[4], a[4] * b[1] + a[5] * b[3] + b[5]]
const determinant = (m) => m[0] * m[3] - m[1] * m[2]
const invertible = (m) => !fuzzyIsNull(determinant(m))
function inverted(m){
    let det = determinant(m)
    if(fuzzyIsNull(det)) return IDENTITY.slice()
    let [a, b, c, d, e, f] = m
    return [d / det, -b / det, -c / det, a / det, (c * f - d * e) / det, (b * e - a * f) / det]
}
// QTransform::rotate (exact for the right angles)
function rotationOf(degrees){
    let sin, cos
    if(degrees === 90 || degrees === -270){ sin = 1; cos = 0 }
    else if(degrees === 270 || degrees === -90){ sin = -1; cos = 0 }
    else if(degrees === 180 || degrees === -180){ sin = 0; cos = -1 }
    else {
        let r = degrees * Math.PI / 180
        sin = Math.sin(r)
        cos = Math.cos(r)
    }
    return [cos, sin, -sin, cos, 0, 0]
}

// ---- QPainterPath: move, line and cubic elements, a subpath closed by a line back to its start ----

const M = 0, L = 1, C = 2

// qt_t_for_arc_angle
function tForArcAngle(angle){
    if(fuzzyIsNull(angle)) return 0
    if(fuzzyCompare(angle, 90)) return 1
    let radians = angle * Math.PI / 180
    let cosAngle = Math.cos(radians), sinAngle = Math.sin(radians)
    let tc = angle / 90
    for(let i = 0; i < 2; i++){
        tc -= ((((2 - 3 * KAPPA) * tc + 3 * (KAPPA - 1)) * tc) * tc + 1 - cosAngle) / (((6 - 9 * KAPPA) * tc + 6 * (KAPPA - 1)) * tc)
    }
    let ts = tc
    for(let i = 0; i < 2; i++){
        ts -= ((((3 * KAPPA - 2) * ts - 6 * KAPPA + 3) * ts + 3 * KAPPA) * ts - sinAngle) / (((9 * KAPPA - 6) * ts + 12 * KAPPA - 6) * ts + 3 * KAPPA)
    }
    return 0.5 * (tc + ts)
}

function bezierCoefficients(t){
    let mt = 1 - t
    let b = mt * mt, c = t * t
    let d = c * t, a = b * mt
    b *= 3 * t
    c *= 3 * mt
    return [a, b, c, d]
}

// qt_find_ellipse_coords
function ellipsePoint(x, y, w, h, angle){
    let w2 = w / 2, h2 = h / 2
    let theta = angle - 360 * Math.floor(angle / 360)
    let t = theta / 90
    let quadrant = Math.trunc(t)
    t -= quadrant
    t = tForArcAngle(90 * t)
    if(quadrant & 1) t = 1 - t
    let [a, b, c, d] = bezierCoefficients(t)
    let px = a + b + c * KAPPA, py = d + c + b * KAPPA
    if(quadrant === 1 || quadrant === 2) px = -px
    if(quadrant === 0 || quadrant === 1) py = -py
    return [x + w2 + w2 * px, y + h2 + h2 * py]
}

// QBezier::parameterSplitLeft / bezierOnInterval on [x1, y1, ..., x4, y4]
function splitLeft(b, t){
    let left = new Array(8)
    left[0] = b[0]; left[1] = b[1]
    left[2] = b[0] + t * (b[2] - b[0]); left[3] = b[1] + t * (b[3] - b[1])
    left[4] = b[2] + t * (b[4] - b[2]); left[5] = b[3] + t * (b[5] - b[3])
    b[4] = b[4] + t * (b[6] - b[4]); b[5] = b[5] + t * (b[7] - b[5])
    b[2] = left[4] + t * (b[4] - left[4]); b[3] = left[5] + t * (b[5] - left[5])
    left[4] = left[2] + t * (left[4] - left[2]); left[5] = left[3] + t * (left[5] - left[3])
    left[6] = b[0] = left[4] + t * (b[2] - left[4]); left[7] = b[1] = left[5] + t * (b[3] - left[5])
    return left
}
function bezierOnInterval(bezier, t0, t1){
    if(t0 === 0 && t1 === 1) return bezier
    let b = bezier.slice()
    splitLeft(b, t0)
    return splitLeft(b, (t1 - t0) / (1 - t0))
}

// qt_curves_for_arc: the start point and the cubic control points of an arc of the ellipse in the rectangle
function curvesForArc(x, y, w, h, startAngle, sweepLength){
    let w2 = w / 2, w2k = w2 * KAPPA, h2 = h / 2, h2k = h2 * KAPPA
    let points = [
        [x + w, y + h2],
        [x + w, y + h2 + h2k], [x + w2 + w2k, y + h], [x + w2, y + h],
        [x + w2 - w2k, y + h], [x, y + h2 + h2k], [x, y + h2],
        [x, y + h2 - h2k], [x + w2 - w2k, y], [x + w2, y],
        [x + w2 + w2k, y], [x + w, y + h2 - h2k], [x + w, y + h2],
    ]
    if(sweepLength > 360) sweepLength = 360
    else if(sweepLength < -360) sweepLength = -360
    let curves = []
    if(startAngle === 0){
        if(sweepLength === 360){
            for(let i = 11; i >= 0; i--) curves.push(points[i])
            return { start: points[12], curves }
        } else if(sweepLength === -360){
            for(let i = 1; i <= 12; i++) curves.push(points[i])
            return { start: points[0], curves }
        }
    }
    let startSegment = Math.floor(startAngle / 90)
    let endSegment = Math.floor((startAngle + sweepLength) / 90)
    let startT = (startAngle - startSegment * 90) / 90
    let endT = (startAngle + sweepLength - endSegment * 90) / 90
    let delta = sweepLength > 0 ? 1 : -1
    if(delta < 0){
        startT = 1 - startT
        endT = 1 - endT
    }
    if(fuzzyIsNull(startT - 1)){
        startT = 0
        startSegment += delta
    }
    if(fuzzyIsNull(endT)){
        endT = 1
        endSegment -= delta
    }
    startT = tForArcAngle(startT * 90)
    endT = tForArcAngle(endT * 90)
    let splitAtStart = !fuzzyIsNull(startT), splitAtEnd = !fuzzyIsNull(endT - 1)
    let end = endSegment + delta
    if(startSegment === end){
        let quadrant = 3 - (((startSegment % 4) + 4) % 4)
        let j = 3 * quadrant
        return { start: delta > 0 ? points[j + 3] : points[j], curves }
    }
    let startPoint = ellipsePoint(x, y, w, h, startAngle)
    let endPoint = ellipsePoint(x, y, w, h, startAngle + sweepLength)
    for(let i = startSegment; i !== end; i += delta){
        let quadrant = 3 - (((i % 4) + 4) % 4)
        let j = 3 * quadrant
        let p = delta > 0 ? [points[j + 3], points[j + 2], points[j + 1], points[j]] : [points[j], points[j + 1], points[j + 2], points[j + 3]]
        let b = [p[0][0], p[0][1], p[1][0], p[1][1], p[2][0], p[2][1], p[3][0], p[3][1]]
        if(startSegment === endSegment && fuzzyCompare(startT, endT)) return { start: startPoint, curves: [] }
        if(i === startSegment){
            if(i === endSegment && splitAtEnd) b = bezierOnInterval(b, startT, endT)
            else if(splitAtStart) b = bezierOnInterval(b, startT, 1)
        } else if(i === endSegment && splitAtEnd){
            b = bezierOnInterval(b, 0, endT)
        }
        curves.push([b[2], b[3]], [b[4], b[5]], [b[6], b[7]])
    }
    curves[curves.length - 1] = endPoint
    return { start: startPoint, curves }
}

class PainterPath {
    constructor(){
        this.elements = []
        this.cStart = 0
        this.requireMoveTo = false
    }

    elementCount(){ return this.elements.length }

    ensureData(){
        if(!this.elements.length){
            this.elements.push({ t: M, x: 0, y: 0 })
            this.cStart = 0
        }
    }

    last(){ return this.elements[this.elements.length - 1] }

    currentPosition(){
        let e = this.last()
        return e ? [e.x, e.y] : [0, 0]
    }

    maybeMoveTo(){
        if(this.requireMoveTo){
            let e = this.last()
            this.elements.push({ t: M, x: e.x, y: e.y })
            this.cStart = this.elements.length - 1
            this.requireMoveTo = false
        }
    }

    moveTo(x, y){
        if(!isFinite_(x) || !isFinite_(y)) return
        this.ensureData()
        this.requireMoveTo = false
        let e = this.last()
        if(e.t === M){
            e.x = x
            e.y = y
        } else {
            this.elements.push({ t: M, x, y })
            this.cStart = this.elements.length - 1
        }
    }

    lineTo(x, y){
        if(!isFinite_(x) || !isFinite_(y)) return
        this.ensureData()
        this.maybeMoveTo()
        let e = this.last()
        if(e.x === x && e.y === y) return
        this.elements.push({ t: L, x, y })
    }

    cubicTo(c1x, c1y, c2x, c2y, x, y){
        if(![c1x, c1y, c2x, c2y, x, y].every(isFinite_)) return
        this.ensureData()
        let e = this.last()
        if(e.x === c1x && e.y === c1y && c1x === c2x && c1y === c2y && c2x === x && c2y === y) return
        this.maybeMoveTo()
        this.elements.push({ t: C, c1x, c1y, c2x, c2y, x, y })
    }

    quadTo(cx, cy, x, y){
        if(![cx, cy, x, y].every(isFinite_)) return
        this.ensureData()
        let [px, py] = this.currentPosition()
        if(px === cx && py === cy && cx === x && cy === y) return
        this.cubicTo((px + 2 * cx) / 3, (py + 2 * cy) / 3, (x + 2 * cx) / 3, (y + 2 * cy) / 3, x, y)
    }

    closeSubpath(){
        if(!this.elements.length) return
        this.requireMoveTo = true
        let first = this.elements[this.cStart], last = this.last()
        if(first.x !== last.x || first.y !== last.y){
            if(fuzzyCompare(first.x, last.x) && fuzzyCompare(first.y, last.y)){
                last.x = first.x
                last.y = first.y
            } else this.elements.push({ t: L, x: first.x, y: first.y })
        }
    }

    arcMoveTo(x, y, w, h, angle){
        if(!w && !h) return
        let [px, py] = ellipsePoint(x, y, w, h, angle)
        this.moveTo(px, py)
    }

    arcTo(x, y, w, h, startAngle, sweepLength){
        if(![x, y, w, h, startAngle, sweepLength].every(isFinite_)) return
        if(!w && !h) return
        this.ensureData()
        let { start, curves } = curvesForArc(x, y, w, h, startAngle, sweepLength)
        this.lineTo(start[0], start[1])
        for(let i = 0; i < curves.length; i += 3) this.cubicTo(curves[i][0], curves[i][1], curves[i + 1][0], curves[i + 1][1], curves[i + 2][0], curves[i + 2][1])
    }

    addRect(x, y, w, h){
        if(!w && !h) return
        this.ensureData()
        this.moveTo(x, y)
        this.elements.push({ t: L, x: x + w, y }, { t: L, x: x + w, y: y + h }, { t: L, x, y: y + h }, { t: L, x, y })
        this.requireMoveTo = true
    }

    addEllipse(x, y, w, h){
        if(!w && !h) return
        this.ensureData()
        let { start, curves } = curvesForArc(x, y, w, h, 0, -360)
        this.moveTo(start[0], start[1])
        for(let i = 0; i < 12; i += 3) this.cubicTo(curves[i][0], curves[i][1], curves[i + 1][0], curves[i + 1][1], curves[i + 2][0], curves[i + 2][1])
        this.requireMoveTo = true
    }

    // addRoundedRect(rect, xr, yr, Qt::AbsoluteSize)
    addRoundedRect(x, y, w, h, xr, yr){
        if(w < 0){ x += w; w = -w }
        if(h < 0){ y += h; h = -h }
        if(!w && !h) return
        let hw = w / 2, hh = h / 2
        xr = hw === 0 ? 0 : 100 * Math.min(xr, hw) / hw
        yr = hh === 0 ? 0 : 100 * Math.min(yr, hh) / hh
        if(xr <= 0 || yr <= 0){
            this.addRect(x, y, w, h)
            return
        }
        let rxx2 = w * xr / 100, ryy2 = h * yr / 100
        this.ensureData()
        this.arcMoveTo(x, y, rxx2, ryy2, 180)
        this.arcTo(x, y, rxx2, ryy2, 180, -90)
        this.arcTo(x + w - rxx2, y, rxx2, ryy2, 90, -90)
        this.arcTo(x + w - rxx2, y + h - ryy2, rxx2, ryy2, 0, -90)
        this.arcTo(x, y + h - ryy2, rxx2, ryy2, 270, -90)
        this.closeSubpath()
        this.requireMoveTo = true
    }

    // the bounds of the points and the control points
    controlBounds(){
        let xs = [], ys = []
        for(let e of this.elements){
            xs.push(e.x)
            ys.push(e.y)
            if(e.t === C){
                xs.push(e.c1x, e.c2x)
                ys.push(e.c1y, e.c2y)
            }
        }
        if(!xs.length) return { width: 0, height: 0 }
        return { width: Math.max(...xs) - Math.min(...xs), height: Math.max(...ys) - Math.min(...ys) }
    }

    // the subpaths: lists of elements from a move
    subpaths(){
        let out = []
        for(let e of this.elements){
            if(e.t === M || !out.length) out.push([e])
            else out[out.length - 1].push(e)
        }
        return out
    }

    // a Path2D of the path with the matrix applied to its points; a subpath back at its start is closed (the stroker
    // joins it)
    toPath2D(m){
        let path = new Path2D()
        for(let sub of this.subpaths()){
            let first = sub[0]
            let [fx, fy] = mapPoint(m, first.x, first.y)
            path.moveTo(fx, fy)
            for(let i = 1; i < sub.length; i++){
                let e = sub[i]
                let [x, y] = mapPoint(m, e.x, e.y)
                if(e.t === C){
                    let [ax, ay] = mapPoint(m, e.c1x, e.c1y), [bx, by] = mapPoint(m, e.c2x, e.c2y)
                    path.bezierCurveTo(ax, ay, bx, by, x, y)
                } else path.lineTo(x, y)
            }
            let last = sub[sub.length - 1]
            if(sub.length > 1 && last.x === first.x && last.y === first.y) path.closePath()
        }
        return path
    }

    // QPainterPath::toFillPolygon: the points, the curves flattened
    polygon(){
        let out = []
        let prev = null
        for(let e of this.elements){
            if(e.t === C && prev){
                for(let i = 1; i <= 16; i++){
                    let [a, b, c, d] = bezierCoefficients(i / 16)
                    out.push([a * prev.x + b * e.c1x + c * e.c2x + d * e.x, a * prev.y + b * e.c1y + c * e.c2y + d * e.y])
                }
            } else out.push([e.x, e.y])
            prev = e
        }
        return out
    }

    clone(){
        let p = new PainterPath()
        p.elements = this.elements.map(e => Object.assign({}, e))
        p.cStart = this.cStart
        p.requireMoveTo = this.requireMoveTo
        return p
    }
}

// ---- styles: CanvasGradient (QQuickContext2DStyle with a gradient), patterns ----

class CanvasGradient {
    constructor(kind, args){
        this.$kind = kind
        this.$args = args
        this.$stops = []
    }

    // gradient_proto_addColorStop: two arguments; an offset in [0, 1], a valid color (a stop at the same offset is
    // replaced: QGradient::setColorAt)
    addColorStop(...args){
        if(args.length !== 2) return this
        let pos = Number(args[0])
        let color = qtColor(args[1])
        if(pos < 0 || pos > 1 || !Number.isFinite(pos)) throw domException(INDEX_SIZE_ERR, 'CanvasGradient: parameter offset out of range')
        if(!color) throw domException(SYNTAX_ERR, 'CanvasGradient: parameter color is not a valid color string')
        let at = this.$stops.findIndex(s => s.pos === pos)
        if(at >= 0) this.$stops[at] = { pos, color }
        else {
            this.$stops.push({ pos, color })
            this.$stops.sort((a, b) => a.pos - b.pos)
        }
        return this
    }

    // the brush as it is now (QBrush copies the gradient)
    $snapshot(){
        return { kind: this.$kind, args: this.$args.slice(), stops: this.$stops.map(s => Object.assign({}, s)) }
    }
}

class CanvasPattern {
    constructor(brush){ this.$brush = brush }
}

// qt_patternForBrush (inverted: a set bit is painted), Dense1Pattern .. DiagCrossPattern
const BRUSH_PATTERNS = [
    [0xff, 0xbb, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff], [0x77, 0xff, 0xdd, 0xff, 0x77, 0xff, 0xdd, 0xff],
    [0x55, 0xbb, 0x55, 0xee, 0x55, 0xbb, 0x55, 0xee], [0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55],
    [0xaa, 0x44, 0xaa, 0x11, 0xaa, 0x44, 0xaa, 0x11], [0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00],
    [0x00, 0x44, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00], [0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00],
    [0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10], [0x10, 0x10, 0x10, 0xff, 0x10, 0x10, 0x10, 0x10],
    [0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01], [0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80],
    [0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81],
]

// ---- image data: QQuickJSContext2DImageData and its pixel array ----

const PIXELS = Symbol('pixels')

// the pixel array: an index reads a component (undefined past the end), a write takes an integer 0..255 only
function pixelArray(width, height, bytes){
    let target = { [PIXELS]: { width, height, bytes } }
    return new Proxy(target, {
        get(t, key){
            if(key === PIXELS) return t[PIXELS]
            if(key === 'length') return width * height * 4
            if(typeof key === 'string' && /^(0|[1-9]\d*)$/.test(key)){
                let i = Number(key)
                return i < bytes.length ? bytes[i] : undefined
            }
            return undefined
        },
        set(t, key, value){
            if(typeof key === 'string' && /^(0|[1-9]\d*)$/.test(key)){
                let i = Number(key), v = toInt32(value)
                if(i < bytes.length && v >= 0 && v <= 255) bytes[i] = v
                return true
            }
            return true
        },
        has(t, key){
            if(typeof key === 'string' && /^(0|[1-9]\d*)$/.test(key)) return Number(key) < bytes.length
            return key === 'length'
        },
    })
}

class CanvasImageData {
    constructor(width, height, bytes){
        let w = Math.max(0, width), h = Math.max(0, height)
        let data = pixelArray(w, h, bytes || new Uint8ClampedArray(w * h * 4))
        Object.defineProperty(this, 'width', { get: () => w, enumerable: true })
        Object.defineProperty(this, 'height', { get: () => h, enumerable: true })
        Object.defineProperty(this, 'data', { get: () => data, enumerable: true })
    }
}

function scratchCanvas(w, h){
    if(typeof document === 'undefined') return null
    let canvas = document.createElement('canvas')
    canvas.width = Math.max(1, w)
    canvas.height = Math.max(1, h)
    return canvas
}

// the pixels of an image data on a canvas of its own
function imageDataCanvas(imageData){
    let { width, height, bytes } = imageData.data[PIXELS]
    let canvas = scratchCanvas(width, height)
    if(!canvas || !width || !height) return null
    let ctx = canvas.getContext('2d')
    let dom = ctx.createImageData(width, height)
    dom.data.set(bytes)
    ctx.putImageData(dom, 0, 0)
    return canvas
}

// ---- the state (QQuickContext2D::State) ----

function defaultState(){
    return {
        matrix: IDENTITY.slice(),
        invertibleCTM: true,
        clips: [],
        fillStyle: { color: parseColor('#000000') },
        fillValue: null,
        strokeStyle: { color: parseColor('#000000') },
        strokeValue: null,
        fillRule: WINDING_FILL,
        globalAlpha: 1,
        lineWidth: 1,
        lineCap: 'butt',
        lineJoin: 'miter',
        lineDash: [],
        lineDashOffset: 0,
        miterLimit: 10,
        shadowOffsetX: 0,
        shadowOffsetY: 0,
        shadowBlur: 0,
        shadowColor: { a: 0, r: 0, g: 0, b: 0 },
        globalCompositeOperation: 'source-over',
        font: DEFAULT_FONT,
        textAlign: 'start',
        textBaseline: 'alphabetic',
    }
}

function copyState(s){
    return Object.assign({}, s, { matrix: s.matrix.slice(), clips: s.clips.slice(), lineDash: s.lineDash.slice() })
}

const COMPOSITE_MODES = new Set(['source-over', 'source-out', 'source-in', 'source-atop', 'destination-atop', 'destination-in',
    'destination-out', 'destination-over', 'lighter', 'copy', 'xor', 'qt-clear', 'qt-destination', 'qt-multiply', 'qt-screen',
    'qt-overlay', 'qt-darken', 'qt-lighten', 'qt-color-dodge', 'qt-color-burn', 'qt-hard-light', 'qt-soft-light',
    'qt-difference', 'qt-exclusion'])
// the DOM operations that change the canvas outside what is drawn (QPainter's do not)
const UNBOUNDED = new Set(['copy', 'source-in', 'source-out', 'destination-in', 'destination-atop'])

// ---- the context ----

class Context2D {
    // item: the Canvas; dom: the DOM context of its canvas; ratioOf(): the device pixel ratio of the backing store
    constructor(item, dom, ratioOf){
        Object.defineProperty(this, '$item', { value: item })
        Object.defineProperty(this, '$dom', { value: dom, writable: true })
        Object.defineProperty(this, '$ratioOf', { value: ratioOf })
        this.$state = defaultState()
        this.$stack = []
        this.$path = new PainterPath()
        this.$texts = []
        this.$pathValue = undefined
        // the base of the DOM state: a clip goes with a restore to it
        dom.save()
    }

    get canvas(){ return this.$item }

    // a command for the canvas (QQuickContext2DCommandBuffer not empty): painted() follows
    $command(){ this.$item.$drawn = true }

    // the canvas got a new backing store (its DOM state is gone): the clips of the states again
    $restoreDom(dom){
        this.$dom = dom
        dom.save()
        for(let s of this.$stack){
            this.$applyClips(s.clips)
            dom.save()
        }
        this.$applyClips(this.$state.clips)
    }

    $applyClips(clips){
        let dom = this.$dom, r = this.$ratioOf()
        dom.setTransform(r, 0, 0, r, 0, 0)
        for(let clip of clips) dom.clip(clip.path.toPath2D(IDENTITY), clip.rule === ODD_EVEN_FILL ? 'evenodd' : 'nonzero')
    }

    // ---- the state ----

    save(){
        this.$stack.push(copyState(this.$state))
        this.$dom.save()
        return this
    }

    restore(){
        this.$popState()
        return this
    }

    // QQuickContext2D::popState
    $popState(){
        if(!this.$stack.length) return
        let next = this.$stack.pop()
        this.$dom.restore()
        this.$command()
        this.$state = next
    }

    // QQuickContext2D::reset: a new state, an empty path and stack, the item's area cleared
    reset(){
        while(this.$stack.length){
            this.$stack.pop()
            this.$dom.restore()
        }
        this.$dom.restore()
        this.$dom.save()
        this.$state = defaultState()
        this.$path = new PainterPath()
        this.$texts = []
        let r = this.$ratioOf()
        this.$dom.setTransform(r, 0, 0, r, 0, 0)
        this.$dom.clearRect(0, 0, this.$item.width, this.$item.height)
        this.$command()
        return this
    }

    // ---- transforms (the path stays where it is: Qt maps it back into the new coordinates) ----

    $setMatrix(next){
        if(!invertible(next)){
            this.$state.invertibleCTM = false
            return
        }
        this.$state.matrix = next
        this.$command()
    }

    rotate(...args){
        if(args.length >= 1){
            let angle = num(args[0])
            if(this.$state.invertibleCTM && Number.isFinite(angle)) this.$setMatrix(multiply(rotationOf(angle * 180 / Math.PI), this.$state.matrix))
        }
        return this
    }

    scale(...args){
        if(args.length >= 2){
            let x = num(args[0]), y = num(args[1])
            if(this.$state.invertibleCTM && Number.isFinite(x) && Number.isFinite(y)) this.$setMatrix(multiply([x, 0, 0, y, 0, 0], this.$state.matrix))
        }
        return this
    }

    translate(...args){
        if(args.length >= 2){
            let x = num(args[0]), y = num(args[1])
            if(this.$state.invertibleCTM && Number.isFinite(x) && Number.isFinite(y)) this.$setMatrix(multiply([1, 0, 0, 1, x, y], this.$state.matrix))
        }
        return this
    }

    shear(...args){
        if(args.length >= 2){
            let h = num(args[0]), v = num(args[1])
            if(this.$state.invertibleCTM && Number.isFinite(h) && Number.isFinite(v)) this.$setMatrix(multiply([1, v, h, 1, 0, 0], this.$state.matrix))
        }
        return this
    }

    // QQuickContext2D::transform: state.matrix * transform
    $transform(a, b, c, d, e, f){
        if(!this.$state.invertibleCTM) return
        if(![a, b, c, d, e, f].every(Number.isFinite)) return
        this.$setMatrix(multiply(this.$state.matrix, [a, b, c, d, e, f]))
    }

    transform(...args){
        if(args.length >= 6) this.$transform(...args.slice(0, 6).map(num))
        return this
    }

    setTransform(...args){
        if(args.length >= 6){
            let v = args.slice(0, 6).map(num)
            if(v.every(Number.isFinite) && invertible(this.$state.matrix)){
                this.$state.matrix = IDENTITY.slice()
                this.$state.invertibleCTM = true
                this.$transform(...v)
            }
        }
        return this
    }

    resetTransform(){
        if(invertible(this.$state.matrix)){
            this.$state.matrix = IDENTITY.slice()
            this.$state.invertibleCTM = true
            this.$transform(1, 0, 0, 1, 0, 0)
        }
        return this
    }

    // ---- compositing and styles ----

    get globalAlpha(){ return this.$state.globalAlpha }
    set globalAlpha(value){
        let v = num(value)
        if(!Number.isFinite(v)) return
        if(v >= 0 && v <= 1 && this.$state.globalAlpha !== v){
            this.$state.globalAlpha = v
            this.$command()
        }
    }

    get globalCompositeOperation(){
        let op = this.$state.globalCompositeOperation
        // qt_composite_mode_to_string writes CompositionMode_Lighten as "lighter"
        return op === 'qt-lighten' ? 'lighter' : op
    }
    set globalCompositeOperation(value){
        let mode = String(value)
        if(!COMPOSITE_MODES.has(mode)) return
        if(mode !== this.$state.globalCompositeOperation){
            this.$state.globalCompositeOperation = mode
            this.$command()
        }
    }

    // a style read back: its color (a texture's brush is black); a gradient (no color) or an invalid color: the value set
    $styleOut(style, value){
        if(style.color) return colorString(style.color)
        if(style.texture) return '#000000'
        return value
    }

    // method_set_fillStyle / method_set_strokeStyle: a color (a string or a color value), a gradient, a pattern
    $styleIn(value, stroke){
        let s = this.$state
        let key = stroke ? 'strokeStyle' : 'fillStyle', valueKey = stroke ? 'strokeValue' : 'fillValue'
        if(value instanceof CanvasGradient){
            s[key] = { gradient: value.$snapshot() }
            s[valueKey] = value
            this.$command()
        } else if(value instanceof CanvasPattern){
            s[key] = Object.assign({}, value.$brush)
            s[valueKey] = value
            this.$command()
        } else if(typeof value === 'string'){
            let color = qtColor(value)
            if(color && !(s[key].color && sameColor(s[key].color, color))){
                s[key] = { color }
                s[valueKey] = value
                this.$command()
            }
        } else if(stroke && value && typeof value === 'object'){
            // an object that is no style: an invalid color (strokeStyle only)
            s[key] = { invalid: true }
            s[valueKey] = value
            this.$command()
        }
    }

    get fillStyle(){ return this.$styleOut(this.$state.fillStyle, this.$state.fillValue) }
    set fillStyle(value){ this.$styleIn(value, false) }
    get strokeStyle(){ return this.$styleOut(this.$state.strokeStyle, this.$state.strokeValue) }
    set strokeStyle(value){ this.$styleIn(value, true) }

    get fillRule(){ return this.$state.fillRule }
    set fillRule(value){
        if(value === 'WindingFill' || (Number.isInteger(value) && value === WINDING_FILL)) this.$state.fillRule = WINDING_FILL
        else if(value === 'OddEvenFill' || (Number.isInteger(value) && value === ODD_EVEN_FILL)) this.$state.fillRule = ODD_EVEN_FILL
    }

    createLinearGradient(...args){
        if(args.length < 4) return this
        let v = args.slice(0, 4).map(num)
        if(!v.every(Number.isFinite)) throw domException(NOT_SUPPORTED_ERR, 'createLinearGradient(): Incorrect arguments')
        return new CanvasGradient('linear', v)
    }

    createRadialGradient(...args){
        if(args.length < 6) return this
        let v = args.slice(0, 6).map(num)
        if(!v.every(Number.isFinite)) throw domException(NOT_SUPPORTED_ERR, 'createRadialGradient(): Incorrect arguments')
        if(v[2] < 0 || v[5] < 0) throw domException(INDEX_SIZE_ERR, 'createRadialGradient(): Incorrect arguments')
        return new CanvasGradient('radial', v)
    }

    // QConicalGradient(x, y, angle in degrees): counter-clockwise from the angle
    createConicalGradient(...args){
        if(args.length < 3) return this
        let x = num(args[0]), y = num(args[1]), angle = num(args[2]) * 180 / Math.PI
        if(!Number.isFinite(x) || !Number.isFinite(y)) throw domException(NOT_SUPPORTED_ERR, 'createConicalGradient(): Incorrect arguments')
        if(!Number.isFinite(angle)) throw domException(INDEX_SIZE_ERR, 'createConicalGradient(): Incorrect arguments')
        return new CanvasGradient('conical', [x, y, angle])
    }

    // method_createPattern: (color, Qt.BrushStyle) a brush pattern, (image or image data, repetition) a texture (always
    // tiled: QPainter does not use the repetition)
    createPattern(...args){
        if(args.length < 2) return undefined
        let color = typeof args[0] === 'string' ? colorFromString(args[0]) : null
        if(color){
            let mode = toInt32(args[1])
            let style = mode >= 0 && mode < 15 ? mode : 1
            return new CanvasPattern({ color, brushStyle: style })
        }
        // the texture of an image data or of the image of a url; another object has none (QBrush())
        let image = null
        if(args[0] instanceof CanvasImageData) image = imageDataCanvas(args[0])
        else if(!args[0] || typeof args[0] !== 'object') image = this.$item.$imageSource(String(args[0]))
        return new CanvasPattern(image ? { texture: image } : { color: parseColor('#000000'), brushStyle: 0 })
    }

    // ---- line styles ----

    get lineCap(){ return this.$state.lineCap }
    set lineCap(value){
        let v = String(value)
        if((v === 'round' || v === 'butt' || v === 'square') && v !== this.$state.lineCap){
            this.$state.lineCap = v
            this.$command()
        }
    }

    get lineJoin(){ return this.$state.lineJoin }
    set lineJoin(value){
        let v = String(value)
        if((v === 'round' || v === 'bevel' || v === 'miter') && v !== this.$state.lineJoin){
            this.$state.lineJoin = v
            this.$command()
        }
    }

    get lineWidth(){ return this.$state.lineWidth }
    set lineWidth(value){
        let w = num(value)
        if(w > 0 && Number.isFinite(w) && w !== this.$state.lineWidth){
            this.$state.lineWidth = w
            this.$command()
        }
    }

    get miterLimit(){ return this.$state.miterLimit }
    set miterLimit(value){
        let v = num(value)
        if(v > 0 && Number.isFinite(v) && v !== this.$state.miterLimit){
            this.$state.miterLimit = v
            this.$command()
        }
    }

    getLineDash(){ return this.$state.lineDash.slice() }

    // method_setLineDash: numbers not negative, an odd list doubled
    setLineDash(...args){
        if(!args.length || !Array.isArray(args[0])) return undefined
        let dashes = []
        for(let v of args[0]){
            let n = num(v)
            if(!Number.isFinite(n) || n < 0) return undefined
            dashes.push(n)
        }
        if(dashes.length % 2) dashes = dashes.concat(dashes)
        this.$state.lineDash = dashes
        this.$command()
        return undefined
    }

    get lineDashOffset(){ return this.$state.lineDashOffset }
    set lineDashOffset(value){
        let v = num(value)
        if(Number.isFinite(v) && v !== this.$state.lineDashOffset){
            this.$state.lineDashOffset = v
            this.$command()
        }
    }

    // ---- shadows ----

    get shadowBlur(){ return this.$state.shadowBlur }
    set shadowBlur(value){
        let v = num(value)
        if(v > 0 && Number.isFinite(v) && v !== this.$state.shadowBlur){
            this.$state.shadowBlur = v
            this.$command()
        }
    }

    get shadowColor(){ return colorName(this.$state.shadowColor) }
    set shadowColor(value){
        let color = qtColor(value)
        if(color && !sameColor(color, this.$state.shadowColor)){
            this.$state.shadowColor = color
            this.$command()
        }
    }

    get shadowOffsetX(){ return this.$state.shadowOffsetX }
    set shadowOffsetX(value){
        let v = num(value)
        if(Number.isFinite(v) && v !== this.$state.shadowOffsetX){
            this.$state.shadowOffsetX = v
            this.$command()
        }
    }

    get shadowOffsetY(){ return this.$state.shadowOffsetY }
    set shadowOffsetY(value){
        let v = num(value)
        if(Number.isFinite(v) && v !== this.$state.shadowOffsetY){
            this.$state.shadowOffsetY = v
            this.$command()
        }
    }

    // ---- the path ----

    // the path property: a Path item (its QPainterPath) or an SVG path string; it begins a new path
    get path(){ return this.$pathValue }
    set path(value){
        this.$beginPath()
        if(value && typeof value === 'object' && typeof value.$svg === 'function') this.$addSvgPath(value.$svg())
        else if(value !== undefined && value !== null) this.$addSvgPath(String(value))
        this.$pathValue = value
    }

    // QGuiSvg::parsePath: the commands of an SVG path into the current path, in item coordinates
    $addSvgPath(d){
        const { parseSvgPath } = require('./Context2DSvg')
        let path = parseSvgPath(d)
        if(!path) return
        let m = this.$state.matrix
        let next = new PainterPath()
        for(let e of path.elements){
            let [x, y] = mapPoint(m, e.x, e.y)
            if(e.t === C){
                let [ax, ay] = mapPoint(m, e.c1x, e.c1y), [bx, by] = mapPoint(m, e.c2x, e.c2y)
                next.elements.push({ t: C, c1x: ax, c1y: ay, c2x: bx, c2y: by, x, y })
            } else next.elements.push({ t: e.t, x, y })
        }
        next.cStart = path.cStart
        next.requireMoveTo = path.requireMoveTo
        this.$path = next
    }

    // a point of the current coordinates in item coordinates
    $map(x, y){ return mapPoint(this.$state.matrix, x, y) }
    $unmap(x, y){ return mapPoint(inverted(this.$state.matrix), x, y) }

    $beginPath(){
        this.$texts = []
        if(!this.$path.elementCount()) return
        this.$path = new PainterPath()
    }

    beginPath(){
        this.$beginPath()
        return this
    }

    // closePath: only a path that is not a point
    closePath(){
        if(this.$path.elementCount()){
            let b = this.$path.controlBounds()
            if(b.width || b.height) this.$path.closeSubpath()
        }
        return this
    }

    moveTo(...args){
        if(args.length >= 2){
            let x = num(args[0]), y = num(args[1])
            if(!Number.isFinite(x) || !Number.isFinite(y)) return undefined
            if(this.$state.invertibleCTM) this.$path.moveTo(...this.$map(x, y))
        }
        return this
    }

    $lineTo(x, y){
        if(!this.$state.invertibleCTM) return
        let [px, py] = this.$map(x, y)
        if(!this.$path.elementCount()) this.$path.moveTo(px, py)
        else {
            let [cx, cy] = this.$path.currentPosition()
            if(cx !== px || cy !== py) this.$path.lineTo(px, py)
        }
    }

    lineTo(...args){
        if(args.length >= 2){
            let x = num(args[0]), y = num(args[1])
            if(!Number.isFinite(x) || !Number.isFinite(y)) return undefined
            this.$lineTo(x, y)
        }
        return this
    }

    quadraticCurveTo(...args){
        if(args.length >= 4){
            let [cpx, cpy, x, y] = args.slice(0, 4).map(num)
            if(![cpx, cpy, x, y].every(Number.isFinite)) return undefined
            if(this.$state.invertibleCTM){
                let [cx, cy] = this.$map(cpx, cpy), [ex, ey] = this.$map(x, y)
                if(!this.$path.elementCount()) this.$path.moveTo(cx, cy)
                let [px, py] = this.$path.currentPosition()
                if(px !== ex || py !== ey) this.$path.quadTo(cx, cy, ex, ey)
            }
        }
        return this
    }

    bezierCurveTo(...args){
        if(args.length >= 6){
            let v = args.slice(0, 6).map(num)
            if(!v.every(Number.isFinite)) return undefined
            if(this.$state.invertibleCTM){
                let [ax, ay] = this.$map(v[0], v[1]), [bx, by] = this.$map(v[2], v[3]), [ex, ey] = this.$map(v[4], v[5])
                if(!this.$path.elementCount()) this.$path.moveTo(ax, ay)
                let [px, py] = this.$path.currentPosition()
                if(px !== ex || py !== ey) this.$path.cubicTo(ax, ay, bx, by, ex, ey)
            }
        }
        return this
    }

    // an element made in the current coordinates (an arc, a rectangle, an ellipse): built on the current point mapped
    // back, its new elements mapped to the item
    $inUserSpace(build){
        let m = this.$state.matrix, inv = inverted(m)
        let path = this.$path
        let local = new PainterPath()
        let had = path.elementCount()
        if(had){
            let last = path.last()
            let [x, y] = mapPoint(inv, last.x, last.y)
            local.elements.push({ t: last.t === M ? M : L, x, y })
            local.requireMoveTo = path.requireMoveTo
        }
        let first = had ? Object.assign({}, local.elements[0]) : null
        build(local)
        let mapped = (e) => {
            let [x, y] = mapPoint(m, e.x, e.y)
            if(e.t === C){
                let [ax, ay] = mapPoint(m, e.c1x, e.c1y), [bx, by] = mapPoint(m, e.c2x, e.c2y)
                return { t: C, c1x: ax, c1y: ay, c2x: bx, c2y: by, x, y }
            }
            return { t: e.t, x, y }
        }
        if(!had){
            path.elements = local.elements.map(mapped)
            path.cStart = local.cStart
        } else {
            let base = path.elements.length - 1
            let head = local.elements[0]
            if(head.x !== first.x || head.y !== first.y){
                let e = mapped(head)
                path.elements[base].x = e.x
                path.elements[base].y = e.y
            }
            for(let k = 1; k < local.elements.length; k++) path.elements.push(mapped(local.elements[k]))
            if(local.cStart > 0) path.cStart = base + local.cStart
        }
        path.requireMoveTo = local.requireMoveTo
    }

    // QQuickContext2D::arc: Qt's angles go the other way; a full turn, else the sweep between the angles
    $arc(xc, yc, radius, sar, ear, antiClockWise){
        if(!this.$state.invertibleCTM) return
        if(![xc, yc, sar, ear, radius].every(Number.isFinite)) return
        if(sar === ear) return
        sar = -sar
        ear = -ear
        antiClockWise = !antiClockWise
        let sa = Math.fround(sar * 180 / Math.PI), ea = Math.fround(ear * 180 / Math.PI)
        let span = 0
        if((!antiClockWise && ea - sa >= 360) || (antiClockWise && sa - ea >= 360)) span = 360
        else {
            if(!antiClockWise && ea < sa) span += 360
            else if(antiClockWise && sa < ea) span -= 360
            if(!(fuzzyCompare(span + (ea - sa) + 1, 1) && fuzzyCompare(Math.abs(span), 360))) span += ea - sa
        }
        let empty = !this.$path.elementCount()
        if(!empty && !radius){
            this.$lineTo(xc, yc)
            return
        }
        this.$inUserSpace((p) => {
            if(empty) p.arcMoveTo(xc - radius, yc - radius, radius * 2, radius * 2, sa)
            p.arcTo(xc - radius, yc - radius, radius * 2, radius * 2, sa, span)
        })
    }

    arc(...args){
        if(args.length >= 5){
            let antiClockwise = args.length === 6 ? !!args[5] : false
            let radius = num(args[2])
            if(Number.isFinite(radius) && radius < 0) throw domException(INDEX_SIZE_ERR, 'Incorrect argument radius')
            this.$arc(num(args[0]), num(args[1]), radius, num(args[3]), num(args[4]), antiClockwise)
        }
        return this
    }

    // QQuickContext2D::addArcTo: the arc of the radius between the two tangents, by arc()
    $addArcTo(p1x, p1y, p2x, p2y, radius){
        let [p0x, p0y] = this.$unmap(...this.$path.currentPosition())
        let ax = p0x - p1x, ay = p0y - p1y, bx = p2x - p1x, by = p2y - p1y
        let la = Math.hypot(ax, ay), lb = Math.hypot(bx, by)
        let cosPhi = (ax * bx + ay * by) / (la * lb)
        if(fuzzyCompare(Math.abs(cosPhi), 1)){
            this.$lineTo(p1x, p1y)
            return
        }
        let tangent = radius / Math.tan(Math.acos(cosPhi) / 2)
        let fa = tangent / la
        let tx = p1x + fa * ax, ty = p1y + fa * ay
        let ox = ay, oy = -ax
        let lo = Math.hypot(ox, oy)
        let fr = radius / lo
        let cosAlpha = (ox * bx + oy * by) / (lo * lb)
        if(cosAlpha < 0){ ox = -ox; oy = -oy }
        let px = tx + fr * ox, py = ty + fr * oy
        ox = -ox
        oy = -oy
        let sa = Math.atan2(oy, ox)
        let fb = tangent / lb
        let t2x = p1x + fb * bx, t2y = p1y + fb * by
        let ea = Math.atan2(t2y - py, t2x - px)
        let anticlockwise = (sa > ea && sa - ea < Math.PI) || (sa < ea && ea - sa > Math.PI)
        this.$arc(px, py, radius, sa, ea, anticlockwise)
    }

    arcTo(...args){
        if(args.length >= 5){
            let radius = num(args[4])
            if(Number.isFinite(radius) && radius < 0) throw domException(INDEX_SIZE_ERR, 'Incorrect argument radius')
            let [x1, y1, x2, y2] = args.slice(0, 4).map(num)
            if(this.$state.invertibleCTM && [x1, y1, x2, y2, radius].every(Number.isFinite)){
                if(!this.$path.elementCount()) this.$path.moveTo(...this.$map(x1, y1))
                else {
                    let [cx, cy] = this.$unmap(...this.$path.currentPosition())
                    if((x1 === cx && y1 === cy) || (x1 === x2 && y1 === y2) || !radius) this.$lineTo(x1, y1)
                    else this.$addArcTo(x1, y1, x2, y2, radius)
                }
            }
        }
        return this
    }

    rect(...args){
        if(args.length >= 4){
            let [x, y, w, h] = args.slice(0, 4).map(num)
            if(this.$state.invertibleCTM && [x, y, w, h].every(Number.isFinite)){
                if(!w && !h) this.$path.moveTo(...this.$map(x, y))
                else this.$inUserSpace((p) => p.addRect(x, y, w, h))
            }
        }
        return this
    }

    roundedRect(...args){
        if(args.length >= 6){
            let [x, y, w, h, xr, yr] = args.slice(0, 6).map(num)
            if(this.$state.invertibleCTM && [x, y, w, h, xr, yr].every(Number.isFinite)){
                if(!w && !h) this.$path.moveTo(...this.$map(x, y))
                else this.$inUserSpace((p) => p.addRoundedRect(x, y, w, h, xr, yr))
            }
        }
        return this
    }

    ellipse(...args){
        if(args.length >= 4){
            let [x, y, w, h] = args.slice(0, 4).map(num)
            if(this.$state.invertibleCTM && [x, y, w, h].every(Number.isFinite)){
                if(!w && !h) this.$path.moveTo(...this.$map(x, y))
                else this.$inUserSpace((p) => p.addEllipse(x, y, w, h))
            }
        }
        return this
    }

    // QQuickContext2D::text: the glyphs of the text at the left end of the baseline, part of the path
    text(...args){
        if(args.length >= 3){
            let x = num(args[1]), y = num(args[2])
            if(!Number.isFinite(x) || !Number.isFinite(y)) return undefined
            if(this.$state.invertibleCTM) this.$texts.push({ text: String(args[0]), x, y, font: this.$state.font, matrix: this.$state.matrix.slice() })
        }
        return this
    }

    fill(){
        let s = this.$state
        if(s.invertibleCTM && (this.$path.elementCount() || this.$texts.length)){
            let path = this.$path.toPath2D(inverted(s.matrix))
            let texts = this.$texts
            this.$draw('fill', (ctx) => {
                ctx.fill(path, s.fillRule === ODD_EVEN_FILL ? 'evenodd' : 'nonzero')
                this.$drawTexts(ctx, texts, true)
            })
        }
        return this
    }

    stroke(){
        let s = this.$state
        if(s.invertibleCTM && (this.$path.elementCount() || this.$texts.length)){
            let path = this.$path.toPath2D(inverted(s.matrix))
            let texts = this.$texts
            this.$draw('stroke', (ctx) => {
                ctx.stroke(path)
                this.$drawTexts(ctx, texts, false)
            })
        }
        return this
    }

    // QQuickContext2D::clip: the path closed, intersected with the clip of the state
    clip(){
        let s = this.$state
        if(s.invertibleCTM){
            let path = this.$path.clone()
            path.closeSubpath()
            s.clips = s.clips.concat([{ path, rule: s.fillRule }])
            this.$applyClips([{ path, rule: s.fillRule }])
            this.$command()
        }
        return this
    }

    // QQuickContext2D::isPointInPath: the point of the item in the path (with the fill rule) or on its border
    isPointInPath(...args){
        if(args.length < 2) return false
        let x = num(args[0]), y = num(args[1])
        let s = this.$state
        if(!s.invertibleCTM || !this.$path.elementCount() || !Number.isFinite(x) || !Number.isFinite(y)) return false
        let r = this.$ratioOf()
        let dom = this.$dom
        dom.setTransform(r, 0, 0, r, 0, 0)
        if(dom.isPointInPath(this.$path.toPath2D(IDENTITY), x * r, y * r, s.fillRule === ODD_EVEN_FILL ? 'evenodd' : 'nonzero')) return true
        let border = this.$path.polygon()
        for(let i = 1; i < border.length; i++){
            let [ax, ay] = border[i - 1], [bx, by] = border[i]
            let collinear = fuzzyCompare((by - ay) * (x - ax), (y - ay) * (bx - ax)) || ((by - ay) * (x - ax) === (y - ay) * (bx - ax))
            if(collinear && (Math.abs(bx - ax) > Math.abs(by - ay) ? within(x, ax, bx) : within(y, ay, by))) return true
        }
        return false
    }

    // ---- rectangles ----

    fillRect(...args){
        if(args.length >= 4){
            let [x, y, w, h] = args.slice(0, 4).map(num)
            if(this.$state.invertibleCTM && [x, y, w, h].every(Number.isFinite)) this.$draw('fill', (ctx) => ctx.fillRect(x, y, w, h))
        }
        return this
    }

    strokeRect(...args){
        if(args.length >= 4){
            let [x, y, w, h] = args.slice(0, 4).map(num)
            if(this.$state.invertibleCTM && [x, y, w, h].every(Number.isFinite)) this.$draw('stroke', (ctx) => ctx.strokeRect(x, y, w, h))
        }
        return this
    }

    // ClearRect: the rectangle in the current coordinates and clip made transparent
    clearRect(...args){
        if(args.length >= 4){
            let [x, y, w, h] = args.slice(0, 4).map(num)
            if(this.$state.invertibleCTM && [x, y, w, h].every(Number.isFinite)){
                let dom = this.$dom, r = this.$ratioOf(), m = this.$state.matrix
                dom.setTransform(r * m[0], r * m[1], r * m[2], r * m[3], r * m[4], r * m[5])
                dom.clearRect(x, y, w, h)
                this.$command()
            }
        }
        return this
    }

    // ---- text ----

    get font(){ return fontString(this.$state.font) }
    set font(value){
        let font = fontFromString(String(value), this.$state.font)
        this.$state.font = font
    }

    get textAlign(){ return this.$state.textAlign }
    set textAlign(value){
        let v = String(value)
        if(v === 'start' || v === 'end' || v === 'left' || v === 'right' || v === 'center') this.$state.textAlign = v
    }

    get textBaseline(){ return this.$state.textBaseline }
    set textBaseline(value){
        let v = String(value)
        if(v === 'alphabetic' || v === 'hanging' || v === 'top' || v === 'bottom' || v === 'middle') this.$state.textBaseline = v
    }

    // QFontMetrics of a font on the DOM context: ascent, descent and the advance of a text (integers)
    $metrics(font, text){
        let dom = this.$dom
        dom.font = fontCss(font)
        let m = dom.measureText(text)
        let ascent = Math.round(m.fontBoundingBoxAscent !== undefined ? m.fontBoundingBoxAscent : m.actualBoundingBoxAscent || 0)
        let descent = Math.round(m.fontBoundingBoxDescent !== undefined ? m.fontBoundingBoxDescent : m.actualBoundingBoxDescent || 0)
        return { ascent, height: ascent + descent, width: Math.round(m.width) }
    }

    // createTextGlyphs: the text placed by the alignment and the baseline as Qt does (left to right)
    $textOrigin(text, x, y){
        let s = this.$state
        let m = this.$metrics(s.font, text)
        let yoffset = 0
        if(s.textBaseline === 'middle') yoffset = (m.ascent >> 1) + m.height - m.ascent
        else if(s.textBaseline === 'alphabetic') yoffset = m.ascent
        else if(s.textBaseline === 'bottom') yoffset = m.height
        let align = s.textAlign === 'start' ? 'left' : s.textAlign === 'end' ? 'right' : s.textAlign
        let xoffset = align === 'center' ? Math.trunc(m.width / 2) : align === 'right' ? m.width : 0
        return [x - xoffset, y - yoffset + m.ascent]
    }

    // the texts of the path (text()), at the left end of their baselines, each with its own matrix
    $drawTexts(ctx, texts, fill){
        if(!texts.length) return
        let r = this.$ratioOf()
        let saved = ctx.getTransform()
        ctx.textAlign = 'left'
        ctx.textBaseline = 'alphabetic'
        for(let t of texts){
            ctx.setTransform(r * t.matrix[0], r * t.matrix[1], r * t.matrix[2], r * t.matrix[3], r * t.matrix[4], r * t.matrix[5])
            ctx.font = fontCss(t.font)
            if(fill) ctx.fillText(t.text, t.x, t.y)
            else ctx.strokeText(t.text, t.x, t.y)
        }
        ctx.setTransform(saved)
    }

    fillText(...args){
        if(args.length >= 3){
            let x = num(args[1]), y = num(args[2])
            if(!Number.isFinite(x) || !Number.isFinite(y)) return undefined
            let text = String(args[0])
            let [tx, ty] = this.$textOrigin(text, x, y)
            this.$draw('fill', (ctx) => {
                ctx.font = fontCss(this.$state.font)
                ctx.textAlign = 'left'
                ctx.textBaseline = 'alphabetic'
                ctx.fillText(text, tx, ty)
            })
        }
        return this
    }

    strokeText(...args){
        if(args.length >= 3){
            let x = num(args[1]), y = num(args[2])
            if(this.$state.invertibleCTM && Number.isFinite(x) && Number.isFinite(y)){
                let text = String(args[0])
                let [tx, ty] = this.$textOrigin(text, x, y)
                this.$draw('stroke', (ctx) => {
                    ctx.font = fontCss(this.$state.font)
                    ctx.textAlign = 'left'
                    ctx.textBaseline = 'alphabetic'
                    ctx.strokeText(text, tx, ty)
                })
            }
        }
        return this
    }

    // method_measureText: { width } as an integer advance
    measureText(...args){
        if(!args.length) return undefined
        return { width: this.$metrics(this.$state.font, String(args[0])).width }
    }

    // ---- images ----

    // method_drawImage: a url, an Image, a Canvas, an image data; 3, 5 or 9 arguments; the source inside the image
    drawImage(...args){
        if(!args.length) return undefined
        if(!this.$state.invertibleCTM) return undefined
        let source = args[0]
        let image = null
        if(typeof source === 'string'){
            image = this.$item.$imageSource(source)
        } else if(source instanceof CanvasImageData){
            image = imageDataCanvas(source)
            if(!image) throw domException(TYPE_MISMATCH_ERR, 'drawImage(), type mismatch')
        } else if(source && typeof source === 'object'){
            if(this.$item.$isImageSource && !this.$item.$isImageSource(source)) throw domException(TYPE_MISMATCH_ERR, 'drawImage(), type mismatch')
            image = this.$item.$imageSource(source)
        } else {
            throw domException(TYPE_MISMATCH_ERR, 'drawImage(), type mismatch')
        }
        if(!image) return undefined
        // the canvas of a Canvas item has device pixels: its image is at the item's size
        let scale = image.$jqRatio || 1
        let iw = (image.naturalWidth || image.width) / scale, ih = (image.naturalHeight || image.height) / scale
        if(!iw || !ih) return undefined
        let sx, sy, sw, sh, dx, dy, dw, dh
        if(args.length >= 9){
            [sx, sy, sw, sh, dx, dy, dw, dh] = args.slice(1, 9).map(num)
        } else if(args.length >= 5){
            sx = 0; sy = 0; sw = iw; sh = ih;
            [dx, dy, dw, dh] = args.slice(1, 5).map(num)
        } else if(args.length >= 3){
            dx = num(args[1]); dy = num(args[2])
            sx = 0; sy = 0; sw = iw; sh = ih; dw = sw; dh = sh
        } else return undefined
        if(![sx, sy, sw, sh, dx, dy, dw, dh].every(Number.isFinite)) return undefined
        if(sx < 0 || sy < 0 || sw === 0 || sh === 0 || sx + sw > iw || sy + sh > ih || sx + sw < 0 || sy + sh < 0) throw domException(INDEX_SIZE_ERR, 'drawImage(), index size error')
        this.$draw('image', (ctx) => ctx.drawImage(image, sx * scale, sy * scale, sw * scale, sh * scale, dx, dy, dw, dh), sw !== dw || sh !== dh)
        return this
    }

    // ---- pixels ----

    createImageData(...args){
        if(args.length === 1){
            let a = args[0]
            if(a instanceof CanvasImageData) return new CanvasImageData(a.width, a.height)
            if(typeof a === 'string'){
                let image = this.$item.$imageSource(a)
                let w = image ? (image.naturalWidth || image.width) : 0, h = image ? (image.naturalHeight || image.height) : 0
                let canvas = image && w && h ? scratchCanvas(w, h) : null
                if(!canvas) return new CanvasImageData(w, h)
                let c = canvas.getContext('2d')
                c.drawImage(image, 0, 0)
                return new CanvasImageData(w, h, c.getImageData(0, 0, w, h).data)
            }
        } else if(args.length === 2){
            let w = num(args[0]), h = num(args[1])
            if(!Number.isFinite(w) || !Number.isFinite(h)) throw domException(NOT_SUPPORTED_ERR, 'createImageData(): invalid arguments')
            if(w > 0 && h > 0) return new CanvasImageData(Math.round(w), Math.round(h))
            throw domException(INDEX_SIZE_ERR, 'createImageData(): invalid arguments')
        }
        return undefined
    }

    // method_getImageData: the pixels of the rectangle of the item (the canvas painted first)
    getImageData(...args){
        if(args.length < 4) return null
        let [x, y, w, h] = args.slice(0, 4).map(num)
        if(![x, y, w, h].every(Number.isFinite)) throw domException(NOT_SUPPORTED_ERR, 'getImageData(): Invalid arguments')
        if(w <= 0 || h <= 0) throw domException(INDEX_SIZE_ERR, 'getImageData(): Invalid arguments')
        this.$item.$flush()
        let iw = Math.round(w), ih = Math.round(h)
        let r = this.$ratioOf()
        let dom = this.$dom
        let bytes
        if(r === 1){
            bytes = dom.getImageData(Math.round(x), Math.round(y), iw, ih).data
        } else {
            // the device pixels of the rectangle at the size of the rectangle
            let canvas = scratchCanvas(iw, ih)
            let c = canvas.getContext('2d')
            c.imageSmoothingEnabled = false
            c.drawImage(dom.canvas, x * r, y * r, w * r, h * r, 0, 0, iw, ih)
            bytes = c.getImageData(0, 0, iw, ih).data
        }
        return new CanvasImageData(iw, ih, new Uint8ClampedArray(bytes))
    }

    // method_putImageData: Qt 6.12 draws only with all seven arguments (the dirty rectangle), with the state
    putImageData(...args){
        if(args.length < 7) return undefined
        let data = args[0]
        if(!data || typeof data !== 'object') throw domException(TYPE_MISMATCH_ERR, 'Context2D::putImageData, the image data type mismatch')
        let dx = num(args[1]), dy = num(args[2])
        if(!Number.isFinite(dx) || !Number.isFinite(dy)) throw domException(NOT_SUPPORTED_ERR, 'putImageData() : Invalid arguments')
        if(!(data instanceof CanvasImageData)) return undefined
        let w = data.width, h = data.height
        let [dirtyX, dirtyY, dirtyWidth, dirtyHeight] = args.slice(3, 7).map(num)
        if(![dirtyX, dirtyY, dirtyWidth, dirtyHeight].every(Number.isFinite)) throw domException(NOT_SUPPORTED_ERR, 'putImageData() : Invalid arguments')
        if(dirtyWidth < 0){ dirtyX += dirtyWidth; dirtyWidth = -dirtyWidth }
        if(dirtyHeight < 0){ dirtyY += dirtyHeight; dirtyHeight = -dirtyHeight }
        if(dirtyX < 0){ dirtyWidth += dirtyX; dirtyX = 0 }
        if(dirtyY < 0){ dirtyHeight += dirtyY; dirtyY = 0 }
        if(dirtyX + dirtyWidth > w) dirtyWidth = w - dirtyX
        if(dirtyY + dirtyHeight > h) dirtyHeight = h - dirtyY
        if(dirtyWidth <= 0 || dirtyHeight <= 0) return undefined
        // the dirty part of the image, drawn at (dx, dy) from the source rectangle of the dirty part again (qt_drawImage
        // copies (dirtyX, dirtyY, ...) of the copy: Qt 6.12 shifts it twice)
        let full = imageDataCanvas(data)
        let dw = Math.round(dirtyWidth), dh = Math.round(dirtyHeight)
        let sub = full && scratchCanvas(dw, dh)
        if(sub){
            sub.getContext('2d').drawImage(full, Math.round(dirtyX), Math.round(dirtyY), dw, dh, 0, 0, dw, dh)
            let image = scratchCanvas(dw, dh)
            image.getContext('2d').drawImage(sub, -Math.round(dirtyX), -Math.round(dirtyY))
            this.$draw('image', (ctx) => ctx.drawImage(image, dx, dy))
        }
        return this
    }

    // ---- not supported ----

    drawFocusRing(){ throw domException(NOT_SUPPORTED_ERR, 'Context2D::drawFocusRing is not supported') }
    setCaretSelectionRect(){ throw domException(NOT_SUPPORTED_ERR, 'Context2D::setCaretSelectionRect is not supported') }
    caretBlinkRate(){ throw domException(NOT_SUPPORTED_ERR, 'Context2D::caretBlinkRate is not supported') }

    // ---- drawing: the state put on a DOM context (QQuickContext2DCommandBuffer::replay) ----

    // the DOM style of a brush on ctx (gradients and textures in the current coordinates)
    $brushOf(ctx, brush){
        if(brush.invalid) return 'rgba(0,0,0,0)'
        if(brush.gradient){
            let g = brush.gradient
            let gradient
            if(g.kind === 'linear') gradient = ctx.createLinearGradient(...g.args)
            else if(g.kind === 'radial') gradient = ctx.createRadialGradient(...g.args)
            else {
                // QConicalGradient: counter-clockwise from the angle; the DOM's turns clockwise
                if(!ctx.createConicGradient) return 'rgba(0,0,0,0)'
                gradient = ctx.createConicGradient(-g.args[2] * Math.PI / 180, g.args[0], g.args[1])
            }
            let stops = g.kind === 'conical' ? g.stops.map(s => ({ pos: 1 - s.pos, color: s.color })).reverse() : g.stops
            if(!stops.length) return 'rgba(0,0,0,0)'
            for(let s of stops) gradient.addColorStop(s.pos, colorCss(s.color))
            return gradient
        }
        if(brush.texture) return ctx.createPattern(brush.texture, 'repeat')
        if(brush.brushStyle !== undefined && brush.brushStyle !== 1){
            if(brush.brushStyle === 0) return 'rgba(0,0,0,0)'
            return ctx.createPattern(this.$brushPattern(brush), 'repeat')
        }
        return colorCss(brush.color)
    }

    // the 8 x 8 pattern of a Qt brush style in its color
    $brushPattern(brush){
        let bits = BRUSH_PATTERNS[brush.brushStyle - 2]
        let canvas = scratchCanvas(8, 8)
        if(!bits || !canvas) return canvas
        let c = canvas.getContext('2d')
        let image = c.createImageData(8, 8)
        for(let y = 0; y < 8; y++){
            for(let x = 0; x < 8; x++){
                if(!(bits[y] & (1 << x))) continue
                let i = (y * 8 + x) * 4
                image.data[i] = c8(brush.color.r)
                image.data[i + 1] = c8(brush.color.g)
                image.data[i + 2] = c8(brush.color.b)
                image.data[i + 3] = c8(brush.color.a)
            }
        }
        c.putImageData(image, 0, 0)
        return canvas
    }

    // the state of a command on ctx: the matrix over the device pixel ratio, the brush or the pen, alpha, composition,
    // shadow; mask: opaque black, no shadow (the coverage of what is drawn)
    $prepare(ctx, kind, mask, op, scaled = false){
        let s = this.$state, r = this.$ratioOf(), m = s.matrix
        ctx.setTransform(r * m[0], r * m[1], r * m[2], r * m[3], r * m[4], r * m[5])
        ctx.globalAlpha = mask ? 1 : s.globalAlpha
        ctx.globalCompositeOperation = op
        // SmoothPixmapTransform (Canvas.smooth) where an image is scaled or turned: an image at its size keeps its pixels
        // (also over the device pixel ratio of the canvas)
        let moved = m[0] === 1 && m[1] === 0 && m[2] === 0 && m[3] === 1
        ctx.imageSmoothingEnabled = this.$item.smooth !== false && (scaled || !moved)
        if(mask){
            ctx.fillStyle = '#000'
            ctx.strokeStyle = '#000'
        } else if(kind === 'fill') ctx.fillStyle = this.$brushOf(ctx, s.fillStyle)
        else if(kind === 'stroke') ctx.strokeStyle = this.$brushOf(ctx, s.strokeStyle)
        if(kind === 'stroke'){
            // QPen: the dashes and their offset in units of the width
            ctx.lineWidth = s.lineWidth
            ctx.lineCap = s.lineCap
            ctx.lineJoin = s.lineJoin
            ctx.miterLimit = s.miterLimit
            ctx.setLineDash(s.lineDash.map(d => d * s.lineWidth))
            ctx.lineDashOffset = s.lineDashOffset * s.lineWidth
        } else ctx.setLineDash([])
        let shadow = !mask && c8(s.shadowColor.a) && (s.shadowBlur || s.shadowOffsetX || s.shadowOffsetY)
        if(shadow){
            // the shadow is drawn in the current coordinates (QPainter): its offset and blur go through the matrix
            let a = r * m[0], b = r * m[1], c = r * m[2], d = r * m[3]
            ctx.shadowColor = colorCss(s.shadowColor)
            ctx.shadowOffsetX = a * s.shadowOffsetX + c * s.shadowOffsetY
            ctx.shadowOffsetY = b * s.shadowOffsetX + d * s.shadowOffsetY
            ctx.shadowBlur = s.shadowBlur * Math.sqrt(Math.abs(a * d - b * c))
        } else {
            ctx.shadowColor = 'rgba(0,0,0,0)'
            ctx.shadowBlur = 0
            ctx.shadowOffsetX = 0
            ctx.shadowOffsetY = 0
        }
    }

    // a drawing command: paint(ctx) on the canvas with the state; a composition the DOM applies outside what is drawn
    // keeps the canvas there (QPainter's compositions are bounded)
    $draw(kind, paint, scaled = false){
        let dom = this.$dom
        let op = this.$state.globalCompositeOperation
        this.$command()
        if(op === 'qt-destination') return
        let dom_op = op.startsWith('qt-') ? (op === 'qt-clear' ? 'destination-out' : op.slice(3)) : op
        if(op !== 'qt-clear' && !UNBOUNDED.has(op)){
            dom.save()
            try {
                this.$prepare(dom, kind, false, dom_op, scaled)
                paint(dom)
            } finally {
                dom.restore()
            }
            return
        }
        let canvas = dom.canvas
        let W = canvas.width, H = canvas.height
        let maskCanvas = scratchCanvas(W, H)
        if(!maskCanvas) return
        let mask = maskCanvas.getContext('2d')
        this.$prepare(mask, kind, true, 'source-over', scaled)
        paint(mask)
        dom.save()
        try {
            if(op === 'qt-clear'){
                dom.setTransform(1, 0, 0, 1, 0, 0)
                dom.globalAlpha = 1
                dom.globalCompositeOperation = 'destination-out'
                dom.shadowColor = 'rgba(0,0,0,0)'
                dom.drawImage(maskCanvas, 0, 0)
                return
            }
            // before: the canvas outside what is drawn
            let outside = scratchCanvas(W, H)
            let o = outside.getContext('2d')
            o.drawImage(canvas, 0, 0)
            o.globalCompositeOperation = 'destination-out'
            o.drawImage(maskCanvas, 0, 0)
            this.$prepare(dom, kind, false, dom_op, scaled)
            paint(dom)
            // what is drawn, inside what is drawn, plus the canvas as it was outside
            dom.setTransform(1, 0, 0, 1, 0, 0)
            dom.globalAlpha = 1
            dom.shadowColor = 'rgba(0,0,0,0)'
            dom.globalCompositeOperation = 'destination-in'
            dom.drawImage(maskCanvas, 0, 0)
            dom.globalCompositeOperation = 'lighter'
            dom.drawImage(outside, 0, 0)
        } finally {
            dom.restore()
        }
    }
}

const within = (p, a, b) => (p >= a && p <= b) || (p >= b && p <= a)

module.exports = { Context2D, CanvasGradient, CanvasPattern, CanvasImageData, PainterPath, qtColor, colorString, fontFromString, fontString,
    ODD_EVEN_FILL, WINDING_FILL, M, L, C }
